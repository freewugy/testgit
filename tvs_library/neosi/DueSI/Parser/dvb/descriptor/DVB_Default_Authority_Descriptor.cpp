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

#include "DVB_Default_Authority_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DVB_Default_Authority_Descriptor::DVB_Default_Authority_Descriptor(const uint8_t * const buffer)
        : Descriptor(buffer), m_bytes(mLength)
{

    memcpy(&m_bytes[0], &buffer[2], mLength);
}

DVB_Default_Authority_Descriptor::~DVB_Default_Authority_Descriptor()
{
}

const DefaultAuthorityByteVector *DVB_Default_Authority_Descriptor::getAuthorityBytes(void) const
{
    return &m_bytes;
}

////////////////////////////////////////////////////////////////////////////////

