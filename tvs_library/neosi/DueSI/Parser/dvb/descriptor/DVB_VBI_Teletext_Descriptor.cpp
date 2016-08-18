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

#include "DVB_VBI_Teletext_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
VbiTeletext::VbiTeletext(const uint8_t * const buffer)
{
    m_iso639_language_code.assign((char *)&buffer[0], 3);
    m_teletext_type = (buffer[3] >> 3) & 0x1F;
    m_teletext_magazine_number = buffer[3] & 0x07;
    m_teletext_page_number = buffer[4];
}

const std::string &VbiTeletext::getIso639LanguageCode(void) const
{
    return m_iso639_language_code;
}

uint8_t VbiTeletext::getTeletextType(void) const
{
    return m_teletext_type;
}

uint8_t VbiTeletext::getTeletextMagazineNumber(void) const
{
    return m_teletext_magazine_number;
}

uint8_t VbiTeletext::getTeletextPageNumber(void) const
{
    return m_teletext_page_number;
}

DVB_VBI_Teletext_Descriptor::DVB_VBI_Teletext_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    for (size_t i = 0; i < mLength; i += 5) {
        ASSERT_MIN_DLEN(i + 5);
        m_vbi_teletexts.push_back(new VbiTeletext(&buffer[i + 2]));
    }
}

DVB_VBI_Teletext_Descriptor::~DVB_VBI_Teletext_Descriptor(void)
{
    for (VbiTeletextIterator i = m_vbi_teletexts.begin(); i != m_vbi_teletexts.end(); ++i)
        delete *i;
}

const VbiTeletextList *DVB_VBI_Teletext_Descriptor::getVbiTeletexts(void) const
{
    return &m_vbi_teletexts;
}

////////////////////////////////////////////////////////////////////////////////

