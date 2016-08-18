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

#include "DVB_AC3_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DVB_AC3_Descriptor::DVB_AC3_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    // EN300468 says that descriptor_length must be >= 1,
    // but it's easy to set sane defaults in this case
    // and some broadcasters already got it wrong.
    if (mLength == 0) {
        m_ac3_type_flag = 0;
        m_bsid_flag = 0;
        m_mainid_flag = 0;
        m_asvc_flag = 0;
        return;
    }

    m_ac3_type_flag = (buffer[2] >> 7) & 0x01;
    m_bsid_flag = (buffer[2] >> 6) & 0x01;
    m_mainid_flag = (buffer[2] >> 5) & 0x01;
    m_asvc_flag = (buffer[2] >> 4) & 0x01;

    size_t headerLength = 1 + m_ac3_type_flag + m_bsid_flag + m_mainid_flag + m_asvc_flag;
    ASSERT_MIN_DLEN(headerLength);

    size_t i = 3;
    if (m_ac3_type_flag == 1)
        m_ac3_type = buffer[i++];

    if (m_bsid_flag == 1)
        m_bsid = buffer[i++];

    if (m_mainid_flag == 1)
        m_mainid = buffer[i++];

    if (m_asvc_flag == 1)
        m_asvc = buffer[i++];

    m_additional_info.resize(mLength - headerLength);
    memcpy(&m_additional_info[0], &buffer[i], mLength - headerLength);
}

uint8_t DVB_AC3_Descriptor::getAc3TypeFlag(void) const
{
    return m_ac3_type_flag;
}

uint8_t DVB_AC3_Descriptor::getBsidFlag(void) const
{
    return m_bsid_flag;
}

uint8_t DVB_AC3_Descriptor::getMainidFlag(void) const
{
    return m_mainid_flag;
}

uint8_t DVB_AC3_Descriptor::getAsvcFlag(void) const
{
    return m_asvc_flag;
}

uint8_t DVB_AC3_Descriptor::getAc3Type(void) const
{
    return m_ac3_type;
}

uint8_t DVB_AC3_Descriptor::getBsid(void) const
{
    return m_bsid;
}

uint8_t DVB_AC3_Descriptor::getMainid(void) const
{
    return m_mainid;
}

uint8_t DVB_AC3_Descriptor::getAsvc(void) const
{
    return m_asvc;
}

const AdditionalInfoVector *DVB_AC3_Descriptor::getAdditionalInfo(void) const
{
    return &m_additional_info;
}



////////////////////////////////////////////////////////////////////////////////

