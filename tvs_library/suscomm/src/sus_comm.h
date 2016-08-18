/*
 * sus_comm.h
 *
 *  Created on: Jan 26, 2015
 *      Author: jhseo
 */

#ifndef SUS_COMM_H_
#define SUS_COMM_H_

typedef enum {
    SUSC_RES__SUCCESS = 0,
    SUSC_RES__DOWNLOAD_FAIL,
    SUSC_RES__LATEST_FW,
    SUSC_RES__NO_FULL_PATCH_INFO,
    SUSC_RES__GENERAL_FAIL,
    SUSC_RES__ALREADY_REPORTED
} SUSC_RESULT_T;

typedef enum {
    UPGRADE_ERR__NO_ERR = 0,
    UPGRADE_ERR__GENERAL,
    UPGRADE_ERR__INVALID_ACCOUNT,
    UPGRADE_ERR__INVALID_URL
} UPGRADE_ERR_T;

typedef enum {
    UPGRADE_MODE__NORMAL = 0,
    UPGRADE_MODE__RECOVERY = 1,
} UPGRADE_MODE_T;

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*DownloadListener)(char* recvBuf, int dataSize, long curr, long total, void* pPrivData);

int requestFwDownloadInPkt(void* pPrivData, int unitSize, DownloadListener listener, char* pFwVersion, UPGRADE_MODE_T mode);
int reportUpgradeResult(int result);

#ifdef __cplusplus
}
#endif

#endif /* SUS_COMM_H_ */
