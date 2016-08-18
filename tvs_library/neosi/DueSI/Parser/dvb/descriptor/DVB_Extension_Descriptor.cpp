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

#include "DVB_Extension_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DVB_Extension_Descriptor::DVB_Extension_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    ASSERT_MIN_DLEN(1);

    m_extension_tag = buffer[2];
    m_selector_bytes.resize(mLength - 1);
    memcpy(&m_selector_bytes[0], &buffer[3], mLength - 1);
}

DVB_Extension_Descriptor::~DVB_Extension_Descriptor()
{
}

uint8_t DVB_Extension_Descriptor::getExtensionTag() const
{
    return m_extension_tag;
}

const SelectorByteVector *DVB_Extension_Descriptor::getSelectorBytes() const
{
    return &m_selector_bytes;
}

////////////////////////////////////////////////////////////////////////////////

