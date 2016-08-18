/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: eschoi $
 * $LastChangedDate: 2015-03-26 05:06:02 +0900 (Thu, 26 Mar 2015) $
 * $LastChangedRevision: 1096 $
 * Description:
 * Note:
 *****************************************************************************/

#include "ScanManager.h"

//#include <cstdbool>
#include <cstdlib>
#include <cstring>

#include "../include/Element.h"
#include "../include/SIChangeEvent.h"
#include "../Parser/Parser.h"
#include "../util/BitStream.h"
#include "../util/ByteStream.h"
#include "../util/Logger.h"
#include "../util/StringUtil.h"
#include "../util/tinyxml/tinyxml.h"
#include "../util/Timer.h"
#include "ScanTypes.h"
#include "Section.h"
#include "Table.h"
#include "TDI.h"
#include "MMF/MMFInterface.h"

//#define __DEBUG_LOG_SM__

#define SCANMANAGER_AGING_TEST  0
void start_si_aging();

static time_t SCAN_MANAGER_TIMEOUT_SEC = 50;
static time_t AUTO_MANUAL_SCAN_TIMEOUT_SEC = 5;
static time_t MANUAL_CHANNEL_SCAN_TIMEOUT_SEC = 15;

static bool bBlockSectionMessage = true;

using namespace std;

SIQ<ScanMsg> ScanManageQ;
SIQ<ScanDataMsg> ScanDataQ;
SIQ<ScanDataMsg> ScanDataWaitQ;
SIQ<pmt_t> PMTPIDQ;
SIQ<uint16_t> OtherTSIDQ;

#ifdef ANDROID
#include <pthread.h>
#endif

#define TDI_INVALID_TASK_ID 0

static const char* TAG = "ScanManager";

static uint8_t NitVer = 0xff;
static bool bScanManagerWork = false;
static bool bBoot = false;
static bool bChMapNoti = false;
static MMF_DemuxHandle mDmxHandle = 0x00;
static bool bcreatedDMX = false;
static bool bHomets = false;
static bool bsetForceDMXDestroy = false;
static bool bfirstTunerLock = false;
static bool bScanCompleted = false;
static bool bSendChannelReadyNotify = false;
string current_uri;

SIVector<ScanTransport*> ListTs;
SIVector<void*> Service_Filter_List;
SIVector<void*> Service_Section_Filter_List;
SIVector<MonitorFilter*> monitorList;
ScanTransport* pCurrentTs = NULL;
int gCurrentChannelNum = 0;
/*	------------------------------------------
 G L O B A L   V A R I A B L E S
 ------------------------------------------ */
#ifdef ANDROID
pthread_mutex_t mScanManagerMutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
pthread_mutex_t mScanDataMutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
//pthread_mutex_t startMutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
#else    
pthread_mutex_t mScanManagerMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mScanDataMutex = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_t startMutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
#endif

class RecursiveMutex
{
 public:

	class Autolock
	{
	public:
	Autolock(RecursiveMutex &aMutex) : mM(&aMutex)
		{
			mM->lock();
		}

        ~Autolock()
        {
			mM->unlock();
        }

		RecursiveMutex *mM;
	};

	RecursiveMutex()
	{
		pthread_mutexattr_init(&mAttr);
		pthread_mutexattr_settype(&mAttr, PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init(&mM, &mAttr);
	}

	~RecursiveMutex()
	{
		pthread_mutex_destroy(&mM);
	//	pthread_cond_destroy(&mCond);
	}

	void lock()
	{
		pthread_mutex_lock(&mM);
	}

	void unlock()
	{
	    pthread_mutex_unlock(&mM);
	}

 private:
	pthread_mutex_t mM;
	pthread_mutexattr_t mAttr;
};

RecursiveMutex startMutex;
RecursiveMutex scaanManagerMutex;
RecursiveMutex scanDataMutex;


void ScanManagerLock()
{
    pthread_mutex_lock(&mScanManagerMutex);
}
void ScanManagerUnlock()
{
    pthread_mutex_unlock(&mScanManagerMutex);
}
void ScanDataQLock()
{
    pthread_mutex_lock(&mScanDataMutex);
}
void ScanDataQUnlock()
{
    pthread_mutex_unlock(&mScanDataMutex);
}
void SendScanManageQ(ScanMsg msg, bool bForce = false)
{
	bool isScanManagerWork = false;
    {
		RecursiveMutex::Autolock lock(startMutex);
    	isScanManagerWork = bScanManagerWork;
    }

    if(isScanManagerWork || bForce) {
        ScanManageQ.enqueue(msg);
    }
}
void SendScanDataQ(ScanDataMsg msg, bool wait = false)
{
    if(wait) {
        ScanDataWaitQ.enqueue(msg);
    } else {
        ScanDataQ.enqueue(msg);
    }
}

/*	------------------------------------------
 L O C A L   V A R I A B L E S
 ------------------------------------------ */
ScanTransport* getTransportStreamInfo(uint16_t tsid)
{
    int sizeN = ListTs.size(), res;
    for (int i = 0; i < sizeN; i++) {
        res = 0;
        ScanTransport* pTs = ListTs.At(i, res);
        if(res && pTs->tsid == tsid) {
            return pTs;
        }
    }
    return NULL;
}
ScanTransport* getTransportStreamInfoFreq(int freq)
{
    int sizeN = ListTs.size(), res;
    for (int idx = 0; idx < sizeN; idx++) {
        res = 0;
        ScanTransport* pTs = ListTs.At(idx, res);
        if(res && pTs->freqKHz == freq) {
            return pTs;
        }
    }
    return NULL;
}
ScanService* getServiceInfo(uint16_t tsid, uint16_t sid)
{
    ScanTransport* pTs = getTransportStreamInfo(tsid);
    if(pTs) {
        int sizeN = pTs->serviceList.sizeNoLock();
        int res;
        for (int itr = 0; itr < sizeN; itr++) {
            res = 0;
            ScanService* service = pTs->serviceList.AtNoLock(itr, res);
            if(res && service->sid == sid) {
                return service;
            }
        }
    }
    return NULL;
}

ScanService* getServiceInfo(ScanTransport* pTs, uint16_t sid)
{
    if(pTs) {
        int sizeN = pTs->serviceList.sizeNoLock();
        int res = 0;
        for (int itr = 0; itr < sizeN; itr++) {
            res = 0;
            ScanService* service = pTs->serviceList.AtNoLock(itr, res);
            if(res && service->sid == sid) {
                return service;
            }
        }
    }
    return NULL;
}
ScanService* getServiceInfo(uint16_t sid)
{
    int sizeN = ListTs.size(), res;
    for (int idx = 0; idx < sizeN; idx++) {
        res = 0;
        ScanTransport* pTs = ListTs.At(idx, res);
        if(res) {
            ScanService* scanService = getServiceInfo(pTs, sid);
            if(scanService) {
                return scanService;
            }
        }
    }
    return NULL;
}

ScanService* getServiceInfoSourceID(ScanTransport* pTs, uint16_t sid)
{
    if(pTs) {
        int res = 0;
        int sizeN = pTs->serviceList.sizeNoLock();
        for (int itr = 0; itr < sizeN; itr++) {
            res = 0;
            ScanService* service = pTs->serviceList.AtNoLock(itr, res);
            if(service->sourceID == sid) {
                return service;
            }
        }
    }
    return NULL;
}

uint16_t getETTPID(ScanTransport* pTs, uint16_t evtpid)
{
    //find event table type
    uint16_t tabletype = 0;
    int res = 0, sizeN;
    sizeN = pTs->mgtItemList.sizeNoLock();
    for (int idx = 0; idx < sizeN; idx++) {
        res = 0;
        mgt_item* itm = pTs->mgtItemList.AtNoLock(idx, res);
        if(res && itm->pid == evtpid) {
            tabletype = itm->table_type;
            break;
        }
    }
    if(tabletype != 0) {
        tabletype = tabletype + 0x100;
        for (int idx = 0; idx < sizeN; idx++) {
            mgt_item* itm = pTs->mgtItemList.AtNoLock(idx, res);
            if(res && itm->table_type == tabletype) {
                return itm->pid;
            }
        }
    }
    return 0;
}

void init_ts(ScanTransport* pTs)
{
    if(pTs) {
        pTs->tsid = 0;
        pTs->deliveryType = eDeliveryTypeUnknown;
        pTs->sdt_version = 0xff;
        pTs->scanned = false;
        pTs->scantried = false;
        pTs->mgt_version = 0xff;
        pTs->freqKHz = -1;
    }
}
void refresh_ts(ScanTransport* pTs)
{
    L_DEBUG(TAG, ">> begin\n");
    int res;
    if(pTs) {
        int sizeSer = pTs->serviceList.sizeNoLock();
        for (int serviceItr = 0; serviceItr < sizeSer; serviceItr++) {
            res = 0;
            ScanService* pScanService = pTs->serviceList.AtNoLock(serviceItr, res);
            if(res) {
                int sizeEvt = pScanService->PsipEvtList.sizeNoLock();
                for (int evtitr = 0; evtitr < sizeEvt; evtitr++) {
                    res = 0;
                    EVT_INFO* info = pScanService->PsipEvtList.AtNoLock(evtitr, res);
                    if(res)
                        delete info;
                }
                pScanService->PsipEvtList.clearNoLock();

                int esSize = pScanService->esList.sizeNoLock();
                for (int esloop = 0; esloop < esSize; esloop++) {
                    res = 0;
                    ES_INFO* info = pScanService->esList.AtNoLock(esloop, res);
                    if(res) {
                        delete info;
                    }
                }
                //L_INFO(TAG, "[del]pService tsid=0x%X sid=0x%X\n",pTs->tsid,pScanService->sid);
                pScanService->esList.clearNoLock();
                delete pScanService;
            } else if(!res) {
                //L_INFO(TAG, "\n\nERROR res\n\n\n");
            }
        }
        pTs->serviceList.clear();
    }
    L_DEBUG(TAG, "<< end\n");
}

void init_service(ScanService *pScanService)
{
    if(pScanService) {
        pScanService->sid = 0;
        pScanService->eit_pf_flag = false;
        pScanService->eit_schedule_flag = false;
        pScanService->eit_pf_mode = eEIT_NoOp;
        pScanService->eit_pf_version = 0xff;
        for (int i = 0; i < 16; i++) {
            pScanService->eit_sch_version[i] = 0xff;
            pScanService->eit_schedule_mode[i] = eEIT_NoOp;
        }
        pScanService->pmt_version = 0xff;
        pScanService->eit_last_tableId = 0;
        pScanService->pmt_pid = 0;

        pScanService->sourceID = 0;
        pScanService->channel_etm_location = 0;
    }
}

ScanTransport* MakeTsList(int start, int end, int sr, int bw, int demod)
{
    bool bfind = false;
    ScanTransport* retTs = NULL;
    int lowfreq;
//        for (lowfreq = start; lowfreq + bw <= 82000; lowfreq += bw) { // for test wjpark
    for (lowfreq = start; lowfreq + bw <= end; lowfreq += bw) {
        ScanTransport* pTs = new ScanTransport;
        if(pTs) {
            init_ts(pTs);
            if(bfind == false) {
                retTs = pTs;
                bfind = true;
            }
            pTs->deliveryType = eDeliveryTypeInBand;
            pTs->deliverySystem.inbnd.modulation = demod;
            pTs->deliverySystem.inbnd.freq = (lowfreq + bw / 2) * 1000;
            pTs->deliverySystem.inbnd.symbolrate = sr;
            pTs->deliverySystem.inbnd.bandwidth = bw / 1000;
            L_DEBUG(TAG,"freq [%d]\n",            pTs->deliverySystem.inbnd.freq);
            ListTs.push_back(pTs);
        }
    }
    L_INFO(TAG, "TS List Size=%d\n", ListTs.size());
    return retTs;
}

void clearTSList()
{
    NitVer = 0xFF;
    RecursiveMutex::Autolock lock(startMutex);
    ListTs.lock();

    int sizeN = ListTs.sizeNoLock(), res;
    for (int idx = 0; idx < sizeN; idx++) {
        res = 0;
        ScanTransport* pTs = ListTs.AtNoLock(idx, res);

        if(res) {
            int sizeSer = pTs->serviceList.sizeNoLock();
            for (int serviceItr = 0; serviceItr < sizeSer; serviceItr++) {
                res = 0;
                ScanService* pScanService = pTs->serviceList.AtNoLock(serviceItr, res);
                ScanManagerLock();
                if(res) {
                    int sizeEvt = pScanService->PsipEvtList.sizeNoLock();
                    for (int evtitr = 0; evtitr < sizeEvt; evtitr++) {
                        res = 0;
                        EVT_INFO* info = pScanService->PsipEvtList.AtNoLock(evtitr, res);
                        if(res)
                            delete info;
                    }
                    pScanService->PsipEvtList.clearNoLock();

                    int esSize = pScanService->esList.sizeNoLock();
                    for (int esloop = 0; esloop < esSize; esloop++) {
                        res = 0;
                        ES_INFO* info = pScanService->esList.AtNoLock(esloop, res);
                        if(res) {
                            delete info;
                        }
                    }
                    pScanService->esList.clearNoLock();
                    delete pScanService;
                }
                ScanManagerUnlock();
            }
            pTs->serviceList.clearNoLock();

            int mgtsize = pTs->mgtItemList.sizeNoLock();
            for (int mgtitr = 0; mgtitr < mgtsize; mgtitr++) {
                res = 0;
                mgt_item* itm = pTs->mgtItemList.AtNoLock(mgtitr, res);
                if(res)
                    delete itm;
            }
            pTs->mgtItemList.clearNoLock();

            delete pTs;
        }
    }
    ListTs.clearNoLock();
    ListTs.unlock();
    pCurrentTs = NULL;
}

void clearMonitorFilter()
{

    monitorList.lock();
    int sizeN = monitorList.sizeNoLock();
    int res = 0;
    L_DEBUG(TAG,"list size[%d]\n",sizeN);
    for (int monitorItr = sizeN - 1; monitorItr >= 0; monitorItr--) {
        res = 0;
        MonitorFilter* pFilter = monitorList.AtNoLock(monitorItr, res);
        if(res) {
            monitorList.eraseNoLock(pFilter);
            pFilter->stopFilter(); 
//            L_DEBUG(TAG,"\n");
            deleteMonitorFilter(pFilter);
        }
    }
    sizeN = monitorList.sizeNoLock();
    L_DEBUG(TAG,"list size[%d]\n",sizeN);
    monitorList.unlock();
}

void clearAllFilter_DVB()
{ 

    int res = 0;
    monitorList.lock();
    int sizeN = monitorList.sizeNoLock();
    for (int monitorItr = sizeN - 1; monitorItr >= 0; monitorItr--) {
        res = 0;
        MonitorFilter* pFilter = monitorList.AtNoLock(monitorItr, res);
        if(res && pFilter->m_pid == 0x12) {
            monitorList.eraseNoLock(pFilter);
            pFilter->stopFilter();
            L_DEBUG(TAG,"\n");
            deleteMonitorFilter(pFilter);
        }
    }
    monitorList.unlock();
}

ScanCmd getReceiveTableType(int pid, uint8_t tid);

void table_cb(int pid, int tid, int tidex, void* pReceiver)
{
    if(bBlockSectionMessage) 
        return;
        
    ScanMsg msg;
    msg.cmd = getReceiveTableType(pid, tid);
    msg.param = pReceiver;
    msg.iparam = tidex;
    SendScanManageQ(msg);
}

void section_cb(int pid, int tid, int tidex, void* pReceiver)
{
    if(bBlockSectionMessage) 
        return;
        
    ScanMsg msg;
    msg.cmd = getReceiveTableType(pid, tid);
    msg.param = pReceiver;
    msg.iparam = tidex;
    SendScanManageQ(msg);
}

void monitor_cb(void* pReceiver)
{
    if(bBlockSectionMessage) 
        return;
        
    ScanMsg msg;
    msg.cmd = eScanCmd_VERSION_CHANGED;
    msg.param = pReceiver;
    SendScanManageQ(msg);
}

ScanCmd getReceiveTableType(int pid, uint8_t tid)
{
	L_DEBUG(TAG,"pid = %d tid = %d",pid, tid);
    if(pid == 0 && tid == 0) { //PAT
        return eScanCmd_PAT_RECEIVED;
    }
    //DVB
    else if(pid == 0x10 && tid == 0x40) { //nit
        return eScanCmd_NIT_RECEIVED;
    } else if(pid == 0x10 && tid == 0x41) { //nit other
    } else if(pid == 0x11 && tid == 0x42) { //sdt
        return eScanCmd_SDT_RECEIVED;
    } else if(pid == 0x11 && tid == 0x46) { //sdt other
        return eScanCmd_SDT_OTHER_RECEIVED;
    } else if(pid == 0x11 && tid == 0x4a) { //bat
        return eScanCmd_BAT_RECEIVED;
    } else if(pid == 0x12 && tid == 0x4e) { //eit pf
        return eScanCmd_EIT_PF_RECEIVED;
    } else if(pid == 0x12 && tid == 0x4f) { //eit pf other
        return eScanCmd_EIT_PF_OTHER_RECEIVED;
    } else if(pid == 0x12 && (tid >= 0x50 && tid <= 0x5f)) { //eit schedule
        return eScanCmd_EIT_SCH_RECEIVED;
    } else if(pid == 0x12 && (tid >= 0x60 && tid <= 0x6f)) { //eit schedule other
        return eScanCmd_EIT_SCH_OTHER_RECEIVED;
    } else if(pid == 0x14 && tid == 0x70) { //tdt
        return eScanCmd_TDT_RECEIVED;
    }
    //PSIP
    else if(pid == 0x1ffb && tid == 0xc7) { //mgt
        return eScanCmd_MGT_RECEIVED;
    } else if(pid == 0x1ffb && tid == 0xc8) { //tvct
        return eScanCmd_VCT_RECEIVED;
    } else if(tid == 0xcb) { //eit
        return eScanCmd_EIT_RECEIVED;
    } else if(tid == 0xcc) { //ett
        return eScanCmd_ETT_RECEIVED;
    } else if(tid == 0x02) { //pmt
        return eScanCmd_PMT_RECEIVED;
    }

    return eScanCmd_Idle;
}

void ParsePAT(Table *pTable, uint16_t* tsid)
{
    ScanTransport* pTs;

    for (int i = 0; i < pTable->m_SectionCount; i++) {
        BitStream *bitBuff = new BitStream(pTable->m_pSection[i], pTable->m_pSectionLen[i], false);

        if(i == 0) {
            uint32_t dummy;
            bitBuff->skipByte(3); //table id ~ section length
            bitBuff->readBits(16, &dummy);
            pTs = getTransportStreamInfo(dummy);

            if(pTs == NULL) {
                pTs = new ScanTransport;
                init_ts(pTs);
                ListTs.push_back(pTs);
                pTs->tsid = dummy;
            }
            pCurrentTs = pTs;
            *tsid = dummy;
            bitBuff->skipByte(3); //reserved ~ last section number
        } else {
            bitBuff->skipByte(8);
        }

        while (bitBuff->getRemainBytes() > 4) {
            uint16_t prognum, pid;
            bitBuff->readBits(16, &prognum); //program number
            bitBuff->skipBits(3); //reserved

            if(prognum == 0) {
                bitBuff->skipBits(13); //network pid
            } else {
                bitBuff->readBits(13, &pid); //program map pid
                pmt_t pmt;
                pmt.pid = pid;
                pmt.prog = prognum;
                PMTPIDQ.enqueue(pmt);

                ScanService* pService = getServiceInfo(pTs, prognum);
                if(pService == NULL) {
                    pService = new ScanService;
                    init_service(pService);
                    pService->sid = prognum;
                    pTs->serviceList.push_back(pService);
                }
                pService->pmt_version = 0xFF;
                pService->pmt_pid = pid;
            }
        }

        delete bitBuff;
    }
}

void ParsePAT_Test(Table *pTable, uint16_t* tsid)
{
    ScanTransport* pTs;

    for (int i = 0; i < pTable->m_SectionCount; i++) {
        BitStream *bitBuff = new BitStream(pTable->m_pSection[i], pTable->m_pSectionLen[i], false);

        if(i == 0) {
            uint32_t dummy;
            bitBuff->skipByte(3); //table id ~ section length
            bitBuff->readBits(16, &dummy);
            pTs = pCurrentTs;
            if(pTs == NULL) {
                pTs = new ScanTransport;
                init_ts(pTs);
                ListTs.push_back(pTs);
            }
            pTs->tsid = dummy;
            *tsid = dummy;
            bitBuff->skipByte(3); //reserved ~ last section number
        } else {
            bitBuff->skipByte(8);
        }

        while (bitBuff->getRemainBytes() > 4) {
            uint16_t prognum, pid;
            bitBuff->readBits(16, &prognum); //program number
            bitBuff->skipBits(3); //reserved

            if(prognum == 0) {
                bitBuff->skipBits(13); //network pid
            } else {
                bitBuff->readBits(13, &pid); //program map pid
                pmt_t pmt;
                pmt.pid = pid;
                pmt.prog = prognum;
                PMTPIDQ.enqueue(pmt);

                ScanService* pService = getServiceInfo(pTs, prognum);
                if(pService == NULL) {
                    pService = new ScanService;
                    init_service(pService);
                    pService->sid = prognum;
                    pTs->serviceList.push_back(pService);
                }
                pService->pmt_version = 0xFF;
                pService->pmt_pid = pid;
            }
        }

        delete bitBuff;
    }
}

void ParsePMT(uint8_t *pBuffer, uint32_t size, int tsid, bool& changeSomething)
{

    changeSomething = true;
    uint16_t program_number; //PRPGRAM NUMBER
    uint16_t ppid; //pcr PID

    uint32_t prog_info_length;
    BitStream *bitBuff = new BitStream(pBuffer, size, false);

    bitBuff->skipByte(3); //Table id ~ section length
    bitBuff->readBits(16, &program_number); //program number
    ScanService* pService = getServiceInfo(tsid, program_number);
    if(pService == NULL) {
        ScanTransport* pTs = getTransportStreamInfo(tsid);
        if(pTs == NULL) {
            delete bitBuff;
            return;
        }
        pService = new ScanService;
        init_service(pService);
        pService->sid = program_number;
        pTs->serviceList.push_back(pService);
    }
    if(pService->pmt_version != Version(pBuffer)) {
        pService->pmt_version = Version(pBuffer);
        //changeSomething = true;
    } else {
        //changeSomething = false;
        delete bitBuff;
        return;
    }

    //Parser�??��?
    delete bitBuff;
    return;
}

void ParsePMT_Test(uint8_t *pBuffer, uint32_t size, int tsid, bool& changeSomething)
{
    uint16_t program_number; //PRPGRAM NUMBER
    uint16_t ppid; //pcr PID
    uint32_t prog_info_length;
    BitStream *bitBuff = new BitStream(pBuffer, size);

    bitBuff->skipByte(3); //Table id ~ section length
    bitBuff->readBits(16, &program_number); //program number
    bitBuff->skipByte(3); //reserved ~ last section number
    bitBuff->skipBits(3); //reserved
    bitBuff->readBits(13, &ppid); //pcr pid
    bitBuff->skipBits(4); //reserved
    bitBuff->readBits(12, &prog_info_length); //program info length
    if(prog_info_length > 0)
        bitBuff->skipByte(prog_info_length);

    ScanService* pService = getServiceInfo(pCurrentTs, program_number);

    if(pService == NULL) {
        pService = new ScanService;
        init_service(pService);
        pService->sid = program_number;

        pCurrentTs->serviceList.push_back(pService);
    }
    pService->pcr_id = ppid;
    pService->pmt_version = Version(pBuffer);
    pService->esList.clear();
    //ES Loop
    while (bitBuff->getRemainBytes() > 4) {
        uint32_t length, type, pid;
        int remain;
        bitBuff->readBits(8, &type); //stream type
        bitBuff->skipBits(3); //reserved
        bitBuff->readBits(13, &pid); //elementary pid
        bitBuff->skipBits(4); //reserved
        bitBuff->readBits(12, &length); //es info length

        bool ignore_es = false;

        if(type == 0x6)
            ignore_es = true;

        if(length > 0) {
            remain = bitBuff->getRemainBytes();
            //ES Descriptor Loop
            while (remain - (int) (bitBuff->getRemainBytes()) < (int) length) {
                uint8_t tag, desc_length;
                int int_tag, int_leng;
                bitBuff->readBits(8, &tag);
                bitBuff->readBits(8, &desc_length);
                int_tag = (int) tag;
                int_leng = (int) desc_length;
                //tag==0x05 : AC-3 Registration Descriptor(format_identifier=0x41432D33) by ATSC
                //tag==0x6a : AC-3 Descriptor by DVB
                //tag==0x7a : Enhanced AC-3 Descriptor by DVB
                //tag==0x81 : AC-3 Audio Descriptor by ATSC
                if(tag == 0x05 || tag == 0x6a || tag == 0x7a || tag == 0x81) {
                    ignore_es = false;
                }
                bitBuff->skipByte(int_leng);
            }
        }

        if(ignore_es == false) {
            ES_INFO* es = new ES_INFO;
            es->stream_type = type;
            es->pid = pid;
            pService->esList.push_back(es);
        }
    }

    delete bitBuff;
}

void ParseNIT(Table *pTable, uint16_t* nwid, bool& changeSomething)
{
	 L_INFO(TAG,"pTable->m_SectionCount [%d]\n",pTable->m_SectionCount);
    for (int i = 0; i < pTable->m_SectionCount; i++) {
        BitStream *bitBuff = new BitStream(pTable->m_pSection[i], pTable->m_pSectionLen[i], false);

        uint16_t nw_desc_len, ts_desc_len;

        if(i == 0) {
            bitBuff->skipByte(3); //table id ~ section length
            bitBuff->readBits(16, nwid); //network id
            bitBuff->skipByte(3); //reserved ~ last section number
            L_INFO(TAG, "NitVer = 0x%X\n", NitVer);
            if(NitVer != Version(pTable->m_pSection[0])) {
                NitVer = Version(pTable->m_pSection[0]);
                changeSomething = true;
            } else {
                changeSomething = false;
                delete bitBuff;
                return;
            }
            L_INFO(TAG, "NitVer = 0x%X\n", NitVer);
        } else {
            bitBuff->skipByte(8);
        }
        L_INFO(TAG,"changeSomething [%d]\n",changeSomething);
        bitBuff->skipBits(4); //reserved
        bitBuff->readBits(12, &nw_desc_len); //network descriptor length;
        bitBuff->skipByte(nw_desc_len + 2); //network descriptor ~ transport stream loop length
        //TS Loop
        while (bitBuff->getRemainBytes() > 4) {
            uint32_t dummy;

            bitBuff->readBits(16, &dummy); //transport stream id
            ScanTransport* pScanTransport = getTransportStreamInfo(dummy);
            if(pScanTransport == NULL) {
                pScanTransport = new ScanTransport;
                init_ts(pScanTransport);
//                L_INFO(TAG,"insert pScanTransport\n");
                ListTs.push_back(pScanTransport);

            }
            pScanTransport->tsid = dummy;
            bitBuff->skipBits(20); //original stream id~reserved future use
            bitBuff->readBits(12, &ts_desc_len); //transport descriptor length
            if(ts_desc_len > 0) {
                int remain = bitBuff->getRemainBytes();
                //ES Descriptor Loop
                while (remain - (int) (bitBuff->getRemainBytes()) < (int) ts_desc_len) {
                    uint8_t tag, desc_length;
                    bitBuff->readBits(8, &tag);
                    bitBuff->readBits(8, &desc_length);
                    switch (tag) {
                        case 0x43: //satelite delivery system
                        {
                            pScanTransport->deliveryType = eDeliveryTypeS;
                            uint32_t read;
                            bitBuff->readBits(32, &read);
                            pScanTransport->deliverySystem.sat.freq = read;
                            pScanTransport->freqKHz = pScanTransport->deliverySystem.sat.freq / 1000;
                            bitBuff->readBits(16, &read);
                            pScanTransport->deliverySystem.sat.orbitalPosition = read;
                            bitBuff->readBits(1, &read);
                            pScanTransport->deliverySystem.sat.westEastFlag = read;
                            bitBuff->readBits(2, &read);
                            pScanTransport->deliverySystem.sat.polarization = read;
                            bitBuff->readBits(2, &read);
                            pScanTransport->deliverySystem.sat.rolloff = read;
                            bitBuff->readBits(1, &read);
                            pScanTransport->deliverySystem.sat.modulationSystem = read;
                            bitBuff->readBits(2, &read);
                            pScanTransport->deliverySystem.sat.modulationType = read;
                            bitBuff->readBits(28, &read);
                            pScanTransport->deliverySystem.sat.symbolRate = read;
                            bitBuff->readBits(4, &read);
                            pScanTransport->deliverySystem.sat.fec_inner = read;
                        }
                            break;
                        case 0x44: //cable delivery system
                        {
                            char bcdstr[20];
                            pScanTransport->deliveryType = eDeliveryTypeC;
                            uint32_t read;
                            bitBuff->readBits(32, &read);
                            sprintf(bcdstr, "%x", read);
                            pScanTransport->deliverySystem.cable.freq = atoi(bcdstr) * 100; //MHz -> Hz
                            pScanTransport->freqKHz = pScanTransport->deliverySystem.cable.freq / 1000;
                            bitBuff->skipBits(12); //reserved
                            bitBuff->readBits(4, &read);
                            pScanTransport->deliverySystem.cable.fec_outer = read;
                            bitBuff->readBits(8, &read);
                            if(read == 1) { //16QAM
                                pScanTransport->deliverySystem.cable.modulation = TDI_TunerProtocol_16QAM;
                            } else if(read == 2) { //32QAM
                                pScanTransport->deliverySystem.cable.modulation = TDI_TunerProtocol_Unknown;
                            } else if(read == 3) { //64QAM
                                pScanTransport->deliverySystem.cable.modulation = TDI_TunerProtocol_64QAM;
                            } else if(read == 4) { //128QAM
                                pScanTransport->deliverySystem.cable.modulation = TDI_TunerProtocol_Unknown;
                            } else if(read == 5) { //256QAM
                                pScanTransport->deliverySystem.cable.modulation = TDI_TunerProtocol_256QAM;
                            }
                            bitBuff->readBits(28, &read);
                            sprintf(bcdstr, "%x", read);
                            pScanTransport->deliverySystem.cable.symbol_rate = atoi(bcdstr) * 100;
                            bitBuff->readBits(4, &read);
                            pScanTransport->deliverySystem.cable.fec_inner = read;
                        }
                            break;
                        case 0x5A: //terrestrial delivery system
                        {
                            pScanTransport->deliveryType = eDeliveryTypeT;
                            uint32_t read;
                            bitBuff->readBits(32, &read);
                            pScanTransport->deliverySystem.terr.freq = read * 10;
                            pScanTransport->freqKHz = pScanTransport->deliverySystem.terr.freq / 1000;
                            bitBuff->readBits(3, &read);
                            pScanTransport->deliverySystem.terr.bandwidth = 8 - read;
                            bitBuff->readBits(1, &read);
                            pScanTransport->deliverySystem.terr.priority = read;
                            bitBuff->readBits(1, &read);
                            pScanTransport->deliverySystem.terr.time_slicing_indicator = read;
                            bitBuff->readBits(1, &read);
                            pScanTransport->deliverySystem.terr.mpe_fec_indicator = read;
                            bitBuff->skipBits(2); //reserved
                            bitBuff->readBits(2, &read);

                            if(read == 0) {
                                pScanTransport->deliverySystem.terr.constellation = TDI_TunerProtocol_QPSK;
                            } else if(read == 1) {
                                pScanTransport->deliverySystem.terr.constellation = TDI_TunerProtocol_16QAM;
                            } else if(read == 2) {
                                pScanTransport->deliverySystem.terr.constellation = TDI_TunerProtocol_64QAM;
                            }
                            bitBuff->readBits(3, &read);
                            pScanTransport->deliverySystem.terr.hierarchy_info = read;
                            bitBuff->readBits(3, &read);
                            pScanTransport->deliverySystem.terr.coderate_HP = read;
                            bitBuff->readBits(3, &read);
                            pScanTransport->deliverySystem.terr.coderate_LP = read;
                            bitBuff->readBits(2, &read);
                            pScanTransport->deliverySystem.terr.guardinterval = read;
                            bitBuff->readBits(2, &read);
                            pScanTransport->deliverySystem.terr.transmissionMode = read;
                            bitBuff->readBits(1, &read);
                            pScanTransport->deliverySystem.terr.otherfreqflag = read;
                            bitBuff->skipBits(32);
                        }
                            break;
                        case 0x79: //S2 satelite delivery system
                        {
                            pScanTransport->deliveryType = eDeliveryTypeS2;
                            uint32_t read;
                            bitBuff->readBits(1, &read);
                            pScanTransport->deliverySystem.sat2.scrambling_sequence_selector = read;
                            bitBuff->readBits(1, &read);
                            pScanTransport->deliverySystem.sat2.multiple_input_stream_flag = read;
                            bitBuff->readBits(1, &read);
                            pScanTransport->deliverySystem.sat2.backward_compatibility_indicator = read;
                            bitBuff->skipBits(5); //reserved
                            if(pScanTransport->deliverySystem.sat2.scrambling_sequence_selector) {
                                bitBuff->skipBits(6); //reserved
                                bitBuff->readBits(18, &read);
                                pScanTransport->deliverySystem.sat2.scrambling_sequence_index = read;
                            }
                            if(pScanTransport->deliverySystem.sat2.multiple_input_stream_flag) {
                                bitBuff->readBits(8, &read);
                                pScanTransport->deliverySystem.sat2.input_stream_identifier = read;
                            }
                        }
                            break;
                        case 0x41: //service list
                        {
                            int length = desc_length;
                            while (length > 0) {
                                uint16_t service_id;
                                uint8_t service_type;
                                bitBuff->readBits(16, &service_id);
                                bitBuff->readBits(8, &service_type);

                                ScanService* pService = getServiceInfo(pScanTransport, service_id);
                                if(pService == NULL) {
                                    pService = new ScanService;
                                    init_service(pService);
                                    pService->sid = service_id;
                                    pScanTransport->serviceList.push_back(pService);
                                }
                                pService->service_type = service_type;

                                length -= 3;
                            }

                        }
                            break;
                        default: //not interest descriptors
                        bitBuff->skipByte(desc_length);
                            break;
                    }
                }
            }
        }

        delete bitBuff;
    }
}
ScanTransport* ParseSDT(Table *pTable, uint16_t* tsid, bool& changeSomething)
{
    int InTsid;
    ScanTransport* pTs = NULL;

    for (int i = 0; i < pTable->m_SectionCount; i++) {
        BitStream *bitBuff = new BitStream(pTable->m_pSection[i], pTable->m_pSectionLen[i], false);

        if(i == 0) {
            uint32_t dummy;
            bitBuff->skipByte(3); //table id ~ section length
            bitBuff->readBits(16, &dummy); //transport stream id
            *tsid = dummy;
            InTsid = (int) *tsid;
            pTs = getTransportStreamInfo(InTsid);
            if(pTs == NULL) {
                pTs = new ScanTransport;
                init_ts(pTs);
                pTs->tsid = InTsid;
                pTs->sdt_version = Version(pTable->m_pSection[i]);
                ListTs.push_back(pTs);
                changeSomething = true;
            } else {
                if(pTs->sdt_version == Version(pTable->m_pSection[i])) {
                    changeSomething = false;
                    L_DEBUG(TAG,"\n");
                    delete bitBuff;
                    return pTs;
                }
                changeSomething = true;
            }
            bitBuff->skipBits(2); //reserved
            bitBuff->readBits(5, &dummy); //version
            pTs->sdt_version = dummy;
            bitBuff->skipBits(1);
            bitBuff->skipByte(5); //section number ~ reserved
        } else {
            bitBuff->skipByte(11);
        }

        //service loop
        while (bitBuff->getRemainBytes() > 4) {
            ScanService *info = NULL;
            uint32_t val, desc_length;
            bitBuff->readBits(16, &val); //service id
            info = getServiceInfo(pTs, val);
            if(info == NULL) {
            	L_DEBUG(TAG,"\n");
                info = new ScanService;
                init_service(info);
                pTs->serviceList.push_back(info);
            }
            info->sid = val;
            bitBuff->skipBits(6); //reserved
            bitBuff->readBits(1, &val); //eit schedule flag
            //printf("eit schedule flag = 0x%x\n",val);
            info->eit_schedule_flag = val;
            if(info->eit_schedule_mode[0] == eEIT_NoOp && info->eit_schedule_flag) {
                info->eit_schedule_mode[0] = eEIT_Standby;
            }
            bitBuff->readBits(1, &val); //eit pf flag
            //printf("eit pf flag = 0x%x\n",val);
            info->eit_pf_flag = val;
            if(info->eit_pf_mode == eEIT_NoOp && info->eit_pf_flag) {
                info->eit_pf_mode = eEIT_Standby;
            }
            bitBuff->skipBits(4); //running status ~ free ca mode
            bitBuff->readBits(12, &desc_length); //descriptor length
            if(desc_length > 0) {
                bitBuff->skipByte(desc_length);
            }
        }

        delete bitBuff;
    }
    return pTs;
}

ScanTransport* ParseDVBEIT(uint8_t *pBuffer, uint16_t* tsid, uint16_t* sid, bool& changeSomething)
{
    *sid = (((uint16_t) pBuffer[3]) << 8) | ((uint16_t) pBuffer[4]);
    *tsid = (((uint16_t) pBuffer[8]) << 8) | ((uint16_t) pBuffer[9]);
    ScanTransport* pTs = getTransportStreamInfo(*tsid);
    if(pTs) {
        ScanService* pScanService = getServiceInfo(pTs, *sid);
        if(pScanService) {
            int tid = pBuffer[0] & 0xF0;
            if(tid == 0x40) {
                if(pScanService->eit_pf_version == Version(pBuffer)) {
                    changeSomething = false;
                    return pTs;
                }
#ifdef __DEBUG_LOG_SM__
                L_INFO(TAG, "pScanService->eit_pf_version 0x%X -> 0x%X\n",pScanService->eit_pf_version,Version(pBuffer));
#endif
                pScanService->eit_pf_version = Version(pBuffer);
                pScanService->eit_pf_mode = eEIT_Received;
            } else {
                int idx = pBuffer[0] & 0x0F;
                if(pScanService->eit_sch_version[idx] == Version(pBuffer)) {
                    changeSomething = false;
                    return pTs;
                }
#ifdef __DEBUG_LOG_SM__
                L_INFO(TAG, "pScanService->eit_sch_version 0x%X -> 0x%X\n",pScanService->eit_sch_version[idx],Version(pBuffer));
#endif
                pScanService->eit_sch_version[idx] = Version(pBuffer);
                pScanService->eit_schedule_mode[idx] = eEIT_Received;
                if(pScanService->eit_last_tableId == 0) {
                    pScanService->eit_last_tableId = pBuffer[13];
                }
            }
        }
    }
    changeSomething = true;
    return pTs;
}
ScanTransport* ParseVCT(Table *pTable, uint16_t* tsid, bool& changeSomething)
{
    int InTsid;
    ScanTransport* pTs = NULL;

    for (int i = 0; i < pTable->m_SectionCount; i++) {
        BitStream *bitBuff = new BitStream(pTable->m_pSection[i], pTable->m_pSectionLen[i], false);

        if(i == 0) {
            uint32_t dummy;
            bitBuff->skipByte(3); //table id ~ section length
            bitBuff->readBits(16, &dummy); //transport stream id
            *tsid = dummy;
            InTsid = (int) *tsid;
            pTs = getTransportStreamInfo(InTsid);
            if(pTs == NULL) {
                pTs = new ScanTransport;
                init_ts(pTs);
                pTs->tsid = InTsid;
                pTs->sdt_version = Version(pTable->m_pSection[i]);
                ListTs.push_back(pTs);
                changeSomething = true;
            } else {
                if(pTs->sdt_version == Version(pTable->m_pSection[i])) {
                    changeSomething = false;
                    delete bitBuff;
                    return pTs;
                }
                changeSomething = true;
            }
            bitBuff->skipByte(4); //reserved ~ protocol version
        } else {
            bitBuff->skipByte(9); //tableid ~ protocol version
        }

        uint32_t num_channel;
        bitBuff->readBits(8, &num_channel);
        for (int channel_loop = 0; channel_loop < num_channel; channel_loop++) {
            bitBuff->skipByte(17); //short name ~ minor channel num
            uint32_t modulation, freq, prog_num, etm_location, sourceid, desc_leng;
            //bitBuff->readBits(8,&modulation);
            //bitBuff->readBits(32,&freq);
            bitBuff->skipByte(7); //modulation ~ channel TSID
            bitBuff->readBits(16, &prog_num);
            bitBuff->readBits(2, &etm_location);
            bitBuff->skipBits(14); //access control ~ service type
            bitBuff->readBits(16, &sourceid);
            bitBuff->skipBits(6); //reserved
            bitBuff->readBits(10, &desc_leng);
            if(desc_leng > 0) {
                bitBuff->skipByte(desc_leng);
            }
            ScanService* pScanService = getServiceInfo(pTs, prog_num);
            if(pScanService == NULL) {
                pScanService = new ScanService;
                init_service(pScanService);
                pTs->serviceList.push_back(pScanService);
                pScanService->sid = prog_num;
            }
            pScanService->sourceID = sourceid;
            pScanService->channel_etm_location = etm_location;
        }

        delete bitBuff;
    }

    return pTs;
}
ScanTransport* ParseMGT(uint8_t *pBuffer, uint32_t size, uint16_t tsid, bool& changeSomething)
{
    ScanTransport* pTs = getTransportStreamInfo(tsid);
    if(pTs) {
        uint8_t version = Version(pBuffer);
        if(pTs->mgt_version != version) {
            changeSomething = true;

            BitStream *bitBuff = new BitStream(pBuffer + 9, size - 9, false);
            int sizeN = pTs->mgtItemList.sizeNoLock();
            int res;
            for (int mgtitr = 0; mgtitr < sizeN; mgtitr++) {
                mgt_item* item = pTs->mgtItemList.AtNoLock(mgtitr, res);
                if(res)
                    delete item;
            }
            pTs->mgtItemList.clearNoLock();
            uint32_t table_defined;
            bitBuff->readBits(16, &table_defined);
            for (int loop = 0; loop < table_defined; loop++) {
                mgt_item* item = new mgt_item;
                uint32_t dummy;
                bitBuff->readBits(16, &dummy);
                item->table_type = dummy;
                bitBuff->skipBits(3);
                bitBuff->readBits(13, &dummy);
                item->pid = dummy;
                bitBuff->skipBits(3);
                bitBuff->readBits(5, &dummy);
                item->version = dummy;
                bitBuff->skipBits(36);
                bitBuff->readBits(12, &dummy);
                if(dummy > 0) {
                    bitBuff->skipByte(dummy);
                }
                L_DEBUG(TAG, "type=0x%X, pid=0x%X, ver=0x%X\n", item->table_type, item->pid, item->version);
                pTs->mgtItemList.push_back(item);
            }
            delete bitBuff;
        } else {
        }
    }
    return pTs;
}
ScanTransport* ParseEIT(Table *pTable, uint16_t pid, uint16_t tsid, bool& changeSomething)
{
    ScanTransport* pTs = getTransportStreamInfo(tsid);
    uint16_t sourceid;
    if(pTs) {
        sourceid = TableIDExt(pTable->m_pSection[0]);
        ScanService* pService = getServiceInfoSourceID(pTs, sourceid);
        if(pService) {
            for (int i = 0; i < pTable->m_SectionCount; i++) {
                BitStream *bitBuff = new BitStream(pTable->m_pSection[i] + 9, pTable->m_pSectionLen[i] - 9, false);
                uint32_t num_evt;
                bitBuff->readBits(8, &num_evt);
                if(num_evt > 0) {
                    uint16_t evt_id;
                    uint8_t etm_location;
                    uint32_t dummy;
                    bitBuff->skipBits(2);
                    bitBuff->readBits(14, &evt_id);
                    bitBuff->skipByte(4);
                    bitBuff->skipBits(2);
                    bitBuff->readBits(2, &etm_location);
                    bitBuff->skipBits(20);
                    bitBuff->readBits(8, &dummy);
                    if(dummy > 0) {
                        bitBuff->skipByte(dummy);
                    }
                    bitBuff->skipBits(4);
                    bitBuff->readBits(12, &dummy);
                    if(dummy > 0) {
                        bitBuff->skipByte(dummy);
                    }
                    EVT_INFO* info = new EVT_INFO;
                    info->pid = pid;
                    info->evtid = evt_id;
                    info->etm_location = etm_location;
                    pService->PsipEvtList.push_back(info);
                }
                delete bitBuff;
            }
        }

    }
    return pTs;
}

int ScanManager::getUriInfo(string uri, string target)
{
	   //tuner://qam64:466000?ch=22&pn=22&ci=0&cp=0&pp=820&vp=820&vc=2&ap=821&ac=4&ct=1&sn=0&sr=6875&bw=8
// ex) tuner://qam64:466000?sr=6875&bw=8&sc=1&sn=0
// ex) pvrfile:///storage/usb.HDD/Default/021115_1635.ts?ch=2&pn=2&ci=0&cp=0&vp=225&vc=2&ap=226&ac=4&ct=1&sn=0&sr=6875&bw=8&ts=0
//    L_DEBUG(TAG, "uri : [%s], target [%s] \n", uri.c_str(),target.c_str());
    int result = 0;
    vector < string > listElm;
    int current_channel_number = 0;
    splitStringByDelimiter(uri, "?", listElm);

    if(listElm.size() != 2) {
        L_ERROR(TAG, "\n=== WRONG EPG URI FORMAT ===\n");
        return current_channel_number;
    }
    string header = listElm.at(0);
    string val = listElm.at(1);

    listElm.clear();

    splitStringByDelimiter(header, "://", listElm);

	string tuner = listElm.at(0);
	string token = listElm.at(1);
	if(tuner.compare(target.c_str()) == 0) { // tuner
		vector < string > listData;
		splitStringByDelimiter(token, ":", listData);
		if(listData.size() != 2) {
			L_ERROR(TAG, "WRONG FORMAT : %s\n", token.c_str());
		}
		string demodType = listData.at(0);
		result = atoi(listData.at(1).c_str());
//        L_DEBUG(TAG, "demod[%s], frequency[%d]\n", demodType.c_str(), result);
		demodType.clear();
		listData.clear();
	} else if(tuner.compare(target.c_str()) == 0) { // pvrfile
		listElm.clear();
		return -1; // pvr mode
	}

    splitStringByDelimiter(val, "&", listElm);
	map < string, string > mapValue;
	for (vector<string>::iterator itr = listElm.begin(); itr != listElm.end(); itr++) {
		string token = (string)(*itr);
		vector < string > listStr;
		splitStringByDelimiter(token, "=", listStr);
		if(listStr.size() != 2) {
//			L_ERROR(TAG, "WRONG FORMAT : %s\n", token.c_str());
			continue;
		}
		mapValue.insert(make_pair(listStr.at(0), listStr.at(1)));
	}

	 for (map<string, string>::iterator itr = mapValue.begin(); itr != mapValue.end(); itr++) {
		string key = itr->first;
		string value = itr->second;
		if(key.compare(target.c_str()) == 0) { // current channel & scan mode info
			result = atoi(value.c_str());
		}
	}
    listElm.clear();
    mapValue.clear();
//    current_uri = uri;
//    L_DEBUG(TAG, "reuslt data [%d]\n", result);
//    L_DEBUG(TAG, "current_uri [%s]\n", current_uri.c_str());
    return result;
}

int ScanManager::getPAT()
{
//	 L_INFO(TAG,"\n");
    int ret = 0;
    string uri = current_uri;
//    L_INFO(TAG,"uri [%s]\n",uri.c_str());
    TableFilter *pTableFilter = new TableFilter(0x0, 0x0);
    if(pTableFilter) {
        pTableFilter->setCallbackFunction(table_cb);
        ret = pTableFilter->startTable(uri);

        if(ret) {
            Service_Filter_List.push_back((void*) pTableFilter);
        } else {
            deleteTableFilter(pTableFilter);
            pTableFilter = NULL;
        }
    }

    return ret;
}
int ScanManager::getPMT(int pmtid, int prog)
{
//	 L_INFO(TAG,"\n");
    int ret = 0;
    string uri = current_uri;
//    L_INFO(TAG, "pmtid =0x%x prog=0x%X\n", pmtid, prog);
    SimpleFilter *pPmtF = new SimpleFilter(pmtid, 0x02, prog);

    if(pPmtF) {
        pPmtF->setCallbackFunction(section_cb);
        ret = pPmtF->startFilter(uri);
        if(ret) {
            Service_Section_Filter_List.push_back((void*) pPmtF);
        } else {
            deleteSimpleFilter(pPmtF);
            pPmtF = NULL;
        }
    }

    return ret;
}

int ScanManager::getNIT()
{
    int ret = 0;
//    L_INFO(TAG,"\n");
    string uri = current_uri;
//    L_INFO(TAG,"uri [%s]\n",uri.c_str());
    TableFilter *pNitF = new TableFilter(0x10, 0x40);
    if(pNitF) {
        pNitF->setCallbackFunction(table_cb);
        ret = pNitF->startTable(uri);
        if(ret) {
            Service_Filter_List.push_back((void*) pNitF);
        } else {
            deleteTableFilter(pNitF);
            pNitF = NULL;
        }
    }

    return ret;
}

int ScanManager::getNITwithID(int nwid)
{
//	 L_INFO(TAG,"\n");
    int ret = 0;
    string uri = current_uri;
//    L_INFO(TAG,"uri [%s]\n",uri.c_str());
    TableFilter *pNitF = new TableFilter(0x10, 0x40, nwid);
    if(pNitF) {
        pNitF->setCallbackFunction(table_cb);
        ret = pNitF->startTable(uri);

        if(ret) {
            Service_Filter_List.push_back((void*) pNitF);
        } else {
            deleteTableFilter(pNitF);
            pNitF = NULL;
        }
    }

    return ret;
}

int ScanManager::getSDT()
{
//	 L_INFO(TAG,"\n");
    int ret = 0;
    string uri = current_uri;
//    L_INFO(TAG,"uri [%s]\n",uri.c_str());
    TableFilter *pSdtF = new TableFilter(0x11, 0x42);
    if(pSdtF) {
        pSdtF->setCallbackFunction(table_cb);
        ret = pSdtF->startTable(uri);
        if(ret) {
            Service_Filter_List.push_back((void*) pSdtF);
        } else {
            deleteTableFilter(pSdtF);
            pSdtF = NULL;
        }
    }
    return ret;
}
int ScanManager::getSDTOther(uint16_t tsid)
{
//	 L_INFO(TAG,"tsid [0x%x]\n",tsid);
    int ret = 0;
    string uri = current_uri;
//    L_INFO(TAG,"tsid = [%d], uri [%s]\n",tsid,uri.c_str());
    TableFilter *pSdtOtherF = new TableFilter(0x11, 0x46, tsid);
    if(pSdtOtherF) {
        pSdtOtherF->setCallbackFunction(table_cb);
        ret = pSdtOtherF->startTable(uri);
        if(ret) {
            Service_Filter_List.push_back((void*) pSdtOtherF);
        } else {
            deleteTableFilter(pSdtOtherF);
            pSdtOtherF = NULL;
        }
    }
    return ret;
}

int ScanManager::getBAT()
{
    int ret = 0;
    string uri = current_uri;
//    L_INFO(TAG,"uri [%s]\n",uri.c_str());
    TableFilter *pBatF = new TableFilter(0x11, 0x4A);
    if(pBatF) {
    	pBatF->setCallbackFunction(table_cb);
        ret = pBatF->startTable(uri);
        if(ret) {
            Service_Filter_List.push_back((void*) pBatF);
        } else {
            deleteTableFilter(pBatF);
            pBatF = NULL;
        }
    }
    return ret;
}


int ScanManager::getTDT()
{
    int ret = 0;
    string uri = current_uri;
    L_INFO(TAG,"uri [%s]\n",uri.c_str());
    SimpleFilter *pTdtF = new SimpleFilter(0x14, 0x70);
    if(pTdtF) {
        pTdtF->setCallbackFunction(section_cb);
        ret = pTdtF->startFilter(uri);

        if(ret) {
            Service_Section_Filter_List.push_back((void*) pTdtF);
        } else {
			L_DEBUG(TAG,"\n");
        	deleteSimpleFilter(pTdtF);
            pTdtF = NULL;
        }
    }

    return ret;
}

int ScanManager::getVCT()
{
    int ret = 0;
    string uri = current_uri;
    TableFilter *pTvctF = new TableFilter(0x1FFB, 0xC8);
    if(pTvctF) {
        pTvctF->setTableIDMask(0xFE);
        pTvctF->setCallbackFunction(table_cb);
        ret = pTvctF->startTable(uri);
        if(ret) {
            Service_Filter_List.push_back((void*) pTvctF);
        } else {
            deleteTableFilter(pTvctF);
            pTvctF = NULL;
        }
    }
    return ret;
}

int ScanManager::getMGT()
{
    int ret = 0;
    SimpleFilter *pMgtF = new SimpleFilter(0x1FFB, 0xC7);
    if(pMgtF) {
        pMgtF->setCallbackFunction(section_cb);
        ret = pMgtF->startFilter(current_uri);

        if(ret) {
            Service_Section_Filter_List.push_back((void*) pMgtF);
        } else {
        	L_DEBUG(TAG,"\n");
            deleteSimpleFilter(pMgtF);
            pMgtF = NULL;
        }
    }

    return ret;
}

void ScanManager::makeTSIDOtherList()
{
    int res;
    ListTs.lock();
    int sizeN = ListTs.sizeNoLock();
    for (int idx = 0; idx < sizeN; idx++) {
        res = 0;
        ScanTransport* info = ListTs.AtNoLock(idx, res);

        if(res && info && info->tsid != current_tsid) {
            OtherTSIDQ.enqueue(info->tsid);
        }
    }
    ListTs.unlock();
}

void scanTunerCallback(int eventId, int extra, const uint8_t* data, size_t size)
{
    string uri;
    bool bScanMode = false;

    if(data == NULL || size == 0)
        	return;

    L_DEBUG(TAG, "=== eventId(%d), extra(%d), URI[%s] ===\n", eventId, extra, data);

    if( eventId == PLAY_INFO_STOPPED  ||  eventId == PLAY_INFO_STARTED ||
    		eventId == PLAY_INFO_TUNER_LOCKED || eventId == PLAY_INFO_TUNER_LOCK_FAILED) {

		uri.assign((char*) data, size);
		ScanManager& manager = ScanManager::getInstance();
		L_DEBUG(TAG, "=== eventId(%d), extra(%d), URI[%s] ===\n", eventId, extra, data);
	//    current_uri = (char*)data;
	//    if(chNum > 0) {
	//        gCurrentChannelNum = chNum;
	//    }

		int result =manager.getUriInfo(uri,"ch");
		if(result != -1) {
			gCurrentChannelNum = result;
		} else {
			L_DEBUG(TAG,"PVR mode !!!! \n\n");
			return;
		}
	//    L_DEBUG(TAG, "gCurrentChannelNum : [%d]\n",gCurrentChannelNum);
		bScanMode = manager.getUriInfo(uri,"sc");
		int freq = manager.getUriInfo(uri,"tuner");
	//    L_DEBUG(TAG, "freq : [%d]\n",freq);
		current_uri = uri;
		manager.tune_result(eventId, freq, bScanMode);
    }else {
    	L_DEBUG(TAG,"invalid callback!!!! \n");
    }
}

void* scan_cmd_task(void* param)
{
	ScanManager& manager = ScanManager::getInstance();
	Parser& parser = Parser::getInstance();
    while (1) {
        ScanManagerLock();
        int ScanQResult = 0;
        ScanMsg msg = ScanManageQ.dequeue(ScanQResult);
        if(ScanQResult == 0) {
            ScanMsg msgidle;
            msgidle.cmd = eScanCmd_Idle;
            ScanManageQ.enqueue(msgidle);
            ScanManagerUnlock();
            siMiliSleep(100); //sleep 100 msec
        } else {
            switch (msg.cmd) {
                case eScanCmd_Idle: {
                    if(manager.getMode() != eInUpdate && manager.mLastOpSec) {
                        time_t current_sec;
                        getSITime_t(&current_sec);
                        if(SCAN_MANAGER_TIMEOUT_SEC != 0 && current_sec - manager.mLastOpSec > SCAN_MANAGER_TIMEOUT_SEC) {
                            manager.mLastOpSec = 0;
                            ScanMsg msgidle;
                            msgidle.cmd = eScanCmd_TimeOuted;
                            SendScanManageQ(msgidle, true);
                            L_INFO(TAG,"\n");
                        }
                    }
                    siMiliSleep(10); //sleep 10 msec
                }
                    break;

                case eScanCmd_TUNE_START: {
                    TDI_Error tuneErr;
                    int mode=0;
                    int tuner = 0;

                    if(manager.getDummyEpgMode()) {
                    	SIChangeEvent event;
						event.setReceived(0);
						event.setToBeReceived(0);
						event.setEventType(SIChangeEvent::SCAN_READY);
						parser.receivedTable("SCAN", NULL, event);
						ScanDataMsg datamsg;
						datamsg.cmd = ScanDataCmd_SCAN_END;
						SendScanDataQ(datamsg);
						manager.setDummyEpgMode(false);
						break;
					}

                    manager.bTunerLocked = false;
                    //TDI_Tuner_Stop(0);
                    if(bcreatedDMX)
                    {
                    //	manager.channelChangeBefore();
                    	MMF_Demux_Destroy(mDmxHandle);
                    	bcreatedDMX = false;
                    }
                    TDI_TunerSettings* setting = (TDI_TunerSettings*) msg.param;
                    mode = 1;
#ifdef __FOR_LINKNET__
                    if(manager.bTestMode)
                    {
                        if(manager.invertTuner)
                        	tuner=1;
                        else
                        	tuner=0;
                    }
#endif
//                    tuneErr = TDI_Tuner_SetSettings(tuner, setting); //wjpark 2015.10.02
//                    MMF_Demux_Create(&mDmxHandle, (uint8_t*)manager.getUri(setting).c_str());

                    if(!bcreatedDMX || bHomets )
					{
                    	L_INFO(TAG, "\n");
						getSITime_t(&manager.mLastOpSec);
						manager.channelChangeBefore();
						MMF_Demux_Create(&mDmxHandle, (uint8_t*)manager.getUri(setting,mode).c_str());
						bcreatedDMX =true;
						L_INFO(TAG, "\n");
					}
                    if(manager.bTestMode) {
                    	L_DEBUG(TAG," current mode [%d] \n",manager.getMode());
                        if(manager.getMode() == eInScanning) {
                            if(manager.bFixedModulation) {
                            	L_INFO(TAG, "\n");
                                manager.scanTsCnt++;
                                ScanDataMsg msg;
                                msg.cmd = ScanDataCmd_TuneParam;
                                msg.iparam = manager.scanTsCnt;
                                SendScanDataQ(msg);
                            } else if(pCurrentTs && pCurrentTs->deliverySystem.inbnd.modulation == TDI_TunerProtocol_64QAM) {
                            	L_INFO(TAG, "\n");
                                manager.scanTsCnt++;

                                ScanDataMsg msg;
                                msg.cmd = ScanDataCmd_TuneParam;
                                msg.iparam = manager.scanTsCnt;
                                SendScanDataQ(msg);
                            }
                            else {
                            	L_DEBUG(TAG," \n");
                            }

                        } else if(manager.getMode() == eInScanEnd){
                        	ScanDataMsg msg;
							msg.cmd = ScanDataCmd_SCAN_DEMUX_CLOSE;
							SendScanDataQ(msg);
                        	L_DEBUG(TAG," \n");
                        }
                    }else if(manager.getMode() == eInScanning) { // normal mode
                    	L_INFO(TAG, "\n");
                        manager.scanTsCnt++;

                        ScanDataMsg msg;
                        msg.cmd = ScanDataCmd_TuneParam;
                        msg.iparam = manager.scanTsCnt;
                        SendScanDataQ(msg);
                    }
                    else if(manager.getMode() == eInScanEnd) {
                    	ScanDataMsg msg;
						msg.cmd = ScanDataCmd_SCAN_DEMUX_CLOSE;
						SendScanDataQ(msg);
                    	L_INFO(TAG, "\n");
                    }
                    else if(manager.getMode() == eFirstConnect) { // scan mode start
                    	L_INFO(TAG, "\n");
						if(bcreatedDMX  && bHomets && bsetForceDMXDestroy )
						{
							L_DEBUG(TAG,"\n");
							MMF_Demux_Destroy(mDmxHandle);
							bcreatedDMX = false;
							bHomets = false;
							bsetForceDMXDestroy = false;
                    	}
                    }
					getSITime_t(&manager.mLastOpSec);
                }
                    break;
                case eScanCmd_CHCHANGE_BEFORE: {
                    manager.channelChangeBefore();
                    getSITime_t(&manager.mLastOpSec);
                }
                    break;
                case eScanCmd_DVB_START:
                case eScanCmd_NIT_REQUEST: {
                    L_INFO(TAG, "CMD : eScanCmd_NIT_REQUEST\n");
                    if(manager.getNIT()) {
                        getSITime_t(&manager.mLastOpSec);
                    } else if(manager.bTunerLocked) {
                        ScanMsg retrymsg;
                        retrymsg.cmd = eScanCmd_Idle;
                        SendScanManageQ(retrymsg);
                        retrymsg.cmd = eScanCmd_NIT_REQUEST;
                        SendScanManageQ(retrymsg);
                    }
                }
                    break;
                case eScanCmd_SDT_REQUEST: {
                    if(manager.getSDT()) {
                        getSITime_t(&manager.mLastOpSec);
                    } else if(manager.bTunerLocked) {
                        ScanMsg retrymsg;
                        retrymsg.cmd = eScanCmd_Idle;
                        SendScanManageQ(retrymsg);
                        retrymsg.cmd = eScanCmd_SDT_REQUEST;
                        SendScanManageQ(retrymsg);
                    }
                }

                case eScanCmd_BAT_REQUEST: {
#if 0
					if(manager.getBAT()) {
						getSITime_t(&manager.mLastOpSec);
					} else if(manager.bTunerLocked) {
						ScanMsg retrymsg;
						retrymsg.cmd = eScanCmd_Idle;
						SendScanManageQ(retrymsg);
						retrymsg.cmd = eScanCmd_BAT_REQUEST;
						SendScanManageQ(retrymsg);
					}
#endif
				}

                    break;
                case eScanCmd_SDT_OTHER_REQUEST: {
                    int othertsSize = OtherTSIDQ.size();

                    for (int otherts = 0; otherts < othertsSize; otherts++) {
                        int qres = 0;
                        uint16_t tsid = OtherTSIDQ.dequeue(qres);
                        if(qres) {
                            if(manager.getSDTOther(tsid)) {
                                getSITime_t(&manager.mLastOpSec);
                            } else if(manager.bTunerLocked) {
                                OtherTSIDQ.enqueue(tsid);
                                ScanMsg retrymsg;
                                retrymsg.cmd = eScanCmd_Idle;
                                SendScanManageQ(retrymsg);
                                retrymsg.cmd = eScanCmd_SDT_OTHER_REQUEST;
                                SendScanManageQ(retrymsg);
                                break;
                            }
                        }
                    }
                }
                    break;
                case eScanCmd_TDT_REQUEST: {
                    L_INFO(TAG, "CMD : eScanCmd_TDT_REQUEST\n");
                    if(manager.getTDT()) {
                        getSITime_t(&manager.mLastOpSec);
                    } else if(manager.bTunerLocked) {
                        ScanMsg retrymsg;
                        retrymsg.cmd = eScanCmd_Idle;
                        SendScanManageQ(retrymsg);
                        retrymsg.cmd = eScanCmd_TDT_REQUEST;
                        SendScanManageQ(retrymsg);
                    }
                }
                    break;
                case eScanCmd_NIT_TEST_REQUEST: {
                    int id = msg.iparam;
                    if(manager.getNITwithID(id)) {
                        getSITime_t(&manager.mLastOpSec);
                    } else if(manager.bTunerLocked) {
                        ScanMsg retrymsg;
                        retrymsg.cmd = eScanCmd_Idle;
                        SendScanManageQ(retrymsg);
                        retrymsg.cmd = eScanCmd_NIT_TEST_REQUEST;
                        retrymsg.iparam = id;
                        SendScanManageQ(retrymsg);
                    }
                }
                    break;

                case eScanCmd_PSIP_START:
                case eScanCmd_VCT_REQUEST: {
                    if(manager.getVCT()) {
                    } else if(manager.bTunerLocked) {
                        ScanMsg retrymsg;
                        retrymsg.cmd = eScanCmd_Idle;
                        SendScanManageQ(retrymsg);
                        retrymsg.cmd = eScanCmd_VCT_REQUEST;
                        SendScanManageQ(retrymsg);
                    }
                    getSITime_t(&manager.mLastOpSec);
                }
                    break;
                case eScanCmd_MGT_REQUEST: {
                    if(manager.getMGT()) {
                    } else if(manager.bTunerLocked) {
                        ScanMsg retrymsg;
                        retrymsg.cmd = eScanCmd_Idle;
                        SendScanManageQ(retrymsg);
                        retrymsg.cmd = eScanCmd_MGT_REQUEST;
                        SendScanManageQ(retrymsg);
                    }
                    getSITime_t(&manager.mLastOpSec);
                }
                    break;

                case eScanCmd_PSI_START:
                case eScanCmd_PAT_REQUEST: {
                    L_INFO(TAG, "eScanCmd_PAT_REQUEST\n");
                    if(manager.getPAT()) {
                        getSITime_t(&manager.mLastOpSec);
                    } else if(manager.bTunerLocked) {
                        ScanMsg retrymsg;
                        retrymsg.cmd = eScanCmd_Idle;
                        SendScanManageQ(retrymsg);
                        retrymsg.cmd = eScanCmd_PAT_REQUEST;
                        SendScanManageQ(retrymsg);
                    }
                }
                    break;
                case eScanCmd_PMT_REQUEST: {
                    L_INFO(TAG, "eScanCmd_PMT_REQUEST count=%d\n", PMTPIDQ.size());
                    int pmtsize = PMTPIDQ.size();
                    for (int pmtcount = 0; pmtcount < pmtsize; pmtcount++) {
                        int Qresult = 0;
                        pmt_t pmt = PMTPIDQ.dequeue(Qresult);
                        if(Qresult) {
                            if(manager.getPMT(pmt.pid, pmt.prog)) {
                                getSITime_t(&manager.mLastOpSec);
                            } else if(manager.bTunerLocked) {
                                PMTPIDQ.enqueue(pmt);
                                ScanMsg retrymsg;
                                retrymsg.cmd = eScanCmd_Idle;
                                SendScanManageQ(retrymsg);
                                retrymsg.cmd = eScanCmd_PMT_REQUEST;
                                SendScanManageQ(retrymsg);
                                break;
                            }
                        }
                    }
                }
                    break;

                case eScanCmd_NIT_RECEIVED: {
                	 L_INFO(TAG, "eScanCmd_NIT_RECEIVED\n");
                    if(Service_Filter_List.erase((TableFilter*) msg.param))
                        manager.receiveNIT(msg.param);
                    getSITime_t(&manager.mLastOpSec);
                }
                    break;
                case eScanCmd_SDT_RECEIVED: {
                	 L_INFO(TAG, "eScanCmd_SDT_RECEIVED\n");
                    if(Service_Filter_List.erase((TableFilter*) msg.param))
                        manager.receiveSDT(msg.param);
                    getSITime_t(&manager.mLastOpSec);
                }
                    break;

                case eScanCmd_BAT_RECEIVED: {
                	 L_INFO(TAG, "eScanCmd_BAT_RECEIVED\n");
                	 if(Service_Filter_List.erase((TableFilter*) msg.param))
						manager.receiveBAT(msg.param);
					getSITime_t(&manager.mLastOpSec);
                }
                    break;

                case eScanCmd_SDT_OTHER_RECEIVED: {
//                	 L_INFO(TAG, "eScanCmd_SDT_OTHER_RECEIVED\n");
                    if(Service_Filter_List.erase((TableFilter*) msg.param))
                        manager.receiveSDTOther(msg.param);
                    getSITime_t(&manager.mLastOpSec);
                }
                    break;

                case eScanCmd_TDT_RECEIVED: {

//                    L_INFO(TAG, "eScanCmd_TDT_RECEIVED\n");
                    if(manager.isTDTTimeSet == false) {
                        L_INFO(TAG, "CMD : eScanCmd_TDT_RECEIVED\n");
                        getSITime_t(&manager.mLastOpSec);
                    }
					if(Service_Section_Filter_List.erase((SimpleFilter*) msg.param)) {
                        SimpleFilter* pFilter = (SimpleFilter*) msg.param;
                        manager.receiveTDT(msg.param);
#if 0 //
                        if(pFilter->isAttached()) {
                            pFilter->clearBuffer();
							if (pFilter->startFilter(current_uri)) {
								Service_Section_Filter_List.push_back(pFilter);
							} else {
								L_DEBUG(TAG,"\n");
								deleteSimpleFilter(pFilter);
							}
                        }
#else
                        deleteSimpleFilter(pFilter);
                        if(manager.getTDT()) {
                            getSITime_t(&manager.mLastOpSec);
                        }
#endif
                    }
                }
                    break;
                case eScanCmd_VCT_RECEIVED: {
                    if(Service_Filter_List.erase((TableFilter*) msg.param))
                        manager.receiveVCT(msg.param);
                    getSITime_t(&manager.mLastOpSec);
                }
                    break;

                case eScanCmd_PAT_RECEIVED: {
                    L_INFO(TAG, "eScanCmd_PAT_RECEIVED\n");
                    if(Service_Filter_List.erase((TableFilter*) msg.param))
                        manager.receivePAT(msg.param);
                    getSITime_t(&manager.mLastOpSec);
                }
                    break;
                case eScanCmd_PMT_RECEIVED: {
//                    L_INFO(TAG, "eScanCmd_PMT_RECEIVED\n");
                    if(Service_Section_Filter_List.erase((SimpleFilter*) msg.param))
                        manager.receivePMT(msg.param);
                    getSITime_t(&manager.mLastOpSec);
                }
                    break;
                case eScanCmd_VERSION_CHANGED: {
                	L_DEBUG(TAG,"\n");
                    MonitorFilter* pFilter = (MonitorFilter*) msg.param;
                    if(monitorList.erase(pFilter)) {
                        pFilter->detachFilter();
                        int tid = pFilter->m_tableID;
                        if(tid == 0x02) { //PMT
                        	L_DEBUG(TAG,"\n");
                            manager.pmt_count++;
                            manager.getPMT(pFilter->m_pid, pFilter->getTableIDExt());
                        } else {
                            if(manager.isDVB()) {
                                if(pFilter->m_pid == 0x10) { //NIT
                                	L_DEBUG(TAG,"NIT version changed\n");
                                    manager.RestartByNitChange();
                                } else if(pFilter->m_pid == 0x11) { //SDT
                                    clearAllFilter_DVB();
                                    if(tid == 0x42) {
                                        manager.sdt_count++;
                                        manager.getSDT();
//                                        ScanTransport* pTs = getTransportStreamInfo(manager.getcurrentTsid());
//                                        if(pTs) {
//                                            refresh_ts(pTs);
//                                        }
                                    } else if(tid == 0x46) {
                                        manager.sdt_count++;
                                        manager.getSDTOther(pFilter->getTableIDExt());
//                                        ScanTransport* pTs = getTransportStreamInfo(manager.getcurrentTsid());
//                                        if(pTs) {
//                                            refresh_ts(pTs);
//                                        }
                                    }
                                }
                                else L_DEBUG(TAG,"\n");
                            } else {
                            	L_DEBUG(TAG,"\n");
                            }
                        }
                        deleteMonitorFilter(pFilter);
                    }
                }
                    break;
                case eScanCmd_TimeOuted: {
                    if(manager.mLastOpSec == 0) {
                        manager.TimeOut();
                    }
                }
                    break;

                default:
                    break;
            }
            ScanManagerUnlock();
            siMiliSleep(5);
        }
    }
    L_INFO(TAG, "\n\n\n\n\n\n\nthread exit\n\n\n\n\n\n\n\n\n\n");
    pthread_exit (NULL);
}

void* scan_data_task(void* param)
{
    bool bInNeedScan = false;
    static int dataWorkCount = 0;
    ScanManager& manager = ScanManager::getInstance();
    Parser& parser = Parser::getInstance();
    static int findchannel = 0;

    if(manager.isDVB()) {
        parser.init("DVB", manager.msoName);
    } else {
        parser.init("PSIP", manager.msoName);
    }

    while (1) {
        ScanDataQLock();
        int Qresult = 0;
        ScanDataMsg msg = ScanDataQ.dequeue(Qresult);
        if(Qresult == 0) {
            ScanDataQUnlock();
            siMiliSleep(150); //sleep 15 msec
        } else {
            switch (msg.cmd) {
                case ScanDataCmd_TuneParam: {
#ifdef __DEBUG_LOG_SM__
                    L_INFO(TAG, "ScanDataCmd_TuneParam NitVer=0x%X\n",NitVer);
#endif
                    int total = 0;
                    if(NitVer != 0xFF || manager.bTestMode) {
                        total = ListTs.size();
                        L_INFO(TAG, "list size[%d]\n",total);
                    }
                    int current = (int) (msg.iparam);
                    L_INFO(TAG, "Scan Progress : %d / %d\n", current, total);
                    SIChangeEvent event;
                    event.setEventType(SIChangeEvent::SCAN_PROCESSING);
                    event.setReceived(current);
                    event.setToBeReceived(total);
                    if(manager.bTestMode) {
                        event.setFoundChannel(findchannel);
                        parser.receivedTable("SCAN_TEST", NULL, event);
                    } else {

                        parser.receivedTable("SCAN", NULL, event);
                    }
                }
                    break;
                case ScanDataCmd_SCAN_START: {
                    bInNeedScan = true;
#ifdef __DEBUG_LOG_SM__
                    L_INFO(TAG, "ScanDataCmd_SCAN_START NitVer=0x%X\n",NitVer);
#endif
                    SIChangeEvent event;
                    int total = 0;
                    if(NitVer != 0xFF || manager.bTestMode) {
                        total = ListTs.size();
                        L_INFO(TAG, "list size[%d]\n",total);
                    }
                    findchannel = 0;
                    event.setReceived(0);
                    event.setToBeReceived(total);
                    event.setEventType(SIChangeEvent::SCAN_READY);
                    if(manager.bTestMode) {
                        parser.receivedTable("SCAN_TEST", NULL, event);
                    } else {
                        parser.receivedTable("SCAN", NULL, event);
                    }
                    if(total == 0) {
                    	ScanDataMsg datamsg;
						datamsg.cmd = ScanDataCmd_SCAN_DEMUX_CLOSE;
						SendScanDataQ(datamsg);
                    }
                }
                    break;
                case ScanDataCmd_SCAN_END: {
                    bInNeedScan = false;
                    bScanCompleted = true;
                    if(manager.getScanType() != eScanMode_OOB) {
                        manager.bTunerLocked = false;
                        //TDI_Tuner_Stop(0);
                    }
                    L_INFO(TAG, "ScanDataCmd_SCAN_END\n");
                    SIChangeEvent event;
                    event.setEventType(SIChangeEvent::SCAN_COMPLETED);
                    if(manager.bTestMode) {
                        L_INFO(TAG, "findchannel = %d\n", findchannel);
                        event.setFoundChannel(findchannel);
                        parser.receivedTable("SCAN_TEST", NULL, event);
                    } else {
                        parser.receivedTable("SCAN", NULL, event);
                    }
                    ScanDataMsg datamsg;
					datamsg.cmd = ScanDataCmd_SCAN_DEMUX_CLOSE;
					SendScanDataQ(datamsg);
                }
                    break;

                case ScanDataCmd_SCAN_DEMUX_CLOSE:
                	L_DEBUG(TAG,"\n\n");
                	if(bcreatedDMX )
					{
                		L_DEBUG(TAG,"\n\n");
                		//manager.channelChangeBefore();
						MMF_Demux_Destroy(mDmxHandle);
						bcreatedDMX = false;
					}
                	break;

                case ScanDataCmd_SCAN_ERR_END: {
                    bInNeedScan = false;
                    bScanCompleted = true;
                    if(manager.getScanType() != eScanMode_OOB) {
                        manager.bTunerLocked = false;
                        //TDI_Tuner_Stop(0);
                    }
                    L_INFO(TAG, "ScanDataCmd_SCAN_ERR_END\n");
#if 1 // wjpark
                    if(bcreatedDMX )
                    {
                    	MMF_Demux_Destroy(mDmxHandle);
                    	bcreatedDMX = false;
                    }
#endif
                    SIChangeEvent event;
                    event.setEventType(SIChangeEvent::SCAN_COMPLETED);
                    event.setFoundChannel(-1);
                    if(manager.bTestMode) {
                        parser.receivedTable("SCAN_TEST", NULL, event);
                    } else {
                        parser.receivedTable("SCAN", NULL, event);
                    }
                    ScanDataMsg datamsg;
					datamsg.cmd = ScanDataCmd_SCAN_DEMUX_CLOSE;
					SendScanDataQ(datamsg);
                }
                    break;
                case ScanDataCmd_SCAN_STOP: {
                    bInNeedScan = false;
                    if(manager.getScanType() != eScanMode_OOB) {
                        manager.bTunerLocked = false;
                        //TDI_Tuner_Stop(0);
                    }
                    L_INFO(TAG, "ScanDataCmd_SCAN_STOP\n");
                    SIChangeEvent event;
                    event.setEventType(SIChangeEvent::SCAN_STOP);
                    if(manager.bTestMode) {
                        parser.receivedTable("SCAN_TEST", NULL, event);
                    } else {
                        parser.receivedTable("SCAN", NULL, event);
                    }
                    ScanDataMsg datamsg;
					datamsg.cmd = ScanDataCmd_SCAN_DEMUX_CLOSE;
					SendScanDataQ(datamsg);
                }
                    break;
                case ScanDataCmd_PSI_Ready: {
                    L_INFO(TAG, "ScanDataCmd_PSI_Ready\n");

                    SimpleFilter* pFilter = (SimpleFilter*) msg.param;
                    if(manager.bTestMode) {
                        findchannel++;
                    } else {
                        Section* pTable = (Section*) pFilter->m_pSection;
                        L_INFO(TAG, "PMT NUM=0x%X gCurrentChannelNum=%d\n", TableIDExt(pTable->m_pSection), gCurrentChannelNum);
                        SIChangeEvent event;
                        int tsid = msg.iparam;
                        event.setReceived(manager.getNwid());
                        event.setToBeReceived(tsid);
                        event.setEventType(SIChangeEvent::CHANNEL_PMT);

                        if(bInNeedScan || (TableIDExt(pTable->m_pSection) == gCurrentChannelNum)) {
                            L_INFO(TAG, "Send PMT NUM=0x%X\n", TableIDExt(pTable->m_pSection));
                            parser.receivedTable("PMT", pTable, event);
                        }
                    }
                    deleteSimpleFilter(pFilter);
                }
                    break;
                case ScanDataCmd_DVB_NIT: {
                    L_INFO(TAG, "CMD : ScanDataCmd_DVB_NIT\n");
                    TableFilter* pFilter = (TableFilter*) msg.param;
                    TableT* pTable = (TableT*) pFilter->m_pTable;
                    SIChangeEvent event;
                    event.setEventType(SIChangeEvent::UNKNOWN);
                    parser.receivedTable("NIT", pTable, event);
                    deleteTableFilter(pFilter);
                }
                    break;
                case ScanDataCmd_DVB_NIT_CHMAP_READY: {
//                	L_INFO(TAG, "CMD : ScanDataCmd_DVB_NIT_CHMAP_READY 1\n");
                	if(!bSendChannelReadyNotify) {
						L_INFO(TAG, "CMD : ScanDataCmd_DVB_NIT_CHMAP_READY \n");
						//TDI_Tuner_Stop(0);
						SIChangeEvent event;
						event.setEventType(SIChangeEvent::CHANNEL_MAP_READY);
						parser.receivedTable("NIT_CHMAP_READY", NULL, event);
						bSendChannelReadyNotify = true;
						manager.bTunerLocked = false;
						bChMapNoti = true;
                	} else {
                		manager.bTunerLocked = false;
						bChMapNoti = true;
                	}


                }
                    break;
                case ScanDataCmd_DVB_NIT_TIMEOUT: {
//                	L_INFO(TAG, "CMD : ScanDataCmd_DVB_NIT_TIMEOUT 1\n");
                    if(!bInNeedScan && !bSendChannelReadyNotify) {
                        L_INFO(TAG, "CMD : ScanDataCmd_DVB_NIT_TIMEOUT \n");
                        //TDI_Tuner_Stop(0);
                        SIChangeEvent event;
                        event.setEventType(SIChangeEvent::CHANNEL_MAP_READY);
                        parser.receivedTable("TIMEOUT_READY", NULL, event);
                        bSendChannelReadyNotify = true;
                        manager.bTunerLocked = false;
					    bChMapNoti = true;

					    if(bcreatedDMX  && bHomets)
						{
							L_DEBUG(TAG,"\n\n");
							MMF_Demux_Destroy(mDmxHandle);
							bcreatedDMX = false;
							bHomets = false;
						} else if(bHomets) {
							bsetForceDMXDestroy = true;
						}
                    }
                    else {
						manager.bTunerLocked = false;
						bChMapNoti = true;
                    }
                }
                    break;

                case ScanDataCmd_CH_MAP_COMPLTE: {
                	L_INFO(TAG, "CMD : ScanDataCmd_CH_MAP_COMPLTE \n");
					SIChangeEvent event;
					event.setEventType(SIChangeEvent::CHANNEL_MAP_READY);
					parser.receivedTable("TIMEOUT_READY", NULL, event);
					bSendChannelReadyNotify = true;
					manager.bTunerLocked = false;
					bChMapNoti = true;
				}
					break;



                case ScanDataCmd_DVB_SDT:
                case ScanDataCmd_DVB_SDT_COMPLETE: {
                    TableFilter* pFilter = (TableFilter*) msg.param;
                    TableT* pTable = (TableT*) pFilter->m_pTable;
                    SIChangeEvent event;
                    if(msg.cmd == ScanDataCmd_DVB_SDT) {
                        event.setEventType(SIChangeEvent::CHANNEL_COMPLETED);
                    } else {
#if SCANMANAGER_AGING_TEST
                        start_si_aging();
#endif                        
                        event.setEventType(SIChangeEvent::CHANNEL_MAP_COMPLETED);
                    }

                    if(!bSendChannelReadyNotify) {
                    	SIChangeEvent event;
                    	event.setEventType(SIChangeEvent::CHANNEL_MAP_READY);
                    	parser.receivedTable("NIT_CHMAP_READY", NULL, event);
                    	bSendChannelReadyNotify =true;
                    	manager.bTunerLocked = false;
						bChMapNoti = true;
                    }

                    L_INFO(TAG, "ScanDataCmd_DVB_SDT tid(0x%X) tsid(0x%X)\n", pTable->m_pSection[0][0], TableIDExt(pTable->m_pSection[0]));
                    parser.receivedTable("SDT", pTable, event);
                    deleteTableFilter(pFilter);

#if 1 // wjpark 2015.10.27
                    if(bcreatedDMX && msg.cmd == ScanDataCmd_DVB_SDT_COMPLETE && bHomets)
					{
                    	L_DEBUG(TAG,"\n\n");
//                    	manager.channelChangeBefore();
						MMF_Demux_Destroy(mDmxHandle);
						bcreatedDMX = false;
                    	bHomets = false;
                    	bChMapNoti = true;
                    	NitVer = 0xFF;
					}
#endif

                }
                	break;

                case ScanDataCmd_DVB_BAT: {
					TableFilter* pFilter = (TableFilter*) msg.param;
					TableT* pTable = (TableT*) pFilter->m_pTable;
					SIChangeEvent event;
					L_INFO(TAG, "ScanDataCmd_DVB_BAT tid(0x%X) tsid(0x%X)\n", pTable->m_pSection[0][0], TableIDExt(pTable->m_pSection[0]));
					parser.receivedTable("BAT", pTable, event);
					deleteTableFilter(pFilter);
                }
                    break;

                case ScanDataCmd_DVB_EIT:
                case ScanDataCmd_DVB_EIT_COMPLETE: {
                    TableFilter* pFilter = (TableFilter*) msg.param;
                    TableT* pTable = (TableT*) pFilter->m_pTable;
                    SIChangeEvent event;
                    if(msg.cmd == ScanDataCmd_DVB_EIT) {
#ifdef __DEBUG_LOG_SM__
                        L_INFO(TAG, "ScanDataCmd_DVB_EIT\n");
#endif
                        event.setEventType(SIChangeEvent::PROGRAM_COMPLETED);
                    } else {
                        L_INFO(TAG, "ScanDataCmd_DVB_EIT_COMPLETE\n");
                        event.setEventType(SIChangeEvent::PROGRAM_MAP_COMPLETED);
                    }

                    parser.receivedTable("EIT", (void*) pTable, event);
                    deleteTableFilter(pFilter);
                }
                    break;
                case ScanDataCmd_DVB_TDT: {
                    //L_INFO(TAG, "ScanDataCmd_DVB_TDT\n");
                    Section* pSection = (Section*) msg.param;
                    SIChangeEvent event;
                    event.setEventType(SIChangeEvent::TDT_DATE);

                    parser.receivedTable("TDT", pSection, event);
                    delete pSection;
                }
                    break;
                case ScanDataCmd_PSIP_TVCT: {
                    L_INFO(TAG, "ScanDataCmd_PSIP_TVCT\n");
                    TableFilter* pFilter = (TableFilter*) msg.param;
                    TableT* pTable = (TableT*) pFilter->m_pTable;

                    //Call Parser Interface
                    SIChangeEvent event;
                    event.setEventType(SIChangeEvent::CHANNEL_MAP_COMPLETED);
                    parser.receivedTable("TVCT", (void*) pTable, event);

                    deleteTableFilter(pFilter);
                }
                    break;
                case ScanDataCmd_PSIP_CVCT: {
                    L_INFO(TAG, "ScanDataCmd_PSIP_CVCT\n");
                    TableFilter* pFilter = (TableFilter*) msg.param;
                    TableT* pTable = (TableT*) pFilter->m_pTable;

                    //Call Parser Interface
                    SIChangeEvent event;
                    event.setEventType(SIChangeEvent::CHANNEL_MAP_COMPLETED);
                    parser.receivedTable("CVCT", (void*) pTable, event);

                    deleteTableFilter(pFilter);
                }
                    break;
                case ScanDataCmd_PSIP_EIT_COMPLETE:
                case ScanDataCmd_PSIP_EIT: {
                    TableFilter* pFilter = (TableFilter*) msg.param;
                    TableT* pTable = (TableT*) pFilter->m_pTable;

                    //Call Parser Interface
                    SIChangeEvent event;
                    if(msg.cmd == ScanDataCmd_PSIP_EIT_COMPLETE) {
                        L_INFO(TAG, "ScanDataCmd_PSIP_EIT_COMPLETE\n");
                        event.setEventType(SIChangeEvent::PROGRAM_MAP_COMPLETED);
                    } else {
                        L_INFO(TAG, "ScanDataCmd_PSIP_EIT\n");
                        event.setEventType(SIChangeEvent::PROGRAM_COMPLETED);
                    }
                    parser.receivedTable("PSIPEIT", pTable, event);

                    deleteTableFilter(pFilter);
                }
                    break;
                case ScanDataCmd_PSIP_ETT: {
                    L_INFO(TAG, "ScanDataCmd_PSIP_ETT\n");
                    SimpleFilter* pFilter = (SimpleFilter*) msg.param;
                    Section* pSection = (Section*) pFilter->m_pSection;

                    //Call Parser Interface
                    SIChangeEvent event;
                    event.setEventType(SIChangeEvent::PROGRAM_UPDATED);
                    parser.receivedTable("ETT", pSection, event);

                    deleteSimpleFilter(pFilter);
                }
                    break;
            }
            ScanDataQUnlock();
            siMiliSleep(5);
        }
    }

    L_INFO(TAG, "\n\n\n\n\n\n\nthread exit\n\n\n\n\n\n\n\n\n\n");
    pthread_exit (NULL);
}

//internal function
ScanManager::~ScanManager()
{
}
void ScanManager::configSIScan()
{
    bUsePSI = false;
    bUseDVB = true;
    bUsePSIP = false;
    bUseOtherTS = false;
    bReceiveEITInScan = true;
    bEITBoost = true;
    modify = 1;
    max_receive_eit_idx = 0;
    mScanMode = eScanMode_OOB;

    string strXML = "";
    try {
#ifdef ANDROID
#ifdef __FOR_LINKNET__
        strXML = get_file_contents("/data/lkn_home/config/si/schema/scanconfig.xml");
#endif
#ifdef __COMMON_MODULE__
        strXML = get_file_contents("/data/tvs/si/config/schema/scanconfig.xml");
#endif
#else
        strXML = get_file_contents("schema/scanconfig.xml");
#endif
    } catch (int err) {
        L_ERROR(TAG, "=== OOPS : errno : %d\n", err);
        return;
    }

    TiXmlDocument doc;
    doc.Parse(strXML.c_str());
    TiXmlNode* node = doc.FirstChild(msoName.c_str());
    TiXmlNode* childNode;
    if(node == NULL) {
        return;
    }

    for (childNode = node->FirstChild(); childNode; childNode = childNode->NextSibling()) {
        std::string nodeName = "";
        nodeName.append(childNode->Value());
        if(nodeName.compare("AP") == 0) { //tuner ?�정
            std::string type = "";
            type.append(childNode->ToElement()->Attribute("TUNER_TYPE"));
            if(type.compare("i") == 0) {
                scan_tuner->type = TDI_TunerType_Ip;
                std::string protocol = "";
                protocol.append(childNode->ToElement()->Attribute("PROTOCOL"));
                if(protocol.compare("udp") == 0) {
                    scan_tuner->protocol = TDI_TunerProtocol_Udp;
                } else if(protocol.compare("rtp") == 0) {
                    scan_tuner->protocol = TDI_TunerProtocol_Rtp;
                }
                sprintf(scan_tuner->parameter.ip.ip, "%s", childNode->ToElement()->Attribute("ip_addr"));
                scan_tuner->parameter.ip.port = atoi(childNode->ToElement()->Attribute("port_no"));
            } else if(type.compare("c") == 0) {
                scan_tuner->type = TDI_TunerType_Cable;
                std::string protocol = "";
                protocol.append(childNode->ToElement()->Attribute("PROTOCOL"));
                if(protocol.compare("qam64") == 0) {
                    scan_tuner->protocol = TDI_TunerProtocol_64QAM;
                } else if(protocol.compare("qam256") == 0) {
                    scan_tuner->protocol = TDI_TunerProtocol_256QAM;
                }
                scan_tuner->parameter.rf.frequencyHz = (atoi(childNode->ToElement()->Attribute("FREQ"))) * 1000;
                scan_tuner->parameter.rf.symbolrate = atoi(childNode->ToElement()->Attribute("SYMBOLRATE"));
            } else if(type.compare("s") == 0) {
                scan_tuner->type = TDI_TunerType_Satellite;
                std::string protocol = "";
                protocol.append(childNode->ToElement()->Attribute("PROTOCOL"));
                if(protocol.compare("qpsk") == 0) {
                    scan_tuner->protocol = TDI_TunerProtocol_QPSK;
                }
            } else if(type.compare("t") == 0) {
                scan_tuner->type = TDI_TunerType_Terrestrial;
                std::string protocol = "";
                protocol.append(childNode->ToElement()->Attribute("PROTOCOL"));
                if(protocol.compare("dvbt") == 0) {
                    scan_tuner->protocol = TDI_TunerProtocol_DVBT;
                } else if(protocol.compare("dvbt2") == 0) {
                    scan_tuner->protocol = TDI_TunerProtocol_DVBT2;
                } else if(protocol.compare("vsb8") == 0) {
                    scan_tuner->protocol = TDI_TunerProtocol_8VSB;
                }
                scan_tuner->parameter.rf.frequencyHz = (atoi(childNode->ToElement()->Attribute("FREQ"))) * 1000;
                int bandwidth = atoi(childNode->ToElement()->Attribute("BW"));
                scan_tuner->parameter.rf.bandwidth = bandwidth - 4;
            } else {
            }
        } else if(nodeName.compare("PSI") == 0) {
            bUsePSI = (bool) atoi(childNode->ToElement()->Attribute("INCLUDE"));
        } else if(nodeName.compare("DVB") == 0) {
            bUseDVB = (bool) atoi(childNode->ToElement()->Attribute("INCLUDE"));
        } else if(nodeName.compare("PSIP") == 0) {
            bUsePSIP = (bool) atoi(childNode->ToElement()->Attribute("INCLUDE"));
        } else if(nodeName.compare("OTHER_TS") == 0) {
            bUseOtherTS = (bool) atoi(childNode->ToElement()->Attribute("INCLUDE"));
        } else if(nodeName.compare("RECEIVE_EIT_IN_SCAN") == 0) {
            bReceiveEITInScan = (bool) atoi(childNode->ToElement()->Attribute("INCLUDE"));
        } else if(nodeName.compare("FILTER_MODIFY") == 0) {
            modify = atoi(childNode->ToElement()->Attribute("NUMBER"));
        } else if(nodeName.compare("MAX_EIT_SCHEDULE") == 0) {
            max_receive_eit_idx = atoi(childNode->ToElement()->Attribute("COUNT")) - 1;
        } else if(nodeName.compare("SCAN_MODE") == 0) {
            mScanMode = (SCAN_MODE) atoi(childNode->ToElement()->Attribute("INCLUDE"));
        } else if(nodeName.compare("EIT_BOOST_MODE") == 0) {
            bEITBoost = (bool) atoi(childNode->ToElement()->Attribute("INCLUDE"));
        } else if(nodeName.compare("TIMEOUT") == 0) {
            SCAN_MANAGER_TIMEOUT_SEC = (time_t) atoi(childNode->ToElement()->Attribute("TIME_SEC"));
            timeoutInSec = (int) SCAN_MANAGER_TIMEOUT_SEC;
        }
    }
    if(max_receive_eit_idx < 0) {
        bReceiveEITInScan = false;
    }
    if(bReceiveEITInScan == false) {
        bEITBoost = false;
    }

    if(bUsePSI == false) {
        psiCompleteInChannel = true;
    }
    if(bUseDVB == false) {
        dvbCompleteInChannel = true;
    }

    bEITBoostBackup = bEITBoost;
    bUsePSIBackup = bUsePSI;
    bUseDVBBackup = bUseDVB;
    bUsePSIPBackup = bUsePSIP;
    bUseOtherTSBackup = bUseOtherTS;
    bReceiveEITInScanBackup = bReceiveEITInScan;
    mScanModeBackup = mScanMode;
    L_DEBUG(TAG,"mScanModeBackup : [%d]\n",mScanModeBackup	);
    memcpy(&scan_tuner_ap, scan_tuner, sizeof(TDI_TunerSettings));
}

void ScanManager::tune_result(int eventId, int freq , bool bScanMode)
{
    if(mManagerMode == eIdle)
        return;

    if(NitVer != 0xFF && bBoot == false)
        bBoot = true;

    if(eventId == PLAY_INFO_TUNER_LOCKED && bScanMode) {
        L_INFO(TAG, "CALLBACK : PLAY_INFO_TUNER_LOCKED\n");
        bBlockSectionMessage=false;
        L_DEBUG(TAG,"mManagerMode : [%d], bfirstTunerLock[%d]\n",mManagerMode,bfirstTunerLock);
        if(mManagerMode == eFirstConnect && bfirstTunerLock == false || bNeedScan ) {
        	channelChangeAfter(freq);
        	bfirstTunerLock = true;
        }
        bTunerLocked = true;
        if(bTestMode && pCurrentTs)
            pCurrentTs->bLocked = true;
	}
    else if(eventId == PLAY_INFO_TUNER_LOCK_FAILED && bScanMode) {
		L_INFO(TAG, "CALLBACK : PLAY_INFO_TUNER_LOCK_FAILED\n");
		bTunerLocked = false;
		if(bTestMode) {
			if(pCurrentTs)
				pCurrentTs->bLocked = false;
			requestTuneNext();
		}else {
			if(mManagerMode == eInScanning) {
				requestTuneNext();
			} else if(mManagerMode == eFirstConnect) {
				FirstConnectTuneFailHandler();
			}
		}
    } else if (eventId == PLAY_INFO_STOPPED ) {
    	L_INFO(TAG, "CALLBACK : PLAY_INFO_STOPPED \n");
        bBlockSectionMessage=true;
    	int scanQsize = ScanManageQ.size();
		int ScanQResult = 0;
		for (int qcount = 0; qcount < scanQsize; qcount++) {
			ScanQResult = 0;
			ScanMsg msg = ScanManageQ.dequeue(ScanQResult);
			if(ScanQResult) {
				switch (msg.cmd) {
					case eScanCmd_TUNE_START:
						L_DEBUG(TAG,"\n");
						ScanManageQ.enqueue(msg);
						 L_INFO(TAG, "\n");
						break;
					default:
						break;
				}
			}
		}
		ScanMsg msg;
		msg.cmd = eScanCmd_CHCHANGE_BEFORE;
		SendScanManageQ(msg, true);
		bTunerLocked = false;
    } else if (eventId == PLAY_INFO_STARTED && !bScanMode) {
    	L_INFO(TAG, "CALLBACK : PLAY_INFO_STARTED\n");

//    	 if(bHomets) {
//			MMF_Demux_Destroy(mDmxHandle);
//			bcreatedDMX = false;
//			bHomets = false;
//			bChMapNoti = true;
//			NitVer = 0xFF;
//		}

        bBlockSectionMessage=false;
		channelChangeAfter(freq);
		bTunerLocked = true;
		if(bTestMode && pCurrentTs)
			pCurrentTs->bLocked = true;
	}
}

void ScanManager::sendChannelComplete()
{
	ScanDataMsg msg;
	msg.cmd = ScanDataCmd_CH_MAP_COMPLTE;
	SendScanDataQ(msg);
}

void ScanManager::start_DVB()
{
	L_DEBUG(TAG,"\n");
    ScanMsg msg;
    sdt_count = 0;
    bat_count = 0;
    eit_pf_count = 0;
    eit_sch_count = 0;
    dvbCompleteInChannel = false;

    msg.cmd = eScanCmd_TDT_REQUEST;
    SendScanManageQ(msg);
    L_DEBUG(TAG,"NIT Version [0x%x] \n",NitVer);
    if(NitVer == 0xFF) {
        msg.cmd = eScanCmd_NIT_REQUEST;
        SendScanManageQ(msg);
    }else if(mManagerMode != eInScanning ) {
    	// nit monitor mode
    	L_DEBUG(TAG,"\n");
    	monitor_DVBService();
    	start_PSI();
    }
//    msg.cmd = eScanCmd_BAT_REQUEST;
//	SendScanManageQ(msg);

}

void ScanManager::monitor_DVBService()
{
    sdt_other_requested = true;

    int ret, sizeN, res;
    sdt_count = 0;
    if(bUseOtherTS) {
        ScanMsg msg;
        ListTs.lock();
        sizeN = ListTs.sizeNoLock();
        L_DEBUG(TAG,"list size [%d]\n",sizeN);
        bool requestOtherTs = false;
        for (int idx = 0; idx < sizeN; idx++) {
            res = 0;
            ScanTransport* pTs = ListTs.AtNoLock(idx, res);
            if(res == 0) {
//            	L_INFO(TAG,"\n");
                continue;
            }
            if(pTs) {
//            	L_INFO(TAG,"\n");
                if(pTs->sdt_version != 0xFF) {
//                	L_INFO(TAG,"\n");
                    MonitorFilter *pMf = new MonitorFilter(0x11, 0x42, pTs->sdt_version, pTs->tsid);
                    if(pMf) {
                        pMf->setCallbackFunction(monitor_cb);
                        pMf->setTableIDMask(0xfb);
                        ret = pMf->startFilter(current_uri);
                        if(ret) {
                            monitorList.push_back(pMf);
                        } else {
                            deleteMonitorFilter(pMf);
                        }
                    }
                } else {
                    if(pTs->tsid == current_tsid) {
                    	L_INFO(TAG,"\n");
                        msg.cmd = eScanCmd_SDT_REQUEST;
                        SendScanManageQ(msg);
                    } else {
//                    	L_INFO(TAG,"\n");
                        requestOtherTs = true;
                        OtherTSIDQ.enqueue(pTs->tsid);
                    }
                    sdt_count++;
                }
            }
//            else L_INFO(TAG,"\n");
        }
        if(requestOtherTs) {
        	L_DEBUG(TAG,"\n\n");
            msg.cmd = eScanCmd_SDT_OTHER_REQUEST;
            SendScanManageQ(msg);
        }
        ListTs.unlock();
    } else {
        ScanTransport* pTs = getTransportStreamInfo(current_tsid);
        if(pTs) {
            if(pTs->sdt_version != 0xFF) {
            	L_DEBUG(TAG,"\n\n");
                MonitorFilter *pMf = new MonitorFilter(0x11, 0x42, pTs->sdt_version, pTs->tsid);
                L_INFO(TAG,"\n");
                if(pMf) {
                    pMf->setCallbackFunction(monitor_cb);
                    ret = pMf->startFilter(current_uri);
                    if(ret) {
                        monitorList.push_back(pMf);
                    } else {
                        deleteMonitorFilter(pMf);
                    }
                }
            } else {
            	L_DEBUG(TAG,"\n\n");
                ScanMsg msg;
                msg.cmd = eScanCmd_SDT_REQUEST;
                SendScanManageQ(msg);
                sdt_count++;
            }
        }
    }
    L_INFO(TAG, "SDT Count=%d ,NitVer[0x%x]\n", sdt_count,NitVer);

    MonitorFilter *pMf = new MonitorFilter(0x10, 0x40, NitVer);
    if(pMf) {
    	L_DEBUG(TAG,"\n\n");
        pMf->setCallbackFunction(monitor_cb);
        ret = pMf->startFilter(current_uri);
        if(ret) {
        	L_DEBUG(TAG,"\n");
            monitorList.push_back(pMf);
        } else {
        	L_DEBUG(TAG,"\n");
            deleteMonitorFilter(pMf);
        }
    }
}

void ScanManager::start_PSIP()
{

    ScanMsg msg;
    msg.cmd = eScanCmd_VCT_REQUEST;
    SendScanManageQ(msg);
}

void ScanManager::start_PSI()
{
    if(bBoot || bNeedScan )
    {
        psiCompleteInChannel = false;
        ScanMsg msg;
        msg.cmd = eScanCmd_PAT_REQUEST;
        SendScanManageQ(msg);
    }
}

void ScanManager::requestTuneNext()
{
	if(bScanCompleted) {
		L_DEBUG(TAG,"Already Scan Completed \n");
		return;
	}
    if(bTestMode) {
        if(bTunerLocked == true) {
            ScanTransport* pTs = getNextTs();
            if(pTs == NULL) {
            	L_INFO(TAG, "\n");
                change2UpdateMode();
                bNeedScan = false;
                return;
            }
            pTs->scantried = true;
            pCurrentTs = pTs;
        } else {
            if(bFixedModulation) {
            	L_INFO(TAG, "\n");
                ScanTransport* pTs = getNextTs();
                if(pTs == NULL) {

                    change2UpdateMode();
                    bNeedScan = false;
                    return;
                }
                pTs->scantried = true;
                pCurrentTs = pTs;
            } else {
            	L_INFO(TAG, "\n");
                // eschoi_20140821 | not fixed moduration for auto (64 & 256 QAM)
                if(scan_tuner->protocol == TDI_TunerProtocol_64QAM) {
                	L_INFO(TAG, "\n");
#if 0
                    scan_tuner->protocol = TDI_TunerProtocol_128QAM;
                    pCurrentTs->deliverySystem.inbnd.modulation = TDI_TunerProtocol_128QAM;
                } else if(scan_tuner->protocol == TDI_TunerProtocol_128QAM) {
#endif
                    scan_tuner->protocol = TDI_TunerProtocol_256QAM;
                    pCurrentTs->deliverySystem.inbnd.modulation = TDI_TunerProtocol_256QAM;
                } else {
                	L_INFO(TAG, "\n");
                    ScanTransport* pTs = getNextTs();
                    if(pTs == NULL) {
                        change2UpdateMode();
                        bNeedScan = false;
                        return;
                    }
                    pTs->scantried = true;
                    pCurrentTs = pTs;
                }
            }
        }
        if(pCurrentTs->deliveryType == eDeliveryTypeInBand) {
        	L_INFO(TAG, "\n");
            scan_tuner->protocol = (TDI_TunerProtocol) pCurrentTs->deliverySystem.inbnd.modulation;
            scan_tuner->parameter.rf.frequencyHz = pCurrentTs->deliverySystem.inbnd.freq;
            scan_tuner->parameter.rf.symbolrate = pCurrentTs->deliverySystem.inbnd.symbolrate;
            scan_tuner->parameter.rf.bandwidth = pCurrentTs->deliverySystem.inbnd.bandwidth;
        } else if(pCurrentTs->deliveryType == eDeliveryTypeC) {
        	L_INFO(TAG, "\n");
            scan_tuner->protocol = (TDI_TunerProtocol) pCurrentTs->deliverySystem.cable.modulation;
            scan_tuner->parameter.rf.frequencyHz = pCurrentTs->deliverySystem.cable.freq;
            //scan_tuner->parameter.rf.symbolrate = pCurrentTs->deliverySystem.cable.symbolrate;
            scan_tuner->parameter.rf.bandwidth = bandwidthkHz / 1000;
        }

        ScanMsg msg;
        msg.cmd = eScanCmd_TUNE_START;
        msg.iparam = 1; // scan mode
        msg.param = (void*) scan_tuner;
        SendScanManageQ(msg, true);
        L_INFO(TAG, "\n");
    } else {
        if(bUseDVB && mScanMode != eScanMode_Normal_InBand) {
            if(ts_map_ready) {
                ScanTransport* pTs = getNextTs();
                if(pTs == NULL) {
                    change2UpdateMode();
                    bNeedScan = false;
                    ScanDataMsg datamsg;
                    datamsg.cmd = ScanDataCmd_SCAN_END;
                    SendScanDataQ(datamsg);
                    mManagerMode = eInScanEnd;
                    return;
                }
                pCurrentTs = pTs;
                pTs->scantried = true;

                switch (pTs->deliveryType) {
                    case eDeliveryTypeUnknown: {
                        L_INFO(TAG, "\t### nextTune(unknown=%d)\n", scan_tuner->parameter.rf.frequencyHz);
#if 0                        
                        scan_tuner->type = TDI_TunerType_Ip;
                        scan_tuner->protocol = (TDI_TunerProtocol) TDI_TunerProtocol_Udp;
                        sprintf(scan_tuner->parameter.ip.ip, "%s", "239.192.60.3");
                        scan_tuner->parameter.ip.port = 49200;
#endif
                    }
                        break;
                    case eDeliveryTypeC: {
                        scan_tuner->type = TDI_TunerType_Cable;
                        scan_tuner->protocol = pTs->deliverySystem.cable.modulation;
                        scan_tuner->parameter.rf.frequencyHz = pTs->deliverySystem.cable.freq;
                        scan_tuner->parameter.rf.symbolrate = pTs->deliverySystem.cable.symbol_rate;

                        L_INFO(TAG, "\t### nextTune(freq=%d)\n", scan_tuner->parameter.rf.frequencyHz);
                    }
                        break;
                    case eDeliveryTypeT: {
                        scan_tuner->type = TDI_TunerType_Terrestrial;
                        scan_tuner->protocol = TDI_TunerProtocol_DVBT; //pTs->deliverySystem.terr.constellation;
                        scan_tuner->parameter.rf.frequencyHz = pTs->deliverySystem.terr.freq;
                        scan_tuner->parameter.rf.symbolrate = 0;
                    }
                        break;
                    case eDeliveryTypeS: {
                        scan_tuner->type = TDI_TunerType_Satellite;
                    }
                        break;
                    case eDeliveryTypeInBand: {
                        scan_tuner->type = TDI_TunerType_Terrestrial;
                        scan_tuner->protocol = TDI_TunerProtocol_8VSB; //pTs->deliverySystem.inbnd.modulation;
                        scan_tuner->parameter.rf.frequencyHz = pTs->deliverySystem.inbnd.freq;
                    }
                        break;
                }

                ScanMsg msg;
                msg.cmd = eScanCmd_TUNE_START;
                msg.iparam = 1; // scan mode
                msg.param = (void*) scan_tuner;
                SendScanManageQ(msg);
                L_DEBUG(TAG,"\n");
            }
        } else if(mScanMode == eScanMode_Normal_InBand) {
            if(bTunerLocked == true) {
                ScanTransport* pTs = getNextTs();
                if(pTs == NULL) {

                    change2UpdateMode();
                    bNeedScan = false;
                    return;
                }
                pTs->scantried = true;
                pCurrentTs = pTs;
            } else {
#if 0			//64QAM�� ó��                if(scan_tuner->protocol == TDI_TunerProtocol_64QAM)
                {
                    scan_tuner->protocol = TDI_TunerProtocol_256QAM;
                    pCurrentTs->inbnd.modulation = TDI_TunerProtocol_256QAM;
                }
                else
#endif
                {
                    ScanTransport* pTs = getNextTs();
                    if(pTs == NULL) {

                        change2UpdateMode();
                        bNeedScan = false;
                        return;
                    }
                    pTs->scantried = true;
                    pCurrentTs = pTs;
                }
            }

            scan_tuner->protocol = (TDI_TunerProtocol) pCurrentTs->deliverySystem.inbnd.modulation;
            scan_tuner->parameter.rf.frequencyHz = pCurrentTs->deliverySystem.inbnd.freq;
            scan_tuner->parameter.rf.symbolrate = pCurrentTs->deliverySystem.inbnd.symbolrate;
            scan_tuner->parameter.rf.bandwidth = pCurrentTs->deliverySystem.inbnd.bandwidth;

            ScanMsg msg;
            msg.cmd = eScanCmd_TUNE_START;
            msg.iparam = 1; // scan mode
            msg.param = (void*) scan_tuner;
            SendScanManageQ(msg, true);
            L_DEBUG(TAG,"\n");
        }
    }
}

void ScanManager::FirstConnectTuneFailHandler()
{
    TimeOut();
}

void ScanManager::TuneHomeTS()
{
	L_DEBUG(TAG,"\n");
    request_tune_ap = true;
    bHomets = true;

    ScanMsg msg;
    msg.cmd = eScanCmd_TUNE_START;
    msg.param = (void*) (&scan_tuner_ap);
    msg.iparam = 0; // tune home ts
    SendScanManageQ(msg, true);
}

void ScanManager::requestEITBoost(ScanTransport* pTs)
{
    if(set_current_ts && max_receive_eit_idx >= 0 && pTs) {
        if(bUseOtherTS) {
            if(pTs->sdt_version == 0xFF) {
                if(pTs->tsid == current_tsid) {
                    int res;
                    int sizeN = pTs->serviceList.sizeNoLock();
                    for (int itr = 0; itr < sizeN; itr++) {
                        ScanService* pScanService = pTs->serviceList.AtNoLock(itr, res);
                        if(res && pScanService
                                && (pScanService->service_type == 0x01 || pScanService->service_type == 0x11 || pScanService->service_type == 0x16
                                        || pScanService->service_type == 0x19)) {
                            if(pScanService->eit_pf_mode == eEIT_NoOp)
                                pScanService->eit_pf_mode = eEIT_Standby;
                            for (int idx = 0; idx <= max_receive_eit_idx; idx++) {
                                if(pScanService->eit_schedule_mode[idx] == eEIT_NoOp)
                                    pScanService->eit_schedule_mode[idx] = eEIT_Standby;
                            }
                        }
                    }
                    //pTs->serviceList.unlock();
                    ScanMsg msg;
                    msg.param = (void*) pTs;
                    msg.mask = 0xff;
                    msg.cmd = eScanCmd_EIT_PF_REQUEST;
                    SendScanManageQ(msg);

                    for (int idx = 0; idx <= max_receive_eit_idx; idx++) {
                        msg.mask = 0xf0;
                        msg.cmd = eScanCmd_EIT_SCH_REQUEST;
                        msg.iparam = 0x50 + idx;
                        SendScanManageQ(msg);
                    }
                } else {
                    //pTs->serviceList.lock();
                    int res;
                    int sizeN = pTs->serviceList.sizeNoLock();
                    for (int itr = 0; itr < sizeN; itr++) {
                        ScanService* pScanService = pTs->serviceList.AtNoLock(itr, res);
                        if(res && pScanService
                                && (pScanService->service_type == 0x01 || pScanService->service_type == 0x11 || pScanService->service_type == 0x16
                                        || pScanService->service_type == 0x19)) {
                            if(pScanService->eit_pf_mode == eEIT_NoOp)
                                pScanService->eit_pf_mode = eEIT_Standby;
                            for (int idx = 0; idx <= max_receive_eit_idx; idx++) {
                                if(pScanService->eit_schedule_mode[idx] == eEIT_NoOp)
                                    pScanService->eit_schedule_mode[idx] = eEIT_Standby;
                            }
                        }
                    }
                    //pTs->serviceList.unlock();
                    ScanMsg msg;
                    msg.param = (void*) pTs;
                    msg.mask = 0xff;
                    msg.cmd = eScanCmd_EIT_PF_OTHER_REQUEST;
                    SendScanManageQ(msg);

                    for (int idx = 0; idx <= max_receive_eit_idx; idx++) {
                        msg.mask = 0xf0;
                        msg.cmd = eScanCmd_EIT_SCH_OTHER_REQUEST;
                        msg.iparam = 0x60 + idx;
                        SendScanManageQ(msg);
                    }
                }
            }
        } else {
            if(pTs->tsid == current_tsid && pTs->sdt_version == 0xFF) {
                //pTs->serviceList.lock();
                int res;
                int sizeN = pTs->serviceList.sizeNoLock();
                for (int itr = 0; itr < sizeN; itr++) {
                    ScanService* pScanService = pTs->serviceList.AtNoLock(itr, res);
                    if(res && pScanService
                            && (pScanService->service_type == 0x01 || pScanService->service_type == 0x11 || pScanService->service_type == 0x16 || pScanService->service_type == 0x19)) {
                        if(pScanService->eit_pf_mode == eEIT_NoOp)
                            pScanService->eit_pf_mode = eEIT_Standby;
                        for (int idx = 0; idx <= max_receive_eit_idx; idx++) {
                            if(pScanService->eit_schedule_mode[idx] == eEIT_NoOp)
                                pScanService->eit_schedule_mode[idx] = eEIT_Standby;
                        }
                    }
                }
                //pTs->serviceList.unlock();
                ScanMsg msg;
                msg.param = (void*) pTs;
                msg.mask = 0xff;
                msg.cmd = eScanCmd_EIT_PF_REQUEST;
                SendScanManageQ(msg);

                for (int idx = 0; idx <= max_receive_eit_idx; idx++) {
                    msg.mask = 0xf0;
                    msg.cmd = eScanCmd_EIT_SCH_REQUEST;
                    msg.iparam = 0x50 + idx;
                    SendScanManageQ(msg);
                }
            }
        }
    } else {
        if(pTs->sdt_version == 0xFF) {
            int res;
            int sizeN = pTs->serviceList.sizeNoLock();
            for (int itr = 0; itr < sizeN; itr++) {
                res = 0;
                ScanService* pScanService = pTs->serviceList.AtNoLock(itr, res);
                if(res && pScanService
                        && (pScanService->service_type == 0x01 || pScanService->service_type == 0x11 || pScanService->service_type == 0x16 || pScanService->service_type == 0x19)) {
                    if(pScanService->eit_pf_mode == eEIT_NoOp)
                        pScanService->eit_pf_mode = eEIT_Standby;
                    for (int idx = 0; idx <= max_receive_eit_idx; idx++) {
                        if(pScanService->eit_schedule_mode[idx] == eEIT_NoOp)
                            pScanService->eit_schedule_mode[idx] = eEIT_Standby;
                    }
                }
            }

            ScanMsg msg;
            msg.param = (void*) pTs;
            msg.mask = 0xfe;
            msg.cmd = eScanCmd_EIT_PF_REQUEST;
            SendScanManageQ(msg);

            for (int idx = 0; idx <= max_receive_eit_idx; idx++) {
                msg.mask = 0xc0;
                msg.cmd = eScanCmd_EIT_SCH_REQUEST;
                msg.iparam = 0x50 + idx;
                SendScanManageQ(msg);
            }
        }
    }
}

void ScanManager::receiveNIT(void* filterhandle)
{

    TableFilter* pFilter = (TableFilter*) filterhandle;
    TableT* pTable = NULL;
    if(pFilter!=NULL) pTable = pFilter->m_pTable;
    if(pFilter==NULL || pTable==NULL)
        return;
    pFilter->detachFilter();
    ScanMsg msg;
    bool changeSomething = false;

    ts_map_ready = true;
    if(pTable) {
        ParseNIT(pTable, &current_nwid, changeSomething);
    }
    if(bTestMode) {
        ScanDataMsg datamsg;
        datamsg.cmd = ScanDataCmd_SCAN_START;
        SendScanDataQ(datamsg);
        change2ScanMode();
        return;
    }
    L_DEBUG(TAG,"bChMapNoti [%d], bHomets[%d]\n",bChMapNoti,bHomets);
    L_DEBUG(TAG,"bBoot[%d], bUseOtherTS[%d] ,set_current_ts[%d] ,sdt_other_requested[%d]\n",bBoot, bUseOtherTS, set_current_ts, sdt_other_requested);
    if(bChMapNoti )
    {
            //request actual SDT
    		L_DEBUG(TAG,"request actual SDT\n");
            sdt_count = 1;
            msg.cmd = eScanCmd_SDT_REQUEST;
            SendScanManageQ(msg);
//            L_DEBUG(TAG,"eScanCmd_SDT_REQUEST , bBoot[%d], bUseOtherTS[%d] ,set_current_ts[%d] ,sdt_other_requested[%d]\n",bBoot, bUseOtherTS, set_current_ts, sdt_other_requested);

            if(bBoot && bUseOtherTS && set_current_ts && sdt_other_requested==false)
			{
                //request other SDT
                makeTSIDOtherList();
                sdt_count += (OtherTSIDQ.size());

                sdt_other_requested = true;
                L_DEBUG(TAG,"\n\n");
                msg.cmd = eScanCmd_SDT_OTHER_REQUEST;
                SendScanManageQ(msg);
            }
            L_INFO(TAG, "SDT Count=%d\n", sdt_count);
    } else if(bHomets) {
    	L_DEBUG(TAG,"\n");
		sdt_count++;
		msg.cmd = eScanCmd_SDT_REQUEST;
		SendScanManageQ(msg);
//    	TimeOut();
//    	monitor_DVBService();
    }
    L_DEBUG(TAG,"sdt_count[%d] \n",sdt_count);
    //notify
    ScanDataMsg datamsg;
    if(mManagerMode == eFirstConnect && bNeedScan) {
        scanTsCnt = 1;

        datamsg.cmd = ScanDataCmd_SCAN_START;
        SendScanDataQ(datamsg);

        datamsg.cmd = ScanDataCmd_TuneParam;
        datamsg.iparam = scanTsCnt;
        SendScanDataQ(datamsg);
        L_DEBUG(TAG,"\n");
    }

    if(changeSomething) {
        datamsg.param = filterhandle;
        datamsg.cmd = ScanDataCmd_DVB_NIT;
        SendScanDataQ(datamsg);
    }

    if(isTDTTimeSet && bChMapNoti) {
        if(sdt_count == 0) {
        	L_DEBUG(TAG,"\n\n");
            datamsg.cmd = ScanDataCmd_DVB_SDT_COMPLETE;
        }
        //send sdt
        int waitQSize = ScanDataWaitQ.size();
        for (int wqloop = 0; wqloop < waitQSize; wqloop++) {
            int qres = 0;
            ScanDataMsg datawaitmsg = ScanDataWaitQ.dequeue(qres);
            if(qres) {
                if(datawaitmsg.cmd == ScanDataCmd_DVB_SDT || datawaitmsg.cmd == ScanDataCmd_DVB_SDT_COMPLETE) {
                    SendScanDataQ(datawaitmsg);
                } else {
                    SendScanDataQ(datawaitmsg, true);
                }
            }
        }
        //send pmt/wait pmt
        waitQSize = ScanDataWaitQ.size();
        for (int wqloop = 0; wqloop < waitQSize; wqloop++) {
            int qres = 0;
            ScanDataMsg datawaitmsg = ScanDataWaitQ.dequeue(qres);
            if(qres) {
                if(datawaitmsg.cmd == ScanDataCmd_PSI_Ready) {
                    ScanTransport* pTs = getTransportStreamInfo(current_tsid);
                    if(pTs) {
                        if(pTs->sdt_version != 0xFF) {
                            SendScanDataQ(datawaitmsg);
                        } else {
                            SendScanDataQ(datawaitmsg, true);
                        }
                    }
                } else {
                    SendScanDataQ(datawaitmsg);
                }
            }
        }
    }

    if(!bReceiveEITInScan && sdt_other_requested && datamsg.cmd == ScanDataCmd_DVB_SDT_COMPLETE) {
        dvbCompleteInChannel = true;
    }
    if(dvbCompleteInChannel && psiCompleteInChannel) {
        if(mManagerMode == eFirstConnect) {
            if(bNeedScan) {
                change2ScanMode();
            } else {
                change2UpdateMode();
            }
        } else if(mManagerMode == eInScanning) {
            requestTuneNext();
        }
    }

    //if(changeSomething && mManagerMode==eInUpdate)
//    L_DEBUG(TAG,"mManagerMode [%d]\n",mManagerMode);
    if(mManagerMode == eInUpdate ) {
        int ret;
        L_DEBUG(TAG,"NIT monitoring ver[0x%x]!!!! \n",NitVer);
        MonitorFilter *pMf = new MonitorFilter(0x10, 0x40, NitVer);
        if(pMf) {
            pMf->setCallbackFunction(monitor_cb);
            ret = pMf->startFilter(current_uri);
            if(ret) {
                monitorList.push_back(pMf);
            } else {
            	L_DEBUG(TAG,"\n");
                deleteMonitorFilter(pMf);
            }
        }
    }

    if(request_tune_ap && (mScanMode != eScanMode_OOB) && isTDTTimeSet) {
        request_tune_ap = false;
        datamsg.cmd = ScanDataCmd_DVB_NIT_CHMAP_READY;
        SendScanDataQ(datamsg);
        sdt_other_requested = false;
    }
}

void ScanManager::receiveBAT(void* filterhandle)
{
    ScanDataMsg datamsg;
    TableFilter* pFilter = (TableFilter*) filterhandle;
	TableT* pTable = NULL;
	if(pFilter != NULL)
		pTable = pFilter->m_pTable;
	if(pFilter == NULL || pTable == NULL)
		return;
	pFilter->detachFilter();

	if(!bReceiveEITInScan &&  sdt_count==0 && NitVer != 0xFF) {
		dvbCompleteInChannel = true;
	}

	datamsg.param = filterhandle;
	datamsg.cmd = ScanDataCmd_DVB_BAT;
	SendScanDataQ(datamsg);

	 if(dvbCompleteInChannel && psiCompleteInChannel) {
		if(mManagerMode == eFirstConnect) {
			if(bNeedScan) {

				change2ScanMode();
			} else {
				change2UpdateMode();
			}
		} else if(mManagerMode == eInScanning) {
			requestTuneNext();
		}
	}
}

void ScanManager::receiveSDT(void* filterhandle)
{
	L_INFO(TAG,"\n");
    int res, sizeN;
    TableFilter* pFilter = (TableFilter*) filterhandle;
    TableT* pTable=NULL;
    if(pFilter!=NULL) pTable = pFilter->m_pTable;
    if(pFilter==NULL || pTable==NULL)
    {
    	L_INFO(TAG,"SDT is NULL\n");
    	return;
    }
    pFilter->detachFilter();

    ScanMsg msg;
    ScanTransport *pTs = NULL;
    bool changeSomething = false;

    if(pTable) {
        pTs = ParseSDT(pTable, &current_tsid, changeSomething);
        L_DEBUG(TAG,"actual sdt tsid = 0x%x\n",current_tsid);
    }

    if(pTs == NULL) {
    	L_INFO(TAG,"pTs is NULL\n");
        return;
    }

    set_current_ts = true;
	sdt_count--;
    if(sdt_count < 0) {
        L_INFO(TAG, "invalid SDT count=%d\n", sdt_count);
    }
    L_DEBUG(TAG,"bUseOtherTS [%d] ts_map_ready[%d] , set_current_ts[%d], sdt_other_requested[%d]\n",bUseOtherTS, ts_map_ready, set_current_ts, sdt_other_requested);
    if(bUseOtherTS && ts_map_ready && set_current_ts && sdt_other_requested == false) {
        //request other SDT
        ListTs.lock();
        sizeN = ListTs.sizeNoLock();
        for (int itr = 0; itr < sizeN; itr++) {
            ScanTransport* info = ListTs.AtNoLock(itr, res);
            if(res == 0)
                continue;
            if(info) {
                msg.param = (void*) info;
                msg.mask = 0xFF;
                int reqtid = 0xff;

                if(info->tsid != current_tsid) {
                    if(info->sdt_version == 0xFF || mManagerMode == eInScanning) {
                        OtherTSIDQ.enqueue(info->tsid);
                    }
                }
            }
        }
        ListTs.unlock();
        sdt_count += (OtherTSIDQ.size());
        sdt_other_requested = true;
        msg.cmd = eScanCmd_SDT_OTHER_REQUEST;
        SendScanManageQ(msg);
        L_INFO(TAG, "SDT Count=%d\n", sdt_count);
    }
    ScanDataMsg datamsg;
    datamsg.param = filterhandle;

    if(!bUseOtherTS) {
        //sdt complete
        datamsg.cmd = ScanDataCmd_DVB_SDT_COMPLETE;
    } else if(ts_map_ready && sdt_count == 0) {
        //sdt complete
        datamsg.cmd = ScanDataCmd_DVB_SDT_COMPLETE;
    } else {
        //sdt not complete
        datamsg.cmd = ScanDataCmd_DVB_SDT;
    }
    if(mManagerMode == eInScanning || mManagerMode == eInUpdate) {
    	L_DEBUG(TAG,"\n");
    	start_PSI();
    }
    //if(changeSomething && mManagerMode==eInUpdate)
    if(mManagerMode == eInUpdate) {
        int ret;
        MonitorFilter *pMf = new MonitorFilter(0x11, 0x42, Version(pTable->m_pSection[0]), pTs->tsid);
        if(pMf) {
            pMf->setCallbackFunction(monitor_cb);
            pMf->setTableIDMask(0xfb);
            ret = pMf->startFilter(current_uri);
            if(ret) {
                monitorList.push_back(pMf);
            } else {
            	L_DEBUG(TAG,"\n");
                deleteMonitorFilter(pMf);
            }
        }
    }

    if(changeSomething) {
    	L_DEBUG(TAG,"\n");
        SendScanDataQ(datamsg, !ts_map_ready);

        if(!bReceiveEITInScan) {
            if(mScanMode == eScanMode_Add_FullSI) {
                pTs->scanned = true;
            }
        }
    } else {
        deleteTableFilter(pFilter);
    }

    if(!bReceiveEITInScan && datamsg.cmd == ScanDataCmd_DVB_SDT_COMPLETE /*&& ScanDataWaitQ.size() == 0*/) {
        dvbCompleteInChannel = true;
    }

    if(datamsg.cmd == ScanDataCmd_DVB_SDT_COMPLETE && ts_map_ready) {
        while (ScanDataWaitQ.size()) {
            int qres = 0;
            ScanDataMsg datawaitmsg = ScanDataWaitQ.dequeue(qres);
            if(qres)
                SendScanDataQ(datawaitmsg);
        }
    } else if(ts_map_ready) {
    	L_DEBUG(TAG,"\n");
        //send pmt
        int waitQSize = ScanDataWaitQ.size();
        for (int wqloop = 0; wqloop < waitQSize; wqloop++) {
            int qres = 0;
            ScanDataMsg datawaitmsg = ScanDataWaitQ.dequeue(qres);
            if(qres) {
                SendScanDataQ(datawaitmsg, !(datawaitmsg.cmd == ScanDataCmd_PSI_Ready));
            }
        }
    }

    if(dvbCompleteInChannel && psiCompleteInChannel) {
        if(mManagerMode == eFirstConnect) {
            if(bNeedScan) {
                change2ScanMode();
            } else {
            	L_DEBUG(TAG,"\n");
                change2UpdateMode();
            }
        } else if(mManagerMode == eInScanning) {
            requestTuneNext();
        }
    }
}

void ScanManager::receiveSDTOther(void* filterhandle)
{
    int res, sizeN;
    TableFilter* pFilter = (TableFilter*) filterhandle;
    TableT* pTable=NULL;
    if(pFilter!=NULL) pTable = pFilter->m_pTable;
    if(pFilter==NULL || pTable==NULL)
        return;
    pFilter->detachFilter();

    ScanTransport *pTs = NULL;
    uint16_t tsid = 0;
    bool changeSomething = false;

    if(pTable) {
        pTs = ParseSDT(pTable, &tsid, changeSomething);
        //printf("other sdt tsid = 0x%x\n",tsid);
    }

    if(pTs == NULL)
        return;

    sdt_count--;
    if(sdt_count < 0) {
        L_INFO(TAG, "invalid SDT count=%d\n", sdt_count);
    }

    ScanDataMsg datamsg;
    datamsg.param = filterhandle;
    if(sdt_count == 0) {
        //L_INFO(TAG, "sdt complete\n ");
        //sdt complete
        datamsg.cmd = ScanDataCmd_DVB_SDT_COMPLETE;
    } else {
        //L_INFO(TAG, "sdt sdt_count=%d\n ",sdt_count);
        //sdt not complete
        datamsg.cmd = ScanDataCmd_DVB_SDT;
    }

    //if(changeSomething && mManagerMode==eInUpdate)
    if(mManagerMode == eInUpdate) {
        int ret;
        L_INFO(TAG, "version [0x%X],tsid[0x%X]\n ",Version(pTable->m_pSection[0]), pTs->tsid);
        MonitorFilter *pMf = new MonitorFilter(0x11, 0x46, Version(pTable->m_pSection[0]), pTs->tsid);
        if(pMf) {
            pMf->setCallbackFunction(monitor_cb);
            pMf->setTableIDMask(0xfb);
            ret = pMf->startFilter(current_uri);
            if(ret) {
                monitorList.push_back(pMf);
            } else {
            	L_DEBUG(TAG,"\n");
                deleteMonitorFilter(pMf);
            }
        }
    }

    if(changeSomething) {
        SendScanDataQ(datamsg);
    } else {
        deleteTableFilter(pFilter);
    }

    if(!bReceiveEITInScan && datamsg.cmd == ScanDataCmd_DVB_SDT_COMPLETE) {
        dvbCompleteInChannel = true;
    }

    if(dvbCompleteInChannel && psiCompleteInChannel) {
        if(mManagerMode == eFirstConnect) {
            if(bNeedScan) {
                change2ScanMode();
            } else {
                change2UpdateMode();
            }
        } else if(mManagerMode == eInScanning) {
            requestTuneNext();
        }
    }

    if(datamsg.cmd == ScanDataCmd_DVB_SDT_COMPLETE && ts_map_ready) {
        while (ScanDataWaitQ.size()) {
            int qres = 0;
            ScanDataMsg datawaitmsg = ScanDataWaitQ.dequeue(qres);
            if(qres)
                SendScanDataQ(datawaitmsg);
        }
    }
}

void ScanManager::receiveTDT(void* filterhandle)
{
//	L_DEBUG(TAG,"\n");
    SimpleFilter* pFilter = (SimpleFilter*) filterhandle;
    uint8_t tdt_version=0xff;
    if(pFilter==NULL || pFilter->m_pSection == NULL || pFilter->m_pSection->m_pSection==NULL || pFilter->m_pSection->m_pSectionLen==0)
    {
    	L_DEBUG(TAG,"TDT info incorrect\n");
    	return;
    }
//    L_DEBUG(TAG,"section length [%d]\n",pFilter->m_pSection->m_pSectionLen);
    ScanDataMsg datamsg;

    uint8_t* buff = new uint8_t[pFilter->m_pSection->m_pSectionLen];
    if(buff) {
        memcpy(buff, pFilter->m_pSection->m_pSection, pFilter->m_pSection->m_pSectionLen);
        SectionT* pSection = new SectionT(buff, pFilter->m_pSection->m_pSectionLen); //pFilter->m_pSection;
        //pFilter->detachFilter();
        if(pSection) {
            datamsg.cmd = ScanDataCmd_DVB_TDT;
            datamsg.param = pSection;
            SendScanDataQ(datamsg);
        }
    }

    if(isTDTTimeSet == false && ts_map_ready) {
        int waitQSize = ScanDataWaitQ.size();
        for (int wqloop = 0; wqloop < waitQSize; wqloop++) {
            int qres = 0;
            ScanDataMsg datawaitmsg = ScanDataWaitQ.dequeue(qres);
            if(qres) {
                SendScanDataQ(datawaitmsg);
            }
        }

        request_tune_ap = false;
        datamsg.cmd = ScanDataCmd_DVB_NIT_CHMAP_READY;
        SendScanDataQ(datamsg);
        sdt_other_requested = false;
    }
    isTDTTimeSet = true;
    delete[] buff;
    buff = NULL;
}

void ScanManager::receiveVCT(void* filterhandle)
{
    TableFilter* pFilter = (TableFilter*) filterhandle;
    TableT* pTable = pFilter->m_pTable;
    pFilter->detachFilter();

    ScanTransport *pTs = NULL;
    uint16_t tsid;
    bool changeSomething = false;
    ScanDataMsg datamsg;

    if(pTable) {
        pTs = ParseVCT(pTable, &current_tsid, changeSomething);
        if(pTable->m_pSection[0][0] == 0xC8) {
            datamsg.cmd = ScanDataCmd_PSIP_TVCT;
        } else {
            datamsg.cmd = ScanDataCmd_PSIP_CVCT;
        }
    }

    //if(!bNeedScan)
    {
        ScanMsg msg;
        msg.cmd = eScanCmd_MGT_REQUEST;
        SendScanManageQ(msg);
    }

    if(changeSomething) {
        datamsg.param = filterhandle;
        SendScanDataQ(datamsg);
    } else {
        deleteTableFilter(pFilter);
    }

}

void ScanManager::receivePAT(void* filterhandle)
{
	L_INFO(TAG, "receivePAT\n");
    TableFilter* pFilter = (TableFilter*) filterhandle;
    TableT* pTable=NULL;
    if(pFilter!=NULL) pTable = pFilter->m_pTable;
    if(pFilter==NULL || pTable==NULL)
    {
    	L_INFO(TAG, "PAT NULL\n");
    	return;
    }
    pFilter->detachFilter();

    if(pTable) {
        if(bTestMode) {
            ParsePAT_Test(pTable, &current_tsid);
        } else {
            ParsePAT(pTable, &current_tsid);
        }
    }

    set_current_ts = true;
    pmt_count = PMTPIDQ.size();
    L_INFO(TAG, "PMT Count=%d\n", pmt_count);
    ScanMsg msg;
    msg.cmd = eScanCmd_PMT_REQUEST;
    SendScanManageQ(msg);

    if(mManagerMode != eInScanning || ((mScanMode != eScanMode_Add_PSI) && (mScanMode != eScanMode_NIT_PSI))) {
    	L_DEBUG(TAG,"bUseDVB[%d],  bUseOtherTS[%d], ts_map_ready[%d],  sdt_other_requested[%d]\n",bUseDVB , bUseOtherTS , ts_map_ready, sdt_other_requested);
        if(bUseDVB && bUseOtherTS && ts_map_ready && sdt_other_requested == false) {
            //request other SDT
            ListTs.lock();
            int sizeN = ListTs.sizeNoLock();
            int res;
            for (int itr = 0; itr < sizeN; itr++) {
                ScanTransport* info = ListTs.AtNoLock(itr, res);
                if(res == 0 && info)
                    continue;

                bool requestpf = false;
                bool requestschedule = false;
                msg.param = (void*) info;
                msg.mask = 0xFF;
                int reqtid = 0xff;

                if(info->tsid != current_tsid) {
                    if(info->sdt_version == 0xFF || mManagerMode == eInScanning) {
                        OtherTSIDQ.enqueue(info->tsid);
                    }
                }
            }
            ListTs.unlock();
            sdt_count += (OtherTSIDQ.size());
            sdt_other_requested = true;
            ScanMsg msg;
            msg.cmd = eScanCmd_SDT_OTHER_REQUEST;
            SendScanManageQ(msg);
            L_INFO(TAG, "SDT Count=%d\n", sdt_count);
        }
    }
    deleteTableFilter(pFilter);

//    if(bNeedScan == false && isScanManagerWork ) {
//		int ret;
//		MonitorFilter *pMf = new MonitorFilter(0x00, 0x00, Version(pSection->m_pSection));//, TableIDExt(pSection->m_pSection));
//		if(pMf) {
//			pMf->setCallbackFunction(monitor_cb);
//			ret = pMf->startFilter(current_uri);
//			if(ret) {
//				monitorList.push_back(pMf);
//			} else {
//				L_DEBUG(TAG,"\n");
//				deleteMonitorFilter(pMf);
//			}
//		}
//	}


}
void ScanManager::receivePMT(void* filterhandle)
{

    SimpleFilter* pFilter = (SimpleFilter*) filterhandle;
    SectionT* pSection = pFilter->m_pSection;
    pFilter->detachFilter();

    if(pSection == NULL) {
        return;
    }

    bool changeSomething = false;

    if(pSection) {
        if(bTestMode) {
            ParsePMT_Test(pSection->m_pSection, pSection->m_pSectionLen, current_tsid, changeSomething);
        } else {
            ParsePMT(pSection->m_pSection, pSection->m_pSectionLen, current_tsid, changeSomething);
        }
    }

    pmt_count--;
    if(pmt_count < 0) {
        L_INFO(TAG, "invalid PMT count=%d\n", pmt_count);
    }

	bool isScanManagerWork = false;
    {
		RecursiveMutex::Autolock lock(startMutex);
    	isScanManagerWork = bScanManagerWork;
    }

    ScanTransport* pTs = getTransportStreamInfo(current_tsid);

    L_INFO(TAG, "PMT count=%d pFilter=%p(%d) ProgNUM=0x%X version=%d\n", pmt_count, pFilter, pFilter->m_pid, TableIDExt(pSection->m_pSection),Version(pSection->m_pSection));
    if(bTestMode) {
        ScanDataMsg datamsg;
        datamsg.cmd = ScanDataCmd_PSI_Ready;
        datamsg.param = filterhandle;
        SendScanDataQ(datamsg);

        if(pmt_count == 0) {
            if(pTs)
                pTs->scanned = true;
            requestTuneNext();
        }
    } else {
        if(changeSomething) {
            ScanDataMsg datamsg;
            datamsg.cmd = ScanDataCmd_PSI_Ready;
            datamsg.param = filterhandle;
            datamsg.iparam = current_tsid;
            L_INFO(TAG, "PMT Prog NUM=0x%X\n",TableIDExt(pSection->m_pSection));
            if(ts_map_ready && pTs && (pTs->sdt_version != 0xFF)) {
                SendScanDataQ(datamsg);
            } else {
                SendScanDataQ(datamsg, true);
            }
        } else {
            deleteSimpleFilter(pFilter);
        }
    }
#if 1 // wjpark 2015-11-24
    if(bNeedScan == false && isScanManagerWork && pSection) {
        int ret;
        MonitorFilter *pMf = new MonitorFilter(pFilter->m_pid, 0x02, Version(pSection->m_pSection));//, TableIDExt(pSection->m_pSection));
        if(pMf) {
            pMf->setCallbackFunction(monitor_cb);
            ret = pMf->startFilter(current_uri);
            if(ret) {
//            	L_DEBUG(TAG,"\n");
                monitorList.push_back(pMf);
            } else {
//            	L_DEBUG(TAG,"\n");
                deleteMonitorFilter(pMf);
            }
        }
    }
#endif
//    L_INFO(TAG,"\n");
    if(pmt_count == 0) {
        // ksh_20140503 | add logic, dvb start when scan
        if(bNeedScan && NitVer == 0xFF) {
            start_DVB();
        }

        psiCompleteInChannel = true;

        if(bNeedScan && pTs && mScanMode == eScanMode_Add_PSI) {
            pTs->scanned = true;
            if(dvbCompleteInChannel) {
                if(mManagerMode == eFirstConnect) {

                    change2ScanMode();
                } else if(mManagerMode == eInScanning) {
                    requestTuneNext();
                }
            }
        } else if(!bNeedScan && mManagerMode == eFirstConnect) {
            if(dvbCompleteInChannel) {
                change2UpdateMode();
            }
        } else if(!bNeedScan && mManagerMode == eInScanEnd) {
        	change2UpdateMode();
        }
    }
}

//external function
void ScanManager::initialize(std::string provider)
{
	L_DEBUG(TAG,"\n");
//    set_heap_trace(true);
    ScanManagerLock();
    static bool bInit = false;
    scanTsCnt = 0;
    if(bInit == false) {
        bInit = true;
        msoName = "";
        bTunerLocked = false;
        mLastOpSec = 0;
        mManagerMode = eIdle;
        msoName = provider;
        scan_tuner = new TDI_TunerSettings;
        isTDTTimeSet = false;

        configSIScan();

        ts_map_ready = false;
        set_current_ts = false;
        sdt_other_requested = false;
        request_tune_ap = false;

        pthread_t task_id = TDI_INVALID_TASK_ID;
        pthread_create(&task_id, NULL, scan_cmd_task, NULL);
        pthread_create(&task_id, NULL, scan_data_task, NULL);

//        TDI_Tuner_Open(&scan_tunerId);
//        TDI_Tuner_SetCallback(scan_tunerId, scanTunerCallback);
//        MMF_Demux_Open();
        MMF_SetCallback(scanTunerCallback);
        scan_tuner->tunerId = scan_tunerId;
    }
    ScanManagerUnlock();
//    print_unfreed_heap_blocks("at ScanManager::initialize()");
}
void ScanManager::setTuneParam(EPGProviderUri uri)
{
    if(uri.ap.compare("i") == 0) {
        scan_tuner->type = TDI_TunerType_Ip;
        scan_tuner->protocol = TDI_TunerProtocol_Udp;
        sprintf(scan_tuner->parameter.ip.ip, "%s", uri.provIP.c_str());
        scan_tuner->parameter.ip.port = uri.provPort;
    } else if(uri.ap.compare("c") == 0) {
        scan_tuner->type = TDI_TunerType_Cable;
        if(uri.demodType.compare("qam64") == 0) {
            scan_tuner->protocol = TDI_TunerProtocol_64QAM;
        } else if(uri.demodType.compare("qam128") == 0) {
            scan_tuner->protocol = TDI_TunerProtocol_128QAM;
        } else if(uri.demodType.compare("qam256") == 0) {
            scan_tuner->protocol = TDI_TunerProtocol_256QAM;
        } else if(uri.demodType.compare("qam16") == 0) {
            scan_tuner->protocol = TDI_TunerProtocol_16QAM;
        } else if(uri.demodType.compare("qam32") == 0) {
            scan_tuner->protocol = TDI_TunerProtocol_32QAM;
        }
        scan_tuner->parameter.rf.frequencyHz = uri.freqkHz * 1000;
        scan_tuner->parameter.rf.symbolrate = uri.symbolrate * 1000;
        scan_tuner->parameter.rf.bandwidth = uri.bandwidth;
        bandwidthkHz = uri.bandwidth * 1000;
    } else if(uri.ap.compare("s") == 0) {
        scan_tuner->type = TDI_TunerType_Satellite;
    } else if(uri.ap.compare("t") == 0) {
        scan_tuner->type = TDI_TunerType_Terrestrial;
        if(uri.demodType.compare("dvbt") == 0) {
            scan_tuner->protocol = TDI_TunerProtocol_DVBT;
        } else if(uri.demodType.compare("dvbt2") == 0) {
            scan_tuner->protocol = TDI_TunerProtocol_DVBT2;
        } else if(uri.demodType.compare("vsb8") == 0) {
            scan_tuner->protocol = TDI_TunerProtocol_8VSB;
        }
        scan_tuner->parameter.rf.frequencyHz = uri.freqkHz * 1000;
    } else if(uri.ap.compare("f") == 0) {
        scan_tuner->type = TDI_TunerType_File;
        sprintf(scan_tuner->parameter.file.name, "/home/ksh78/work/TSFile/2013_0701_639mhz.trp");
    }
    //scan_tuner->type = TDI_TunerType_File;
    //sprintf(scan_tuner->parameter.file.name,"/home/ksh78/work/TSFile/cmb.tp");
    memcpy(&scan_tuner_ap, scan_tuner, sizeof(TDI_TunerSettings));
    epg_uri = uri;
}

string ScanManager::getUri(TDI_TunerSettings* scan_tuner,int mode)
{
    ostringstream stringStream;
    EPGProviderUri uri;
    char chr[128];
    memset(chr, 0, 128);
    int tunerid = 0;
    //tuner%d://qam64:%d?sr=%d&bw=%d&sc=1
    uri = convertUri(scan_tuner);
    if(invertTuner) tunerid=1;
    sprintf(chr, "tuner://%s:%d?sr=%d&bw=%d&sc=%d&sn=%d",uri.demodType.c_str(),uri.freqkHz, uri.symbolrate,uri.bandwidth,mode,tunerid);
    stringStream << chr;
    L_DEBUG(TAG,"[%s]\n", stringStream.str().c_str());
    return stringStream.str();
}

EPGProviderUri ScanManager::convertUri(TDI_TunerSettings* scan_tuner)
{
	EPGProviderUri  uri;

	if(scan_tuner->protocol == TDI_TunerProtocol_64QAM) {
		uri.demodType = "qam64";
	} else if(scan_tuner->protocol  == TDI_TunerProtocol_128QAM) {
		uri.demodType = "qam256";
	} else if(scan_tuner->protocol  == TDI_TunerProtocol_256QAM) {
		uri.demodType = "qam256";
	} else if(scan_tuner->protocol  == TDI_TunerProtocol_16QAM) {
		uri.demodType = "qam16";
	} else if(scan_tuner->protocol  == TDI_TunerProtocol_32QAM) {
		uri.demodType = "qam32";
	}
	uri.freqkHz = scan_tuner->parameter.rf.frequencyHz/1000;
	uri.symbolrate =  scan_tuner->parameter.rf.symbolrate/1000;
	uri.bandwidth = scan_tuner->parameter.rf.bandwidth;
	L_DEBUG(TAG,"type = [%s] , freq = [%d], symbol [%d], bandwidth[%d]\n", uri.demodType.c_str(),uri.freqkHz, uri.symbolrate, uri.bandwidth);
	return uri;
}


void ScanManager::setWepgChannelMapFile(string wepgChannelMapFileUrl)
{

    mWepgChannelMapFileURL = wepgChannelMapFileUrl;

    bReceiveEITInScan = false;
    bEITBoost = false;
    max_receive_eit_idx = -1;
    SCAN_MANAGER_TIMEOUT_SEC = (time_t) MANUAL_CHANNEL_SCAN_TIMEOUT_SEC; // temp
    L_INFO(TAG, "SCAN_MANAGER_TIMEOUT_SEC=%d\n", (int )SCAN_MANAGER_TIMEOUT_SEC);
    Parser::getInstance().setWepgChannelMapUrl(mWepgChannelMapFileURL);
}

void ScanManager::scanStart()
{
	bScanCompleted = false;
    ScanManagerLock();
    bTestMode = false;
    request_tune_ap = false;
//    current_uri = getUri(1);
    int res;
    {
    	RecursiveMutex::Autolock lock(startMutex);
    	bScanManagerWork = false;
    }
//    MMF_Demux_Create(&mDmxHandle, (uint8_t*)current_uri.c_str());
//    L_DEBUG(TAG, "uri[%s] \n",current_uri.c_str());
    ScanDataMsg datamsg;
    ScanDataQLock();
    int Qresult = ScanDataQ.size();
    ScanDataQ.lock();
    while (Qresult) {
        datamsg = ScanDataQ.dequeueNoLock(Qresult);
        if(Qresult) {
            switch (datamsg.cmd) {
                case ScanDataCmd_DVB_NIT:
                case ScanDataCmd_DVB_SDT:
                case ScanDataCmd_DVB_SDT_COMPLETE:
                case ScanDataCmd_DVB_EIT:
                case ScanDataCmd_DVB_EIT_COMPLETE:
                case ScanDataCmd_PSIP_TVCT:
                case ScanDataCmd_PSIP_CVCT:
                case ScanDataCmd_PSIP_EIT:
                case ScanDataCmd_PSIP_EIT_COMPLETE: {
                    TableFilter* pFilter = (TableFilter*) datamsg.param;
                    deleteTableFilter(pFilter);
                }
                    break;
                case ScanDataCmd_PSI_Ready:
                case ScanDataCmd_PSIP_MGT:
                case ScanDataCmd_PSIP_ETT: {
                    SimpleFilter* pFilter = (SimpleFilter*) datamsg.param;
                    deleteSimpleFilter(pFilter);
                }
                    break;

                case ScanDataCmd_DVB_TDT: {
                    SectionT* pSection = (SectionT*) datamsg.param;
                    delete pSection;
                }
                    break;
            }
        }
    }
    ScanDataQ.unlock();

    Qresult = ScanDataWaitQ.size();
    ScanDataWaitQ.lock();
    while (Qresult) {
        datamsg = ScanDataWaitQ.dequeueNoLock(Qresult);
        if(Qresult) {
            switch (datamsg.cmd) {
                case ScanDataCmd_DVB_NIT:
                case ScanDataCmd_DVB_SDT:
                case ScanDataCmd_DVB_SDT_COMPLETE:
                case ScanDataCmd_DVB_EIT:
                case ScanDataCmd_DVB_EIT_COMPLETE:
                case ScanDataCmd_PSIP_TVCT:
                case ScanDataCmd_PSIP_CVCT:
                case ScanDataCmd_PSIP_EIT:
                case ScanDataCmd_PSIP_EIT_COMPLETE: {
                    TableFilter* pFilter = (TableFilter*) datamsg.param;
                    deleteTableFilter(pFilter);
                }
                    break;
                case ScanDataCmd_PSI_Ready:
                case ScanDataCmd_PSIP_MGT:
                case ScanDataCmd_PSIP_ETT: {
                    SimpleFilter* pFilter = (SimpleFilter*) datamsg.param;
                    deleteSimpleFilter(pFilter);
                }
                    break;
                case ScanDataCmd_DVB_TDT: {
                    SectionT* pSection = (SectionT*) datamsg.param;
                    delete pSection;
                }
                    break;
            }
        }
    }
    ScanDataWaitQ.unlock();
    ScanDataQUnlock();

    PMTPIDQ.clear();
    OtherTSIDQ.clear();

    {
        //Service_Filter_List.lock();
        int sizeN = Service_Filter_List.sizeNoLock();
        for (int idx = 0; idx < sizeN; idx++) {
            TableFilter* pFilter = (TableFilter*) Service_Filter_List.AtNoLock(idx, res);
            if(res && pFilter) {
                pFilter->stopTable();
                deleteTableFilter(pFilter);
            }
        }
        Service_Filter_List.clearNoLock();
        //Service_Filter_List.unlock();

        //Service_Section_Filter_List.lock();
        sizeN = Service_Section_Filter_List.sizeNoLock();
        for (int idx = 0; idx < sizeN; idx++) {
            SimpleFilter* pFilter = (SimpleFilter*) Service_Section_Filter_List.AtNoLock(idx, res);
            if(res && pFilter) {
                pFilter->stopFilter();
                deleteSimpleFilter(pFilter);
            }
        }
        Service_Section_Filter_List.clearNoLock();
        //Service_Section_Filter_List.unlock();
    }
    clearMonitorFilter();

    clearTSList();
    set_current_ts = false;
    sdt_other_requested = false;
    current_tsid = 0;
    scanTsCnt = 0;
    ts_map_ready = false;
    pmt_count = 0;
    sdt_count = 0;
    eit_pf_count = 0;
    eit_sch_count = 0;
    bEITBoost = bEITBoostBackup;
    bUsePSI = bUsePSIBackup;
    bUseDVB = bUseDVBBackup;
    bUsePSIP = bUsePSIPBackup;
    bUseOtherTS = bUseOtherTSBackup;
    bReceiveEITInScan = bReceiveEITInScanBackup;
    mScanMode = mScanModeBackup;

    clearMonitorBaseFilter();
    clearTableBaseFilter();
    clearSimpleBaseFilter();

    {
    	RecursiveMutex::Autolock lock(startMutex);
    	bScanManagerWork = false;
    }

    bNeedScan = true;
    memcpy(scan_tuner, &scan_tuner_ap, sizeof(TDI_TunerSettings));

    ScanMsg msg;
    //first channel tune
    msg.cmd = eScanCmd_TUNE_START;
    msg.iparam = 1; // scan mode
    msg.param = (void*) scan_tuner;

    mManagerMode = eFirstConnect;

    if(bUsePSI) {
        psiCompleteInChannel = false;
    } else {
        psiCompleteInChannel = true;
    }

    if(bUseDVB) {
        dvbCompleteInChannel = false;
    } else {
        dvbCompleteInChannel = true;
    }
    if(strlen(mWepgChannelMapFileURL.c_str()) > 10) {
        bReceiveEITInScan = false;
        bEITBoost = false;
        max_receive_eit_idx = -1;
    }
    ScanManageQ.clear();
    SendScanManageQ(msg, true);
    ScanManagerUnlock();
}
void ScanManager::setChMapNotify()
{
	bChMapNoti =true;
	sendChannelComplete();
//	mManagerMode = eInUpdate;
}

void ScanManager::start()
{

    ScanManagerLock();
    bTestMode = false;

    {
    	RecursiveMutex::Autolock lock(startMutex);
    	bScanManagerWork = true;
    }
    bNeedScan = false;

    bUsePSI = bUsePSIBackup;
    bUseDVB = bUseDVBBackup;
    bUsePSIP = bUsePSIPBackup;

    ScanDataMsg datamsg;
    ScanDataQLock();
    int Qresult = ScanDataWaitQ.size();
    ScanDataWaitQ.lock();
    while (Qresult) {
        datamsg = ScanDataWaitQ.dequeueNoLock(Qresult);
        if(Qresult) {
            switch (datamsg.cmd) {
                case ScanDataCmd_DVB_NIT:
                case ScanDataCmd_DVB_SDT:
                case ScanDataCmd_DVB_SDT_COMPLETE:
                case ScanDataCmd_DVB_EIT:
                case ScanDataCmd_DVB_EIT_COMPLETE:
                case ScanDataCmd_PSIP_TVCT:
                case ScanDataCmd_PSIP_CVCT:
                case ScanDataCmd_PSIP_EIT:
                case ScanDataCmd_PSIP_EIT_COMPLETE: {
                    TableFilter* pFilter = (TableFilter*) datamsg.param;
                    deleteTableFilter(pFilter);
                }
                    break;
                case ScanDataCmd_PSI_Ready:
                case ScanDataCmd_PSIP_MGT:
                case ScanDataCmd_PSIP_ETT: {
                    SimpleFilter* pFilter = (SimpleFilter*) datamsg.param;
                    deleteSimpleFilter(pFilter);
                }
                    break;
                case ScanDataCmd_DVB_TDT: {
                    SectionT* pSection = (SectionT*) datamsg.param;
                    L_INFO(TAG,"\n");
                    delete pSection;
                }
                    break;
            }
        }
    }
    ScanDataWaitQ.unlock();
    ScanDataQUnlock();

    clearMonitorBaseFilter();
    clearTableBaseFilter();
    clearSimpleBaseFilter();

    if((request_tune_ap == false && bTunerLocked) || mScanMode == eScanMode_OOB) {
        if(bUseDVB == true) {
        	L_DEBUG(TAG,"\n");
            start_DVB();
        }
        if(bUsePSI == true) {
#if 0 // wjpark actual SDT 받은후에 start 2015-12-04
        	L_DEBUG(TAG,"\n");
            start_PSI();
#endif
        }
    }

    if(strlen(mWepgChannelMapFileURL.c_str()) > 10) {
        bReceiveEITInScan = false;
        bEITBoost = false;
        max_receive_eit_idx = -1;
    }

    mManagerMode = eFirstConnect;
    ScanManagerUnlock();
}

void ScanManager::scanStop()
{

    ScanManagerLock();
    request_tune_ap = false;
    bScanCompleted = true;
    int res;
    {
    	RecursiveMutex::Autolock lock(startMutex);
    	bScanManagerWork = false;
    }

    bNeedScan = false;
    if(bcreatedDMX)
    {
    	channelChangeBefore();
    	MMF_Demux_Destroy(mDmxHandle);
    	bcreatedDMX = false;
    }
    ScanManageQ.clear();

    ScanDataMsg datamsg;
    ScanDataQLock();
    int Qresult = ScanDataQ.size();
    ScanDataQ.lock();
    while (Qresult) {
        datamsg = ScanDataQ.dequeueNoLock(Qresult);
        if(Qresult) {
            switch (datamsg.cmd) {
                case ScanDataCmd_DVB_NIT:
                case ScanDataCmd_DVB_SDT:
                case ScanDataCmd_DVB_SDT_COMPLETE:
                case ScanDataCmd_DVB_EIT:
                case ScanDataCmd_DVB_EIT_COMPLETE:
                case ScanDataCmd_PSIP_TVCT:
                case ScanDataCmd_PSIP_CVCT:
                case ScanDataCmd_PSIP_EIT:
                case ScanDataCmd_PSIP_EIT_COMPLETE: {
                    TableFilter* pFilter = (TableFilter*) datamsg.param;
                    deleteTableFilter(pFilter);
                }
                    break;
                case ScanDataCmd_PSI_Ready:
                case ScanDataCmd_PSIP_MGT:
                case ScanDataCmd_PSIP_ETT: {
                    SimpleFilter* pFilter = (SimpleFilter*) datamsg.param;
                    deleteSimpleFilter(pFilter);
                }
                    break;
                case ScanDataCmd_DVB_TDT: {
                    SectionT* pSection = (SectionT*) datamsg.param;
                    delete pSection;
                }
                    break;
            }
        }
    }
    ScanDataQ.unlock();

    Qresult = ScanDataWaitQ.size();
    ScanDataWaitQ.lock();
    while (Qresult) {
        datamsg = ScanDataWaitQ.dequeueNoLock(Qresult);
        if(Qresult) {
            switch (datamsg.cmd) {
                case ScanDataCmd_DVB_NIT:
                case ScanDataCmd_DVB_SDT:
                case ScanDataCmd_DVB_SDT_COMPLETE:
                case ScanDataCmd_DVB_EIT:
                case ScanDataCmd_DVB_EIT_COMPLETE:
                case ScanDataCmd_PSIP_TVCT:
                case ScanDataCmd_PSIP_CVCT:
                case ScanDataCmd_PSIP_EIT:
                case ScanDataCmd_PSIP_EIT_COMPLETE: {
                    TableFilter* pFilter = (TableFilter*) datamsg.param;
                    deleteTableFilter(pFilter);
                }
                    break;
                case ScanDataCmd_PSI_Ready:
                case ScanDataCmd_PSIP_MGT:
                case ScanDataCmd_PSIP_ETT: {
                    SimpleFilter* pFilter = (SimpleFilter*) datamsg.param;
                    deleteSimpleFilter(pFilter);
                }
                    break;
                case ScanDataCmd_DVB_TDT: {
                    SectionT* pSection = (SectionT*) datamsg.param;
                    delete pSection;
                }
                    break;
            }
        }
    }
    ScanDataWaitQ.unlock();
    ScanDataQUnlock();

    PMTPIDQ.clear();
    OtherTSIDQ.clear();
    //if(mScanMode != eScanMode_OOB)
    {
        //Service_Filter_List.lock();
        int sizeN = Service_Filter_List.sizeNoLock();
        for (int idx = 0; idx < sizeN; idx++) {
            TableFilter* pFilter = (TableFilter*) Service_Filter_List.AtNoLock(idx, res);
            if(res && pFilter) {
                pFilter->stopTable();
                L_INFO(TAG, "[SCAN ERROR] not received : pid=0x%X tid=0x%X tidex=0x%X\n", pFilter->getPID(), pFilter->m_tableID,pFilter->getTableIDExt());
                deleteTableFilter(pFilter);
            }
        }
        Service_Filter_List.clearNoLock();
        //Service_Filter_List.unlock();
        //Service_Section_Filter_List.lock();
        sizeN = Service_Section_Filter_List.sizeNoLock();
        for (int idx = 0; idx < sizeN; idx++) {
            SimpleFilter* pFilter = (SimpleFilter*) Service_Section_Filter_List.AtNoLock(idx, res);
            if(res && pFilter) {
                pFilter->stopFilter();
                L_INFO(TAG, "[SCAN ERROR] not received : pid=0x%X tid=0x%X tidex=0x%X\n", pFilter->m_pid, pFilter->m_tableID,pFilter->getTableIDExt());
                deleteSimpleFilter(pFilter);
            }
        }
        Service_Section_Filter_List.clearNoLock();
        //Service_Section_Filter_List.unlock();
        clearMonitorFilter();
    }

    if(bTestMode == false) {
        clearTSList();
    }

    clearMonitorBaseFilter();
    clearTableBaseFilter();
    clearSimpleBaseFilter();

    set_current_ts = false;
    sdt_other_requested = false;
    current_tsid = 0;
    scanTsCnt = 0;
    ts_map_ready = false;
    pmt_count = 0;
    sdt_count = 0;
    eit_pf_count = 0;
    eit_sch_count = 0;

    // eschoi_20150326 | fix block logic
    mManagerMode = eFirstConnect; // eIdle;

    //ScanDataMsg datamsg;
    datamsg.cmd = ScanDataCmd_SCAN_STOP;
    ScanDataQ.enqueue(datamsg);
    ScanManageQ.clear();
    ScanManagerUnlock();
}

void ScanManager::stop()
{
    //TODO
}

void ScanManager::resetEitMode()
{

    ListTs.lock();
    int sizeN = ListTs.sizeNoLock(), res;
    for (int idx = 0; idx < sizeN; idx++) {
        res = 0;
        ScanTransport* pTs = ListTs.AtNoLock(idx, res);
        if(res && pTs) {
            int sizeSer = pTs->serviceList.sizeNoLock();
            for (int serviceItr = 0; serviceItr < sizeSer; serviceItr++) {
                ScanService* pScanService = pTs->serviceList.AtNoLock(serviceItr, res);
                if(res && pScanService) {
                    if(pScanService->eit_pf_mode == eEIT_Requested
                            || pScanService->eit_pf_mode == eEIT_Standby
                            || (pScanService->eit_pf_flag && pScanService->eit_pf_version == 0xFF)) {
                        pScanService->eit_pf_mode = eEIT_NoOp;
                    } else if(pScanService->eit_pf_mode == eEIT_NoOp
                            && pScanService->eit_pf_version != 0xFF) {
                        pScanService->eit_pf_mode = eEIT_Received;
                    }
                    for (int schidx = 0; schidx < 16 && schidx <= max_receive_eit_idx; schidx++) {
                        if(pScanService->eit_schedule_mode[schidx] == eEIT_Requested
                                || pScanService->eit_schedule_mode[schidx] == eEIT_Standby
                                || (pScanService->eit_schedule_flag && pScanService->eit_sch_version[schidx] == 0xFF)) {
                            pScanService->eit_schedule_mode[schidx] = eEIT_NoOp;
                        } else if(pScanService->eit_schedule_mode[schidx] == eEIT_NoOp
                                && pScanService->eit_sch_version[schidx] != 0xFF) {
                            pScanService->eit_schedule_mode[schidx] = eEIT_Received;
                        }
                    }
                }
            }
        }
    }
    ListTs.unlock();
}

void ScanManager::channelChangeBefore()
{
    if(mScanMode == eScanMode_OOB)
        return;

    //ScanManagerLock();
    int res, sizeN;
    {
    	RecursiveMutex::Autolock lock(startMutex);
    	bScanManagerWork = false;
    }

    //ScanManageQ.clear();

    PMTPIDQ.clear();
    OtherTSIDQ.clear();

    //Service_Filter_List.lock();
    sizeN = Service_Filter_List.sizeNoLock();
    for (int idx = 0; idx < sizeN; idx++) {
        TableFilter* pFilter = (TableFilter*) Service_Filter_List.AtNoLock(idx, res);
        if(res && pFilter) {
            //pFilter->stopTable();
            if(bNeedScan)
                L_INFO(TAG, "[SCAN ERROR] not received : pid=0x%X tid=0x%X tidex=0x%X\n", pFilter->getPID(), pFilter->m_tableID,pFilter->getTableIDExt());
            deleteTableFilter(pFilter);
        }
    }
    Service_Filter_List.clearNoLock();
    //Service_Filter_List.unlock();

    //Service_Section_Filter_List.lock();
    sizeN = Service_Section_Filter_List.sizeNoLock();
    for (int idx = 0; idx < sizeN; idx++) {
        SimpleFilter* pFilter = (SimpleFilter*) Service_Section_Filter_List.AtNoLock(idx, res);
        if(res && pFilter) {
            //pFilter->stopFilter();
            if(bNeedScan)
                L_INFO(TAG, "[SCAN ERROR] not received : pid=0x%X tid=0x%X tidex=0x%X\n", pFilter->m_pid, pFilter->m_tableID,pFilter->getTableIDExt());
            L_INFO(TAG,"\n");
            deleteSimpleFilter(pFilter);
        }
    }
    Service_Section_Filter_List.clearNoLock();
    //Service_Section_Filter_List.unlock();

    clearMonitorBaseFilter();
    clearTableBaseFilter();
    clearSimpleBaseFilter();
    clearMonitorFilter();

    set_current_ts = false;
    sdt_other_requested = false;
    current_tsid = 0;
    {
    	RecursiveMutex::Autolock lock(startMutex);
    	bScanManagerWork = true;

    }
    eit_pf_count = 0;
    eit_sch_count = 0;
    //ScanManageQ.clear();
    //ScanManagerUnlock();
}
void ScanManager::channelChangeAfter(int freq)
{
	L_DEBUG(TAG,"\n");
    if(mScanMode == eScanMode_OOB) {
        if(bNeedScan != true) {
            return;
        }
    }
//    ScanManagerLock();
    {
    	RecursiveMutex::Autolock lock(startMutex);
    	bScanManagerWork = true;
    }

    if(mManagerMode == eInUpdate && freq != 0) {
        ScanTransport* pTs = getTransportStreamInfoFreq(freq);
        if(pTs) {
            set_current_ts = true;
            current_tsid = pTs->tsid;
            L_INFO(TAG, "current tsid(0x%X) set by freq=%d\n", current_tsid, freq);
        }
    }

    if(mManagerMode != eInScanning || (mScanMode != eScanMode_Add_PSI)) {
        if(bTestMode) {
            if(mScanMode == eScanMode_NIT_PSI) {
                if(NitVer == 0xff) {
                    ScanMsg msg;

                    msg.cmd = eScanCmd_NIT_TEST_REQUEST;
                    msg.iparam = TestNwId;
                    SendScanManageQ(msg);
                } else {
                    if(mManagerMode == eInScanning) {
                    	L_DEBUG(TAG,"\n");
                    	start_PSI();
                    }
                }
            } else {
                if(mManagerMode != eInUpdate)
                	L_DEBUG(TAG,"\n");
                    start_PSI();
            }
        } else {
            if(bUseDVB == true) {
            	L_DEBUG(TAG,"mManagerMode [%d]\n",mManagerMode);
                if(mManagerMode == eInUpdate) {
                    ScanMsg msg;
                    msg.cmd = eScanCmd_TDT_REQUEST;
                    SendScanManageQ(msg);
                    start_DVB();
                } else {
				// ksh_20140503 | block logic, dvb start when channel scan
					if(bNeedScan == false) {
						L_DEBUG(TAG,"dvb start when channel scan \n");
//						if(bHomets) start_PSI();
						start_DVB();
					}else {
						if(bUsePSI == true) {
							L_DEBUG(TAG,"\n");
							start_PSI();
						}
					}
				}
			}
        }
        L_DEBUG(TAG,"\n");
#if 0 // wjpark actual SDT 받은후에 start 2015-12-04
		if(bUsePSI == true) {
			start_PSI();
		}
#endif
	}

	else
    {
#if 1 // wjpark actual SDT 받은후에 start 2015-12-04
		if(mManagerMode == eInScanning) {
			if(bUsePSI == true) {
				L_DEBUG(TAG,"\n");
				start_PSI();
			}
		}
#endif
    }

//    ScanManagerUnlock();
}

bool ScanManager::isDVB()
{
    if(bUseDVB) {
        return true;
    }
    return false;
}
void ScanManager::TimeOut()
{
    if(mManagerMode == eIdle || mManagerMode == eInUpdate) {
        getSITime_t(&mLastOpSec);
        return;
    }

    if(isDVB()) {
        DVBTimeOut();
    } else {
    }
}
void ScanManager::DVBTimeOut()
{

    int res, sizeN;
    if(mScanMode == eScanMode_OOB || mScanMode == eScanMode_SemiOOB) {
        return;
    }

    if(mManagerMode == eInScanning) {

    	{
        	RecursiveMutex::Autolock lock(startMutex);
        	bScanManagerWork = false;
        }
        ScanManageQ.clear();

        PMTPIDQ.clear();
        OtherTSIDQ.clear();

        std::list<void*>::iterator itr;
        if(mScanMode == eScanMode_Add_FullSI || mScanMode == eScanMode_Normal_InBand) {
            sizeN = Service_Filter_List.sizeNoLock();
            for (int idx = 0; idx < sizeN; idx++) {
                TableFilter* pFilter = (TableFilter*) Service_Filter_List.AtNoLock(idx, res);
                if(res && pFilter) {
                    pFilter->stopTable();
                    L_INFO(TAG, "[SCAN ERROR] not received : pid=0x%X tid=0x%X tidex=0x%X\n", pFilter->getPID(), pFilter->m_tableID,pFilter->getTableIDExt());
                    deleteTableFilter(pFilter);
                }
            }
            Service_Filter_List.clearNoLock();
        }

        sizeN = Service_Section_Filter_List.sizeNoLock();
        for (int idx = 0; idx < sizeN; idx++) {
            SimpleFilter* pFilter = (SimpleFilter*) Service_Section_Filter_List.AtNoLock(idx, res);
            if(res && pFilter) {
                pFilter->stopFilter();
                L_INFO(TAG, "[SCAN ERROR] not received : pid=0x%X tid=0x%X tidex=0x%X\n", pFilter->m_pid, pFilter->m_tableID,pFilter->getTableIDExt());
                L_INFO(TAG,"\n");
                deleteSimpleFilter(pFilter);
            }
        }
        Service_Section_Filter_List.clearNoLock();

        {
        	RecursiveMutex::Autolock lock(startMutex);
        	bScanManagerWork = true;
        }

        requestTuneNext();
#if 0
        if(ts_map_ready)
        {
            //send pmt
            int waitQSize = ScanDataWaitQ.size();
            for(int wqloop=0; wqloop<waitQSize; wqloop++)
            {
                int qres=0;
                ScanDataMsg datawaitmsg = ScanDataWaitQ.dequeue(qres);
                if(qres)
                {
                    if(datawaitmsg.cmd == ScanDataCmd_PSI_Ready)
                    {
                        SendScanDataQ(datawaitmsg);
                    }
                }
            }
        }
#endif
    } else if(mManagerMode == eFirstConnect) {
        if(!ts_map_ready) {
        	L_DEBUG(TAG,"\n");
            if(bNeedScan) {
                ScanDataMsg datamsg;
                datamsg.cmd = ScanDataCmd_SCAN_START;
                SendScanDataQ(datamsg);
            } else if(bBoot == false) {
            	L_DEBUG(TAG,"\n");
                ScanDataMsg datamsg;
                datamsg.cmd = ScanDataCmd_DVB_NIT_TIMEOUT;
                SendScanDataQ(datamsg);
                if(request_tune_ap) {
                    request_tune_ap = false;
                }
                bBoot = true;
                isTDTTimeSet = true;
            } else if(bcreatedDMX  && bHomets) {
				L_DEBUG(TAG,"\n\n");
				MMF_Demux_Destroy(mDmxHandle);
				bcreatedDMX = false;
				bHomets = false;
            }
            return;
        } else if(bNeedScan) {
            {
            	RecursiveMutex::Autolock lock(startMutex);
            	bScanManagerWork = false;
            }

            ScanManageQ.clear();

            PMTPIDQ.clear();
            OtherTSIDQ.clear();

            std::list<void*>::iterator itr;
            if(mScanMode == eScanMode_Add_FullSI || mScanMode == eScanMode_Normal_InBand) {
                sizeN = Service_Filter_List.sizeNoLock();
                for (int idx = 0; idx < sizeN; idx++) {
                    TableFilter* pFilter = (TableFilter*) Service_Filter_List.AtNoLock(idx, res);
                    if(res && pFilter) {
                        pFilter->stopTable();
                        L_INFO(TAG, "[SCAN ERROR] not received : pid=0x%X tid=0x%X tidex=0x%X\n", pFilter->getPID(), pFilter->m_tableID,pFilter->getTableIDExt());
                        deleteTableFilter(pFilter);
                    }
                }
                Service_Filter_List.clearNoLock();
            }

            sizeN = Service_Section_Filter_List.sizeNoLock();
            for (int idx = 0; idx < sizeN; idx++) {
                SimpleFilter* pFilter = (SimpleFilter*) Service_Section_Filter_List.AtNoLock(idx, res);
                if(res && pFilter) {
                    pFilter->stopFilter();
                    L_INFO(TAG, "[SCAN ERROR] not received : pid=0x%X tid=0x%X tidex=0x%X\n", pFilter->m_pid, pFilter->m_tableID,pFilter->getTableIDExt());
                    L_INFO(TAG,"\n");
                    deleteSimpleFilter(pFilter);
                }
            }
            Service_Section_Filter_List.clearNoLock();

           {
            	RecursiveMutex::Autolock lock(startMutex);
            	bScanManagerWork = true;
            }

            psiCompleteInChannel = true;
            dvbCompleteInChannel = true;
            ScanManageQ.clear();
            //send scan complete
            L_INFO(TAG, "SEND ScanDataCmd_SCAN_ERR_END\n");

            ScanDataMsg datamsg;
            datamsg.cmd = ScanDataCmd_SCAN_ERR_END;
            SendScanDataQ(datamsg);
        } else {
        	L_DEBUG(TAG,"\n");
            if(isTDTTimeSet == false && ts_map_ready) {
            	L_DEBUG(TAG,"\n");
                ScanDataMsg datamsg;
                int waitQSize = ScanDataWaitQ.size();
                for (int wqloop = 0; wqloop < waitQSize; wqloop++) {
                	L_DEBUG(TAG,"\n");
                    int qres = 0;
                    ScanDataMsg datawaitmsg = ScanDataWaitQ.dequeue(qres);
                    if(qres) {
                    	L_DEBUG(TAG,"\n");
                        SendScanDataQ(datawaitmsg);
                    }
                }
                request_tune_ap = false;
                datamsg.cmd = ScanDataCmd_DVB_NIT_CHMAP_READY;
                SendScanDataQ(datamsg);
                sdt_other_requested = false;
            }
            isTDTTimeSet = true;
            change2UpdateMode();
        }
    }
}

void ScanManager::ResetEitFilterCount()
{
    eit_pf_count = 0;
    eit_sch_count = 0;
}

void ScanManager::RestartByNitChange()
{
	L_DEBUG(TAG,"\n");
    int res;
    {
    	RecursiveMutex::Autolock lock(startMutex);
    	bScanManagerWork = false;
    }

    ScanDataMsg datamsg;
    ScanDataQLock();
    int Qresult = 1;
    ScanDataQ.lock();
    while (Qresult) {
        datamsg = ScanDataQ.dequeueNoLock(Qresult);
        if(Qresult) {
            switch (datamsg.cmd) {
                case ScanDataCmd_DVB_NIT:
                case ScanDataCmd_DVB_SDT:
                case ScanDataCmd_DVB_SDT_COMPLETE:
                case ScanDataCmd_DVB_EIT:
                case ScanDataCmd_DVB_EIT_COMPLETE:
                case ScanDataCmd_PSIP_TVCT:
                case ScanDataCmd_PSIP_CVCT:
                case ScanDataCmd_PSIP_EIT:
                case ScanDataCmd_PSIP_EIT_COMPLETE: {
                    TableFilter* pFilter = (TableFilter*) datamsg.param;
                    deleteTableFilter(pFilter);
                }
                    break;
                case ScanDataCmd_PSI_Ready:
                case ScanDataCmd_PSIP_MGT:
                case ScanDataCmd_PSIP_ETT: {
                    SimpleFilter* pFilter = (SimpleFilter*) datamsg.param;
                    deleteSimpleFilter(pFilter);
                }
                    break;
                case ScanDataCmd_DVB_TDT: {
                    SectionT* pSection = (SectionT*) datamsg.param;
                    delete pSection;
                }
                    break;
            }
        }
    }
    ScanDataQ.unlock();

    Qresult = 1;
    ScanDataWaitQ.lock();
    while (Qresult) {
        datamsg = ScanDataWaitQ.dequeueNoLock(Qresult);
        if(Qresult) {
            switch (datamsg.cmd) {
                case ScanDataCmd_DVB_NIT:
                case ScanDataCmd_DVB_SDT:
                case ScanDataCmd_DVB_SDT_COMPLETE:
                case ScanDataCmd_DVB_EIT:
                case ScanDataCmd_DVB_EIT_COMPLETE:
                case ScanDataCmd_PSIP_TVCT:
                case ScanDataCmd_PSIP_CVCT:
                case ScanDataCmd_PSIP_EIT:
                case ScanDataCmd_PSIP_EIT_COMPLETE: {
                    TableFilter* pFilter = (TableFilter*) datamsg.param;
                    deleteTableFilter(pFilter);
                }
                    break;
                case ScanDataCmd_PSI_Ready:
                case ScanDataCmd_PSIP_MGT:
                case ScanDataCmd_PSIP_ETT: {
                    SimpleFilter* pFilter = (SimpleFilter*) datamsg.param;
                    deleteSimpleFilter(pFilter);
                }
                    break;
                case ScanDataCmd_DVB_TDT: {
                    SectionT* pSection = (SectionT*) datamsg.param;
                    delete pSection;
                }
                    break;
            }
        }
    }
    ScanDataWaitQ.unlock();
    ScanDataQUnlock();

    PMTPIDQ.clear();
    OtherTSIDQ.clear();

    Service_Filter_List.lock();
    int sizeN = Service_Filter_List.sizeNoLock();
    for (int idx = sizeN - 1; idx >= 0; idx--) {
        TableFilter* pFilter = (TableFilter*) Service_Filter_List.AtNoLock(idx, res);
        if(res && pFilter) {
            deleteTableFilter(pFilter);
        }
    }
    Service_Filter_List.clearNoLock();
    Service_Filter_List.unlock();

    Service_Section_Filter_List.lock();
    sizeN = Service_Section_Filter_List.sizeNoLock();
    for (int idx = sizeN - 1; idx >= 0; idx--) {
        SimpleFilter* simpleFilter = (SimpleFilter*) Service_Section_Filter_List.AtNoLock(idx, res);
        if(res && simpleFilter) {
            deleteSimpleFilter(simpleFilter);
        }
    }
    Service_Section_Filter_List.clearNoLock();
    Service_Section_Filter_List.unlock();
    clearMonitorFilter();

    clearTSList();
    set_current_ts = false;
    sdt_other_requested = false;
    current_tsid = 0;
    scanTsCnt = 0;
    ts_map_ready = false;
    pmt_count = 0;
    sdt_count = 0;
    eit_pf_count = 0;
    eit_sch_count = 0;
    bEITBoost = bEITBoostBackup;

    clearMonitorBaseFilter();
    clearTableBaseFilter();
    clearSimpleBaseFilter();

    ScanManageQ.clear();
    {
    	RecursiveMutex::Autolock lock(startMutex);
    	bScanManagerWork = true;
    }
    start_DVB();
//    if(bUsePSI) {
//        start_PSI();
//    }
}

void ScanManager::change2ScanMode()
{

    mManagerMode = eInScanning;

    ScanTransport* pTs = getTransportStreamInfo(current_tsid);
    if(pTs) {
        pTs->scanned = true;
        pTs->scantried = true;
    }

    if(mScanMode == eScanMode_OOB || mScanMode == eScanMode_SemiOOB) {
        change2UpdateMode();
        return;
    }

    requestTuneNext();
}
void ScanManager::change2UpdateMode()
{
    mManagerMode = eInUpdate;
    bEITBoost = false;
    if(bTestMode) {
    } else {
        if(bUseDVB) {
            if(bTunerLocked) {
            	start_DVB();
            }
        } else {
        }
    }

    if(bNeedScan == true
            && (mScanMode == eScanMode_OOB || mScanMode == eScanMode_SemiOOB || mScanMode == eScanMode_Normal_InBand || mScanMode == eScanMode_NIT_PSI)) {
        bNeedScan = false;
        ScanDataMsg datamsg;
        datamsg.cmd = ScanDataCmd_SCAN_END;
        SendScanDataQ(datamsg);
        mManagerMode = eInScanEnd;
    }
}

ScanTransport* ScanManager::getNextTs()
{
    if(mScanMode == eScanMode_OOB || mScanMode == eScanMode_SemiOOB) {
        return NULL;
    }

    ListTs.lock();
    int sizeN = ListTs.sizeNoLock();
    int res;
    for (int idx = 0; idx < sizeN; idx++) {
        ScanTransport* pTs = ListTs.AtNoLock(idx, res);
        if(res && pTs && pTs->scanned == false && pTs->scantried == false) {
            ListTs.unlock();
            pCurrentTs = pTs;
            return pTs;
        }
    }
    ListTs.unlock();
    return NULL;
}

void ScanManager::setManualFreq(int Freq)
{
    frequencyStartHz = Freq * 1000;
}
void ScanManager::setAutoFreq(int startFreq, int endFreq)
{
    frequencyStartHz = startFreq * 1000;
    frequencyEndHz = endFreq * 1000;
}

void ScanManager::scanStart(bool bAuto, bool bFixedMod)
{
    ScanManagerLock();
    bScanCompleted = false;
    bTestMode = true;

//    current_uri = getUri(1);
    request_tune_ap = false;

    int res;
    {
    	RecursiveMutex::Autolock lock(startMutex);
    	bScanManagerWork = false;
    }

    if(bBoot == false)
        bBoot = true;

    ScanDataMsg datamsg;
    ScanDataQLock();
    int Qresult = ScanDataQ.size();
    ScanDataQ.lock();
    while (Qresult) {
        datamsg = ScanDataQ.dequeueNoLock(Qresult);
        if(Qresult) {
            switch (datamsg.cmd) {
                case ScanDataCmd_DVB_NIT:
                case ScanDataCmd_DVB_SDT:
                case ScanDataCmd_DVB_SDT_COMPLETE:
                case ScanDataCmd_DVB_EIT:
                case ScanDataCmd_DVB_EIT_COMPLETE:
                case ScanDataCmd_PSIP_TVCT:
                case ScanDataCmd_PSIP_CVCT:
                case ScanDataCmd_PSIP_EIT:
                case ScanDataCmd_PSIP_EIT_COMPLETE: {
                    TableFilter* pFilter = (TableFilter*) datamsg.param;
                    deleteTableFilter(pFilter);
                }
                    break;
                case ScanDataCmd_PSI_Ready:
                case ScanDataCmd_PSIP_MGT:
                case ScanDataCmd_PSIP_ETT: {
                    SimpleFilter* pFilter = (SimpleFilter*) datamsg.param;
                    deleteSimpleFilter(pFilter);
                }
                    break;
                case ScanDataCmd_DVB_TDT: {
                    SectionT* pSection = (SectionT*) datamsg.param;
                    delete pSection;
                }
                    break;
            }
        }
    }
    ScanDataQ.unlock();

    Qresult = ScanDataWaitQ.size();
    ScanDataWaitQ.lock();
    while (Qresult) {
        datamsg = ScanDataWaitQ.dequeueNoLock(Qresult);
        if(Qresult) {
            switch (datamsg.cmd) {
                case ScanDataCmd_DVB_NIT:
                case ScanDataCmd_DVB_SDT:
                case ScanDataCmd_DVB_SDT_COMPLETE:
                case ScanDataCmd_DVB_EIT:
                case ScanDataCmd_DVB_EIT_COMPLETE:
                case ScanDataCmd_PSIP_TVCT:
                case ScanDataCmd_PSIP_CVCT:
                case ScanDataCmd_PSIP_EIT:
                case ScanDataCmd_PSIP_EIT_COMPLETE: {
                    TableFilter* pFilter = (TableFilter*) datamsg.param;
                    deleteTableFilter(pFilter);
                }
                    break;
                case ScanDataCmd_PSI_Ready:
                case ScanDataCmd_PSIP_MGT:
                case ScanDataCmd_PSIP_ETT: {
                    SimpleFilter* pFilter = (SimpleFilter*) datamsg.param;
                    deleteSimpleFilter(pFilter);
                }
                    break;
                case ScanDataCmd_DVB_TDT: {
                    SectionT* pSection = (SectionT*) datamsg.param;
                    delete pSection;
                }
                    break;
            }
        }
    }
    ScanDataWaitQ.unlock();
    ScanDataQUnlock();

    PMTPIDQ.clear();

    int sizeN = Service_Filter_List.sizeNoLock();
    for (int idx = 0; idx < sizeN; idx++) {
        TableFilter* pFilter = (TableFilter*) Service_Filter_List.AtNoLock(idx, res);
        if(res && pFilter) {
            pFilter->stopTable();
            deleteTableFilter(pFilter);
        }
    }
    Service_Filter_List.clearNoLock();

    sizeN = Service_Section_Filter_List.sizeNoLock();
    for (int idx = 0; idx < sizeN; idx++) {
        SimpleFilter* pFilter = (SimpleFilter*) Service_Section_Filter_List.AtNoLock(idx, res);
        if(res && pFilter) {
            pFilter->stopFilter();
            deleteSimpleFilter(pFilter);
        }
    }
    Service_Section_Filter_List.clearNoLock();

    clearTSList();
    clearMonitorBaseFilter();
    clearTableBaseFilter();
    clearSimpleBaseFilter();

    set_current_ts = false;
    sdt_other_requested = false;
    current_tsid = 0;
    scanTsCnt = 0;
    ts_map_ready = false;
    pmt_count = 0;
    sdt_count = 0;
    eit_pf_count = 0;
    eit_sch_count = 0;

    {
    	RecursiveMutex::Autolock lock(startMutex);
    	bScanManagerWork = false;
    }

    bNeedScan = true;
    mScanMode = eScanMode_Normal_InBand;
    bFixedModulation = bFixedMod;

    memcpy(scan_tuner, &scan_tuner_ap, sizeof(TDI_TunerSettings));
    if(bFixedModulation == false)
        scan_tuner->protocol = TDI_TunerProtocol_64QAM;

    if(bAuto) {
        int start, end, sr, bw;
        start = frequencyStartHz;
        end = frequencyEndHz;
        sr = scan_tuner->parameter.rf.symbolrate;
        bw = bandwidthkHz;

        ScanTransport* pTs = NULL;
        pTs = MakeTsList(start, end, sr, bw, scan_tuner->protocol);

        if(pTs) {
            pCurrentTs = pTs;
            pTs->scantried = true;
            scan_tuner->protocol = (TDI_TunerProtocol) pTs->deliverySystem.inbnd.modulation;
            scan_tuner->parameter.rf.frequencyHz = pTs->deliverySystem.inbnd.freq;
            scan_tuner->parameter.rf.symbolrate = pTs->deliverySystem.inbnd.symbolrate;
            scan_tuner->parameter.rf.bandwidth = pTs->deliverySystem.inbnd.bandwidth;
        }
    } else {
        scan_tuner->parameter.rf.frequencyHz = frequencyStartHz * 1000;
        ScanTransport* pTs = new ScanTransport;
        if(pTs) {
            init_ts(pTs);
            pTs->deliveryType = eDeliveryTypeInBand;
            pTs->deliverySystem.inbnd.modulation = scan_tuner->protocol;
            pTs->deliverySystem.inbnd.freq = scan_tuner->parameter.rf.frequencyHz;
            pTs->deliverySystem.inbnd.symbolrate = scan_tuner->parameter.rf.symbolrate;
            pTs->deliverySystem.inbnd.bandwidth = scan_tuner->parameter.rf.bandwidth;
            pTs->scantried = true;
            ListTs.push_back(pTs);
            pCurrentTs = pTs;
        }
    }

    ScanManageQ.clear();
    SCAN_MANAGER_TIMEOUT_SEC = AUTO_MANUAL_SCAN_TIMEOUT_SEC;
    L_INFO(TAG, "SCAN_MANAGER_TIMEOUT_SEC=%d\n", (int )SCAN_MANAGER_TIMEOUT_SEC);

    ScanMsg msg;
    //first channel tune
    msg.cmd = eScanCmd_TUNE_START;
    msg.iparam = 1; // scan mode
    //?�단?�?그냥 ip tune

    msg.param = (void*) scan_tuner;

    SendScanManageQ(msg, true);
    mManagerMode = eInScanning;

    datamsg.cmd = ScanDataCmd_SCAN_START;
    ScanDataQ.enqueue(datamsg);

    ScanManagerUnlock();
}

void ScanManager::scanStartManualChannel(int networkId)
{
    ScanManagerLock();
    bTestMode = true;
    bScanCompleted = false;
    SCAN_MANAGER_TIMEOUT_SEC = (time_t) MANUAL_CHANNEL_SCAN_TIMEOUT_SEC;
    L_INFO(TAG, "SCAN_MANAGER_TIMEOUT_SEC=%d\n", (int )SCAN_MANAGER_TIMEOUT_SEC);
    Parser::getInstance().setWepgChannelMapUrl(mWepgChannelMapFileURL);

    request_tune_ap = false;

    int res;
    {
    	RecursiveMutex::Autolock lock(startMutex);
    	bScanManagerWork = false;
    }

    ScanManageQ.clear();
    if(bBoot == false)
        bBoot = true;

    ScanDataMsg datamsg;
    ScanDataQLock();
    int Qresult = ScanDataQ.size();
    ScanDataQ.lock();
    while (Qresult) {
        datamsg = ScanDataQ.dequeueNoLock(Qresult);
        if(Qresult) {
            switch (datamsg.cmd) {
                case ScanDataCmd_DVB_NIT:
                case ScanDataCmd_DVB_SDT:
                case ScanDataCmd_DVB_SDT_COMPLETE:
                case ScanDataCmd_DVB_EIT:
                case ScanDataCmd_DVB_EIT_COMPLETE:
                case ScanDataCmd_PSIP_TVCT:
                case ScanDataCmd_PSIP_CVCT:
                case ScanDataCmd_PSIP_EIT:
                case ScanDataCmd_PSIP_EIT_COMPLETE: {
                    TableFilter* pFilter = (TableFilter*) datamsg.param;
                    deleteTableFilter(pFilter);
                }
                    break;
                case ScanDataCmd_PSI_Ready:
                case ScanDataCmd_PSIP_MGT:
                case ScanDataCmd_PSIP_ETT: {
                    SimpleFilter* pFilter = (SimpleFilter*) datamsg.param;
                    deleteSimpleFilter(pFilter);
                }
                    break;
                case ScanDataCmd_DVB_TDT: {
                    SectionT* pSection = (SectionT*) datamsg.param;
                    delete pSection;
                }
                    break;
            }
        }
    }
    ScanDataQ.unlock();

    Qresult = ScanDataWaitQ.size();
    ScanDataWaitQ.lock();
    while (Qresult) {
        datamsg = ScanDataWaitQ.dequeueNoLock(Qresult);
        if(Qresult) {
            switch (datamsg.cmd) {
                case ScanDataCmd_DVB_NIT:
                case ScanDataCmd_DVB_SDT:
                case ScanDataCmd_DVB_SDT_COMPLETE:
                case ScanDataCmd_DVB_EIT:
                case ScanDataCmd_DVB_EIT_COMPLETE:
                case ScanDataCmd_PSIP_TVCT:
                case ScanDataCmd_PSIP_CVCT:
                case ScanDataCmd_PSIP_EIT:
                case ScanDataCmd_PSIP_EIT_COMPLETE: {
                    TableFilter* pFilter = (TableFilter*) datamsg.param;
                    deleteTableFilter(pFilter);
                }
                    break;
                case ScanDataCmd_PSI_Ready:
                case ScanDataCmd_PSIP_MGT:
                case ScanDataCmd_PSIP_ETT: {
                    SimpleFilter* pFilter = (SimpleFilter*) datamsg.param;
                    deleteSimpleFilter(pFilter);
                }
                    break;
                case ScanDataCmd_DVB_TDT: {
                    SectionT* pSection = (SectionT*) datamsg.param;
                    delete pSection;
                }
                    break;
            }
        }
    }
    ScanDataWaitQ.unlock();
    ScanDataQUnlock();

    PMTPIDQ.clear();

    int sizeN = Service_Filter_List.sizeNoLock();
    for (int idx = 0; idx < sizeN; idx++) {
        TableFilter* pFilter = (TableFilter*) Service_Filter_List.AtNoLock(idx, res);
        if(res && pFilter) {
            pFilter->stopTable();
            deleteTableFilter(pFilter);
        }
    }
    Service_Filter_List.clearNoLock();

    sizeN = Service_Section_Filter_List.sizeNoLock();
    for (int idx = 0; idx < sizeN; idx++) {
        SimpleFilter* pFilter = (SimpleFilter*) Service_Section_Filter_List.AtNoLock(idx, res);
        if(res && pFilter) {
            pFilter->stopFilter();
            deleteSimpleFilter(pFilter);
        }
    }
    Service_Section_Filter_List.clearNoLock();

    clearTSList();
    clearMonitorBaseFilter();
    clearTableBaseFilter();
    clearSimpleBaseFilter();

    set_current_ts = false;
    sdt_other_requested = false;
    current_tsid = 0;
    scanTsCnt = 0;
    ts_map_ready = false;
    pmt_count = 0;
    sdt_count = 0;
    eit_pf_count = 0;
    eit_sch_count = 0;

    {
    	RecursiveMutex::Autolock lock(startMutex);
    	bScanManagerWork = false;
    }

    bNeedScan = true;
    mScanMode = eScanMode_NIT_PSI;
    bFixedModulation = true;
    TestNwId = networkId;

    memcpy(scan_tuner, &scan_tuner_ap, sizeof(TDI_TunerSettings));
    scan_tuner->parameter.rf.frequencyHz = frequencyStartHz * 1000;

    ScanMsg msg;
    //first channel tune
    msg.cmd = eScanCmd_TUNE_START;

    msg.param = (void*) scan_tuner;

    SendScanManageQ(msg, true);
    mManagerMode = eFirstConnect;

    ScanManagerUnlock();
}

void ScanManager::cleanupFactoryTest()
{
    ScanManagerLock();
    bTestMode = false;
    invertTuner = false;
    SCAN_MANAGER_TIMEOUT_SEC = (time_t) timeoutInSec;
    L_INFO(TAG, "SCAN_MANAGER_TIMEOUT_SEC=%d\n", (int )SCAN_MANAGER_TIMEOUT_SEC);


    int res;

    {
    	RecursiveMutex::Autolock lock(startMutex);
    	bScanManagerWork = false;
    }

    ScanManageQ.clear();

    ScanDataMsg datamsg;
    ScanDataQLock();
    int Qresult = ScanDataQ.size();
    ScanDataQ.lock();
    while (Qresult) {
        datamsg = ScanDataQ.dequeueNoLock(Qresult);
        if(Qresult) {
            switch (datamsg.cmd) {
                case ScanDataCmd_DVB_NIT:
                case ScanDataCmd_DVB_SDT:
                case ScanDataCmd_DVB_SDT_COMPLETE:
                case ScanDataCmd_DVB_EIT:
                case ScanDataCmd_DVB_EIT_COMPLETE:
                case ScanDataCmd_PSIP_TVCT:
                case ScanDataCmd_PSIP_CVCT:
                case ScanDataCmd_PSIP_EIT:
                case ScanDataCmd_PSIP_EIT_COMPLETE: {
                    TableFilter* pFilter = (TableFilter*) datamsg.param;
                    deleteTableFilter(pFilter);
                }
                    break;
                case ScanDataCmd_PSI_Ready:
                case ScanDataCmd_PSIP_MGT:
                case ScanDataCmd_PSIP_ETT: {
                    SimpleFilter* pFilter = (SimpleFilter*) datamsg.param;
                    deleteSimpleFilter(pFilter);
                }
                    break;
                case ScanDataCmd_DVB_TDT: {
                    SectionT* pSection = (SectionT*) datamsg.param;
                    delete pSection;
                }
                    break;
            }
        }
    }
    ScanDataQ.unlock();

    Qresult = ScanDataWaitQ.size();
    ScanDataWaitQ.lock();
    while (Qresult) {
        datamsg = ScanDataWaitQ.dequeueNoLock(Qresult);
        if(Qresult) {
            switch (datamsg.cmd) {
                case ScanDataCmd_DVB_NIT:
                case ScanDataCmd_DVB_SDT:
                case ScanDataCmd_DVB_SDT_COMPLETE:
                case ScanDataCmd_DVB_EIT:
                case ScanDataCmd_DVB_EIT_COMPLETE:
                case ScanDataCmd_PSIP_TVCT:
                case ScanDataCmd_PSIP_CVCT:
                case ScanDataCmd_PSIP_EIT:
                case ScanDataCmd_PSIP_EIT_COMPLETE: {
                    TableFilter* pFilter = (TableFilter*) datamsg.param;
                    deleteTableFilter(pFilter);
                }
                    break;
                case ScanDataCmd_PSI_Ready:
                case ScanDataCmd_PSIP_MGT:
                case ScanDataCmd_PSIP_ETT: {
                    SimpleFilter* pFilter = (SimpleFilter*) datamsg.param;
                    deleteSimpleFilter(pFilter);
                }
                    break;
			case ScanDataCmd_DVB_TDT: {
                    SectionT* pSection = (SectionT*) datamsg.param;
                    delete pSection;
                }
                    break;
            }
        }
    }
    ScanDataWaitQ.unlock();
    ScanDataQUnlock();

    PMTPIDQ.clear();
    int sizeN = Service_Filter_List.sizeNoLock();
    for (int idx = 0; idx < sizeN; idx++) {
        TableFilter* pFilter = (TableFilter*) Service_Filter_List.AtNoLock(idx, res);
        if(res && pFilter) {
            pFilter->stopTable();
            deleteTableFilter(pFilter);
        }
    }
    Service_Filter_List.clearNoLock();

    sizeN = Service_Section_Filter_List.sizeNoLock();
    for (int idx = 0; idx < sizeN; idx++) {
        SimpleFilter* pFilter = (SimpleFilter*) Service_Section_Filter_List.AtNoLock(idx, res);
        if(res && pFilter) {
            pFilter->stopFilter();
            deleteSimpleFilter(pFilter);
        }
    }
    Service_Section_Filter_List.clearNoLock();

    clearTSList();
    clearMonitorBaseFilter();
    clearTableBaseFilter();
    clearSimpleBaseFilter();

    set_current_ts = false;
    sdt_other_requested = false;
    current_tsid = 0;
    scanTsCnt = 0;
    ts_map_ready = false;
    pmt_count = 0;
    sdt_count = 0;
    eit_pf_count = 0;
    eit_sch_count = 0;
    bEITBoost = bEITBoostBackup;
    bUsePSI = bUsePSIBackup;
    bUseDVB = bUseDVBBackup;
    bUsePSIP = bUsePSIPBackup;
    bUseOtherTS = bUseOtherTSBackup;
    bReceiveEITInScan = bReceiveEITInScanBackup;
    mScanMode = mScanModeBackup;

    {
    	RecursiveMutex::Autolock lock(startMutex);
    	bScanManagerWork = true;
    }

    mManagerMode = eFirstConnect;

    if(bTunerLocked) {
        if(bUseDVB == true) {
            start_DVB();
        }
        if(bUsePSI == true) {
#if 0 // wjpark actual SDT 받은후에 start 2015-12-04
            start_PSI();
#endif
        }
    }

    ScanManagerUnlock();
}

std::list<ServiceInfo*> ScanManager::getChList()
{
    std::list<ServiceInfo*> retList;
    {
        ListTs.lock();
        int sizeTs = ListTs.sizeNoLock();
        int res = 0;
        L_INFO(TAG, "sizeTs=%d\n", sizeTs);
        for (int idx = 0; idx < sizeTs; idx++) {
            ScanTransport* pTs = ListTs.AtNoLock(idx, res);
            if(res && pTs) {
                if(pTs->bLocked) {
                    int sizeSer = pTs->serviceList.sizeNoLock();
                    L_INFO(TAG, "sizeSer=%d\n", sizeSer);
                    for (int idxSer = 0; idxSer < sizeSer; idxSer++) {
                        ScanService* pScanService = pTs->serviceList.AtNoLock(idxSer, res);
                        if(res && pScanService) {
                            if(pScanService->pmt_version != 0xFF) {
                                ServiceInfo* pInfo = makeServiceInfo(pTs, pScanService);
                                if(pInfo) {
                                    retList.push_back(pInfo);
                                }
                            }
                        }
                    }
                }
            }
        }
        ListTs.unlock();
    }

    return retList;
}

ServiceInfo* ScanManager::makeServiceInfo(ScanTransport* pTs, ScanService* pScanService)
{
    bool bFind = false;
    int tunerID=0;
    ServiceInfo* pInfo = new ServiceInfo;
    if(pInfo) {
        pInfo->transport_stream_id = pTs->tsid;
        pInfo->original_network_id = 1;
        pInfo->service_id = pScanService->sid;
        pInfo->channel_number = pInfo->service_id;

        if(pTs->deliveryType == eDeliveryTypeInBand) {
            pInfo->frequency = pTs->deliverySystem.inbnd.freq / 1000;
            if(pTs->deliverySystem.inbnd.modulation == TDI_TunerProtocol_64QAM) {
                pInfo->modulation = 3;
            } else if(pTs->deliverySystem.inbnd.modulation == TDI_TunerProtocol_128QAM) {
                pInfo->modulation = 4;
            } else if(pTs->deliverySystem.inbnd.modulation == TDI_TunerProtocol_256QAM) {
                pInfo->modulation = 5;
            }
        } else if(pTs->deliveryType == eDeliveryTypeC) {
            pInfo->frequency = pTs->deliverySystem.cable.freq / 1000;
            if(pTs->deliverySystem.cable.modulation == TDI_TunerProtocol_64QAM) {
                pInfo->modulation = 3;
            } else if(pTs->deliverySystem.cable.modulation == TDI_TunerProtocol_128QAM) {
                pInfo->modulation = 4;
            } else if(pTs->deliverySystem.cable.modulation == TDI_TunerProtocol_256QAM) {
                pInfo->modulation = 5;
            }
        }

        pInfo->pcr_pid = pScanService->pcr_id;
        int sizees = pScanService->esList.sizeNoLock();
        int res = 0;
        for (int idx = 0; idx < sizees; idx++) {
            ES_INFO* pes = pScanService->esList.AtNoLock(idx, res);
            if(res && pes) {
                elementary_stream es;
                es.stream_pid = pes->pid;
                es.stream_type = pes->stream_type;
                pInfo->list_elementary_stream.push_back(es);
                L_INFO(TAG, "pid=0x%X, streamtype=0x%X\n", es.stream_pid, es.stream_type);

                if(bFind == false
                        && (es.stream_type == 0x1 || es.stream_type == 0x2 || es.stream_type == 0x10 || es.stream_type == 0x1b
                                || es.stream_type == 0x3 || es.stream_type == 0x4 || es.stream_type == 0x6 || es.stream_type == 0xf
                                || es.stream_type == 0x81)) {
                    bFind = true;
                }
            }
        }
        pInfo->init();
        if(invertTuner) tunerID=1;
        pInfo->setTunerID(tunerID);
    }
    if(bFind) {
        return pInfo;
    } else {
        delete pInfo;
        return NULL;
    }
}

void aging_timer_cb(int id)
{
    if(id == ID_TIMER_CHECK_SCAN_MANAGER) {
        ScanManagerLock();
        ScanManager::getInstance().RestartByNitChange();
        ScanManagerUnlock();
    }
}

void start_si_aging()
{
    set_timer(ID_TIMER_CHECK_SCAN_MANAGER, 20 * 60000, (TimerHandler *) aging_timer_cb);
}
