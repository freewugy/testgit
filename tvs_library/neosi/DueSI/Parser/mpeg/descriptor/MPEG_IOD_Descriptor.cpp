/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: loneowl $
 * $LastChangedDate: 2014-04-15 13:01:53 +0900 (화, 15 4월 2014) $
 * $LastChangedRevision: 613 $
 * Description:
 * Note:
 *****************************************************************************/

#include "MPEG_IOD_Descriptor.h"

// util.
#include "BitStream.h"
#include "Logger.h"

static const char* TAG = "MPEG_IOD_Desc";
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MPEG_IOD_Descriptor::MPEG_IOD_Descriptor()
{
    m_Scope_of_IOD_label = 0;
    m_IOD_label = 0;
    m_InitialObjectDescriptor = 0;
}

MPEG_IOD_Descriptor::~MPEG_IOD_Descriptor()
{
}

bool MPEG_IOD_Descriptor::parse(BitStream* bs)
{
    // 1. get {descriptor_tag, descriptor_length}.
    if(false == BaseDescriptor::parse(bs)) {
        return false;
    }

    if(false == bs->readBits(8, &m_Scope_of_IOD_label)
            || false == bs->readBits(8, &m_IOD_label)
            || false == bs->readBits(8, &m_InitialObjectDescriptor)) {
        L_ERROR(TAG, "parse() failed\n");
        return false;
    }

    return true;
}



////////////////////////////////////////////////////////////////////////////////

