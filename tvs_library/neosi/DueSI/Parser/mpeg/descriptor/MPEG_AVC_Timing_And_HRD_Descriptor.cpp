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

#include "MPEG_AVC_Timing_And_HRD_Descriptor.h"

// util.
#include "BitStream.h"
#include "Logger.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MPEG_AVC_Timing_And_HRD_Descriptor::MPEG_AVC_Timing_And_HRD_Descriptor()
{
}

MPEG_AVC_Timing_And_HRD_Descriptor::~MPEG_AVC_Timing_And_HRD_Descriptor()
{
}

bool MPEG_AVC_Timing_And_HRD_Descriptor::parse(BitStream* bs)
{
    // 1. get {descriptor_tag, descriptor_length}.
    if(false == BaseDescriptor::parse(bs)) {
        return false;
    }

    // TODO : parse.
    return bs->skipByte(m_descriptor_length);
}



////////////////////////////////////////////////////////////////////////////////

