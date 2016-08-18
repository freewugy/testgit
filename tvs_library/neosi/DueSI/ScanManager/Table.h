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

#ifndef _TABLE_H_
#define _TABLE_H_

#include <stdint.h>

/**
 * @class Table
 * @brief 수신한 Section을 모아 Table을 완성하도록 하는 역할
 */
typedef class Table {
public:
    /**
     * @brief 생성자
     */
    Table();

    /**
     * @brief 소멸자
     */
    ~Table();

    /**
     * @brief table을 구성하는 Section의 수 (last section number +1)
     * @param count number of Sections
     */
    void setSectionCount(int count);

    /**
     * @brief section에 대한 buffer할당 및 저장
     * @param pSect
     * @param len
     * @return
     */
    void setSection(uint8_t* pSect, int len);
    /**
     * @brief Table이 완성되었은지 여부를 체크
     * @return bool
     */
    bool isComplete();
    /**
     * @brief Table이 DVB EIT인지 여부를 체크
     * @return bool
     */
    bool isDVBEIT();
    /**
     * @brief DVB EIT인 경우에 Table이 완성이 되었은지 여부를 체크
     * @return bool
     */
    bool isSegmentComplete();

    /**
     * @brief
     */
    uint8_t** m_pSection;
    /**
     * @brief
     */
    uint32_t* m_pSectionLen;
    /**
     * @brief
     */
    int m_SectionCount;
    /**
     * @brief
     */
    uint8_t m_Ver;

	void releaseBuffer();
} TableT;

#endif //_TABLE_H_
