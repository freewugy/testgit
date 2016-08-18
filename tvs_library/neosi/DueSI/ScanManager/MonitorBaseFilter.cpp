/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: eschoi $
 * $LastChangedDate: 2015-03-24 22:29:35 +0900 (Tue, 24 Mar 2015) $
 * $LastChangedRevision: 1092 $
 * Description:
 * Note:
 *****************************************************************************/

#include <stdio.h>
#include <memory.h>
#include <unistd.h>

#include "Logger.h"
#include "ByteStream.h"
#include "ScanTypes.h"

#include "../include/mmf/TVSMMFClient.h"
#include "../MMF/MMFInterface.h"

#include "MonitorBaseFilter.h"
#ifdef ANDROID
#include <pthread.h>
#endif

#include "TDI.h"
#include "SIQ.h"
#include "Timer.h"
#include "Section.h"

static const char* TAG = "MonitorBaseFilter";

#define __DEBUG_LOG_SM__

SIQ<MonitorMsg> MonitorQ;
SIQ<FilterDelMsg> DelMBFReqQ;
std::vector<MonitorFilter*>::iterator monitorfilteritr;
SIVector<MonitorBaseFilter*> workMonitorBaseFilterList;
#ifdef ANDROID
pthread_mutex_t mMonitorBaseMutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
#else    
pthread_mutex_t mMonitorBaseMutex = PTHREAD_MUTEX_INITIALIZER;
#endif

static bool MonitorFilterThreadDestroy = false;
static bool monitorBaseFilterThreadcreated = false;
static bool BlockMonitorBaseCallback = false;
static bool BlockMonitorBaseCallbackByClean = false;
#define MAX_MONITORQSIZE  100

static int monitor_filter_count = 0;

void MonitorBaseLock()
{
//	L_INFO(TAG, "\n");
	pthread_mutex_lock(&mMonitorBaseMutex);
}
void MonitorBaseUnlock()
{
//	L_INFO(TAG, "\n");
	pthread_mutex_unlock(&mMonitorBaseMutex);
}

bool checkMonitorPidFilter(int sectionFilterHandle, int pid, const unsigned char *sectionData, unsigned int length, void *pReceiver)
{
	MonitorBaseFilter* filter = (MonitorBaseFilter*) pReceiver;
	if (SectionLength(sectionData) == length) {
		if (IsValidSectionCheck((uint8_t *) sectionData, SectionLength(sectionData))) {
			return true;
		} else {
			L_DEBUG(TAG,"[ERROR] CRC Error\n");
		}
	} else {
		L_DEBUG(TAG,"[ERROR] SectionLength(sectionData)=%d length=%d\n", SectionLength(sectionData), length);
	}
	return false;
}

int isFilterAlive(MonitorBaseFilter* pFilter)
{
    if(pFilter) {
        workMonitorBaseFilterList.lock();
        int sizeN = workMonitorBaseFilterList.sizeNoLock();
        for (int itr = 0; itr < sizeN; itr++) {
            int res;
            MonitorBaseFilter* pBaseFilter = workMonitorBaseFilterList.AtNoLock(itr, res);
            if(res && pBaseFilter && (pFilter == pBaseFilter)) {
                workMonitorBaseFilterList.unlock();
                return 1;
            }
        }
        workMonitorBaseFilterList.unlock();
    }
    return 0;
}
static void TDI_Monitor_cb(const int sectionFilterHandle, const int eventId, const int pid, const unsigned char *filterData, const unsigned int length, void *pReceiver)
{
//	L_DEBUG(TAG, "TDI_Monitor_cb : sectionFilterHandle[%d], eventId[%d], pid[0x%x], length[%d]\n", sectionFilterHandle, eventId, pid, length);
    if(MonitorQ.size()>MAX_MONITORQSIZE && !BlockMonitorBaseCallback)
    {
        BlockMonitorBaseCallback=true;
        L_INFO(TAG, "\n\n\nBlock MonitorBaseCallback\n");
    }
    
    if(BlockMonitorBaseCallback || BlockMonitorBaseCallbackByClean)
    {
        //siMiliSleep(3);
        return;
    }

    if(filterData == NULL || pReceiver == NULL) {
        L_ERROR(TAG, " === OOPS : DATA or FILTER IS NULL === \n");
        return;
    }

    MonitorBaseFilter* pBaseFilter = (MonitorBaseFilter*) pReceiver;

    if (checkMonitorPidFilter(sectionFilterHandle, pid, filterData, length, pReceiver)) {
		 if(isFilterAlive(pBaseFilter)) {
			MonitorMsg msg;
			msg.pReceiver = pReceiver;
			msg.pid = pid;
			msg.tid = filterData[0];
			msg.tidex = TableIDExt(filterData);
			msg.version = Version(filterData);
			MonitorQ.enqueue(msg);
		}
    }

}

void* tMonitorReceiver(void* param)
{
	L_INFO(TAG, "tMonitorReceiver >> begin");
    int count = 0;
    int workCount = 0;
    while (1) {
#if 1        
        workCount++;
        if(workCount % 5000 == 0) {
            workCount = 0;
            L_INFO(TAG, "thread work filter count = %d, this count[%d]\n", workMonitorBaseFilterList.size(), monitor_filter_count);
        }
#endif        
        int Qresult = 1;
        while (Qresult) {
            MonitorBaseLock();
            FilterDelMsg delmsg = DelMBFReqQ.dequeue(Qresult);
            if(Qresult) {
                
                if(delmsg.cmd == 1) {
                    MonitorFilter* pFilter = (MonitorFilter*) delmsg.filter;
                    siMiliSleep(3);
					delete pFilter;
                } else if(delmsg.cmd == 2) {
                    MonitorBaseFilter* pBaseFilter = (MonitorBaseFilter*) delmsg.basefilter;
                    siMiliSleep(3);
//                    L_DEBUG(TAG,"[%p] pid=0x%X tid=0x%X\n",pBaseFilter,pBaseFilter->getPid(),pBaseFilter->getTableID());
					delete pBaseFilter;
                }
                else
                {
                    siMiliSleep(10);
                }
                
            }
            MonitorBaseUnlock();
        }
        
        if(BlockMonitorBaseCallback && MonitorQ.size()==0)
        {
            BlockMonitorBaseCallback=false;
            L_INFO(TAG, "\n\nUnBlock MonitorBaseCallback\n");
        }

        if(workMonitorBaseFilterList.size() == 0 && MonitorFilterThreadDestroy == false) {
//            MonitorQ.clear();
//            siMiliSleep(10);
            MonitorFilterThreadDestroy = true;
//            L_DEBUG(TAG,"workMonitorBaseFilterList size is 0 [%d]\n",monitor_filter_count);
        } else {
            MonitorMsg msg = MonitorQ.dequeue(Qresult);
            if(Qresult == 0) {
                siMiliSleep(10);
            } else {
                MonitorBaseFilter* pBaseFilter = (MonitorBaseFilter*) msg.pReceiver;
                if(isFilterAlive(pBaseFilter) && pBaseFilter->getPid() == msg.pid) {
					MonitorBaseLock();
					MonitorFilter* pFilter = pBaseFilter->getMatchedMonitorFilter(msg.tid, msg.tidex);
					MonitorBaseUnlock();					
					if(pFilter) {
						if(pFilter->m_ver != msg.version) {
							pFilter->stopFilter();
							L_INFO(TAG, "pid=0x%04X, tid=0x%02X(0x%02X) tidex=0x%04X(0x%04X) ver=0x%02X->0x%02X\n", pFilter->m_pid,
									pFilter->m_tableID, msg.tid, pFilter->getTableIDExt(), msg.tidex, pFilter->m_ver, msg.version);
							//callback
							pFilter->processSection(msg.tid);
						}
					}
				}
                //workMonitorBaseFilterList.unlock();
                siMiliSleep(5);
            }
        }
		if(MonitorFilterThreadDestroy) {
            MonitorQ.clear();
            siMiliSleep(30);
            MonitorFilterThreadDestroy = false;
        }
    }
    L_INFO(TAG, "tMonitorReceiver << end");
}

void CreatMonitorTask(void *(taskfn)(void*), uint32_t priority)
{
    if(monitorBaseFilterThreadcreated)
        return;
    monitorBaseFilterThreadcreated = true;
#ifdef __DEBUG_LOG_SM__
    L_INFO(TAG, "Create MonitorFilter Thread\n");
#endif
    MonitorQ.clear();
    pthread_t task;
    MonitorFilterThreadDestroy = false;

    pthread_create(&task, NULL, taskfn, NULL);

    return;
}

MonitorBaseFilter::MonitorBaseFilter()
{
	monitor_filter_count++;
	MonitorBaseLock();
	mHdlTdi = 0x7FFFFFFFFFFFFFFF;
	mHdl = this;
	mPid = 0x1FFF;
	mTableID = 0xFF;
	bWorking = false;
	UseModified = 0;
	mTidmask = 0xFF;

	CreatMonitorTask(tMonitorReceiver, 0);
	workMonitorBaseFilterList.push_back(this);
//	L_DEBUG(TAG,"monitor base count [%d] [%p]\n",monitor_filter_count,this);
	MonitorBaseUnlock();
	BlockMonitorBaseCallbackByClean=false;
}

MonitorBaseFilter::~MonitorBaseFilter()
{
//    L_DEBUG(TAG,"[%p]\n",this);
	if(mHdlTdi != 0x7FFFFFFFFFFFFFFF) {
//		MMF_Filter_Close(mHdlTdi);
		MMF_Filter_Stop(mHdlTdi);
		mHdlTdi = 0x7FFFFFFFFFFFFFFF;
	}

	monitor_filter_count--;
//	L_DEBUG(TAG,"count [%d], [%p]\n",monitor_filter_count,this);
}

void MonitorBaseFilter::createBaseFilter(int tableId, int mask, int pid, void*& hdl, MonitorFilter* filter)
{
	MonitorBaseLock();
	if(filter) {
//		L_DEBUG(TAG,"[%p]\n",this);
		mListFilter.push_back(filter);

		mPid = pid;
		mTableID = tableId;
		int demux_hdl = 0;

		if(mHdlTdi == 0x7FFFFFFFFFFFFFFF) {
//			MMF_Filter_Open( &mHdlTdi);
			UseModified = 0;
		}
		mTidmask = mask;
		hdl = mHdl;
	}
	MonitorBaseUnlock();
}

void MonitorBaseFilter::createBaseFilter1(int tableId, int mask, int pid, void*& hdl, int magic, MonitorFilter* filter)
{
	mPid = pid;
	mTableID = tableId;
	int demux_hdl = 0;
    MonitorBaseLock();
	if(filter) {
		int res;
        mModListFilter.lock();
        MonitorModifiedParam* param = mModListFilter.AtNoLock(magic & 0xF, res);
        if(res && param) {
            param->mListFilter.push_back(filter);
        }
        mModListFilter.unlock();
	}
	MonitorBaseUnlock();
	mTidmask = mask;
	hdl = mHdl;
}
int MonitorBaseFilter::startBaseFilter(string uri)
{
	int ret = 1;
	if(bWorking == false) {
//		L_DEBUG(TAG,"[%p]\n",this);
		bool err = false;
		uint8_t mask[12] = { 0, };
		uint8_t mode[12] = { 0, };
		uint8_t comp[12] = { 0, };

		comp[0] = (uint8_t) mTableID;
		mask[0] = mTidmask;
		// ksh_20140506 | net need to set negative mask
		comp[6] = 0;
		mask[6] = 0xFF;
		 if(mHdlTdi != 0x7FFFFFFFFFFFFFFF) {
			L_DEBUG(TAG,"Fail startBaseFilter\n");
			return 0;
		}
		if(mHdlTdi == 0x7FFFFFFFFFFFFFFF) {
//          err = TDI_Filter_Start(mHdlTdi, mPid, &mask, TDI_Monitor_cb, (void*) this);
			err = MMF_Filter_Start(&mHdlTdi ,(uint8_t*)uri.c_str(), mPid, mask, mode, comp, TDI_Monitor_cb, (void*)this, SECTION_MODE_VERSION_CHANGE);
			L_DEBUG(TAG,"handle [%lld], URI [%s]\n",mHdlTdi,uri.c_str());
		}
//		L_DEBUG(TAG,"[%p]\n",this);
		if(err) {
			ret = 1;
			bWorking = true;
		} else {
			ret = 0;
		}

	}
	return ret;
}

void MonitorBaseFilter::stopBaseFilter()
{
	if(bWorking == true) {
		if(UseModified == 0) {
			mListFilter.lock();
			int sizeN = mListFilter.sizeNoLock();
			for (int i = 0; i < sizeN; i++) {
				int res;
				MonitorFilter* filter = mListFilter.AtNoLock(i, res);
				if(res && filter && filter->isWorking()) {
					mListFilter.unlock();
					return;
				}
			}
			mListFilter.unlock();
		} 
		if(mHdlTdi != 0x7FFFFFFFFFFFFFFF) {
//			TDI_Filter_Stop(mHdlTdi);
			MMF_Filter_Stop(mHdlTdi);
		}
		bWorking = false;
	}
}

void MonitorBaseFilter::closeBaseFilter()
{
	if(mHdlTdi != 0x7FFFFFFFFFFFFFFF) {
//		MMF_Filter_Close(mHdlTdi);
//		if(bWorking)
			MMF_Filter_Stop(mHdlTdi);
		mHdlTdi = 0x7FFFFFFFFFFFFFFF;
	}
}

bool MonitorBaseFilter::deleteMonitorFilter(MonitorFilter* filter)
{
	bool bfind = false;
	int magic;
	if(filter) {
		if(UseModified == 1) {
			magic = ((filter->getTableIDExt()) & 0xF);
		} else {
			return false;
		}
		int res;
		mModListFilter.lock();
		MonitorModifiedParam *param = mModListFilter.AtNoLock(magic, res);
		if(res && param)
			bfind = param->mListFilter.erase(filter);
		mModListFilter.unlock();
	}
	return bfind;
}

int MonitorBaseFilter::getTableID()
{
	return mTableID;
}

int MonitorBaseFilter::getPid()
{
	return mPid;
}

void* MonitorBaseFilter::getHandle()
{
	return mHdl;
}

MonitorFilter* MonitorBaseFilter::getMatchedMonitorFilter(uint8_t tid, uint16_t key)
{
	int sizeN = mListFilter.size();
	for (int idx = 0; idx < sizeN; idx++) {
		int res;
		mListFilter.lock();
		MonitorFilter* pFilter = mListFilter.AtNoLock(idx, res);
		mListFilter.unlock();
		if(res && pFilter && (pFilter->m_tableID & mTidmask) == (tid & mTidmask)
				&& (pFilter->getTableIDExt() == 0xFFFE || key == pFilter->getTableIDExt())) {
			if(pFilter->isWorking()) {
				return pFilter;
			} else {
				return NULL;
			}
		}
	}
	return NULL;
}

MonitorFilter* MonitorBaseFilter::getMatchedMonitorFilter1(uint16_t key, int& row, int& col)
{
    int magic = (key & 0xF);
    int res = 0;
    return NULL;
}

void MonitorBaseFilter::deleteMatchedMonitorFilter(int row, int col, MonitorFilter* pFilter)
{
    int res = 0;
    if(pFilter) {
        mModListFilter.lock();
        MonitorModifiedParam *param = mModListFilter.AtNoLock(row, res);
        if(res && param) {
            param->mListFilter.erase(pFilter);
        }
        mModListFilter.unlock();
    }
}

void deleteMonitorBaseFilter(MonitorBaseFilter* pBaseFilter, MonitorFilter* pFilter)
{
   MonitorBaseLock();
    if(isFilterAlive(pBaseFilter)) {
    	//workMonitorBaseFilterList.unlock();
        if(pFilter) {
            FilterDelMsg msg;
            if(pBaseFilter->UseModified == 0) {
//                L_DEBUG(TAG,"mListFilter size = %d",pBaseFilter->mListFilter.size());
                if(pBaseFilter->mListFilter.erase(pFilter) && pBaseFilter->mListFilter.size() == 0) {
                    pBaseFilter->closeBaseFilter();
                    workMonitorBaseFilterList.erase(pBaseFilter);
                    unregisterMonitorBaseFilter(pBaseFilter);
                    if(DelMBFReqQ.size()==0)
                    {
                        msg.cmd = 100;
                        DelMBFReqQ.enqueue(msg);
                        siMiliSleep(5);
                    }
                    
//                    L_DEBUG(TAG,"send del q [%p]\n",pBaseFilter);
                    msg.cmd = 2;
                    msg.basefilter = (void*) pBaseFilter;
                    msg.filter = NULL;
                    DelMBFReqQ.enqueue(msg);
                }
            }
        }
    }
    MonitorBaseUnlock();

}

void deleteMonitorFilterRequest(MonitorFilter* pFilter)
{
    if(pFilter) {
        MonitorBaseLock();
        FilterDelMsg msg;
        if(DelMBFReqQ.size()==0)
        {
            msg.cmd = 100;
            DelMBFReqQ.enqueue(msg);
            siMiliSleep(5);
        }
        
        msg.cmd = 1;
        msg.filter = (void*) pFilter;
        msg.basefilter = NULL;
		DelMBFReqQ.enqueue(msg);
		MonitorBaseUnlock();
    }
}

void clearMonitorBaseFilterQ()
{
    BlockMonitorBaseCallbackByClean = true;
	MonitorQ.clear();
}
