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

#include "MPEG_PrivateDataIndicator_Descriptor.h"

// util.
#include "BitStream.h"
#include "Logger.h"

static const char* TAG = "MPEG_PrivateDataIndicator_Desc";
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MPEG_PrivateDataIndicator_Descriptor::MPEG_PrivateDataIndicator_Descriptor()
{
    m_private_data_indicator = 0;
}

MPEG_PrivateDataIndicator_Descriptor::~MPEG_PrivateDataIndicator_Descriptor()
{
}

bool MPEG_PrivateDataIndicator_Descriptor::parse(BitStream* bs)
{
    // 1. get {descriptor_tag, descriptor_length}.
    if(false == BaseDescriptor::parse(bs)) {
        return false;
    }

    if(false == bs->readBits(32, &m_private_data_indicator)) {
        L_ERROR(TAG, "parse() failed\n");
        return false;
    }

    return true;
}



////////////////////////////////////////////////////////////////////////////////

