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

#ifndef _SECTION_H_
#define _SECTION_H_

#include <stdint.h>

typedef class Section {
public:
	/**
	 * @brief 생성자
	 */
	Section(uint8_t* pBuff,uint32_t size);

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

}SectionT;

int IsValidSectionCheck(uint8_t *d, int len);

#endif //_SECTION_H_
