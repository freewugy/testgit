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

#include "DVB_Partial_Transport_Stream_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DVB_Partial_Transport_Stream_Descriptor::DVB_Partial_Transport_Stream_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    size_t headerLength = 1;
    ASSERT_MIN_DLEN(headerLength);

    m_peak_rate = ((buffer[2]) & 0x3f) << 16 | r16(&buffer[3]);
    m_minimum_overall_smoothing_rate = ((buffer[5]) & 0x3f) << 16 | r16(&buffer[6]);
    m_maximum_overall_smoothing_rate = (buffer[8] & 0x3f) << 8 | buffer[9];
}

DVB_Partial_Transport_Stream_Descriptor::~DVB_Partial_Transport_Stream_Descriptor()
{
}

uint32_t DVB_Partial_Transport_Stream_Descriptor::getPeakRate() const
{
    return m_peak_rate;
}

uint32_t DVB_Partial_Transport_Stream_Descriptor::getMinimumOverallSmoothingRate() const
{
    return m_minimum_overall_smoothing_rate;
}

uint16_t DVB_Partial_Transport_Stream_Descriptor::getMaximumOverallSmoothingRate() const
{
    return m_maximum_overall_smoothing_rate;
}

////////////////////////////////////////////////////////////////////////////////

