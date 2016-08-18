/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: loneowl $
 * $LastChangedDate: 2014-04-10 15:55:50 +0900 (목, 10 4월 2014) $
 * $LastChangedRevision: 580 $
 * Description:
 * Note:
 *****************************************************************************/

#include "DVB_Service_Availablity_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DVB_Service_Availablity_Descriptor::DVB_Service_Availablity_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    ASSERT_MIN_DLEN(1);

    m_availability_flag = (buffer[2] >> 7) & 0x01;

    for (size_t i = 0; i < mLength - 1; i += 2) {
        ASSERT_MIN_DLEN(i + 3);
        m_cell_ids.push_back(r16(&buffer[i + 3]));
    }
}

uint8_t DVB_Service_Availablity_Descriptor::getAvailabilityFlag(void) const
{
    return m_availability_flag;
}

const CellIdList *DVB_Service_Availablity_Descriptor::getCellIds(void) const
{
    return &m_cell_ids;
}

////////////////////////////////////////////////////////////////////////////////

