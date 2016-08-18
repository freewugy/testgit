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

#include "TableFilter.h"
#include "TableBaseFilter.h"
#include "TDI.h"
#include "SIVector.h"

static const char* TAG = "TableFilter";

SIVector<TableBaseFilter*> BaseFilterList;
static int table_base_count =0;
static int table_filtercount = 0;

TableBaseFilter* getRealFilter(int tableId, int pid, int tidmask)
{
    BaseFilterList.lock();
    int sizeN = BaseFilterList.sizeNoLock();
    int res = 0;
    for (int itr = 0; itr < sizeN; itr++) {
        TableBaseFilter* pFilter = BaseFilterList.AtNoLock(itr, res);
        if(res && pFilter && pFilter->getPid() == pid && (pFilter->getTableID() & pFilter->mTidmask) == (tableId & tidmask)) {
            BaseFilterList.unlock();
            return pFilter;
        }
    }
    BaseFilterList.unlock();
    return NULL;
}

TableBaseFilter* getRealFilter(void* hdl)
{
    if(hdl) {
        BaseFilterList.lock();
        int sizeN = BaseFilterList.sizeNoLock();
        int res = 0;
        for (int itr = 0; itr < sizeN; itr++) {
            TableBaseFilter* pFilter = BaseFilterList.AtNoLock(itr, res);
            if(res && pFilter && pFilter->getHandle() == hdl) {
                BaseFilterList.unlock();
                return pFilter;
            }
        }
        BaseFilterList.unlock();
    }
    return NULL;
}

TableFilter::~TableFilter()
{

    if(m_pTable != NULL) {
        delete m_pTable;
        m_pTable = NULL;
    }
    table_filtercount--;
//	L_DEBUG(TAG,"[%p] table_filtercount[%d]\n", this,table_filtercount);
}

void TableFilter::createTableFilter(int tableId, int pid)
{
    m_pid = pid;
    m_tableID = tableId;
    mcbfunc = NULL;
    table_filtercount++;
//	L_DEBUG(TAG,"[%p] table_filtercount[%d]\n", this,table_filtercount);
}

int TableFilter::startTable(string uri)
{
    int ret = 0;
    TableBaseFilter* pRealFilter = attachFilter();
    if(pRealFilter == NULL) {
//    	L_DEBUG(TAG,"\n");
        return 0;
    }
    bWorking = true;
    ret = pRealFilter->startBaseFilter(uri);
    return ret;
}

int TableFilter::startTable1(string uri)
{
    int ret = 0;

    TableBaseFilter* pRealFilter = attachFilter1();
    if(pRealFilter == NULL) {
        return 0;
    }
    bWorking = true;
    ret = pRealFilter->startBaseFilter(uri);

    return ret;
}

void TableFilter::stopTable()
{
    bWorking = false;

    TableBaseFilter* pRealFilter = getRealFilter(m_hdl);
    if(pRealFilter == NULL) {
        return;
    }

    pRealFilter->stopBaseFilter();
}

void TableFilter::setTableIDExt(uint16_t id)
{
    m_tableIDExt = id;
}

uint16_t TableFilter::getTableIDExt()
{
    return m_tableIDExt;
}

bool TableFilter::isWorking()
{
    return bWorking;
}

void TableFilter::setModTSID(int tsid)
{
    m_tsid = tsid;
}
int TableFilter::getModTSID()
{
    return m_tsid;
}

void TableFilter::processTable()
{
    if(mcbfunc) {
        mcbfunc(m_pid, m_tableID, m_tableIDExt, this);
        mcbfunc = NULL;
    }
}

void TableFilter::setCallbackFunction(TableFilterCallback func)
{
    mcbfunc = func;
}
void TableFilter::setTableIDMask(uint8_t mask)
{
    m_tidmask = mask;
}

TableBaseFilter* TableFilter::attachFilter()
{
    if(m_hdl == NULL) {
        TableBaseFilter* pRealFilter = getRealFilter(m_tableID, m_pid, m_tidmask);
        if(pRealFilter == NULL) {
            pRealFilter = new TableBaseFilter;
            if(pRealFilter) {
                BaseFilterList.push_back(pRealFilter);
                table_base_count++;
//				L_DEBUG(TAG,"[%p], table_base_count[%d]\n",pRealFilter,table_base_count);
            } else {
                return NULL;
            }
        }
        pRealFilter->createBaseFilter(m_tableID, m_tidmask, m_pid, m_hdl, this);
        return pRealFilter;
    } else {
        TableBaseFilter* pRealFilter = getRealFilter(m_hdl);
        return pRealFilter;
    }
}
TableBaseFilter* TableFilter::attachFilter1()
{
    if(m_hdl == NULL) {
        TableBaseFilter* pRealFilter = getRealFilter(m_tableID, m_pid, m_tidmask);
        if(pRealFilter == NULL) {
            pRealFilter = new TableBaseFilter;
            if(pRealFilter) {
                BaseFilterList.push_back(pRealFilter);
            } else {
                return NULL;
            }
        }

        pRealFilter->createBaseFilter1(m_tableID, m_tidmask, m_pid, m_hdl, m_tableIDExt, this);
        return pRealFilter;
    } else {
        TableBaseFilter* pRealFilter = getRealFilter(m_hdl);
        return pRealFilter;
    }
}

void TableFilter::detachFilter()
{
    if(m_hdl != NULL) {
#if 1
        BaseFilterList.lock();
        int sizeN = BaseFilterList.sizeNoLock();
        int res;
        for(int itr=0; itr<sizeN; itr++)
        {
            TableBaseFilter* pFilter = BaseFilterList.AtNoLock(itr,res);
            if(res && pFilter && pFilter->getHandle() == m_hdl)
            {
                deleteTableBaseFilter(pFilter,this);
                break;
            }
        }
        BaseFilterList.unlock();
#else
        BaseFilterList.lock();
        TableBaseFilter* pFilter = (TableBaseFilter*) m_hdl;
        deleteTableBaseFilter(pFilter, this);
        BaseFilterList.unlock();
#endif		
        m_hdl = NULL;
    }
}

void TableFilter::releaseBuffer()
{
    if(m_pTable)
    {
        m_pTable->releaseBuffer();
        delete m_pTable;
        m_pTable=NULL;
    }
}

void unregisterTableBaseFilter(TableBaseFilter* pRealFilter)
{
    if(pRealFilter) {
        BaseFilterList.eraseNoLock(pRealFilter);
        table_base_count--;
//		L_DEBUG(TAG,"[%p], table_base_count[%d]\n",pRealFilter,table_base_count);
    }
}

void deleteTableFilter(TableFilter* filter)
{
//	L_DEBUG(TAG,"\n");
    if(filter) {
        filter->detachFilter();
        deleteTableFilterRequest(filter);
    }
}

void clearTableBaseFilter()
{
    clearTableBaseFilterQ();
}
