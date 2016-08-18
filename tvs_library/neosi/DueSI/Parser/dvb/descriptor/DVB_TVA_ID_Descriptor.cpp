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

#include "DVB_TVA_ID_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
TVAIdentifier::TVAIdentifier(const uint8_t * const buffer)
{
    m_id = r16(&buffer[0]);
    m_running_status = buffer[2] & 0x03;
}

TVAIdentifier::~TVAIdentifier()
{
}

uint16_t TVAIdentifier::getId() const
{
    return m_id;
}

uint8_t TVAIdentifier::getRunningStatus() const
{
    return m_running_status;
}

DVB_TVA_ID_Descriptor::DVB_TVA_ID_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    for (size_t i = 0; i < mLength; i += 3) {
        ASSERT_MIN_DLEN(i + 3);
        m_identifiers.push_back(new TVAIdentifier(&buffer[i + 2]));
    }
}

DVB_TVA_ID_Descriptor::~DVB_TVA_ID_Descriptor()
{
    for (TVAIdentifierIterator it = m_identifiers.begin(); it != m_identifiers.end(); ++it)
        delete *it;
}

const TVAIdentifierList *DVB_TVA_ID_Descriptor::getIdentifier() const
{
    return &m_identifiers;
}

////////////////////////////////////////////////////////////////////////////////

