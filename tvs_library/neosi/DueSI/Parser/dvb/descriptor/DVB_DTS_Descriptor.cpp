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

#include "DVB_DTS_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DVB_DTS_Descriptor::DVB_DTS_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    ASSERT_MIN_DLEN(5);

    m_sample_rate = (buffer[2] >> 4) & 0x0f;
    m_bit_rate = ((buffer[2] & 0x0f) << 2) | (buffer[3] >> 6) & 0x02;
    m_number_of_blocks = ((buffer[3] & 0x3f) << 2) | (buffer[4] >> 7) & 0x01;
    m_frame_size = ((buffer[4] & 0x7f) << 7) | (buffer[5] >> 1);
    m_surround_mode = ((buffer[5] & 0x01) << 6) | (buffer[6] >> 3) & 0x1f;
    m_lfe_flag = (buffer[6] >> 2) & 0x01;
    m_extended_surround_flag = buffer[6] & 0x03;

    m_additional_info_bytes.resize(mLength - 5);
    memcpy(&m_additional_info_bytes[0], &buffer[7], mLength - 5);
}

DVB_DTS_Descriptor::~DVB_DTS_Descriptor()
{
}

uint8_t DVB_DTS_Descriptor::getSampleRate() const
{
    return m_sample_rate;
}

uint8_t DVB_DTS_Descriptor::getBitRate() const
{
    return m_bit_rate;
}

uint8_t DVB_DTS_Descriptor::getNumberOfBlocks() const
{
    return m_number_of_blocks;
}

uint16_t DVB_DTS_Descriptor::getFrameSize() const
{
    return m_frame_size;
}

uint8_t DVB_DTS_Descriptor::getLfeFlag() const
{
    return m_lfe_flag;
}

uint8_t DVB_DTS_Descriptor::getExtendedSurroundFlag() const
{
    return m_extended_surround_flag;
}

const AdditionalInfoByteVector *DVB_DTS_Descriptor::getAdditionalInfoBytes() const
{
    return &m_additional_info_bytes;
}

////////////////////////////////////////////////////////////////////////////////

