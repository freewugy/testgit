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

#if !defined(_SIMPLE_FILTER_H)
#define _SIMPLE_FILTER_H


#include "Section.h"
#include "SIVector.h"
#include <string>

using namespace std;

typedef void (*SimpleFilterCallback)(int pid, int tid, int tidex, void* pReceiver);

class SimpleBaseFilter;

/**
 * @class TableSectionFilter
 * @brief pid �?tableid ?�에 ?�당?�는 SI data�??�신?�여 ?�달
 */
class SimpleFilter
{

public:
	/**
	 * @brief
	 */
	Section *m_pSection;
	/**
	 * @brief ?�성??
	 * @param pid
	 * @param tableId
	 */
	SimpleFilter(int pid, int tableId, int ext_id = 0xFFFE) {
		m_hdl = NULL;
		m_pSection = NULL;
		m_tableIDExt = ext_id;
		m_ETMId = 0xFFFE;
		bWorking=false;
		
		createSimpleFilter(tableId, pid);
	}

	///?�멸??
	virtual ~SimpleFilter();
	
	/**
	 * @brief tableid, pid ?�정
	 * @param tableId
	 * @param pid
	 */
	void createSimpleFilter(int tableId, int pid);

	/**
	 * @brief Filtering start
	 */
	int startFilter(std::string uri);
	int startFilter1(std::string uri);

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
	
	void setETMID(uint32_t id);
	uint32_t getETMID();

	/**
	 * @brief Filter ?�작 ?��? 반환
	 */
	bool isWorking();
	
	bool isAttached();
	void clearBuffer();

	/**
	 * @brief Table ?�성?�의 처리
	 */
	void processSection();
	
	/**
	 * @brief
	 */
	int m_tableID;
	
	
	/**
	 * @brief
	 */
	int m_pid;
	
	void setCallbackFunction(SimpleFilterCallback func);
	
	SimpleBaseFilter* attachFilter();
	SimpleBaseFilter* attachFilter1();
	void detachFilter();
private:


	/**
	 * @brief
	 */
	int m_tableIDExt;
	/**
	 * @brief
	 */
	void*  m_hdl;

	/**
	 * @brief
	 */
	bool bWorking;
	
	uint32_t m_ETMId;
	
	SimpleFilterCallback mcbfunc;
};
void unregisterSimpleBaseFilter(SimpleBaseFilter* pRealFilter);
void clearSimpleBaseFilter();
void deleteSimpleFilter(SimpleFilter* filter);

#endif // !defined(EA_5050218B_5847_4ad0_AA57_31AABCBFFBBE__INCLUDED_)
