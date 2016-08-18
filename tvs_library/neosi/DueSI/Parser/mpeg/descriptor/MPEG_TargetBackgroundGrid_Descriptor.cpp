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

#include "MPEG_TargetBackgroundGrid_Descriptor.h"

// util.
#include "BitStream.h"
#include "Logger.h"

static const char* TAG = "MPEG_TargetBackgroundGrid_Desc";
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MPEG_TargetBackgroundGrid_Descriptor::MPEG_TargetBackgroundGrid_Descriptor()
{
    m_horizontal_size = 0;
    m_vertical_size = 0;
    m_aspect_ratio_information = 0;
}

MPEG_TargetBackgroundGrid_Descriptor::~MPEG_TargetBackgroundGrid_Descriptor()
{
}

bool MPEG_TargetBackgroundGrid_Descriptor::parse(BitStream* bs)
{
    // 1. get {descriptor_tag, descriptor_length}.
    if(false == BaseDescriptor::parse(bs)) {
        return false;
    }

    if(false == bs->readBits(14, &m_horizontal_size)
            || false == bs->readBits(14, &m_vertical_size)
            || false == bs->readBits(4, &m_aspect_ratio_information)) {
        L_ERROR(TAG, "parse() failed\n");
        return false;
    }

    return true;
}



////////////////////////////////////////////////////////////////////////////////

