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

#include "MPEG_MultiplexBufUtil_Descriptor.h"

// util.
#include "BitStream.h"
#include "Logger.h"

static const char* TAG = "MPEG_MultiplexBufUtil_Desc";
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MPEG_MultiplexBufUtil_Descriptor::MPEG_MultiplexBufUtil_Descriptor()
{
    m_bound_valid_flag = 0;
    m_LTW_offset_lower_bound = 0;
    m_LTW_offset_upper_bound = 0;
}

MPEG_MultiplexBufUtil_Descriptor::~MPEG_MultiplexBufUtil_Descriptor()
{
}

bool MPEG_MultiplexBufUtil_Descriptor::parse(BitStream* bs)
{
    // 1. get {descriptor_tag, descriptor_length}.
    if(false == BaseDescriptor::parse(bs)) {
        return false;
    }

    if(false == bs->readBits(1, &m_bound_valid_flag)
            || false == bs->readBits(15, &m_LTW_offset_lower_bound)
            || false == bs->skipBits(1)
            || false == bs->readBits(15, &m_LTW_offset_upper_bound)) {
        L_ERROR(TAG, "parse() failed\n");
        return false;
    }

    return true;
}



////////////////////////////////////////////////////////////////////////////////

