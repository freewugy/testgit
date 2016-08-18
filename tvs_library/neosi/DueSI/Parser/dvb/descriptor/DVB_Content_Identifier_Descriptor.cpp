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

#include "DVB_Content_Identifier_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
ContentReferenceIdentifier::ContentReferenceIdentifier(const uint8_t * const buffer)
{
    m_type = (buffer[0] >> 2) & 0x3f;
    m_location = buffer[0] & 0x03;

    if (m_location == 0) {
        m_length = buffer[1];
        m_crid_bytes.reserve(m_length);
        memcpy(&m_crid_bytes[0], buffer+2, m_length);
        m_reference = 0x0000;
    } else if (m_location == 1) {
        m_length = 0;
        m_reference = r16(&buffer[2]);
    }
    // else DVB reserved
}

ContentReferenceIdentifier::~ContentReferenceIdentifier()
{
}

uint8_t ContentReferenceIdentifier::getType() const
{
    return m_type;
}

uint8_t ContentReferenceIdentifier::getLocation() const
{
    return m_location;
}

uint8_t ContentReferenceIdentifier::getLength() const
{
    return m_length;
}

const ContentReferenceIdentifierByteVector *ContentReferenceIdentifier::getBytes() const
{
    return &m_crid_bytes;
}

uint16_t ContentReferenceIdentifier::getReference() const
{
    return m_reference;
}

DVB_Content_Identifier_Descriptor::DVB_Content_Identifier_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    for (size_t i = 0; i < mLength; /* inc inside loop */) {
        ContentReferenceIdentifier *crid = new ContentReferenceIdentifier(&buffer[2 + i]);

        if (crid->getLocation() == 0)
            i += crid->getLength() + 2;
        else if (crid->getLocation() == 1)
            i += 3;

        m_identifiers.push_back(crid);
    }
}

DVB_Content_Identifier_Descriptor::~DVB_Content_Identifier_Descriptor()
{
    for (ContentReferenceIdentifierIterator it = m_identifiers.begin(); it != m_identifiers.end(); ++it)
        delete *it;
}

const ContentReferenceIdentifierList *DVB_Content_Identifier_Descriptor::getIdentifier() const
{
    return &m_identifiers;
}

////////////////////////////////////////////////////////////////////////////////

