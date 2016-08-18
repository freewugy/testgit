/*
 * sus_comm.c
 *
 *  Created on: Jan 26, 2015
 *      Author: jhseo
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "sus_comm.h"
#include "operation_mgr.h"
#include "utils.h"
#include "sys_info.h"

const char * SUSCOMM_VERSION = "0.1.18";

extern  int loadServerInfo();

enum {
    B_FALSE = 0,
    B_TRUE = 1
};

////////////////////////////////////////////////////
// ProtoType Declaration
////////////////////////////////////////////////////
void* opeartionMgrForFwDownload(void* param);
DownloadListener sDownloadListenerCB = NULL;
////////////////////////////////////////////////////

pthread_t sOpMgrPid = 0;
int sAlreadyReported = 0;

void setAlreadyReported(int isReported) {
    sAlreadyReported = isReported;
}

int isAlreadyReported() {
    int isReported = B_FALSE;

    if (sAlreadyReported == B_FALSE) {
        isReported = B_FALSE;
    } else {
        isReported = B_TRUE;
    }

    return isReported;
}

int requestFwDownloadInPkt(void* pPrivData, int unitSize, DownloadListener downloadlistener, char* pFwVersion, UPGRADE_MODE_T mode)
{

    Log("\n=====================================");
    Log("\nSUS COMM library - Ver[%s]", SUSCOMM_VERSION);
    Log("\n=====================================\n");
    Log("called requestFwDownloadInPkt.., mode[%s]\n", (mode == UPGRADE_MODE__NORMAL)?"Normal":"Recovery");

    int err;
    void * result;
    IMG_FILE_INFO_T fileInfo;

    Log("Start requestFwDownload\n");
    setAlreadyReported(B_FALSE);

    loadServerInfo();

    sDownloadListenerCB = downloadlistener;

    if (pFwVersion != NULL) {
        fileInfo.pFwVersion = strdup(pFwVersion);
    } else {
        fileInfo.pFwVersion = NULL;
    }

    fileInfo.pktInfo.pPrivData = pPrivData;
    fileInfo.pktInfo.unitSize = unitSize;
    fileInfo.pktInfo.upgradeMode = mode;

    if (unitSize <= 0) {
        return SUSC_RES__GENERAL_FAIL;
    }

    err = pthread_create(&sOpMgrPid, NULL, opeartionMgrForFwDownload, (void *) &fileInfo);

    err = pthread_join(sOpMgrPid, &result);

    int reported = isAlreadyReported();
    if (reported == B_TRUE) {
        result = SUSC_RES__ALREADY_REPORTED;
        sOpMgrPid = 0;
        setAlreadyReported(B_FALSE);
    }

    Log("Finish requestFwDownloadInPkt\n");

    return (int) result;
}


int reportUpgradeResult(int result)
{
    int reportResult = reportUpgradeResultToSus(result);

    Log("called sendUpgradeResult..reportResult[%d]\n", reportResult);

    //1. save sus version info to susVersion.dat
    if(result == UPGRADE_ERR__NO_ERR) {
        saveSusVersion();
        saveGroupInfo();
    }

    setAlreadyReported(B_TRUE);

    if(sOpMgrPid != 0) {
#ifdef ENV_PC
        pthread_cancel(sOpMgrPid);
#else
        int status;
        if ( (status = pthread_kill(sOpMgrPid, SIGUSR1)) != 0)
        {
            Log("Error cancelling thread %d, error = %d\n", sOpMgrPid, status);
        } else {
            Log("Downloading thread is cancelled\n");
        }
#endif
    }

    //2. remove partialPatchNum.dat
    return reportResult;
}
