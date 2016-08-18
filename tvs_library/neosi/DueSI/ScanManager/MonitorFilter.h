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

#ifndef _MONITOR_FILTER_H
#define _MONITOR_FILTER_H

#include <stdint.h>


typedef void (*MonitorCallback)(void* pReceiver);

class MonitorBaseFilter;

/**
 * @class TableSectionFilter
 * @brief pid �?tableid ?�에 ?�당?�는 SI data�??�신?�여 ?�달
 */
class MonitorFilter {

public:
    /**
     * @brief ?�성??
     * @param pid
     * @param tableId
     */
    MonitorFilter(int pid, int tableId, int version, int ext_id = 0xFFFE)
    {
        m_hdl = NULL;
        m_tableIDExt = ext_id;
        bWorking = false;
        m_ver = version;
        m_tidmask = 0xff;
        createMonitorFilter(tableId, pid);
    }

    ///?�멸??
    virtual ~MonitorFilter();

    /**
     * @brief tableid, pid ?�정
     * @param tableId
     * @param pid
     */
    void createMonitorFilter(int tableId, int pid);

    /**
     * @brief Filtering start
     */
    int startFilter(string uri);
    int startFilter1(string uri);

    /**
     * @brief Filtering stop
     */
    void stopFilter();

    /**
     * @brief Tableid extension ?�정
     */
    void setTableIDExt(uint16_t id);

    /**
     * @brief Tableid extension 반환
     */
    uint16_t getTableIDExt();

    /**
     * @brief Filter ?�작 ?��? 반환
     */
    bool isWorking();

    /**
     * @brief Table ?�성?�의 처리
     */
    void processSection(int tid);

    /**
     * @brief
     */
    int m_pid;
    int m_tableID;
    int m_ver;

    void setCallbackFunction(MonitorCallback func);

    void setTableIDMask(uint8_t mask);

    MonitorBaseFilter* attachFilter();
    MonitorBaseFilter* attachFilter1();
    void detachFilter();
private:

    /**
     * @brief
     */
    int m_tableIDExt;
    /**
     * @brief
     */
    void* m_hdl;

    /**
     * @brief
     */
    bool bWorking;

    uint8_t m_tidmask;

    MonitorCallback mcbfunc;
};

void deleteMonitorFilter(MonitorFilter* filter);
void unregisterMonitorBaseFilter(MonitorBaseFilter* pRealFilter);
void clearMonitorBaseFilter();
#endif // !defined(EA_5050218B_5847_4ad0_AA57_31AABCBFFBBE__INCLUDED_)
