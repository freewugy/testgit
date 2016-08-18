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

#include "MPEG_CA_Descriptor.h"

// util.
#include "BitStream.h"
#include "Logger.h"

static const char* TAG = "MPEG_CA_Desc";

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MPEG_CA_Descriptor::MPEG_CA_Descriptor()
{
    m_CA_system_id = 0;
    m_CA_PID = 0;
}

MPEG_CA_Descriptor::~MPEG_CA_Descriptor()
{
}

bool MPEG_CA_Descriptor::parse(BitStream* bs)
{
    // 1. get {descriptor_tag, descriptor_length}.
    if(false == BaseDescriptor::parse(bs)) {
        return false;
    }

    if(false == bs->readBits(16, &m_CA_system_id)
            || false == bs->skipBits(3)
            || false == bs->readBits(13, &m_CA_PID)) {
        L_ERROR(TAG, "parse() failed\n");
        return false;
    }

    for(int i = 4; i < m_descriptor_length; i++) {
        uint8_t data;
        if(false == bs->readBits(8, &data)) {
            L_ERROR(TAG, "[%d] parse() failed\n", i);
            return false;
        }
        m_private_data.push_back(data);
    }

    return true;
}



////////////////////////////////////////////////////////////////////////////////

