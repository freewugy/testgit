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

#include "SimpleFilter.h"
#include "SimpleBaseFilter.h"
#include "TDI.h"
#include "SIVector.h"

SIVector<SimpleBaseFilter*> SimpleBaseFilterList;

SimpleBaseFilter* getSimpleBaseFilter(int tableId, int pid)
{
	SimpleBaseFilterList.lock();
	int sizeN = SimpleBaseFilterList.sizeNoLock();
	int res;
	for(int baseitr=0; baseitr<sizeN; baseitr++)
	{
		SimpleBaseFilter* pFilter = SimpleBaseFilterList.AtNoLock(baseitr,res);
		if(res && pFilter && pFilter->getPid() == pid && pFilter->getTableID() == tableId)
		{
			SimpleBaseFilterList.unlock();
			return pFilter;
		}
	}
	SimpleBaseFilterList.unlock();
	return NULL;
}

SimpleBaseFilter* getSimpleBaseFilter(void*  hdl)
{
	if(hdl)
	{
		SimpleBaseFilterList.lock();
		int sizeN = SimpleBaseFilterList.sizeNoLock();
		int res;
		for(int baseitr=0; baseitr<sizeN; baseitr++)
		{
			SimpleBaseFilter* pFilter = SimpleBaseFilterList.AtNoLock(baseitr,res);
			if(res && pFilter && pFilter->getHandle() == hdl)
			{
				SimpleBaseFilterList.unlock();
				return pFilter;
			}
		}
		SimpleBaseFilterList.unlock();
	}
	return NULL;
}

SimpleFilter::~SimpleFilter(){
	
	if(m_pSection != NULL) {
		delete m_pSection;
		m_pSection = NULL;
	}
}

void SimpleFilter::createSimpleFilter(int tableId, int pid){
	m_pid = pid;
	m_tableID = tableId;
	mcbfunc = NULL;
}


int SimpleFilter::startFilter(string uri){
	int ret=0;
	
	SimpleBaseFilter* pRealFilter = attachFilter();
	if(pRealFilter == NULL)
	{
		return 0;
	}
	bWorking = true;
	ret = pRealFilter->startBaseFilter(uri);
	
	return ret;
}
int SimpleFilter::startFilter1(string uri){
	int ret=0;
	
	SimpleBaseFilter* pRealFilter = attachFilter1();
	if(pRealFilter == NULL)
	{
		return 0;
	}
	bWorking = true;
	ret = pRealFilter->startBaseFilter(uri);
	return ret;
}

void SimpleFilter::stopFilter(){
	bWorking = false;

	SimpleBaseFilter* pRealFilter = getSimpleBaseFilter(m_hdl);
	if(pRealFilter == NULL)
	{
		return;
	}
	
	pRealFilter->stopBaseFilter();
}

void SimpleFilter::setTableIDExt(uint16_t id)
{
	m_tableIDExt = id;
}

uint16_t SimpleFilter::getTableIDExt() {
	return m_tableIDExt;
}
void SimpleFilter::setETMID(uint32_t id)
{
	m_ETMId = id;
}
uint32_t SimpleFilter::getETMID()
{
	return m_ETMId;
}

bool SimpleFilter::isWorking()
{
	return bWorking;
}

void SimpleFilter::processSection()
{
	//printf("%s::%s : hdl:%d, PID:0x%X, TID:0x%X, ExtTID:0x%X table complete \n ",__FILE__, __FUNCTION__,m_hdl, m_pid, m_tableID, m_tableIDExt);

	if(mcbfunc)
	{
		mcbfunc(m_pid, m_tableID, m_tableIDExt, this);
	}
}

void SimpleFilter::setCallbackFunction(SimpleFilterCallback func)
{
	mcbfunc = func;
}

SimpleBaseFilter* SimpleFilter::attachFilter()
{
	if(m_hdl == NULL)
	{
		SimpleBaseFilter* pRealFilter = getSimpleBaseFilter(m_tableID, m_pid);
		if(pRealFilter == NULL)
		{
			pRealFilter = new SimpleBaseFilter;
			if(pRealFilter)
			{
				SimpleBaseFilterList.push_back(pRealFilter);
			}
			else
			{
				return NULL;
			}
		}
	
		pRealFilter->createBaseFilter(m_tableID, m_pid, m_hdl, this);
		return pRealFilter;
	}
	else
	{
		SimpleBaseFilter* pRealFilter = getSimpleBaseFilter(m_hdl);
		return pRealFilter;
	}
}
SimpleBaseFilter* SimpleFilter::attachFilter1()
{
	if(m_hdl == NULL)
	{
		SimpleBaseFilter* pRealFilter = getSimpleBaseFilter(m_tableID, m_pid);
		if(pRealFilter == NULL)
		{
			pRealFilter = new SimpleBaseFilter;
			if(pRealFilter)
			{
				SimpleBaseFilterList.push_back(pRealFilter);
			}
			else
			{
				return NULL;
			}
		}
		pRealFilter->createBaseFilter1(m_tableID,m_pid, m_hdl, m_ETMId, this);
		return pRealFilter;
	}
	else
	{
		SimpleBaseFilter* pRealFilter = getSimpleBaseFilter(m_hdl);
		return pRealFilter;
	}
}
void SimpleFilter::detachFilter()
{
	if(m_hdl != NULL)	{
		SimpleBaseFilterList.lock();
#if 1
		int sizeN = SimpleBaseFilterList.sizeNoLock();
		int res;
		for(int baseitr=0; baseitr<sizeN; baseitr++)
		{
			SimpleBaseFilter* pFilter = SimpleBaseFilterList.AtNoLock(baseitr,res);
			if(res && pFilter && pFilter->getHandle() == m_hdl)
			{
				deleteSimpleBaseFilter(pFilter,this);
				break;
			}
		}
#else
		SimpleBaseFilter* pFilter = (SimpleBaseFilter*)m_hdl;
		deleteSimpleBaseFilter(pFilter,this);
#endif		
		SimpleBaseFilterList.unlock();
		m_hdl = NULL;
	}
}

bool SimpleFilter::isAttached()
{
	if(m_hdl!=NULL)
	{
		return true;
	}
	else
	{
		return false;
	}
	
}

void SimpleFilter::clearBuffer()
{
	if(m_pSection != NULL) {
		delete m_pSection;
		m_pSection = NULL;
	}
}


void unregisterSimpleBaseFilter(SimpleBaseFilter* pRealFilter)
{
	if(pRealFilter)
	{
		SimpleBaseFilterList.eraseNoLock(pRealFilter);
	}
}

void deleteSimpleFilter(SimpleFilter* filter)
{
	if(filter)
	{
		filter->detachFilter();
		deleteSimpleFilterRequest(filter);
	}
}

void clearSimpleBaseFilter()
{
	clearSimpleBaseFilterQ();
}
