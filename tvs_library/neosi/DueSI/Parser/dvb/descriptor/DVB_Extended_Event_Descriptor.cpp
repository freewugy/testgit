/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: loneowl $
 * $LastChangedDate: 2014-04-11 11:46:21 +0900 (금, 11 4월 2014) $
 * $LastChangedRevision: 583 $
 * Description:
 * Note:
 *****************************************************************************/

#include "DVB_Extended_Event_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
ExtendedEvent::ExtendedEvent(const uint8_t * const buffer)
{
    m_item_description_length = buffer[0];
    m_item_description.assign((char *)&buffer[1], m_item_description_length);
    m_item_length = buffer[m_item_description_length + 1];
    m_item.assign((char *)&buffer[m_item_description_length + 2], m_item_length);
}

const std::string &ExtendedEvent::getItemDescription(void) const
{
    return m_item_description;
}

const std::string &ExtendedEvent::getItem(void) const
{
    return m_item;
}

DVB_Extended_Event_Descriptor::DVB_Extended_Event_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    size_t headerLength = 6;
    ASSERT_MIN_DLEN(headerLength);

    m_descriptor_number = (buffer[2] >> 4) & 0x0f;
    m_last_descriptor_number = buffer[2] & 0x0f;
    m_iso639_language_code.assign((char *)&buffer[3], 3);
    m_length_of_items = buffer[6];

    headerLength += m_length_of_items;
    ASSERT_MIN_DLEN(headerLength);

    ExtendedEvent *e;
    for (size_t i = 0; i < m_length_of_items; i += e->m_item_description_length + e->m_item_length + 2) {
        e = new ExtendedEvent(&buffer[i + 7]);
        m_items.push_back(e);
    }

    m_text_length = buffer[m_length_of_items + 7];

    headerLength += m_text_length;
    ASSERT_MIN_DLEN(headerLength);

    m_text.assign((char *)&buffer[m_length_of_items + 8], m_text_length);

}

DVB_Extended_Event_Descriptor::~DVB_Extended_Event_Descriptor(void)
{
    for (ExtendedEventIterator i = m_items.begin(); i != m_items.end(); ++i)
        delete *i;
}

uint8_t DVB_Extended_Event_Descriptor::getDescriptorNumber(void) const
{
    return m_descriptor_number;
}

uint8_t DVB_Extended_Event_Descriptor::getLastDescriptorNumber(void) const
{
    return m_last_descriptor_number;
}

const std::string &DVB_Extended_Event_Descriptor::getIso639LanguageCode(void) const
{
    return m_iso639_language_code;
}

const ExtendedEventList *DVB_Extended_Event_Descriptor::getItems(void) const
{
    return &m_items;
}

const std::string &DVB_Extended_Event_Descriptor::getText(void) const
{
    return m_text;
}

////////////////////////////////////////////////////////////////////////////////

