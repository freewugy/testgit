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
#include "Logger.h"

#include "MonitorFilter.h"
#include "MonitorBaseFilter.h"
#include "TDI.h"
#include "SIVector.h"

SIVector<MonitorBaseFilter*> MonitorBaseFilterList;
static int monitor_base_count =0;
static int filtercount = 0;

static const char* TAG = "MonitorFilter";

MonitorBaseFilter* getMonitorBaseFilter(int tableId, int pid, int tidmask)
{
    MonitorBaseFilterList.lock();
    int sizeN = MonitorBaseFilterList.sizeNoLock();
    int res;
    for (int mbaseitr = 0; mbaseitr < sizeN; mbaseitr++) {
        MonitorBaseFilter* pFilter = MonitorBaseFilterList.AtNoLock(mbaseitr, res);
        if(res && pFilter && pFilter->getPid() == pid && (pFilter->getTableID() & tidmask) == (tableId & tidmask)) {
            MonitorBaseFilterList.unlock();
//            L_DEBUG(TAG,"[%p]\n",pFilter);
            return pFilter;
        }
    }
    MonitorBaseFilterList.unlock();
    return NULL;
}

MonitorBaseFilter* getMonitorBaseFilter(void* hdl)
{
    if(hdl) {
        MonitorBaseFilterList.lock();
        int sizeN = MonitorBaseFilterList.sizeNoLock();
        int res;
        for (int mbaseitr = 0; mbaseitr < sizeN; mbaseitr++) {
            MonitorBaseFilter* pFilter = MonitorBaseFilterList.AtNoLock(mbaseitr, res);
            if(res && pFilter && pFilter->getHandle() == hdl) {
                MonitorBaseFilterList.unlock();
                return pFilter;
            }
        }
        MonitorBaseFilterList.unlock();
    }
    return NULL;
}

MonitorFilter::~MonitorFilter()
{
//	L_DEBUG(TAG,"[%p]\n",this);
	filtercount--;
//	L_DEBUG(TAG,"[%p] filtercount[%d]\n", this,filtercount);
}

void MonitorFilter::createMonitorFilter(int tableId, int pid)
{
    m_pid = pid;
    m_tableID = tableId;
    mcbfunc = NULL;
	filtercount++;
//	L_DEBUG(TAG,"[%p] filtercount[%d]\n", this,filtercount);
}

int MonitorFilter::startFilter(string uri)
{
    int ret = 0;

    MonitorBaseFilter* pRealFilter = attachFilter();
    if(pRealFilter == NULL) {
//    	L_DEBUG(TAG,"\n");
        return 0;
    }
    bWorking = true;
    ret = pRealFilter->startBaseFilter(uri);
//    L_DEBUG(TAG,"[%p]\n",pRealFilter);
    return ret;
}
int MonitorFilter::startFilter1(string uri)
{
    int ret = 0;
    MonitorBaseFilter* pRealFilter = attachFilter1();
    if(pRealFilter == NULL) {
        return 0;
    }
    bWorking = true;
    ret = pRealFilter->startBaseFilter(uri);

    return ret;
}

void MonitorFilter::stopFilter()
{
    bWorking = false;
}

void MonitorFilter::setTableIDExt(uint16_t id)
{
    m_tableIDExt = id;
}

uint16_t MonitorFilter::getTableIDExt()
{
    return m_tableIDExt;
}

bool MonitorFilter::isWorking()
{
    return bWorking;
}

void MonitorFilter::processSection(int tid)
{
    if(mcbfunc) {
        m_tableID = tid;
        mcbfunc(this);
        mcbfunc = NULL;
    }
}

void MonitorFilter::setCallbackFunction(MonitorCallback func)
{
    mcbfunc = func;
}

MonitorBaseFilter* MonitorFilter::attachFilter()
{
    if(m_hdl == NULL) {
        MonitorBaseFilter* pRealFilter = getMonitorBaseFilter(m_tableID, m_pid, m_tidmask);
        if(pRealFilter == NULL) {
            pRealFilter = new MonitorBaseFilter;
            if(pRealFilter) {
                MonitorBaseFilterList.push_back(pRealFilter);
                monitor_base_count++;
//                L_DEBUG(TAG,"[%p], monitor_base_count[%d]\n",pRealFilter,monitor_base_count);
            } else {
                return NULL;
            }
        }
        pRealFilter->createBaseFilter(m_tableID, m_tidmask, m_pid, m_hdl, this);
//        L_DEBUG(TAG,"pRealFilter[%p] m_hdl[%p]\n",pRealFilter,m_hdl);
        return pRealFilter;
    } else {
        MonitorBaseFilter* pRealFilter = getMonitorBaseFilter(m_hdl);
//        L_DEBUG(TAG,"[%p]\n",pRealFilter);
        return pRealFilter;
    }
}

MonitorBaseFilter* MonitorFilter::attachFilter1()
{
    if(m_hdl == NULL) {
        MonitorBaseFilter* pRealFilter = getMonitorBaseFilter(m_tableID, m_pid, m_tidmask);
        if(pRealFilter == NULL) {
            pRealFilter = new MonitorBaseFilter;
            if(pRealFilter) {
                MonitorBaseFilterList.push_back(pRealFilter);
				monitor_base_count++;
//				L_DEBUG(TAG,"[%p], monitor_base_count[%d]\n",pRealFilter,monitor_base_count);
            } else {
                return NULL;
            }
        }

        pRealFilter->createBaseFilter1(m_tableID, m_tidmask, m_pid, m_hdl, m_tableIDExt, this);
        return pRealFilter;
    } else {
        MonitorBaseFilter* pRealFilter = getMonitorBaseFilter(m_hdl);
        return pRealFilter;
    }
}

void MonitorFilter::detachFilter()
{
    if(m_hdl != NULL) {
        MonitorBaseFilterList.lock();
#if 1
        int sizeN = MonitorBaseFilterList.sizeNoLock();
        int res;
        for(int mbaseitr=0; mbaseitr<sizeN; mbaseitr++)
        {
            MonitorBaseFilter* pFilter = MonitorBaseFilterList.AtNoLock(mbaseitr,res);
            if(res && pFilter && pFilter->getHandle() == m_hdl)
            {
                deleteMonitorBaseFilter(pFilter,this);
                break;
            }
        }
#else
        MonitorBaseFilter* pFilter = (MonitorBaseFilter*) m_hdl;
        deleteMonitorBaseFilter(pFilter, this);
#endif			
        MonitorBaseFilterList.unlock();

        m_hdl = NULL;
    }
}

void MonitorFilter::setTableIDMask(uint8_t mask)
{
    m_tidmask = mask;
}

void deleteMonitorFilter(MonitorFilter* filter)
{
    if(filter) {
        filter->detachFilter();
        deleteMonitorFilterRequest(filter);
    }
}

void unregisterMonitorBaseFilter(MonitorBaseFilter* pRealFilter)
{
    if(pRealFilter) {
        MonitorBaseFilterList.eraseNoLock(pRealFilter);
		monitor_base_count--;
//		L_DEBUG(TAG,"[%p], monitor_base_count[%d]\n",pRealFilter,monitor_base_count);
    }
}

void clearMonitorBaseFilter()
{
    clearMonitorBaseFilterQ();
}
