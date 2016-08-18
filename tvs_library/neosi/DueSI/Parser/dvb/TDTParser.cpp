/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: eschoi $
 * $LastChangedDate: 2014-06-03 13:58:35 +0900 (화, 03 6월 2014) $
 * $LastChangedRevision: 837 $
 * Description:
 * Note:
 *****************************************************************************/

#include "TDTParser.h"
#include "Section.h"
#include "ByteStream.h"

#define TDT_HEADER_LENGTH   8

static const char* TAG = "TDTParser";
TDTParser::~TDTParser() {

}

void TDTParser::process(void* sectionData)
{
    Section* pSec = (Section*)sectionData;

    uint32_t mjd_time, bcd_time;

    if(pSec->m_pSection && pSec->m_pSectionLen == TDT_HEADER_LENGTH) {
        mjd_time = r16(&pSec->m_pSection[3]);
        bcd_time = (pSec->m_pSection[5] << 16) | r16(&pSec->m_pSection[6]);

        m_CurrentSeconds = TimeConvertor::getInstance().convertTimeMJDBCD2Seconds(mjd_time, bcd_time) + mUTCOffSet * 3600;
    } else {
         L_ERROR(TAG, "section length is SHORT[%u]\n", pSec->m_pSectionLen);
    }
}

void TDTParser::printCurrentTime() {
    L_INFO(TAG, "%s\n", TimeConvertor::getInstance().convertSeconds2Date(m_CurrentSeconds).c_str());
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
