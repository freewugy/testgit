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
#include <stdio.h>
#include <memory.h>
#include <unistd.h>

#include "Logger.h"
#include "TableBaseFilter.h"
#include "MMF/MMFInterface.h"
#include "ByteStream.h"
#ifdef ANDROID
#include <pthread.h>
#endif

#include "TDI.h"
#include "SIQ.h"
#include "ScanTypes.h"
#include "Timer.h"

static const char* TAG = "TableBaseFilter";

#ifdef ANDROID
pthread_mutex_t mTableBaseMutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
pthread_mutex_t CandiMutex[16] = {PTHREAD_RECURSIVE_MUTEX_INITIALIZER,};
#else    
pthread_mutex_t mTableBaseMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t CandiMutex[16] = {PTHREAD_MUTEX_INITIALIZER,};
#endif

static bool TableFilterThreadDestroy = false;
static bool tableBaseFilterThreadcreated = false;

unsigned long TableFilterallocCount=0;

static bool BlockTableBaseCallback = false;
#define MAX_TABLEQSIZE  8000

SIQ<SectionMsg> sectionQ;
SIQ<FilterDelMsg> DeleteReqQ;
SIVector<TableBaseFilter*> workTableBaseFilterList;

void TableBaseLock()
{
//	L_INFO(TAG, "\n");
	pthread_mutex_lock(&mTableBaseMutex);
}

void TableBaseUnlock()
{
//	L_INFO(TAG, "\n");
	pthread_mutex_unlock(&mTableBaseMutex);
}

int isFilterAlive(TableBaseFilter* pFilter)
{
    if(pFilter) {
        workTableBaseFilterList.lock();
        int sizeN = workTableBaseFilterList.sizeNoLock();

        for (int itr = 0; itr < sizeN; itr++) {
            int res;
            TableBaseFilter* pBaseFilter = workTableBaseFilterList.AtNoLock(itr, res);
            if(res && pBaseFilter && pFilter == pBaseFilter) {
                workTableBaseFilterList.unlock();
                return 1;
            }
        }
        workTableBaseFilterList.unlock();
    }
    return 0;
}
static void TDI_section_cb(const int sectionFilterHandle, const int eventId, const int pid, const unsigned char *filterData, const unsigned int length, void *pReceiver)
{
//    L_DEBUG(TAG, "TDI_section_cb : sectionFilterHandle[%d], eventId[%d], pid[0x%x], length[%d]\n", sectionFilterHandle, eventId, pid, r16(&filterData[1]) & 0x0fff);

    if(TableFilterThreadDestroy)
        return;

    if(sectionQ.size()>MAX_TABLEQSIZE && !BlockTableBaseCallback)
    {
        BlockTableBaseCallback=true;
//        L_INFO(TAG, "\n\n\nBlock TableBaseCallback\n");
    }   
        
    if(BlockTableBaseCallback)
    {
        siMiliSleep(10);
        return;
    }

    if(filterData == NULL || pReceiver == NULL) {
        L_ERROR(TAG, " === OOPS : DATA or FILTER IS NULL === \n");
        return;
    }
    TableBaseFilter* pFilter = (TableBaseFilter*) pReceiver;

    if(isFilterAlive(pFilter)) {
        uint16_t extid = TableIDExt(filterData);
        if(pFilter->checkCandidate(extid))
        {
            SectionMsg msg;
            msg.pReceiver = pReceiver;
            msg.length = SectionLength(filterData);
            msg.pid = pid;
            if(msg.length > 0) {
                TableFilterallocCount++;
                msg.pBuff = (uint8_t*)malloc(msg.length*sizeof(uint8_t));//new uint8_t[msg.length];
                memcpy(msg.pBuff, filterData, msg.length);
                sectionQ.enqueue(msg);
                {
                    //free(msg.pBuffer);
                    //delete[] msg.pBuffer;
                    TableFilterallocCount--;
                }

            } else {
                L_ERROR(TAG, " === OOPS : DATA IS WRONG AND SHORT === \n");
            }
        }
        else
        {
            //L_INFO(TAG, " === NO CANDIDATAE === \n");
        }
    }
}

void* tSectionReceiver(void* param)
{
        int workCount = 0;
	while (1) {
		int delResult = 1;
		while (delResult) {
		    TableBaseLock();
			FilterDelMsg delmsg = DeleteReqQ.dequeue(delResult);
			if(delResult) {
				if(delmsg.cmd == 1) {
					TableFilter* pFilter = (TableFilter*) delmsg.filter;
                    pFilter->releaseBuffer();
                    siMiliSleep(3);
                    delete pFilter;
				} else if(delmsg.cmd == 2) {
					TableBaseFilter* pRealFilter = (TableBaseFilter*) delmsg.basefilter;
					siMiliSleep(3);
					delete pRealFilter;
				}
				else
                {
                    siMiliSleep(10);
                }
			}
			TableBaseUnlock();
		}

		if(BlockTableBaseCallback && sectionQ.size()==0)
        {
            BlockTableBaseCallback=false;
            L_INFO(TAG, "\n\nUnBlock TableBaseCallback\n");
        }

		if(workTableBaseFilterList.size() == 0 && TableFilterThreadDestroy == false) {
			sectionQ.lock();
            int Qresult = 1;
            while (Qresult) {
                SectionMsg msg = sectionQ.dequeueNoLock(Qresult);
                if(Qresult)
                {
                    free(msg.pBuff);
                    //delete[] msg.pBuffer;
                    TableFilterallocCount--;
                }
            }
            sectionQ.unlock();
            siMiliSleep(30);
            TableFilterThreadDestroy = false;
        } else {
            int secQResult = 0;
            SectionMsg msg = sectionQ.dequeue(secQResult);
            if(secQResult == 0) {
                siMiliSleep(10);
            } else {
                TableBaseFilter* pRealFilter = (TableBaseFilter*) msg.pReceiver;
                if(isFilterAlive(pRealFilter) && pRealFilter->getPid() == msg.pid) {
                    if(pRealFilter->UseModified == 1) {
                        int row, col;
//                        L_INFO(TAG, "\n");
                        TableBaseLock();
                        uint16_t extid = TableIDExt(msg.pBuff);
                        TableFilter* pFilter = pRealFilter->getMatchedTableFilter1(msg.pBuff[0], extid, row, col);
						TableBaseUnlock();
                        if(pFilter) 
                        {
                            if(pFilter->m_pTable == NULL) {
                                pFilter->m_pTable = new Table;
                                Table *pTable = pFilter->m_pTable;

                                pTable->setSectionCount(msg.pBuff[7] + 1);

                                pTable->setSection(msg.pBuff, msg.length);
                            } else {
                                Table *pTable = pFilter->m_pTable;
                                pTable->setSection(msg.pBuff, msg.length);
                            }
                            //msg.pBuffer = NULL;
                            TableFilterallocCount--;

                            if(pFilter->m_pTable->isComplete()) {
                                pFilter->stopTable();
                                pRealFilter->eraseCandidate(extid);
                                pFilter->processTable();
                            }
                        }
                        
                    } else if(pRealFilter->UseModified == 0) {
//                    	L_INFO(TAG, "\n");
                        TableBaseLock();
                        TableFilter* pFilter = pRealFilter->getMatchedTableFilter(TableIDExt(msg.pBuff));
						TableBaseUnlock();
						if(pFilter) {
                            if(pFilter->m_pTable == NULL) {
                                pFilter->m_pTable = new Table;
                                if(pFilter->m_pTable == NULL)
                                    break;
                                Table *pTable = pFilter->m_pTable;

                                pTable->setSectionCount(msg.pBuff[7] + 1);

                                pTable->setSection(msg.pBuff, msg.length);
                            } else {
                                Table *pTable = pFilter->m_pTable;
                                pTable->setSection(msg.pBuff, msg.length);
                            }
                            //msg.pBuffer = NULL;
                            TableFilterallocCount--;
                            if(pFilter->m_pTable->isComplete()) {
                                pFilter->stopTable();
                                //callback
                                pFilter->processTable();
                            }
                        }
                    }
                }
                if(msg.pBuff != NULL) {
                    //delete[] msg.pBuffer;
                    free(msg.pBuff);
                    TableFilterallocCount--;
                }
                siMiliSleep(5);
            }
        }
    }
    L_INFO(TAG, "\n\n\n\n\n\n\nthread exit\n\n\n\n\n\n\n\n\n\n");
}

void CreatTableTask(void *(taskfn)(void*), uint32_t priority)
{

	if(tableBaseFilterThreadcreated) {
		return;
	}
	tableBaseFilterThreadcreated = true;
#ifdef __DEBUG_LOG_SM__
	L_INFO(TAG, "Create TableFilter Thread\n");
#endif
    sectionQ.lock();
    int Qresult = 1;
    while (Qresult) {
        SectionMsg msg = sectionQ.dequeueNoLock(Qresult);
        if(Qresult)
        {
            free(msg.pBuff);
            //delete[] msg.pBuffer;
        }
    }
    sectionQ.unlock();
    pthread_t task;
    TableFilterThreadDestroy = false;
    pthread_create(&task, NULL, taskfn, NULL);
}

static int table_filter_count = 0;

TableBaseFilter::TableBaseFilter()
{
	table_filter_count++;
	TableBaseLock();
	mHdlTdi = 0x7FFFFFFFFFFFFFFF;
	mHdl = this;
	mPid = 0x1FFF;
	mTableID = 0xFF;
	bWorking = false;
	UseModified = 0;
	CreatTableTask(tSectionReceiver, 0);
	workTableBaseFilterList.push_back(this);
	TableBaseUnlock();
//	L_DEBUG(TAG,"count [%d],[%p]\n",table_filter_count,this);
}

TableBaseFilter::~TableBaseFilter()
{
	table_filter_count--;
	if(mHdlTdi != 0x7FFFFFFFFFFFFFFF) {
//		MMF_Filter_Close(mHdlTdi);
		MMF_Filter_Stop(mHdlTdi);
		mHdlTdi = 0x7FFFFFFFFFFFFFFF;
	}
//	L_DEBUG(TAG,"table_filter_count [%d],[%p]\n",table_filter_count,this);
}

void TableBaseFilter::createBaseFilter(int tableId, int mask, int pid, void*& hdl, TableFilter* filter)
{
//	L_INFO(TAG, "%s", __FUNCTION__);

	TableBaseLock();
	if(filter) {
		mListFilter.push_back(filter);

		mPid = pid;
		mTableID = tableId;
		int demux_hdl = 0;
		if(mHdlTdi == 0x7FFFFFFFFFFFFFFF) {
//			MMF_Filter_Open( &mHdlTdi);
			UseModified = 0;
            mCandidateList[15].push_back(0xffff);
		}
		mTidmask = mask;
		hdl = mHdl;
	}
	TableBaseUnlock();
}

void TableBaseFilter::createBaseFilter1(int tableId, int mask, int pid, void*& hdl, int magic, TableFilter* filter)
{
	L_INFO(TAG, "%s", __FUNCTION__);

	TableBaseLock();
	mPid = pid;
	mTableID = tableId;
	int demux_hdl = 0;
	if(filter) {
		if(mHdlTdi == 0x7FFFFFFFFFFFFFFF) {
//			MMF_Filter_Open(&mHdlTdi);
			for (int i = 0; i < 16; i++) {
				ModifiedParam* param = new ModifiedParam;
				if(param) {
					mModListFilter.push_back(param);
				}
			}
			UseModified = 1;
		}

		int res;
		mModListFilter.lock();
		ModifiedParam* param = mModListFilter.AtNoLock((magic & 0xF), res);
		if(res && param) {
			param->mListFilter.push_back(filter);
		}
		mModListFilter.unlock();
		mTidmask = mask;
		hdl = mHdl;
	}
	TableBaseUnlock();
}

void TableBaseFilter::createBaseFilter2(int tableId, int mask, int pid, void*& hdl, int magic, TableFilter* filter)
{
	L_INFO(TAG, "%s", __FUNCTION__);

	TableBaseLock();
	if(filter) {
		bool bFind = false;
		mPid = pid;
		mTableID = tableId;
		int demux_hdl = 0;

		if(mHdlTdi == 0x7FFFFFFFFFFFFFFF) {
//			MMF_Filter_Open( &mHdlTdi);
			UseModified = 2;
			for (int i = 0; i < 16; i++) {
				ModifiedParam* param = new ModifiedParam;
				if(param) {
					mModListFilter.push_back(param);
				}
			}
		}

		int res;
		mModListFilter.lock();
		ModifiedParam* param = mModListFilter.AtNoLock((magic & 0xF), res);
		if(res && param) {
			param->mListFilter.push_back(filter);
		}
		mModListFilter.unlock();
		mTidmask = mask;
		hdl = mHdl;
        pthread_mutex_lock(&CandiMutex[magic&0xf]);
        mCandidateList[magic&0xf].push_back(magic);
        pthread_mutex_unlock(&CandiMutex[magic&0xf]);
	}
	TableBaseUnlock();
}

int TableBaseFilter::startBaseFilter(string uri)
{
//	L_INFO(TAG, "%s", __FUNCTION__);
	int ret = 1;
	if(bWorking == false) {
		bool err = false;
		bool isScanMode = false;
		uint8_t mask[12] = { 0, };
		uint8_t mode[12] = { 0, };
		uint8_t comp[12] = { 0, };

		mask[0] = mTidmask;
		comp[0] = mTableID;

		if(mHdlTdi == 0x7FFFFFFFFFFFFFFF) {
//			err = TDI_Filter_Start(mHdlTdi, mPid, &mask, TDI_section_cb, (void*) this);
			err = MMF_Filter_Start(&mHdlTdi ,(uint8_t*)uri.c_str(), mPid, mask, mode, comp, TDI_section_cb, (void*)this, SECTION_MODE_CONTINOUS);
//			L_DEBUG(TAG,"handle [%lld], URI [%s]\n",mHdlTdi,uri.c_str());
		}

		if(err) {
			ret = 1;
			bWorking = true;
		} else {
			ret = 0;
		}
	}
	return ret;
}

void TableBaseFilter::stopBaseFilter()
{
//	L_INFO(TAG, "%s", __FUNCTION__);

	if(bWorking == true) {
		if(UseModified == 0) {
			mListFilter.lock();
			int sizeN = mListFilter.sizeNoLock();
			for (int i = 0; i < sizeN; i++) {
				int res;
				TableFilter* filter = mListFilter.AtNoLock(i, res);
				if(filter->isWorking()) {
					mListFilter.unlock();
					return;
				}
			}
			mListFilter.unlock();
		} else {
			mModListFilter.lock();
			for (int i = 0; i < 16; i++) {
				int res;
				ModifiedParam* param = mModListFilter.AtNoLock(i, res);
				if(res && param) {
					param->mListFilter.lock();
					int sizeN = param->mListFilter.sizeNoLock();
					for (int idx = 0; idx < sizeN; idx++) {
						TableFilter* filter = param->mListFilter.AtNoLock(idx, res);
						if(res && filter && filter->isWorking()) {
							param->mListFilter.unlock();
							mModListFilter.unlock();
							return;
						}
					}
					param->mListFilter.unlock();
				}
			}
			mModListFilter.unlock();
		}
		if(mHdlTdi != 0x7FFFFFFFFFFFFFFF) {
//			TDI_Filter_Stop(mHdlTdi);
			MMF_Filter_Stop(mHdlTdi);
//			mHdlTdi = 0x7FFFFFFFFFFFFFFF;
		}
		bWorking = false;
	}
}

void TableBaseFilter::closeBaseFilter()
{
	L_INFO(TAG, "%s", __FUNCTION__);
	if(mHdlTdi != 0x7FFFFFFFFFFFFFFF) {
//		MMF_Filter_Close(mHdlTdi);
//		L_DEBUG(TAG, " mHdlTdi =[%d]\n",mHdlTdi);
//		if(bWorking)
			MMF_Filter_Stop(mHdlTdi);
		mHdlTdi = 0x7FFFFFFFFFFFFFFF;
	}
}

bool TableBaseFilter::deleteTableFilter(TableFilter* filter)
{
	L_INFO(TAG, "%s", __FUNCTION__);
	bool bfind = false;
	int magic;
	if(filter) {
		if(UseModified == 1) {
			magic = (filter->getTableIDExt() & 0xF);
		} else if(UseModified == 2) {
			magic = (filter->getModTSID() & 0xF);
		} else {
			return false;
		}
		int res;
		mModListFilter.lock();
		ModifiedParam *param = mModListFilter.AtNoLock(magic, res);
		if(res && param)
			bfind = param->mListFilter.erase(filter);
		mModListFilter.unlock();
	}
	return bfind;
}

int TableBaseFilter::getTableID()
{
	return mTableID;
}

int TableBaseFilter::getPid()
{
	return mPid;
}

void* TableBaseFilter::getHandle()
{
	return mHdl;
}

TableFilter* TableBaseFilter::getMatchedTableFilter(uint16_t key)
{
	int sizeN = mListFilter.size();
	for (int idx = 0; idx < sizeN; idx++) {
		int res;
		mListFilter.lock();
		TableFilter* pFilter = mListFilter.AtNoLock(idx, res);
		mListFilter.unlock();

		if(res && pFilter && (pFilter->getTableIDExt() == 0xFFFE || key == pFilter->getTableIDExt())) {
			if(pFilter->isWorking()) {
				return pFilter;
			} else {
				return NULL;
			}
		}
	}
	return NULL;
}

TableFilter* TableBaseFilter::getMatchedTableFilter1(uint8_t tid, uint16_t key, int& row, int& col)
{
	L_INFO(TAG, "%s", __FUNCTION__);

	int magic = (key & 0xF);
	int res;

	mModListFilter.lock();
	ModifiedParam *param = mModListFilter.AtNoLock(magic, res);
	if(res && param) {
		row = magic;
		//param->mListFilter.lock();
		int sizeN = param->mListFilter.sizeNoLock();
		for (col = 0; col < sizeN; col++) {
			TableFilter* pFilter = param->mListFilter.AtNoLock(col, res);
			if(res && pFilter && key == pFilter->getTableIDExt() && ((tid & 0xF) == (pFilter->m_tableID & 0xF)) && tid >= 0x50) {
				if(pFilter->isWorking()) {
					//param->mListFilter.unlock();
					mModListFilter.unlock();
					return pFilter;
				} else {
					//param->mListFilter.unlock();
					mModListFilter.unlock();
					return NULL;
				}
			} else if(res && pFilter && key == pFilter->getTableIDExt() && pFilter->m_tableID < 0x50 && tid < 0x50) {
				if(pFilter->isWorking()) {
					//param->mListFilter.unlock();
					mModListFilter.unlock();
					return pFilter;
				} else {
					//param->mListFilter.unlock();
					mModListFilter.unlock();
					return NULL;
				}
			}
		}
		//param->mListFilter.unlock();
		mModListFilter.unlock();
	}
	return NULL;
}

void TableBaseFilter::deleteMatchedTableFilter(int row, int col, TableFilter* pFilter)
{
	L_INFO(TAG, "%s", __FUNCTION__);
    int res;
    if(pFilter) {
        mModListFilter.lock();
        ModifiedParam *param = mModListFilter.AtNoLock(row, res);
        if(res && param)
            param->mListFilter.erase(pFilter);
		mModListFilter.unlock();

    }
}

bool TableBaseFilter::checkCandidate(uint16_t extid)
{
    int idx = extid & 0xf;
    bool bFind=false;
    pthread_mutex_lock(&CandiMutex[idx]);
    for(list<uint16_t>::iterator Itr = mCandidateList[idx].begin(); Itr != mCandidateList[idx].end(); Itr++)
    {
        if(*Itr == extid)
            bFind=true;
    }
    pthread_mutex_unlock(&CandiMutex[idx]);
    
    if(bFind == false)
    {
        pthread_mutex_lock(&CandiMutex[0xf]);
        for(list<uint16_t>::iterator Itr = mCandidateList[15].begin(); Itr != mCandidateList[15].end(); Itr++)
        {
            if(*Itr == 0xffff)
                bFind=true;
        }
        pthread_mutex_unlock(&CandiMutex[0xf]);
    }
    return bFind;
}

void TableBaseFilter::eraseCandidate(uint16_t extid)
{
    int idx = extid & 0xf;
    pthread_mutex_lock(&CandiMutex[idx]);
    for(list<uint16_t>::iterator Itr = mCandidateList[idx].begin(); Itr != mCandidateList[idx].end(); Itr++)
    {
        if(*Itr == extid)
        {
            mCandidateList[idx].erase(Itr);
            break;
        }
    }
    pthread_mutex_unlock(&CandiMutex[idx]);
}



void deleteTableBaseFilter(TableBaseFilter* basefilter, TableFilter* filter)
{
//	L_INFO(TAG, "\n");
	TableBaseLock();
//	workTableBaseFilterList.lock();
	if(isFilterAlive(basefilter)) {
		bool findfilter = false;
		if(filter) {
			FilterDelMsg msg;
			if(basefilter->UseModified == 0) {
				findfilter = basefilter->mListFilter.erase(filter);
				if(findfilter) {
					if(basefilter->mListFilter.size() == 0) {
						basefilter->closeBaseFilter();
						workTableBaseFilterList.erase(basefilter);
						unregisterTableBaseFilter(basefilter);
						if(DeleteReqQ.size()==0)
                        {
                            msg.cmd = 100;
                            DeleteReqQ.enqueue(msg);
                            siMiliSleep(3);
                        }
						msg.cmd = 2;
						msg.basefilter = (void*) basefilter;
						msg.filter = (void*) filter;
						DeleteReqQ.enqueue(msg);
					}
				}
			} else {
				findfilter = basefilter->deleteTableFilter(filter);
				if(findfilter) {
					bool remainSomething = false;
					basefilter->mModListFilter.lock();
					for (int i = 0; i < 16; i++) {
						int res;
						ModifiedParam* param = basefilter->mModListFilter.AtNoLock(i, res);
						if(res && param && param->mListFilter.size() != 0) {
							remainSomething = true;
							break;
						}
					}
					basefilter->mModListFilter.unlock();
					if(remainSomething == false) {
						basefilter->closeBaseFilter();
						workTableBaseFilterList.erase(basefilter);
						unregisterTableBaseFilter(basefilter);
                        if(DeleteReqQ.size()==0)
                        {
                            msg.cmd = 100;
                            DeleteReqQ.enqueue(msg);
                            siMiliSleep(3);
                        }
						msg.cmd = 2;
						msg.basefilter = (void*) basefilter;
						msg.filter = (void*) filter;
						DeleteReqQ.enqueue(msg);
					}
				}
			}
		}
	}
	TableBaseUnlock();
}

void deleteTableFilterRequest(TableFilter* pFilter)
{
    if(pFilter) {
        TableBaseLock();
        FilterDelMsg msg;
        if(DeleteReqQ.size()==0)
        {
            msg.cmd = 100;
            DeleteReqQ.enqueue(msg);
            siMiliSleep(3);
        }
        msg.cmd = 1;
        msg.filter = (void*) pFilter;
        msg.basefilter = NULL;
        DeleteReqQ.enqueue(msg);
        TableBaseUnlock();
    }
}

void clearTableBaseFilterQ()
{
    int secQResult = 0;
    sectionQ.lock();
    while (sectionQ.sizeNoLock()) {
        SectionMsg msg = sectionQ.dequeueNoLock(secQResult);
        if(secQResult) {
            free(msg.pBuff);
            //delete[] msg.pBuffer;
        }
    }
    sectionQ.unlock();
}
