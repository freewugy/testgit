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

#include "DVB_Frequency_List_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DVB_Frequency_List_Descriptor::DVB_Frequency_List_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    ASSERT_MIN_DLEN(1);

    m_coding_type = buffer[2] & 0x03;

    for (size_t i = 0; i < mLength - 1; i += 4) {
        ASSERT_MIN_DLEN(i + 5);
        m_centre_frequencies.push_back(r32(&buffer[i + 3]));
    }
}

uint8_t DVB_Frequency_List_Descriptor::getCodingType(void) const
{
    return m_coding_type;
}

const CentreFrequencyList *DVB_Frequency_List_Descriptor::getCentreFrequencies(void) const
{
    return &m_centre_frequencies;
}

////////////////////////////////////////////////////////////////////////////////

