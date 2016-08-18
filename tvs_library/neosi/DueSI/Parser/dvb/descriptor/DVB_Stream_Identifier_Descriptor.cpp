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

#include "DVB_Stream_Identifier_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DVB_Stream_Identifier_Descriptor::DVB_Stream_Identifier_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    ASSERT_MIN_DLEN(1);

    m_component_tag = buffer[2];
}

uint8_t DVB_Stream_Identifier_Descriptor::getComponentTag(void) const
{
    return m_component_tag;
}

////////////////////////////////////////////////////////////////////////////////

