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

#include "DVB_Short_Smoothing_Buffer_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DVB_Short_Smoothing_Buffer_Descriptor::DVB_Short_Smoothing_Buffer_Descriptor(const uint8_t * const buffer) : Descriptor(buffer), m_private_data_bytes(mLength-1)
{
    m_sb_size = (buffer[2] >> 6) & 0x03;
    m_sb_leak_rate = buffer[2] & 0x4f;

    memcpy(&m_private_data_bytes[0], &buffer[3], mLength-1);
}

DVB_Short_Smoothing_Buffer_Descriptor::~DVB_Short_Smoothing_Buffer_Descriptor()
{
}

uint8_t DVB_Short_Smoothing_Buffer_Descriptor::getSbSize(void) const
{
    return m_sb_size;
}

uint8_t DVB_Short_Smoothing_Buffer_Descriptor::getSbLeakRate(void) const
{
    return m_sb_leak_rate;
}

const PrivateDataByteVector *DVB_Short_Smoothing_Buffer_Descriptor::getPrivateDataBytes(void) const
{
    return &m_private_data_bytes;
}

////////////////////////////////////////////////////////////////////////////////

