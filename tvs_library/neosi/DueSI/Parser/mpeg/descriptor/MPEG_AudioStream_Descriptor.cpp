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

#include "MPEG_AudioStream_Descriptor.h"

// util.
#include "BitStream.h"
#include "Logger.h"

static const char* TAG = "MPEG_AudioStream_Desc";
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MPEG_AudioStream_Descriptor::MPEG_AudioStream_Descriptor()
{
    m_byte_data = 0;
    m_free_format_flag = 0;
    m_ID = 0;
    m_layer = 0;
    m_variable_rate_audio_indicator = 0;
}

MPEG_AudioStream_Descriptor::~MPEG_AudioStream_Descriptor()
{
}

bool MPEG_AudioStream_Descriptor::parse(BitStream* bs)
{
    // 1. get {descriptor_tag, descriptor_length}.
    if(false == BaseDescriptor::parse(bs)) {
        return false;
    }

    if(false == bs->readBits(8, &m_byte_data)
            || false == bs->readBits(1, &m_free_format_flag)
            || false == bs->readBits(1, &m_ID)
            || false == bs->readBits(2, &m_layer)
            || false == bs->readBits(1, &m_variable_rate_audio_indicator)
            || false == bs->skipBits(3)) {
        L_ERROR(TAG, "parse() failed\n");
        return false;
    }

    return true;
}



////////////////////////////////////////////////////////////////////////////////

