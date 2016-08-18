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

#include "MPEG_ISO639_Lang_Descriptor.h"

// util.
#include "BitStream.h"
#include "Logger.h"

static const char* TAG = "MPEG_ISO639_Lang_Desc";
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MPEG_ISO639_Lang_Descriptor::MPEG_ISO639_Lang_Descriptor()
{
}

MPEG_ISO639_Lang_Descriptor::~MPEG_ISO639_Lang_Descriptor()
{
}

bool MPEG_ISO639_Lang_Descriptor::parse(BitStream* bs)
{
    // 1. get {descriptor_tag, descriptor_length}.
    if(false == BaseDescriptor::parse(bs)) {
        return false;
    }

    for(int i = 0; i < m_descriptor_length; i += 4) {
        descISO639List iso639;
        if (false == bs->readByte(3, iso639.ISO_639_language_code) ||
                false == bs->readBits(8, &iso639.audio_type)){
            L_ERROR(TAG, "[%d] parse() failed\n", i);
            return false;
        }

        m_ISO639_List.push_back(iso639);
    }

    return true;
}



////////////////////////////////////////////////////////////////////////////////

