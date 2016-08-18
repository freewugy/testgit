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

#ifndef _SIMPLE_BASE_FILTER_H
#define _SIMPLE_BASE_FILTER_H


#include "SimpleFilter.h"
#include "SIVector.h"
#include "TVSMMFClient.h"


typedef struct{
	SIVector<SimpleFilter*> mListFilter;
}SimpleModifiedParam;


/**
 * @class RealSectionFilter
 * @brief table filter에 따라서 TDI단의 Section Filter를 할당/동작 시킴
 */
class SimpleBaseFilter
{

public:	   
	int UseModified;
	pthread_mutex_t mSectionMutex;
	/**
	 * @brief 생성자
	 */
	SimpleBaseFilter();
	/**
	 * @brief 소멸자
	 */
	virtual ~SimpleBaseFilter();

	/**
	 * @brief Table filter 생성
	 * @param tableId
	 * @param pid
	 * @param hdl
	 * @param filter
	 */
	void createBaseFilter(int tableId, int pid, void*& hdl, SimpleFilter* filter);
	void createBaseFilter1(int tableId, int pid, void*& hdl, int magic, SimpleFilter* filter);

	/**
	 * @brief Filter Start
	 */
	int startBaseFilter(string uri);

	/**
	 * @brief Filter stop
	 */
	void stopBaseFilter();
	void closeBaseFilter();

	/**
	 * @brief Filter 자원 해제
	 * @param filter
	 */
	bool deleteSimpleFilter(SimpleFilter* filter);
	void deleteRequest(SimpleFilter* filter);

	/**
	 * @brief filter의 설정된 Table ID반환
	 * @return int
	 */
	int getTableID();
	/**
	 * @brief filter에 설정된 pid 반환
	 * @return int
	 */
	int getPid();
	/**
	 * @brief Filter Handle 반환
	 * @return THANDLE
	 */
	void* getHandle();

	
	SIVector<SimpleFilter*> mListFilter;
	SIVector<SimpleModifiedParam*> mModListFilter;
	
	SimpleFilter* getMatchedSimpleFilter(uint16_t key);
	SimpleFilter* getMatchedSimpleFilter1(uint32_t key,int& row,int& col);
	void deleteMatchedSimpleFilter(int row, int col, SimpleFilter* filter);

private:
	bool bWorking;
	int mPid;
	int mTableID;
	void* mHdl;
	MMF_SectionFilterHandle mHdlTdi;
	int mDemuxType;
	
};

void deleteSimpleFilterRequest(SimpleFilter* filter);
void deleteSimpleBaseFilter(SimpleBaseFilter* basefilter, SimpleFilter* filter);
void clearSimpleBaseFilterQ();
#endif // _SIMPLE_BASE_FILTER_H
