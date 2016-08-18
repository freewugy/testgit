/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: eschoi $
 * $LastChangedDate: 2015-02-27 11:14:42 +0900 (금, 27 2월 2015) $
 * $LastChangedRevision: 1069 $
 * Description:
 * Note:
 *****************************************************************************/

#ifndef _SECTION_H_
#define _SECTION_H_

#include <stdint.h>

typedef class Section {
public:
    /**
     * @brief 생성자
     */
    Section(uint8_t* pBuff, uint32_t size);

    /**
     * @brief 소멸자
     */
    ~Section();

    /**
     * @brief
     */
    uint8_t* m_pSection;
    /**
     * @brief
     */
    uint32_t m_pSectionLen;

    void releaseBuffer();

} SectionT;

#endif //_SECTION_H_
