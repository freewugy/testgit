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

#if !defined(_TABLE_FILTER_)
#define _TABLE_FILTER_

#include "Table.h"

typedef void (*TableFilterCallback)(int pid, int tid, int tidex, void* pReceiver);

class TableBaseFilter;

/**
 * @class TableSectionFilter
 * @brief pid �?tableid ?�에 ?�당?�는 SI data�??�신?�여 ?�달
 */
class TableFilter {

public:
    /**
     * @brief
     */
    Table *m_pTable;
    /**
     * @brief ?�성??
     * @param pid
     * @param tableId
     */
    TableFilter(int pid, int tableId, int ext_id = 0xFFFE)
    {
        m_hdl = NULL;
        m_pTable = NULL;
        m_tableIDExt = ext_id;
        bWorking = false;
        m_tidmask = 0xff;
		m_tsid = 0;

        createTableFilter(tableId, pid);
    }

    ///?�멸??
    virtual ~TableFilter();

    /**
     * @brief tableid, pid ?�정
     * @param tableId
     * @param pid
     */
    void createTableFilter(int tableId, int pid);

    /**
     * @brief Filtering start
     */
    int startTable(string uri);
    int startTable1(string uri);

    /**
     * @brief Filtering stop
     */
    void stopTable();

    int getPID()
    {
        return m_pid;
    }
    ;
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
    void processTable();

    void setModTSID(int tsid);
    int getModTSID();

    /**
     * @brief
     */
    int m_tableID;
    void setCallbackFunction(TableFilterCallback func);
    void setTableIDMask(uint8_t mask);

    TableBaseFilter* attachFilter();
    TableBaseFilter* attachFilter1();
    TableBaseFilter* attachFilter2();
    void detachFilter();
	void releaseBuffer();
private:

    /**
     * @brief
     */
    int m_pid;

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
    /**
     * @brief H/W Demux : 1, S/W Demux : 0
     */
    //int m_demuxType;
    int m_tsid;
    uint8_t m_tidmask;

    TableFilterCallback mcbfunc;
};

void deleteTableFilter(TableFilter* filter);
void unregisterTableBaseFilter(TableBaseFilter* pRealFilter);
void clearTableBaseFilter();
#endif // !defined(EA_5050218B_5847_4ad0_AA57_31AABCBFFBBE__INCLUDED_)
