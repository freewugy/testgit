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

#include "MPEG_VideoStream_Descriptor.h"

// util.
#include "BitStream.h"
#include "Logger.h"

static const char* TAG = "MPEG_VideoStream_Desc";

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MPEG_VideoStream_Descriptor::MPEG_VideoStream_Descriptor()
{
    m_multiple_frame_rate_flag = 0;
    m_frame_rate_code = 0;
    m_MPEG_1_only_flag = 0;
    m_constrained_parameter_flag = 0;
    m_still_picture_flag = 0;
    m_profile_and_level_indication = 0;
    m_chroma_format = 0;
    m_frame_rate_extension_flag = 0;
}

MPEG_VideoStream_Descriptor::~MPEG_VideoStream_Descriptor()
{
}

bool MPEG_VideoStream_Descriptor::parse(BitStream* bs)
{
    // 1. get {descriptor_tag, descriptor_length}.
    if(false == BaseDescriptor::parse(bs)) {
        return false;
    }

    if(false == bs->readBits(1, &m_multiple_frame_rate_flag)
            || false == bs->readBits(4, &m_frame_rate_code)
            || false == bs->readBits(1, &m_MPEG_1_only_flag)
            || false == bs->readBits(1, &m_constrained_parameter_flag)
            || false == bs->readBits(1, &m_still_picture_flag)) {
        L_ERROR(TAG, "parse() failed\n");
        return false;
    }

    if(m_MPEG_1_only_flag == 1) {
        if(false == bs->readBits(1, &m_profile_and_level_indication)
                || false == bs->readBits(1, &m_chroma_format)
                || false == bs->readBits(1, &m_frame_rate_extension_flag)
                || false == bs->skipBits(5)) {
            L_ERROR(TAG, "parse() failed\n");
            return false;
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////

