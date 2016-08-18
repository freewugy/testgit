/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: loneowl $
 * $LastChangedDate: 2014-05-08 10:00:31 +0900 (Thu, 08 May 2014) $
 * $LastChangedRevision: 740 $
 * Description:
 * Note:
 *****************************************************************************/

#ifndef SCANMANAGER_H
#define SCANMANAGER_H

#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <time.h>
#include <memory.h>

#include <list>
#include <map>
#include <string>

#include "SIManager.h"
#include "TDI.h"
#include "SIQ.h"
#include "SIVector.h"

#include "TableFilter.h"
#include "SimpleFilter.h"
#include "MonitorFilter.h"
#include "ServiceInfo.h"
#include <list>
#include "MMF/MMFInterface.h"

typedef enum {
    eScanMode_OOB,
    eScanMode_SemiOOB,
    eScanMode_Add_PSI,
    eScanMode_NIT_PSI,
    eScanMode_Add_FullSI,
    eScanMode_Normal_InBand,
} SCAN_MODE;

typedef enum {
    eIdle,
    eFirstConnect,
    eInScanning,
    eInUpdate,
    eInScanEnd,
} SCAN_MANAGER_MODE;

typedef enum {
    eEIT_NoOp,
    eEIT_Standby,
    eEIT_Requested,
    eEIT_Received,
} EIT_MODE;

typedef enum _DeliveryType {
    eDeliveryTypeUnknown = 0,
    eDeliveryTypeT,
    eDeliveryTypeC,
    eDeliveryTypeS,
    eDeliveryTypeT2,
    eDeliveryTypeC2,
    eDeliveryTypeS2,
    eDeliveryTypeInBand,
} DeliveryType;

typedef struct _DeliveryC {
    uint32_t freq;
    uint8_t fec_outer;
    TDI_TunerProtocol modulation;
    uint32_t symbol_rate;
    uint8_t fec_inner;
} DeliveryC;

typedef struct _DeliveryC2 {
    uint8_t pip_id;
    uint8_t data_slice_id;
    uint32_t freq;
    uint8_t tuning_freq_type;
    uint8_t active_OFDM_symbol_duration;
    uint8_t guard_interval;
} DeliveryC2;

typedef struct _DeliveryT {
    uint32_t freq;
    uint8_t bandwidth;
    uint8_t priority;
    uint8_t time_slicing_indicator;
    uint8_t mpe_fec_indicator;
    TDI_TunerProtocol constellation;
    uint8_t hierarchy_info;
    uint8_t coderate_HP;
    uint8_t coderate_LP;
    uint8_t guardinterval;
    uint8_t transmissionMode;
    uint8_t otherfreqflag;
} DeliveryT;

typedef struct _DeliveryT2 {
    uint8_t plp_id;
    uint16_t t2_system_id;

} DeliveryT2;

typedef struct _DeliveryS {
    uint32_t freq;
    uint16_t orbitalPosition;
    uint8_t westEastFlag;
    uint8_t polarization;
    uint8_t rolloff;
    uint8_t modulationSystem;
    uint8_t modulationType;
    uint32_t symbolRate;
    uint8_t fec_inner;
} DeliveryS;

typedef struct _DeliveryS2 {
    uint8_t scrambling_sequence_selector;
    uint8_t multiple_input_stream_flag;
    uint8_t backward_compatibility_indicator;
    uint32_t scrambling_sequence_index;
    uint8_t input_stream_identifier;
} DeliveryS2;

typedef struct _DeliveryInband {
    uint8_t modulation;
    uint32_t freq;
    uint32_t symbolrate;
    uint32_t bandwidth;
} DeliveryInBand;

typedef struct _ES_INFOS_ {
    int pid;
    int stream_type;
    int langcode;
} ES_INFO;

typedef struct _EVT_INFOS_ {
    uint16_t pid;
    uint16_t evtid;
    uint8_t etm_location;
} EVT_INFO;

typedef struct SERVICE {
    uint16_t sid;
    uint8_t service_type;

    uint8_t eit_pf_version;
    uint8_t eit_sch_version[16];

    bool eit_pf_flag;
    bool eit_schedule_flag;
    EIT_MODE eit_pf_mode;
    EIT_MODE eit_schedule_mode[16];
    uint8_t eit_last_tableId;

    uint16_t pmt_pid;
    uint8_t pmt_version;

    //psip
    uint16_t sourceID;
    uint8_t channel_etm_location;
    SIVector<EVT_INFO*> PsipEvtList;

    uint16_t pcr_id;
    SIVector<ES_INFO*> esList;
} ScanService;

typedef struct _mgt_item_ {
    uint16_t table_type;
    uint16_t pid;
    uint8_t version;
} mgt_item;

typedef struct TSINFO {
    uint16_t tsid;
    DeliveryType deliveryType;
    union {
        DeliveryT terr;
        DeliveryC cable;
        DeliveryS sat;
        DeliveryT2 terr2;
        DeliveryC2 cable2;
        DeliveryS2 sat2;
        DeliveryInBand inbnd;
    } deliverySystem;
    uint8_t sdt_version;
    SIVector<ScanService*> serviceList;
    bool scanned;
    bool scantried;
    int freqKHz;

    //psip
    uint8_t mgt_version;
    SIVector<mgt_item*> mgtItemList;

    bool bLocked;
} ScanTransport;

//scan class
class ScanManager {
public:
    virtual ~ScanManager();

    static ScanManager& getInstance()
    {
        static ScanManager instance;
        return instance;
    }

    int scanTsCnt;
    bool bTunerLocked;
    time_t mLastOpSec;
    int modify;
    std::string msoName;

    int getPAT();
    int getPMT(int pmtid, int prog);
    int getNIT();
    int getNITwithID(int nwid);
    int getSDT();
    int getSDTOther(uint16_t tsid);
    int getBAT();
    int getEITDVB(int tid, int sid, uint8_t mask, int modify, int tsid = 0);
    void requestEITBoost(ScanTransport* pTs);
    int getTDT();
    int getVCT();
    int getMGT();

    void tune_result(int tune_event, int freq, bool bScanMode);
    void receiveNIT(void* filter);
    void receiveSDT(void* filter);
    void receiveSDTOther(void* filter);
    void receiveEITPF(void* filter);
    void receiveEITPFOther(void* filter);
    void receiveEITSchedule(void* filter);
    void receiveEITScheduleOther(void* filter);
    void receiveTDT(void* filterhandle);

    void receiveBAT(void* filter);
    void receiveVCT(void* filter);
    void receivePAT(void* filter);
    void receivePMT(void* filter);

    void start_DVB();
    void monitor_DVBService();
    void start_PSIP();
    void start_PSI();
    void requestTuneNext();
    void FirstConnectTuneFailHandler();
    ScanTransport* getNextTs();
    SCAN_MANAGER_MODE getMode()
    {
        return mManagerMode;
    }
    SCAN_MODE getScanType()
    {
        return mScanMode;
    }
    int getcurrentTsid()
    {
        return current_tsid;
    }
    int getNwid()
    {
        return current_nwid;
    }
    bool isDVB();
    void TimeOut();
    void ResetEitFilterCount();
    void RestartByNitChange();
    void resetEitMode();

    void initialize(std::string provider);
    void setTuneParam(EPGProviderUri uri);
    void setWepgChannelMapFile(string wepgChannelMapFileURL);
    void scanStart();
    void scanStop();
    void start();
    void stop();
    void channelChangeBefore();
    void channelChangeAfter(int freq);
    void TuneHomeTS();

    int pmt_count;
    int sdt_count;
    int bat_count;
    int eit_pf_count;
    int eit_sch_count;

    bool bNeedScan;
    bool bUsePSI;
    bool bUseDVB;
    bool bUsePSIP;
    bool isTDTTimeSet;

private:
    bool bUseOtherTS;
    bool bReceiveEITInScan;
    bool bEITBoost;
    bool psiCompleteInChannel;
    bool dvbCompleteInChannel;
    int max_receive_eit_idx;
    SCAN_MODE mScanMode;
    SCAN_MANAGER_MODE mManagerMode;

    bool bUsePSIBackup;
    bool bUseDVBBackup;
    bool bUsePSIPBackup;
    bool bUseOtherTSBackup;
    bool bReceiveEITInScanBackup;
    bool bEITBoostBackup;
    SCAN_MODE mScanModeBackup;

    bool ts_map_ready;
    bool set_current_ts;
    bool sdt_other_requested;
    bool request_tune_ap;

    uint16_t current_nwid;
    uint16_t current_tsid;
    int scan_tunerId;

    TDI_TunerSettings* scan_tuner;
    TDI_TunerSettings scan_tuner_ap;

    string mWepgChannelMapFileURL;

    ScanManager()
    {
    };

    ScanManager(ScanManager const&); // Don't Implement
    void operator=(ScanManager const&); // Don't implement

    void makeTSIDOtherList();
    void configSIScan();

    void change2ScanMode();
    void change2UpdateMode();
    void DVBTimeOut();

public:
    int frequencyStartHz;
    int frequencyEndHz;
    int bandwidthkHz;
    bool invertTuner;
    bool bTestMode;
    bool bFixedModulation;
    int TestNwId;
    int timeoutInSec;
    EPGProviderUri epg_uri;
    bool bDummyEpgMode;
//    string scan_uri;
//    string psi_uri;
//    void start_Tune();
    bool getDummyEpgMode() {
    	return bDummyEpgMode;
    };

    void setDummyEpgMode(bool mode) {
       	bDummyEpgMode = mode;
   };
    void setChMapNotify();
    void sendChannelComplete();
    void setManualFreq(int Freq);
    string getUri(TDI_TunerSettings* scan_tuner,int mode);
    EPGProviderUri convertUri(TDI_TunerSettings* scan_tuner);
    int getUriInfo(string uri, string target);
    void setAutoFreq(int startFreq, int endFreq);
    void scanStart(bool bAuto, bool bFixedMod);
    void scanStartManualChannel(int networkId);
    void cleanupFactoryTest();
    void setTunerInvert(bool invert)
    {
        invertTuner = invert;
    }
    std::list<ServiceInfo*> getChList();
    ServiceInfo* makeServiceInfo(ScanTransport* pTs, ScanService* pSer);
};

#endif // SCANMANAGER_H
