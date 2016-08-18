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
#ifndef _TABLE_BASE_FILTER_H
#define _TABLE_BASE_FILTER_H

#include "TableFilter.h"
#include "SIVector.h"
#include <list>
#include "TVSMMFClient.h"

typedef struct{
	SIVector<TableFilter*> mListFilter;
}ModifiedParam;


/**
 * @class RealSectionFilter
 * @brief table filter에 따라서 TDI단의 Section Filter를 할당/동작 시킴
 */
class TableBaseFilter
{

public:	   
	int UseModified;
	pthread_mutex_t mSectionMutex;
	/**
	 * @brief 생성자
	 */
	TableBaseFilter();
	/**
	 * @brief 소멸자
	 */
	virtual ~TableBaseFilter();

	/**
	 * @brief Table filter 생성
	 * @param tableId
	 * @param pid
	 * @param hdl
	 * @param filter
	 */
	void createBaseFilter(int tableId, int mask, int pid, void*& hdl, TableFilter* filter);
	void createBaseFilter1(int tableId, int mask, int pid, void*& hdl, int magic, TableFilter* filter);
	void createBaseFilter2(int tableId, int mask, int pid, void*& hdl, int magic, TableFilter* filter);

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
	bool deleteTableFilter(TableFilter* filter);

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
	
	SIVector<TableFilter*> mListFilter;
	SIVector<ModifiedParam*> mModListFilter;
	TableFilter* getMatchedTableFilter(uint16_t key);
	TableFilter* getMatchedTableFilter1(uint8_t tid, uint16_t key,int& row,int& col);
	void deleteMatchedTableFilter(int row, int col,TableFilter* filter);
	uint8_t mTidmask;
	list<uint16_t> mCandidateList[16];
	bool checkCandidate(uint16_t extid);
	void eraseCandidate(uint16_t extid);
	
private:
	bool bWorking;
	int mPid;
	int mTableID;
	void* mHdl;
	MMF_SectionFilterHandle mHdlTdi;
	int mDemuxType;
	
	
};

void deleteTableFilterRequest(TableFilter* filter);
void deleteTableBaseFilter(TableBaseFilter* basefilter, TableFilter* filter);
void clearTableBaseFilterQ();
#endif // _REAL_SECTION_FILTER_H
