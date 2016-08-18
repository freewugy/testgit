/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: eschoi $
 * $LastChangedDate: 2015-03-06 09:20:34 +0900 (Fri, 06 Mar 2015) $
 * $LastChangedRevision: 1081 $
 * Description:
 * Note:
 *****************************************************************************/

#ifndef _SCAN_TYPES_H
#define _SCAN_TYPES_H

#include <stdint.h>
#include "SIVector.h"


typedef enum {
    eScanCmd_Idle = 0,
    eScanCmd_TUNE_START = 100,
    eScanCmd_CHCHANGE_BEFORE,
    eScanCmd_VERSION_CHANGED,

    eScanCmd_DVB_START = 200,
    eScanCmd_NIT_REQUEST,
    eScanCmd_NIT_RECEIVED,
    eScanCmd_SDT_REQUEST,
    eScanCmd_SDT_RECEIVED,
    eScanCmd_SDT_OTHER_REQUEST,
    eScanCmd_SDT_OTHER_RECEIVED,
    eScanCmd_EIT_PF_REQUEST,
    eScanCmd_EIT_PF_RECEIVED,
    eScanCmd_EIT_PF_OTHER_REQUEST,
    eScanCmd_EIT_PF_OTHER_RECEIVED,
    eScanCmd_EIT_SCH_REQUEST,
    eScanCmd_EIT_SCH_RECEIVED,
    eScanCmd_EIT_SCH_OTHER_REQUEST,
    eScanCmd_EIT_SCH_OTHER_RECEIVED,
    eScanCmd_TDT_REQUEST,
    eScanCmd_TDT_RECEIVED,
    eScanCmd_BAT_REQUEST,
    eScanCmd_BAT_RECEIVED,

    eScanCmd_PSIP_START = 300,
    eScanCmd_VCT_REQUEST,
    eScanCmd_VCT_RECEIVED,
    eScanCmd_MGT_REQUEST,
    eScanCmd_MGT_RECEIVED,
    eScanCmd_EIT_REQUEST,
    eScanCmd_EIT_RECEIVED,
    eScanCmd_CH_ETT_REQUEST,
    eScanCmd_ETT_REQUEST,
    eScanCmd_ETT_RECEIVED,

    eScanCmd_PSI_START = 400,
    eScanCmd_PAT_REQUEST,
    eScanCmd_PAT_RECEIVED,
    eScanCmd_PMT_REQUEST,
    eScanCmd_PMT_RECEIVED,
    eScanCmd_TimeOuted,

    eScanCmd_NIT_TEST_REQUEST,
    eScanCmd_Max
} ScanCmd;

typedef enum {
    ScanDataCmd_Idle = 0,
    ScanDataCmd_TuneParam,
    ScanDataCmd_PSI_Ready,
    ScanDataCmd_DVB_NIT,
    ScanDataCmd_DVB_NIT_CHMAP_READY,
    ScanDataCmd_DVB_NIT_TIMEOUT,
    ScanDataCmd_DVB_SDT,
    ScanDataCmd_DVB_SDT_COMPLETE,
    ScanDataCmd_DVB_EIT,
    ScanDataCmd_DVB_EIT_COMPLETE,
    ScanDataCmd_DVB_TDT,
    ScanDataCmd_DVB_BAT,
    ScanDataCmd_PSIP_TVCT,
    ScanDataCmd_PSIP_CVCT,
    ScanDataCmd_PSIP_MGT,
    ScanDataCmd_PSIP_EIT,
    ScanDataCmd_PSIP_EIT_COMPLETE,
    ScanDataCmd_PSIP_ETT,
    ScanDataCmd_SCAN_START,
    ScanDataCmd_SCAN_END,
    ScanDataCmd_SCAN_ERR_END,
    ScanDataCmd_SCAN_STOP,
    ScanDataCmd_SCAN_DEMUX_CLOSE,
    ScanDataCmd_CH_MAP_COMPLTE,
    ScanDataCmd_Max
} ScanDataCmd;

typedef struct {
    ScanCmd cmd;
    int iparam;
    uint8_t mask;
    void* param;
} ScanMsg;

typedef struct {
    ScanDataCmd cmd;
    int iparam;
    void* param;
} ScanDataMsg;

typedef struct
{
    int pid;
    uint8_t tid;
    uint16_t tidex;
    uint8_t version;
    void* pReceiver;
}MonitorMsg;

typedef struct
{
    int cmd;
    void *basefilter;
    void *filter;
}FilterDelMsg;

typedef struct
{
	int sectionFilterHandle;
    void* pReceiver;
    uint8_t *pBuff;
    int length;
    int pid;
}SectionMsg;

typedef struct {
    uint16_t pid;
    uint16_t prog;
} pmt_t;

typedef struct {
    SIVector<void*> mListFilter;
} FilterMdParam;

#endif // _SCAN_TYPES_H
