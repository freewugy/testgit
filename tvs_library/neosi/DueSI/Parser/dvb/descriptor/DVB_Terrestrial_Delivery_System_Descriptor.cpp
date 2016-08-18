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

#include "DVB_Terrestrial_Delivery_System_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DVB_Terrestrial_Delivery_System_Descriptor::DVB_Terrestrial_Delivery_System_Descriptor(const uint8_t * const buffer)
        : Descriptor(buffer)
{
    ASSERT_MIN_DLEN(7);

    mCenterFreq = r32(&buffer[2]);
    mBandWidth = (buffer[6] >> 5) & 0x07;
    mPriority = (buffer[6] >> 4) & 0x01;
    mTimeSlicingIndicator = (buffer[6] >> 3) & 0x01;
    mMpeFECIndicator = (buffer[6] >> 2) & 0x01;
    mConstellation = (buffer[7] >> 6) & 0x03;
    mHierarchyInfo = (buffer[7] >> 3) & 0x07;
    mCodeRateHpStream = buffer[7] & 0x07;
    mCodeRateLpStream = (buffer[8] >> 5) & 0x07;
    mGuardInterval = (buffer[8] >> 3) & 0x03;
    mTransmissionMode = (buffer[8] >> 1) & 0x03;
    mOtherFrequencyFlag = buffer[8] & 0x01;
}

////////////////////////////////////////////////////////////////////////////////

