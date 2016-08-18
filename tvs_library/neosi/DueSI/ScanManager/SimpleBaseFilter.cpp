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

#include "SimpleBaseFilter.h"

#include <pthread.h>
//#include <cstdint>
#include <cstring>
#include <vector>
#include <unistd.h>
#include "MMF/MMFInterface.h"
#include "../util/ByteStream.h"
#include "ScanTypes.h"
#include "Section.h"
#include "Logger.h"

#ifdef ANDROID
#include <pthread.h>
#endif
#include "TDI.h"
#include "SIQ.h"
#include "Timer.h"

static const char* TAG = "SimpleBaseFilter";

SIQ<SectionMsg> simpleQ;
SIQ<FilterDelMsg> DeleteSFReqQ;
std::vector<SimpleFilter*>::iterator simplefilteritr;
SIVector<SimpleBaseFilter*> workSimpleBaseFilterList;

#ifdef ANDROID
pthread_mutex_t mSimpleBaseMutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
#else    
pthread_mutex_t mSimpleBaseMutex = PTHREAD_MUTEX_INITIALIZER;
#endif

static bool SimpleFilterThreadDestroy=false;
static bool SimpleBaseFilterThreadcreated = false;

void SimpleBaseLock()
{
//	L_INFO(TAG, "\n");
	pthread_mutex_lock(&mSimpleBaseMutex);
}
void SimpleBaseUnlock()
{
//	L_INFO(TAG, "\n");
	pthread_mutex_unlock(&mSimpleBaseMutex);
}

bool simplecheckPidFilter(int sectionFilterHandle, int pid, const unsigned char *sectionData, unsigned int length, void *pReceiver)
{
	SimpleBaseFilter* filter = (SimpleBaseFilter*) pReceiver;
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

int isSimpleFilterAlive(SimpleBaseFilter* filter)
{
	if(filter)
	{
		workSimpleBaseFilterList.lock();
		int sizeN = workSimpleBaseFilterList.sizeNoLock();
		for(int itr=0; itr<sizeN; itr++)
		{
			int res;
			SimpleBaseFilter* bfilter = workSimpleBaseFilterList.AtNoLock(itr,res);
			if(res && bfilter && filter == bfilter)
			{
				workSimpleBaseFilterList.unlock();
				return 1;
			}
		}
		workSimpleBaseFilterList.unlock();
	}
	return 0;
}

static void Simple_section_cb(const int sectionFilterHandle, const int eventId, const int pid, const unsigned char *filterData, const unsigned int length, void *pReceiver)
{
//    L_DEBUG(TAG, "Simple_section_cb : sectionFilterHandle[%d], eventId[%d], pid[0x%x], length[%d]\n", sectionFilterHandle, eventId, pid, length);
    
    SimpleBaseLock();
    uint16_t section_Length=0;
	SimpleBaseFilter* filter = (SimpleBaseFilter*)pReceiver;
	if(isSimpleFilterAlive(filter))
	{
		SectionMsg msg;
		msg.sectionFilterHandle = sectionFilterHandle;
		msg.pReceiver = pReceiver;
		msg.length = length;
		msg.pid = pid;
		if(pid != 0x14) {
			section_Length = r16(&filterData[1]) & 0x0fff;
			L_DEBUG(TAG,"section length [%d], [%d], [%d]\n",section_Length, length,SectionLength(filterData));
//			section_Length = SectionLength(filterData);
//			L_DEBUG(TAG,"section length [%d]\n",section_Length);
		}else {
			section_Length = length;
		}
		if(length < section_Length) {
			L_ERROR(TAG, "TOO SHORT DATA : Expected[%u], Received[%u]\n", section_Length, length);
			SimpleBaseUnlock();
			return;
		}
		if (simplecheckPidFilter(sectionFilterHandle, pid, filterData, length, pReceiver)) {
			msg.pBuff = new uint8_t[section_Length];
			if (msg.pBuff) {
				memcpy(msg.pBuff, filterData, section_Length);
				msg.length = section_Length;
				simpleQ.enqueue(msg);
			}
		}
	}
	SimpleBaseUnlock();

}

void* tSimpleReceiver(void* param)
{
	int count=0;
	while(1)
	{
#if 0
		count++;
		if(count%500 == 0)
		{
		}
#endif	
		while(DeleteSFReqQ.size())
		{
			int Qresult=0;
			FilterDelMsg delmsg = DeleteSFReqQ.dequeue(Qresult);
			if(Qresult)
			{
				SimpleBaseLock();
				if(delmsg.cmd == 1)
				{
					SimpleFilter* filter = (SimpleFilter*)delmsg.filter;
					siMiliSleep(3);
					delete filter;
				}
				else if(delmsg.cmd == 2)
				{
					SimpleBaseFilter* pRealFilter = (SimpleBaseFilter*)delmsg.basefilter;
					siMiliSleep(3);
					L_DEBUG(TAG,"pRealFilter : [%p]\n",pRealFilter);
					L_DEBUG(TAG,"pRealFilter : tid:[0x%X]\n",pRealFilter->getTableID());
					delete pRealFilter;
				}
				else
                {
                    siMiliSleep(10);
                }
				SimpleBaseUnlock();
			}
			SimpleBaseUnlock();
		}

		if(workSimpleBaseFilterList.size()==0 && SimpleFilterThreadDestroy==false)
		{
			SimpleFilterThreadDestroy=true;
		}
		else
		{
			int simpleQResult=0;
			SectionMsg msg = simpleQ.dequeue(simpleQResult);
			if(simpleQResult==0)
			{
				siMiliSleep(10);			
			}
			else
			{
				SimpleBaseFilter* pRealFilter = (SimpleBaseFilter*)msg.pReceiver;
				if(isSimpleFilterAlive(pRealFilter) && pRealFilter->getPid() == msg.pid)
				{
					//pthread_mutex_lock(&pRealFilter->mSectionMutex);
					if(pRealFilter->UseModified == 0)
					{
						SimpleBaseLock();
						SimpleFilter* pFilter=pRealFilter->getMatchedSimpleFilter(TableIDExt(msg.pBuff));
						SimpleBaseUnlock();
						if(pFilter)
						{
							if(pFilter->m_pSection == NULL) {
								pFilter->m_pSection = new Section(msg.pBuff, msg.length);
								if(pFilter->m_pSection == NULL)
								{
									delete[] msg.pBuff;
									break;
								}
								else
								{  
									pFilter->stopFilter();
									//callback
									pFilter->processSection();
								}
							}
							else
							{
								delete[] msg.pBuff;
							}
			
							
							msg.pBuff=NULL;
						}
						if(msg.pBuff!=NULL)
						{
							delete[] msg.pBuff;
						}
					}
					else 
					{
						L_DEBUG(TAG,"[ERROR] : it can't appear this prograrm !!!!!!!!!!!!!!!!!!!!!  \n");
						int row,col;
						SimpleBaseLock();
						SimpleFilter* pFilter=pRealFilter->getMatchedSimpleFilter1(EtmID(msg.pBuff),row,col);
						SimpleBaseUnlock();
						if(pFilter)
						{
							if(pFilter->m_pSection == NULL) {
								pFilter->m_pSection = new Section(msg.pBuff, msg.length);
							}
							else
							{
								delete[] msg.pBuff;
							}
							msg.pBuff=NULL;

							pFilter->stopFilter();
							//callback
							pFilter->processSection();
						}
						else
						{
							delete[] msg.pBuff;
						}
					}
					//pthread_mutex_unlock(&pRealFilter->mSectionMutex);
				}
				siMiliSleep(5);
			}
		}


#if 0		
		if(SimpleFilterThreadDestroy)
		{
			SimpleBaseLock();
#ifdef __DEBUG_LOG_DM__
			L_INFO(TAG, "Destory SimpleFilter Thread\n");
#endif
			int Qrsult=1;
			while(Qrsult)
			{
				SectionMsg msg = simpleQ.dequeue(Qrsult);
				if(Qrsult)
					delete[] msg.pBuff;
			}
			SimpleBaseFilterThreadcreated = false;
			SimpleBaseUnlock();
			pthread_exit(NULL);
			return NULL;
		}
#else
		if(SimpleFilterThreadDestroy)
		{
			int Qrsult=1;
			while(Qrsult)
			{
				SectionMsg msg = simpleQ.dequeue(Qrsult);
				if(Qrsult)
					delete[] msg.pBuff;
			}
			siMiliSleep(30);
			SimpleFilterThreadDestroy=false;
		}
#endif		
	}
}

void CreatSectionTask(void *(taskfn)(void*), uint32_t priority)
{
	if(SimpleBaseFilterThreadcreated) return ;
	SimpleBaseFilterThreadcreated = true;

	int Qrsult=1;
	while(Qrsult)
	{
		SectionMsg msg = simpleQ.dequeue(Qrsult);
		if(Qrsult)
			delete[] msg.pBuff;
	}
	pthread_t task;
	SimpleFilterThreadDestroy=false;
#if 0	
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create (&task, &attr, taskfn, NULL);
	pthread_attr_destroy(&attr);
#else
	pthread_create (&task, NULL, taskfn, NULL);
#endif	
	return ;
}

static int simple_filter_count = 0;

SimpleBaseFilter::SimpleBaseFilter(){
	SimpleBaseLock();
	//pthread_mutex_init(&mSectionMutex, NULL);
	simple_filter_count++;
	mHdl = this;
	mHdlTdi = 0x7FFFFFFFFFFFFFFF;
	mPid = 0x1FFF;
	mTableID = 0xFF;
	bWorking=false;
	workSimpleBaseFilterList.push_back(this);
	CreatSectionTask(tSimpleReceiver, 0);
	SimpleBaseUnlock();
	L_DEBUG(TAG,"count [%d], [%p]\n",simple_filter_count,this);
}

SimpleBaseFilter::~SimpleBaseFilter(){

	//workSimpleBaseFilterList.erase(this);
	simple_filter_count--;
	if(mHdlTdi != 0x7FFFFFFFFFFFFFFF)	{
//		MMF_Filter_Close(mHdlTdi);
		MMF_Filter_Stop(mHdlTdi);
		mHdlTdi = 0x7FFFFFFFFFFFFFFF;
	}

	if(UseModified)
	{
		mModListFilter.lock();
		for(int idx=0; idx<16; idx++)
		{
			int ret;
			SimpleModifiedParam* param = mModListFilter.AtNoLock(idx,ret);
			if(ret && param)
			{
				param->mListFilter.clear();
				delete param;
			}
		}
		mModListFilter.unlock();
	}
	L_DEBUG(TAG,"count [%d], [%p]\n",simple_filter_count,this);
}

void SimpleBaseFilter::createBaseFilter(int tableId, int pid, void*& hdl, SimpleFilter* filter) {
	SimpleBaseLock();
	if(filter)
	{
		mListFilter.push_back(filter);

		mPid = pid;
		mTableID = tableId;
		int demux_hdl = 0;
		UseModified=0;
		if(mHdlTdi == 0x7FFFFFFFFFFFFFFF) {
//			MMF_Filter_Open( &mHdlTdi);
		}
		hdl = mHdl;
	}
	SimpleBaseUnlock();
}
void SimpleBaseFilter::createBaseFilter1(int tableId, int pid, void*& hdl, int magic, SimpleFilter* filter) {
	SimpleBaseLock();
	if(filter)
	{
		mPid = pid;
		mTableID = tableId;
		int demux_hdl = 0;

		if(mHdlTdi == 0x7FFFFFFFFFFFFFFF) {
//			MMF_Filter_Open( &mHdlTdi);
			for(int i=0;i<16;i++)
			{
				SimpleModifiedParam* param = new SimpleModifiedParam;
				if(param)
				{
					mModListFilter.push_back(param);
				}
			} 
			UseModified=1;
		}
		int res;
		SimpleModifiedParam* param = mModListFilter.At(((magic>>16)&0xF),res);
		if(res && param)
			param->mListFilter.push_back(filter);
	}

	SimpleBaseUnlock();
	hdl = mHdl;
}

int SimpleBaseFilter::startBaseFilter(string uri) {
	int ret = 1;
	if(bWorking==false)
	{
		bool err =  false;
		uint8_t mask[12] = { 0, };
		uint8_t mode[12] = { 0, };
		uint8_t comp[12] = { 0, };

		comp[0] = (uint8_t) mTableID;
		mask[0] = 0xff;

		if(mHdlTdi == 0x7FFFFFFFFFFFFFFF)
		{
//			err = TDI_Filter_Start(mHdlTdi, mPid, &mask, Simple_section_cb, (void*) this);
			err = MMF_Filter_Start(&mHdlTdi,(uint8_t*)uri.c_str(), mPid, mask, mode, comp, Simple_section_cb, (void*)this, SECTION_MODE_ONE_SHOT);
//			L_DEBUG(TAG,"handle [%lld], URI [%s]\n",mHdlTdi,uri.c_str());
		}
		if(err)
		{
			ret = 1;
			bWorking = true;
		}
		else
		{
			ret = 0;
		}
	}
	return ret;
}

void SimpleBaseFilter::stopBaseFilter(){
	if(bWorking == true)
	{
		if(UseModified == 0)
		{
			mListFilter.lock();
			int sizeN = mListFilter.sizeNoLock();
			for(int i=0;i<sizeN;i++)
			{
				int res;
				SimpleFilter* filter = mListFilter.AtNoLock(i,res);
				if(res && filter && filter->isWorking())
				{
					mListFilter.unlock();
					return;
				}
			}
			mListFilter.unlock();
		}
		else
		{
			mModListFilter.lock();
			for(int i=0;i<16;i++)
			{
				int res;
				SimpleModifiedParam* param = mModListFilter.AtNoLock(i,res);
				if(res && param)
				{
					param->mListFilter.lock();
					int sizeN = param->mListFilter.sizeNoLock();
					for(int idx=0; idx<sizeN;idx++)
					{
						SimpleFilter* filter = param->mListFilter.AtNoLock(idx,res);
						if(res && filter && filter->isWorking())
						{
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
		if(mHdlTdi != 0x7FFFFFFFFFFFFFFF)
		{
//			TDI_Filter_Stop(mHdlTdi);
			MMF_Filter_Stop(mHdlTdi);
//			mHdlTdi = 0x7FFFFFFFFFFFFFFF;
		}
		bWorking=false;
	}

}

void SimpleBaseFilter::closeBaseFilter()
{
	if(mHdlTdi != 0x7FFFFFFFFFFFFFFF)	{
//		MMF_Filter_Close(mHdlTdi);
//		if(bWorking)
		MMF_Filter_Stop(mHdlTdi);
		mHdlTdi = 0x7FFFFFFFFFFFFFFF;
	}
}

bool SimpleBaseFilter::deleteSimpleFilter(SimpleFilter* filter)
{
	bool bfind=false;
	if(filter)
	{
		int magic;

		if(UseModified == 1)
		{
			magic = ((filter->getETMID()>>16)&0xF);
		}
		else
		{
			return false;
		}
		int res;
		SimpleModifiedParam *param = mModListFilter.At(magic,res);
		if(res && param)
			bfind=param->mListFilter.erase(filter);
	}
	return bfind;
}

int SimpleBaseFilter::getTableID()
{
	return mTableID;
}

int SimpleBaseFilter::getPid()
{
	return mPid;
}

void* SimpleBaseFilter::getHandle()
{
	return mHdl;
}

SimpleFilter* SimpleBaseFilter::getMatchedSimpleFilter(uint16_t key)
{
	int sizeN = mListFilter.size();
	for(int idx=0; idx<sizeN ; idx++)
	{
		int res;
		mListFilter.lock();
		SimpleFilter* pFilter=mListFilter.AtNoLock(idx,res);
		mListFilter.unlock();

		if(res && pFilter  && 	(pFilter->getTableIDExt() == 0xFFFE || key == pFilter->getTableIDExt()) )
		{
			if(pFilter->isWorking())
			{
				return pFilter;
			}
			else
			{
				return NULL;
			}
		}
	}
	return NULL;
}

SimpleFilter* SimpleBaseFilter::getMatchedSimpleFilter1(uint32_t key,int& row,int& col)
{
	int magic = ((key>>16)&0xF);
	int res;
	SimpleModifiedParam *param = mModListFilter.At(magic,res);
	if(res && param)
	{
		row = magic;
		int sizeN = param->mListFilter.size();
		for(col = 0 ; col<sizeN; col++)
		{
			SimpleFilter* pFilter = param->mListFilter.At(col,res);
			if(res&& pFilter && key == pFilter->getETMID())
			{
				if(pFilter->isWorking())
				{
					return pFilter;
				}
				else
				{
					return NULL;
				}
			}
		}
	}
	return NULL;
}

void SimpleBaseFilter::deleteMatchedSimpleFilter(int row, int col, SimpleFilter* filter)
{
	int res;
	if(filter)
	{
		SimpleModifiedParam *param = mModListFilter.At(row,res);
		if(res && param)
		{
			param->mListFilter.erase(filter);
		}
	}
}

void deleteSimpleBaseFilter(SimpleBaseFilter* basefilter, SimpleFilter* filter)
{
	SimpleBaseLock();
	if(isSimpleFilterAlive(basefilter))
	{
		bool findfilter=false;
		if(filter)
		{
			FilterDelMsg msg;
			if(basefilter->UseModified == 0)
			{
				findfilter=basefilter->mListFilter.erase(filter);
				if(findfilter)
				{
					if(basefilter->mListFilter.size() == 0)
					{
						basefilter->closeBaseFilter();
						workSimpleBaseFilterList.erase(basefilter);
						unregisterSimpleBaseFilter(basefilter);
						if(DeleteSFReqQ.size()==0)
		                {
		                    msg.cmd = 100;
		                    DeleteSFReqQ.enqueue(msg);
		                    siMiliSleep(3);
		                }
						msg.cmd = 2;
						msg.basefilter =(void*)basefilter;
						msg.filter = (void*)filter;
						DeleteSFReqQ.enqueue(msg);
					}
				}
			}
			else
			{
				L_DEBUG(TAG,"[ERROR] : it can't appear this prograrm !!!!!!!!!!!!!!!!!!!!!  \n");
				findfilter=basefilter->deleteSimpleFilter(filter);
				if(findfilter)
				{
					bool remainSomething=false;
					basefilter->mModListFilter.lock();
					for(int i=0;i<16;i++)
					{
						int res;
						SimpleModifiedParam* param = basefilter->mModListFilter.AtNoLock(i,res);
						if(res&& param && param->mListFilter.size() != 0)
						{
							remainSomething=true;
							break;
						}				
					}
					basefilter->mModListFilter.unlock();
					if(remainSomething==false)
					{
						basefilter->closeBaseFilter();
						workSimpleBaseFilterList.erase(basefilter);
						unregisterSimpleBaseFilter(basefilter);
						if(DeleteSFReqQ.size()==0)
                        {
                            msg.cmd = 100;
                            DeleteSFReqQ.enqueue(msg);
                            siMiliSleep(3);
                        }
						msg.cmd = 2;
						msg.basefilter =(void*)basefilter;
						msg.filter = (void*)filter;
						DeleteSFReqQ.enqueue(msg);
					}
				}
			}
		}
	}	
	SimpleBaseUnlock();
}

void deleteSimpleFilterRequest(SimpleFilter* filter)
{
	if(filter)
	{
		FilterDelMsg msg;
		if(DeleteSFReqQ.size()==0)
        {
            msg.cmd = 100;
            DeleteSFReqQ.enqueue(msg);
            siMiliSleep(3);
        }
		msg.cmd = 1;
		msg.filter = (void*)filter;
		DeleteSFReqQ.enqueue(msg);
	}
}

void clearSimpleBaseFilterQ()
{
	int secQResult=0;
	simpleQ.lock();
	while(simpleQ.sizeNoLock())
	{
		SectionMsg msg = simpleQ.dequeueNoLock(secQResult);
		if(secQResult)
		{
			delete[] msg.pBuff;
		}
	}
	simpleQ.unlock();
}
