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

#ifndef _MONITOR_BASE_FILTER_H
#define _MONITOR_BASE_FILTER_H

#include "MonitorFilter.h"
#include "SIVector.h"
#include "../include/mmf/TVSMMFClient.h"
#include "../MMF/MMFInterface.h"
typedef struct {
    SIVector<MonitorFilter*> mListFilter;
} MonitorModifiedParam;

/**
 * @class RealSectionFilter
 * @brief table filter에 따라서 TDI단의 Section Filter를 할당/동작 시킴
 */
class MonitorBaseFilter {

public:
    int UseModified;
    pthread_mutex_t mSectionMutex;
    /**
     * @brief 생성자
     */
    MonitorBaseFilter();
    /**
     * @brief 소멸자
     */
    virtual ~MonitorBaseFilter();

    /**
     * @brief Table filter 생성
     * @param tableId
     * @param pid
     * @param hdl
     * @param filter
     */
    void createBaseFilter(int tableId, int mask, int pid, void*& hdl, MonitorFilter* filter);
    void createBaseFilter1(int tableId, int mask, int pid, void*& hdl, int magic, MonitorFilter* filter);

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
    bool deleteMonitorFilter(MonitorFilter* filter);

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
    void deleteRequest();

    SIVector<MonitorFilter*> mListFilter;
    SIVector<MonitorModifiedParam*> mModListFilter;
    MonitorFilter* getMatchedMonitorFilter(uint8_t tid, uint16_t key);
    MonitorFilter* getMatchedMonitorFilter1(uint16_t key, int& row, int& col);
    void deleteMatchedMonitorFilter(int row, int col, MonitorFilter* filter);
    uint8_t mTidmask;
private:
    bool bWorking;
    int mPid;
    int mTableID;
    void* mHdl;
    MMF_SectionFilterHandle mHdlTdi;
    int mDemuxType;
    char mem_barrier[100];
};

void deleteMonitorFilterRequest(MonitorFilter* filter);
void deleteMonitorBaseFilter(MonitorBaseFilter* basefilter, MonitorFilter* filter);
void clearMonitorBaseFilterQ();
#endif // _REAL_SECTION_FILTER_H
