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

#include "MPEG_SystemClock_Descriptor.h"

// util.
#include "BitStream.h"
#include "Logger.h"

static const char* TAG = "MPEG_SystemClock_Desc";
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MPEG_SystemClock_Descriptor::MPEG_SystemClock_Descriptor()
{
    m_external_clock_reference_indicator = 0;
    m_clock_accuracy_integer = 0;
    m_clock_accuracy_exponent = 0;
}

MPEG_SystemClock_Descriptor::~MPEG_SystemClock_Descriptor()
{
}

bool MPEG_SystemClock_Descriptor::parse(BitStream* bs)
{
    // 1. get {descriptor_tag, descriptor_length}.
    if(false == BaseDescriptor::parse(bs)) {
        return false;
    }

    if(false == bs->readBits(1, &m_external_clock_reference_indicator)
            || false == bs->skipBits(1)
            || false == bs->readBits(6, &m_clock_accuracy_integer)
            || false == bs->readBits(3, &m_clock_accuracy_exponent)
            || false == bs->skipBits(5)) {
        L_ERROR(TAG, "parse() failed\n");
        return false;
    }

    return true;
}



////////////////////////////////////////////////////////////////////////////////

