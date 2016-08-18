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

#include "DVB_Time_Slice_FEC_Identifier_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DVB_Time_Slice_FEC_Identifier_Descriptor::DVB_Time_Slice_FEC_Identifier_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    ASSERT_MIN_DLEN(3);

    m_time_slicing = (buffer[2] >> 7) & 0x01;
    m_mpe_fec = (buffer[2] >> 5) & 0x03;
    m_frame_size = buffer[2] & 0x03;
    m_max_burst_duration = buffer[3];
    m_max_average_rate = (buffer[4] >> 4) & 0x0f;
    m_time_slice_fec_id = buffer[4] & 0x0f;

    m_id_selector_bytes.resize(mLength - 3);
    memcpy(&m_id_selector_bytes[0], buffer+5, mLength - 3);
}

DVB_Time_Slice_FEC_Identifier_Descriptor::~DVB_Time_Slice_FEC_Identifier_Descriptor()
{
}

uint8_t DVB_Time_Slice_FEC_Identifier_Descriptor::getTimeSlicing() const
{
    return m_time_slicing;
}

uint8_t DVB_Time_Slice_FEC_Identifier_Descriptor::getMpeFec() const
{
    return m_mpe_fec;
}

uint8_t DVB_Time_Slice_FEC_Identifier_Descriptor::getFrameSize() const
{
    return m_frame_size;
}

uint8_t DVB_Time_Slice_FEC_Identifier_Descriptor::getMaxBurstDuration() const
{
    return m_max_burst_duration;
}

uint8_t DVB_Time_Slice_FEC_Identifier_Descriptor::getMaxAverageRate() const
{
    return m_max_average_rate;
}

uint8_t DVB_Time_Slice_FEC_Identifier_Descriptor::getTimeSliceFecId() const
{
    return m_time_slice_fec_id;
}

const TimeSliceFecIdentifierByteVector* DVB_Time_Slice_FEC_Identifier_Descriptor::getIdSelectorBytes() const
{
    return &m_id_selector_bytes;
}

////////////////////////////////////////////////////////////////////////////////

