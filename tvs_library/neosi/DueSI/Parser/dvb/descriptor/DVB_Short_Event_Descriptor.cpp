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

#include "DVB_Short_Event_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DVB_Short_Event_Descriptor::DVB_Short_Event_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    size_t headerLength = 5;
    ASSERT_MIN_DLEN(headerLength);

    m_iso639_language_code.assign((char *)&buffer[2], 3);
    m_event_name_length = buffer[5];

    headerLength += m_event_name_length;
    ASSERT_MIN_DLEN(headerLength);

    m_event_name.assign((char *)&buffer[6], m_event_name_length);
    m_text_length = buffer[6 + m_event_name_length];

    headerLength += m_text_length;
    ASSERT_MIN_DLEN(headerLength);

    m_text.assign((char *)&buffer[7 + m_event_name_length], m_text_length);
}

const std::string &DVB_Short_Event_Descriptor::getIso639LanguageCode(void) const
{
    return m_iso639_language_code;
}

const std::string &DVB_Short_Event_Descriptor::getEventName(void) const
{
    return m_event_name;
}

const std::string &DVB_Short_Event_Descriptor::getText(void) const
{
    return m_text;
}

////////////////////////////////////////////////////////////////////////////////

