/*
 * operation_mgr.c
 *
 *  Created on: Jan 27, 2015
 *      Author: jhseo
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "curl/curl.h"

#include "sus_comm.h"
#include "json_format.h"
#include "msg_format.h"
#include "msg_decoder.h"
#include "msg_encoder.h"
#include "https_client.h"
#include "ftps_client.h"
#include "operation_mgr.h"
#include "sys_info.h"
#include "utils.h"


//#define ENV_PC 1

extern DownloadListener sDownloadListenerCB;

enum {
    TRUE = 0,
    FALSE = 1
};

const char * SW_TYPE__PARTIAL = "partial";

#if defined(ENV_PC)
const char * SUS_VERSION = "/home/jhseo/susVersion.dat";
const char * SUS_MESSAGE_FILE = "/home/jhseo/.lastSusMessage.tmp";
#else
const char * SUS_VERSION = "/data/lkn_home/config/susVersion.dat";
const char * SUS_MESSAGE_FILE = "/data/lkn_home/run/susc/.lastSusMessage.tmp";
#endif

UPGRADE_INFO_T* sUpgradeInfo = NULL;
REPORT_INFO_T sReportInfo;

int saveSusVersion() {

    FILE * pFp = fopen(SUS_VERSION, "w");
    if (pFp == NULL) {
	Log("Fail to open [%s]file in saveSusVersion()\n", SUS_VERSION);
        return FALSE;
    }

    if ((sReportInfo.pFullVer == NULL)
            || (sReportInfo.pPartialVer == NULL)) {
        fclose(pFp);

      Log("SUS version to save is invalid\n");
        return FALSE;
    }

    char line[300];
    memset(line, 0x00, sizeof(char)*300);

    sprintf(line, "FULL=%s\n", sReportInfo.pFullVer);
    fputs(line,pFp);

    memset(line, 0x00, sizeof(char)*300);

    sprintf(line, "PARTIAL=%s\n", sReportInfo.pPartialVer);
    fputs(line, pFp);
    fflush(pFp);
    fclose(pFp);

    Log("Save sus version to file[F:%s, P:%s]\n",
            sReportInfo.pFullVer,
            sReportInfo.pPartialVer);

    return TRUE;
}

void saveGroupInfo() {
    if((sUpgradeInfo != NULL) && (sUpgradeInfo->pGroupId != NULL)) {
        saveGroupId(sUpgradeInfo->pGroupId);

        Log("Saved Group id[%s]\n", sUpgradeInfo->pGroupId);
    } else {
        Log("Failed saving group id because of invalid data\n");
    }
}

void saveUpgradeInfoForReport(UPGRADE_INFO_T* pUpgradeInfo)
{

    if(pUpgradeInfo->fwInfoList[0].pFullVer != NULL) {
        sReportInfo.pFullVer = strdup(pUpgradeInfo->fwInfoList[0].pFullVer);
    }

    if(pUpgradeInfo->fwInfoList[0].pPartialVer != NULL) {
        sReportInfo.pPartialVer = strdup(pUpgradeInfo->fwInfoList[0].pPartialVer);
    }

    if(pUpgradeInfo->fwInfoList[0].pFileUrl != NULL) {
        sReportInfo.pFileUrl = strdup(pUpgradeInfo->fwInfoList[0].pFileUrl);
    }

//    showUpgradeInfoForReport();
}

void showUpgradeInfoForReport()
{
    Log("###########################\n");
    Log("> Full Ver[%s]\n", sReportInfo.pFullVer);
    Log("> Part Ver[%s]\n", sReportInfo.pPartialVer);
    Log("> File Url[%s]\n", sReportInfo.pFileUrl);
    Log("###########################\n");
}

void initCurl()
{
    curl_global_init(CURL_GLOBAL_ALL);
}

void destroyCurl()
{
    curl_global_cleanup();
}

void cleanupUpgradeInfo(UPGRADE_INFO_T * pUpgradeInfo)
{
    if(pUpgradeInfo == NULL) {
        return;
    }

    if(pUpgradeInfo->fwInfoList[0].pFileName != NULL) {
        free(pUpgradeInfo->fwInfoList[0].pFileName);
    }

    if(pUpgradeInfo->fwInfoList[0].pFilePath != NULL) {
        free(pUpgradeInfo->fwInfoList[0].pFilePath);
    }

    if(pUpgradeInfo->fwInfoList[0].pFileSize != NULL) {
        free(pUpgradeInfo->fwInfoList[0].pFileSize);
    }

    if(pUpgradeInfo->fwInfoList[0].pFileUrl != NULL) {
        free(pUpgradeInfo->fwInfoList[0].pFileUrl);
    }

    if(pUpgradeInfo->fwInfoList[0].pFullVer != NULL) {
        free(pUpgradeInfo->fwInfoList[0].pFullVer);
    }

    if(pUpgradeInfo->fwInfoList[0].pMd5 != NULL) {
        free(pUpgradeInfo->fwInfoList[0].pMd5);
    }

    if(pUpgradeInfo->fwInfoList[0].pMode != NULL) {
        free(pUpgradeInfo->fwInfoList[0].pMode);
    }

    if(pUpgradeInfo->fwInfoList[0].pPartialVer != NULL) {
        free(pUpgradeInfo->fwInfoList[0].pPartialVer);
    }

    if(pUpgradeInfo->fwInfoList[0].pSwGuide != NULL) {
        free(pUpgradeInfo->fwInfoList[0].pSwGuide);
    }

    if(pUpgradeInfo->fwInfoList[0].pSwId != NULL) {
        free(pUpgradeInfo->fwInfoList[0].pSwId);
    }

    if(pUpgradeInfo->fwInfoList[0].pSwType != NULL) {
        free(pUpgradeInfo->fwInfoList[0].pSwType);
    }
}

int sendReportMsg(REPORT_TYPE_T msgType)
{
    char * pReportMsg = (char *) makeSusReportMsg(msgType);
    if(pReportMsg == NULL) {
        Log("Failed to make SUS REPORT msg\n");
        return UPGRADE_ERR__GENERAL;
    }

    char * pRespMsg = NULL;
    requestSusMsg(pReportMsg, &pRespMsg);
    Log("Received Report Response Msg[%s]\n",
                (pRespMsg!=NULL)?pRespMsg:"NO MSG");

    return UPGRADE_ERR__NO_ERR;
}

int reportDownloadStart()
{
    int result = sendReportMsg(REPORT_TYPE__START);
    return result;
}

int reportUpgradeResultToSus(int upgradeResult)
{
    int result;

    initCurl();

    Log("##########################\n");
    Log("Try to sending report msg...result[%d]\n", upgradeResult);
    if(upgradeResult == UPGRADE_ERR__NO_ERR) {
        result = sendReportMsg(REPORT_TYPE__OK);
    } else {
        result = sendReportMsg(REPORT_TYPE__ERR);
    }

    destroyCurl();

    return result;
}

int isContainedFullPatch(UPGRADE_INFO_T * pUpgradeInfo) {

    if ((pUpgradeInfo == NULL)
            || (pUpgradeInfo->fwInfoNum <= 0)) {
        return FALSE;
    }

    int idx = 0;
    int infoNum = pUpgradeInfo->fwInfoNum;
    for (idx = 0; idx < infoNum; idx++) {
        FW_INFO_T* pInfo = &(pUpgradeInfo->fwInfoList[idx]);

        if (pInfo == NULL) {
            continue;
        }

        if (pInfo->pSwType != NULL) {
            toLowerString(pInfo->pSwType, strlen(pInfo->pSwType));

            if (strcmp(pInfo->pSwType, SW_TYPE__PARTIAL) == 0) {
                return FALSE;
            }
        }
    }

    return TRUE;
}

#ifndef ENV_PC
void thread_exit_handler(int sig)
{
    Log("exit current thread\n");
    pthread_exit(SUSC_RES__GENERAL_FAIL);
}
#endif

int isExistedUpgradeInfo() {
    int isExistedInfo = FALSE;

    struct stat st_info;

    if (stat(SUS_MESSAGE_FILE, &st_info) == 0) {
        if (st_info.st_size > 0) {
            isExistedInfo = TRUE;
        }
    }

    return isExistedInfo;
}

int readFromExistedSusMessage(char ** ppRespMsg) {

    Log("try to reading from existed sus message file...\n");

    FILE * pFp = fopen(SUS_MESSAGE_FILE, "r");
    if (pFp == NULL) {
        Log("Error! sus message file is not existed\n");
        return FALSE;
    }

    struct stat file_info;
    int fileSize = 0;
    if (stat(SUS_MESSAGE_FILE, &file_info) == 0) {
        fileSize = file_info.st_size;

        *ppRespMsg = malloc(fileSize + 1);
        if (*ppRespMsg == NULL) {
            if (pFp != NULL) {
                fclose(pFp);
            }

            Log("Error! memory allocation is failed\n");
            return FALSE;
        }

        memset(*ppRespMsg, 0x00, fileSize + 1);

        fread(*ppRespMsg, (fileSize+1), 1, pFp);

        Log("Read from existed sus msg[%s]\n", *ppRespMsg);
    }

    if (pFp != NULL) {
        fclose(pFp);
    }

    return TRUE;
}

int isRecoveryMode(UPGRADE_MODE_T mode) {
    if (mode == UPGRADE_MODE__RECOVERY) {
        return TRUE;
    }

    return FALSE;
}

void initReportInfo() {
    memset(&sReportInfo, 0x00, sizeof(REPORT_INFO_T));
    Log("Init report info data\n");
}

void* opeartionMgrForFwDownload(void* param)
{
    int result = SUSC_RES__GENERAL_FAIL;

    initReportInfo();
#ifdef ENV_PC
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
#else
    struct sigaction actions;
    memset(&actions, 0, sizeof(actions));
    sigemptyset(&actions.sa_mask);
    actions.sa_flags = 0;
    actions.sa_handler = thread_exit_handler;
    sigaction(SIGUSR1,&actions,NULL);
#endif

    Log("Operation Manager is created\n");

    IMG_FILE_INFO_T* pImgDataInfo = (IMG_FILE_INFO_T *) param;
    if (pImgDataInfo == NULL) {
        Log("Failed to make SUS Check msg because of NULL parameter\n");
        return ((void *)result);
    }

    initCurl();

    char * pRespMsg = NULL;

    if ((isExistedUpgradeInfo() == FALSE)
        || (readFromExistedSusMessage(&pRespMsg) == FALSE)
        || (isRecoveryMode(pImgDataInfo->pktInfo.upgradeMode) == TRUE)) {

        char * pCheckMsg = (char *) makeSusCheckMsg(pImgDataInfo);
        if(pCheckMsg == NULL) {
            Log("Failed to make SUS Check msg\n");
            return ((void *)result);
        }

        result = requestSusMsg(pCheckMsg, &pRespMsg);
        Log(">> Received CHECK Result[%d], Resp Msg[%s]\n", result, pRespMsg);

        if (pCheckMsg != NULL) {
            free(pCheckMsg);
            pCheckMsg = NULL;
        }

        if (result != CURLE_OK) {
            return ((void *)SUSC_RES__GENERAL_FAIL);
        }
    }

    int parseResult = JSON_RET_FAIL;
//    UPGRADE_INFO_T * pUpgradeInfo = NULL;

    if(pRespMsg != NULL) {
        parseResult = parseCheckRespMsg(pRespMsg, &sUpgradeInfo);

        if (pRespMsg != NULL) {
            free(pRespMsg);
            pRespMsg = NULL;
        }

        if(isContainedFullPatch(sUpgradeInfo) == FALSE) {
            Log(">> NO FULL PATCH INFO IN CHECK RESPONSE...\n");

            return ((void *) SUSC_RES__LATEST_FW);
        }
    }


    if((parseResult == JSON_RET_SUCCESS)
            && (sUpgradeInfo != NULL)
            && (sUpgradeInfo->fwInfoNum > 0)) {

        saveUpgradeInfoForReport(sUpgradeInfo);

        reportDownloadStart();

        Log("Trying to downloading file\n");
        int downloadResult = downloadFwFileInPkt(sUpgradeInfo, pImgDataInfo, 0);

        if (downloadResult == UPGRADE_ERR__NO_ERR) {
            result = SUSC_RES__SUCCESS;
        } else {
            result = SUSC_RES__DOWNLOAD_FAIL;
        }

        Log("Finish downloading file...Result[%d]\n", result);
    }

    if (sUpgradeInfo->fwInfoNum == 0) {
        result = SUSC_RES__LATEST_FW;
        Log("Current fw is latest fw version\n");
    }

    destroyCurl();

    return ((void *) result);
}

