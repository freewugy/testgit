/*
 * ftps_client.c
 *
 *  Created on: Feb 5, 2015
 *      Author: jhseo
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "curl/curl.h"
#include "ftps_client.h"
#include "sus_comm.h"
#include "msg_format.h"
#include "operation_mgr.h"
#include "utils.h"

extern DownloadListener sDownloadListenerCB;

DOWNLOAD_PROGRESS_T sProgInfo;
DOWNLOAD_STRUCT_T sChunk;
long sPrevDownloadDataSize = 0;
long REPORT_UNIT_SIZE = 0;
long sTotalFileSize = 0;
long sTotalDownloadedSize = 0;
long sStartOffset = 0;
int sRetryCount = 0;
const long FTP_SOCKET_TIMEOUT_SEC = 15;
const int MAX_RETRY_DOWNLOAD_COUNT = 3;
const int WAITING_TIME_FOR_NEXT_SESSIOIN = 5;

void setTotalFileSize(long size) {
    sTotalFileSize = size;
}

long getTotalFileSize() {
    return sTotalFileSize;
}

void setReportedDownloadSize(long size)
{
    sPrevDownloadDataSize = size;
}

long getReportedDownloadSize()
{
    return sPrevDownloadDataSize;
}

void increaseTotalDownloadedSize(long size) {
    sTotalDownloadedSize += size;
}

long getTotalDownloadedSize() {
    return sTotalDownloadedSize;
}

int isNeedReportDownloadProgress(long currentDownloadSize)
{
    long downloadSize = currentDownloadSize - getReportedDownloadSize();
    if(downloadSize >= REPORT_UNIT_SIZE) {
        return 1;
    } else {
        return 0;
    }
}

char* convertFtpsUrlScheme(char* pOriginUrl)
{
    if(pOriginUrl == NULL) {
        return NULL;
    }

    char tempBuf[MAX_URL_LEN];
    memset(tempBuf, 0x00, sizeof(char) * MAX_URL_LEN);

    char protocolScheme[MAX_PROTOCOL_SCH_LEN + 1];
    memset(protocolScheme, 0x00, sizeof(char) * (MAX_PROTOCOL_SCH_LEN + 1));

    memcpy(protocolScheme, pOriginUrl, MAX_PROTOCOL_SCH_LEN);

    if(strncmp(protocolScheme, FTPS_PROTOCOL_SCHEME, MAX_PROTOCOL_SCH_LEN)) {
        Log("ConvertFtpUrl...no change protocol scheme because protocol scheme is [%s]\n", protocolScheme);
        return pOriginUrl;
    }

    char * startingUrlPos = pOriginUrl + strlen(FTPS_PROTOCOL_SCHEME);
    sprintf(tempBuf, "%s%s", FTP_PROTOCOL_SCHEME, startingUrlPos);

    return strdup(tempBuf);
}

int reportDownloadProgress(void *p, curl_off_t dltotal, curl_off_t dlnow,
        curl_off_t ultotal, curl_off_t ulnow)
{
    if (dlnow == 0) {
        setReportedDownloadSize(0);
        return 0;
    }

    if(isNeedReportDownloadProgress(dlnow) || (dlnow == dltotal)) {

        if(getReportedDownloadSize() == dlnow) {
            return 0;
        }

        long totalFileSize = getTotalFileSize();

        if(sDownloadListenerCB != NULL) {
            long receivedUnitSize = dlnow - getReportedDownloadSize();
            increaseTotalDownloadedSize(receivedUnitSize);

            long receivedDataSize = getTotalDownloadedSize();

//            Log(">>> report progress, StartOffset[%ld]/dlnow[%ld]/total[%ld]/unit[%d]/accum[%ld]\n",
//                    sStartOffset, dlnow, dltotal, receivedUnitSize, receivedDataSize);

            sDownloadListenerCB(sChunk.pktDwInfo.pRecvBuffer,
                                sChunk.downloadSize,
                                receivedDataSize,
                                totalFileSize,
                                sChunk.pktDwInfo.pPrivData);

            setReportedDownloadSize(dlnow);
        }
    }

    return 0;
}

size_t dumpRecvedDataToPktMem(void *contents, size_t size, size_t nmemb, void *pUserData)
{
    size_t realsize = size * nmemb;
    DOWNLOAD_STRUCT_T* mem = (DOWNLOAD_STRUCT_T *) pUserData;

    if(mem->pktDwInfo.pRecvBuffer == NULL) {
        /* out of memory! */
        Log("not enough memory (malloc returned NULL)\n");
        return 0;
    }

    if (realsize <= sChunk.pktDwInfo.pktUnitSize) {
        memcpy(mem->pktDwInfo.pRecvBuffer, contents, realsize);
        mem->totalDownloadSize += realsize;
        mem->downloadSize = realsize;

        sRetryCount = 0;
    } else {
        Log("Error! Received data size(%d) is over than buffer size(%d)\n",
                    realsize, sChunk.pktDwInfo.pktUnitSize);
    }

    return realsize;
}


int calc_total_filesize(UPGRADE_INFO_T* pUpgradeInfo) {
    if (pUpgradeInfo == NULL) {
        return 0;
    }

    int totalSize = 0;
    int fileNum = pUpgradeInfo->fwInfoNum;
    int idx = 0;
    for (idx = 0; idx < fileNum; idx++) {
        int fileSize = atoi(pUpgradeInfo->fwInfoList[idx].pFileSize);
        totalSize += fileSize;
    }

    Log("total file downloadSize[%d]\n", totalSize);
    return totalSize;
}

int initProgressData(CURL* pCurlHandle, UPGRADE_INFO_T* pUpgradeInfo, IMG_FILE_INFO_T* pDwnInfo)
{
    sProgInfo.curl = pCurlHandle;
    sProgInfo.lastruntime = 0;

    if ((pUpgradeInfo == NULL) || (pDwnInfo == NULL)) {
        return SUSC_RES__GENERAL_FAIL;
    }

    int pktUnitSize = pDwnInfo->pktInfo.unitSize;

    sChunk.pktDwInfo.pRecvBuffer = malloc(pktUnitSize);
    if (sChunk.pktDwInfo.pRecvBuffer == NULL) {
        Log("Error! Memory allocation for buffer is failed\n");

        return SUSC_RES__GENERAL_FAIL;
    }

    sChunk.pktDwInfo.pPrivData = pDwnInfo->pktInfo.pPrivData;
    sChunk.pktDwInfo.pktUnitSize = pktUnitSize;
    REPORT_UNIT_SIZE = ((long) pktUnitSize);

    sChunk.downloadSize = 0;
    sChunk.totalDownloadSize = 0;

    setReportedDownloadSize(0);

    long totalFileSize = calc_total_filesize(pUpgradeInfo);
    setTotalFileSize(totalFileSize);

    sStartOffset = 0;
    sTotalDownloadedSize = 0;
    return SUSC_RES__SUCCESS;
}

void cleanup_download_buffer() {
    Log("cleanup downloading memory buffer.....!!!!\n");

    if(sChunk.pktDwInfo.pRecvBuffer != NULL) {
        free(sChunk.pktDwInfo.pRecvBuffer);
    }
}

int downloadFwFileInPkt(UPGRADE_INFO_T* pUpgradeInfo, IMG_FILE_INFO_T* pImgDataInfo, long startOffset)
{
    CURL *curl;
    CURLcode res = CURLE_COULDNT_CONNECT;
    int result = UPGRADE_ERR__GENERAL;

    if((pUpgradeInfo == NULL)
            || (pImgDataInfo == NULL)) {
        return UPGRADE_ERR__GENERAL;
    }

    curl = curl_easy_init();

    if(curl) {

        int unitSize = pImgDataInfo->pktInfo.unitSize;

        Log("#####START DOWNLOADING IN PKT (%d)#########\n", sRetryCount);
        Log("Pkt Frame Size[%d], Start Offset[%ld]\n", unitSize, startOffset);

        if (startOffset == 0) {
                if(initProgressData(curl, pUpgradeInfo, pImgDataInfo) != SUSC_RES__SUCCESS) {
                    return UPGRADE_ERR__GENERAL;
                }
        }

        FW_INFO_T upgradeInfo = pUpgradeInfo->fwInfoList[0];
        char * convertedFileUrl = convertFtpsUrlScheme(upgradeInfo.pFileUrl);

        Log("Origin Url[%s] ==> converted Url[%s]\n",
                upgradeInfo.pFileUrl, convertedFileUrl);

        curl_easy_setopt(curl, CURLOPT_URL, convertedFileUrl);

        curl_easy_setopt(curl, CURLOPT_RESUME_FROM, startOffset);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, dumpRecvedDataToPktMem);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &sChunk);
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, unitSize);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, FTP_SOCKET_TIMEOUT_SEC);
        curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 1L);
        curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, FTP_SOCKET_TIMEOUT_SEC);
        curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, reportDownloadProgress);
        curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &sProgInfo);

        res = curl_easy_perform(curl);

        curl_easy_cleanup(curl);

        if(CURLE_OK != res) {
            /* we failed */
            fprintf(stderr, "curl told us %d\n", res);

            result = retryDownloadFwFile(pUpgradeInfo, pImgDataInfo, getTotalDownloadedSize());
        } else {
            result = UPGRADE_ERR__NO_ERR;
        }
    }

    return result;
}

int retryDownloadFwFile(UPGRADE_INFO_T* pUpgradeInfo, IMG_FILE_INFO_T* pImgDataInfo, long startOffset) {
    int res = UPGRADE_ERR__NO_ERR;

    sleep(WAITING_TIME_FOR_NEXT_SESSIOIN);

    sRetryCount++;

    sStartOffset = startOffset;
    if (sRetryCount > MAX_RETRY_DOWNLOAD_COUNT) {
        cleanup_download_buffer();
        res = UPGRADE_ERR__GENERAL;
    } else {
        res = downloadFwFileInPkt(pUpgradeInfo, pImgDataInfo, startOffset);
    }

    return res;
}

