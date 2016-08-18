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

#include "DVB_Component_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DVB_Component_Descriptor::DVB_Component_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    ASSERT_MIN_DLEN(6);

    m_stream_content = buffer[2] & 0x0f;
    m_component_type = buffer[3];
    m_component_tag = buffer[4];
    m_iso639_language_code.assign((char *) &buffer[5], 3);
    m_text.assign((char *) &buffer[8], mLength - 6);
}

uint8_t DVB_Component_Descriptor::getStreamContent(void) const
{
    return m_stream_content;
}

uint8_t DVB_Component_Descriptor::getComponentType(void) const
{
    return m_component_type;
}

uint8_t DVB_Component_Descriptor::getComponentTag(void) const
{
    return m_component_tag;
}

const std::string &DVB_Component_Descriptor::getIso639LanguageCode(void) const
{
    return m_iso639_language_code;
}

const std::string &DVB_Component_Descriptor::getText(void) const
{
    return m_text;
}

////////////////////////////////////////////////////////////////////////////////

