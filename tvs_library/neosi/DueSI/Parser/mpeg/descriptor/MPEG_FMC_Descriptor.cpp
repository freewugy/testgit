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

#include "MPEG_FMC_Descriptor.h"

// util.
#include "BitStream.h"
#include "Logger.h"

static const char* TAG = "MPEG_FMC_Descriptor";
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MPEG_FMC_Descriptor::MPEG_FMC_Descriptor()
{
}

MPEG_FMC_Descriptor::~MPEG_FMC_Descriptor()
{
}

bool MPEG_FMC_Descriptor::parse(BitStream* bs)
{
    // 1. get {descriptor_tag, descriptor_length}.
    if(false == BaseDescriptor::parse(bs)) {
        return false;
    }

    for(int i = 0; i < m_descriptor_length; i += 3) {
        FMC fmc;
        if (false == bs->readBits(16, &fmc.m_ES_ID) ||
                false == bs->readBits(8, &fmc.m_FlexMuxChannel)){
            L_ERROR(TAG, "[%d] parse() failed\n", i);
            return false;
        }

        m_FMC_List.push_back(fmc);
    }

    return true;
}



////////////////////////////////////////////////////////////////////////////////

