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

#include "MPEG_SmoothingBuf_Descriptor.h"

// util.
#include "BitStream.h"
#include "Logger.h"

static const char* TAG = "MPEG_SmoothingBuf_Desc";
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MPEG_SmoothingBuf_Descriptor::MPEG_SmoothingBuf_Descriptor()
{
    m_sb_leak_rate = 0;
    m_sb_size = 0;
}

MPEG_SmoothingBuf_Descriptor::~MPEG_SmoothingBuf_Descriptor()
{
}

bool MPEG_SmoothingBuf_Descriptor::parse(BitStream* bs)
{
    // 1. get {descriptor_tag, descriptor_length}.
    if(false == BaseDescriptor::parse(bs)) {
        return false;
    }

    if(false == bs->skipBits(2)
            || false == bs->readBits(22, &m_sb_leak_rate)
            || false == bs->skipBits(2)
            || false == bs->readBits(22, &m_sb_size)) {
        L_ERROR(TAG, "parse() failed\n");
        return false;
    }

    return true;
}



////////////////////////////////////////////////////////////////////////////////

