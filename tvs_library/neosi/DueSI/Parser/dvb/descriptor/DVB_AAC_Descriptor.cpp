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

#include "DVB_AAC_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DVB_AAC_Descriptor::DVB_AAC_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    size_t headerLength = 2;
    ASSERT_MIN_DLEN(headerLength);

    m_profile_level = buffer[2];
    m_aac_type_flag = (buffer[3] >> 7) & 0x01;

    size_t i = 4;
    if (m_aac_type_flag == 0x01) {
        headerLength++;
        ASSERT_MIN_DLEN(headerLength);

        m_aac_type = buffer[i++];
    }

    m_additional_info_bytes.resize(mLength - headerLength);
    memcpy(&m_additional_info_bytes[0], &buffer[i], mLength - headerLength);
}

DVB_AAC_Descriptor::~DVB_AAC_Descriptor()
{
}

uint8_t DVB_AAC_Descriptor::getProfileLevel() const
{
    return m_profile_level;
}

uint8_t DVB_AAC_Descriptor::getAACTypeFlag() const
{
    return m_aac_type_flag;
}

uint8_t DVB_AAC_Descriptor::getAACType() const
{
    return m_aac_type;
}

const AdditionalInfoByteVector *DVB_AAC_Descriptor::getAdditionalInfoBytes() const
{
    return &m_additional_info_bytes;
}

////////////////////////////////////////////////////////////////////////////////

