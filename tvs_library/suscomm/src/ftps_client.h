/*
 * ftps_client.h
 *
 *  Created on: Feb 5, 2015
 *      Author: jhseo
 */

#ifndef FTPS_CLIENT_H_
#define FTPS_CLIENT_H_

#include "curl/curl.h"
#include "operation_mgr.h"

static const char *	FTPS_PROTOCOL_SCHEME = "ftps://";
static const char * FTP_PROTOCOL_SCHEME	= "ftp://";
static const int MAX_URL_LEN	= 512;
static const int MAX_PROTOCOL_SCH_LEN =	7;

static const long FTPS_CONNECT_TIMEOUT_SEC = 10;

typedef struct download_progress {
    double lastruntime;
    CURL *curl;
} DOWNLOAD_PROGRESS_T;

typedef struct FILE_DW_DATA_S {
    FILE*           pDwonloadFile;
} FILE_DW_DATA_T;

typedef struct MEM_DW_DATA_S {
    char*           pRecvBuffer;
} MEM_DW_DATA_T;

typedef struct PKT_DW_DATA_S {
    char*           pRecvBuffer;
    size_t          pktUnitSize;
    void*           pPrivData;
} PKT_DW_DATA_T;

typedef struct DownloadStruct {
    PKT_DW_DATA_T   pktDwInfo;
    size_t          downloadSize;
    size_t          totalDownloadSize;
} DOWNLOAD_STRUCT_T;

int downloadFwFileInPkt(UPGRADE_INFO_T* pUpgradeInfo, IMG_FILE_INFO_T* pImgDataInfo, long startOffset);
int retryDownloadFwFile(UPGRADE_INFO_T* pUpgradeInfo, IMG_FILE_INFO_T* pImgDataInfo, long startOffset);
#endif /* FTPS_CLIENT_H_ */
