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

#include "MPEG_Hierarchy_Descriptor.h"

// util.
#include "BitStream.h"
#include "Logger.h"

static const char* TAG = "MPEG_Hierarchy_Desc";
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MPEG_Hierarchy_Descriptor::MPEG_Hierarchy_Descriptor()
{
    m_hierarchy_type = 0;
    m_hierarchy_layer_index = 0;
    m_hierarchy_embedded_layer_index = 0;
    m_hierarchy_channel = 0;
}

MPEG_Hierarchy_Descriptor::~MPEG_Hierarchy_Descriptor()
{
}

bool MPEG_Hierarchy_Descriptor::parse(BitStream* bs)
{
    // 1. get {descriptor_tag, descriptor_length}.
    if(false == BaseDescriptor::parse(bs)) {
        return false;
    }

    if(false == bs->skipBits(4)
            || false == bs->readBits(4, &m_hierarchy_type)
            || false == bs->skipBits(2)
            || false == bs->readBits(6, &m_hierarchy_layer_index)
            || false == bs->skipBits(2)
            || false == bs->readBits(6, &m_hierarchy_embedded_layer_index)
            || false == bs->skipBits(2)
            || false == bs->readBits(6, &m_hierarchy_channel)) {
        L_ERROR(TAG, "parse() failed\n");
        return false;
    }

    return true;
}



////////////////////////////////////////////////////////////////////////////////

