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

#include "DVB_Subtitling_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
Subtitling::Subtitling(const uint8_t * const buffer)
{
    m_iso639_language_code.assign((char *)&buffer[0], 3);
    m_subtitling_type = buffer[3];
    m_composition_page_id = r16(&buffer[4]);
    m_ancillary_page_id = r16(&buffer[6]);
}

const std::string &Subtitling::getIso639LanguageCode(void) const
{
    return m_iso639_language_code;
}

uint8_t Subtitling::getSubtitlingType(void) const
{
    return m_subtitling_type;
}

uint16_t Subtitling::getCompositionPageId(void) const
{
    return m_composition_page_id;
}

uint16_t Subtitling::getAncillaryPageId(void) const
{
    return m_ancillary_page_id;
}

DVB_Subtitling_Descriptor::DVB_Subtitling_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    for (size_t i = 0; i < mLength; i += 8) {
        ASSERT_MIN_DLEN(i + 8);
        m_subtitlings.push_back(new Subtitling(&buffer[i + 2]));
    }
}

DVB_Subtitling_Descriptor::~DVB_Subtitling_Descriptor(void)
{
    for (SubtitlingIterator i = m_subtitlings.begin(); i != m_subtitlings.end(); ++i)
        delete *i;
}

const SubtitlingList *DVB_Subtitling_Descriptor::getSubtitlings(void) const
{
    return &m_subtitlings;
}

////////////////////////////////////////////////////////////////////////////////

