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

#include "DVB_Scrambling_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DVB_Scrambling_Descriptor::DVB_Scrambling_Descriptor(const uint8_t* const buffer):Descriptor(buffer)
{
    ASSERT_MIN_DLEN(1);

    m_scrambling_mode = buffer[2];
}

DVB_Scrambling_Descriptor::~DVB_Scrambling_Descriptor()
{
}

uint8_t DVB_Scrambling_Descriptor::getScramblingMode() const
{
    return m_scrambling_mode;
}

////////////////////////////////////////////////////////////////////////////////

