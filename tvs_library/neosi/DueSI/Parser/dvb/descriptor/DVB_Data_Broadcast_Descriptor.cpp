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

#include "DVB_Data_Broadcast_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DVB_Data_Broadcast_Descriptor::DVB_Data_Broadcast_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    ASSERT_MIN_DLEN(8);

    m_data_broadcast_id = r16(&buffer[2]);
    m_component_tag = buffer[4];
    m_selector_length = buffer[5];

    ASSERT_MIN_DLEN(m_selector_length + 8);

    for (size_t i = 0; i < m_selector_length; ++i)
        m_selector_bytes.push_back(buffer[i + 6]);

    m_iso639_language_code.assign((char *)&buffer[m_selector_length + 6], 3);
    m_text_length = buffer[m_selector_length + 9];

    ASSERT_MIN_DLEN(m_text_length + m_selector_length + 8);

    m_text.assign((char *)&buffer[m_selector_length + 10], m_text_length);
}

uint16_t DVB_Data_Broadcast_Descriptor::getDataBroadcastId(void) const
{
    return m_data_broadcast_id;
}

uint8_t DVB_Data_Broadcast_Descriptor::getComponentTag(void) const
{
    return m_component_tag;
}

const selectorByteList *DVB_Data_Broadcast_Descriptor::getSelectorBytes(void) const
{
    return &m_selector_bytes;
}

const std::string &DVB_Data_Broadcast_Descriptor::getIso639LanguageCode(void) const
{
    return m_iso639_language_code;
}

const std::string &DVB_Data_Broadcast_Descriptor::getText(void) const
{
    return m_text;
}

////////////////////////////////////////////////////////////////////////////////

