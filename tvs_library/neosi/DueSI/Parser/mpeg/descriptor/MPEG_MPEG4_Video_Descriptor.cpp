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

#include "MPEG_MPEG4_Video_Descriptor.h"

// util.
#include "BitStream.h"
#include "Logger.h"

static const char* TAG = "MPEG_MPEG4_Video_Desc";
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MPEG_MPEG4_Video_Descriptor::MPEG_MPEG4_Video_Descriptor()
{
    m_MPEG4_visual_profile_and_level = 0;
}

MPEG_MPEG4_Video_Descriptor::~MPEG_MPEG4_Video_Descriptor()
{
}

bool MPEG_MPEG4_Video_Descriptor::parse(BitStream* bs)
{
    // 1. get {descriptor_tag, descriptor_length}.
    if(false == BaseDescriptor::parse(bs)) {
        return false;
    }

    if(false == bs->readBits(8, &m_MPEG4_visual_profile_and_level)) {
        L_ERROR(TAG, "parse() failed\n");
        return false;
    }

    return true;
}



////////////////////////////////////////////////////////////////////////////////

