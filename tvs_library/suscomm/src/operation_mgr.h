/*
 * operation_mgr.h
 *
 *  Created on: Feb 6, 2015
 *      Author: jhseo
 */

#ifndef OPERATION_MGR_H_
#define OPERATION_MGR_H_

#include "msg_format.h"
#include "sus_comm.h"

typedef struct {
    void*           pPrivData;
    int             unitSize;
    UPGRADE_MODE_T  upgradeMode;
} PKT_INFO_T;

typedef struct {
    char*           pFwVersion;
    int *           pDataSize;
    PKT_INFO_T      pktInfo;
} IMG_FILE_INFO_T;


void saveUpgradeInfoForReport(UPGRADE_INFO_T* pUpgradeInfo);
void initCurl();
void destroyCurl();
void cleanupUpgradeInfo(UPGRADE_INFO_T * pUpgradeInfo);
int sendReportMsg(REPORT_TYPE_T msgType);
int reportDownloadStart();
int reportUpgradeResultToSus(int upgradeResult);
void* opeartionMgrForFwDownload(void* param);
int saveSusVersion();
void saveGroupInfo();


#endif /* OPERATION_MGR_H_ */
