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

#ifndef DVB_TERRESTRIAL_DELIVERY_SYSTEM_DESCRIPTOR_H_
#define DVB_TERRESTRIAL_DELIVERY_SYSTEM_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class DVB_Terrestrial_Delivery_System_Descriptor: public Descriptor {
protected:
    uint32_t mCenterFreq;
    uint8_t mBandWidth;
    uint8_t mPriority;
    uint8_t mTimeSlicingIndicator;
    uint8_t mMpeFECIndicator;
    uint8_t mConstellation;
    uint8_t mHierarchyInfo;
    uint8_t mCodeRateHpStream;
    uint8_t mCodeRateLpStream;
    uint8_t mGuardInterval;
    uint8_t mTransmissionMode;
    uint8_t mOtherFrequencyFlag;

public:
    DVB_Terrestrial_Delivery_System_Descriptor(const uint8_t * const buffer);

    uint32_t getCentreFrequency(void) const
    {
        return mCenterFreq;
    }

    uint8_t getBandwidth(void) const
    {
        return mBandWidth;
    }

    uint8_t getPriority(void) const
    {
        return mPriority;
    }

    uint8_t getTimeSlicingIndicator(void) const
    {
        return mTimeSlicingIndicator;
    }

    uint8_t getMpeFecIndicator(void) const
    {
        return mMpeFECIndicator;
    }

    uint8_t getConstellation(void) const
    {
        return mConstellation;
    }

    uint8_t getHierarchyInformation(void) const
    {
        return mHierarchyInfo;
    }

    uint8_t getCodeRateHpStream(void) const
    {
        return mCodeRateHpStream;
    }

    uint8_t getCodeRateLpStream(void) const
    {
        return mCodeRateLpStream;
    }

    uint8_t getGuardInterval(void) const
    {
        return mGuardInterval;
    }

    uint8_t getTransmissionMode(void) const
    {
        return mTransmissionMode;
    }

    uint8_t getOtherFrequencyFlag(void) const
    {
        return mOtherFrequencyFlag;
    }

};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_TERRESTRIAL_DELIVERY_SYSTEM_DESCRIPTOR_H_ */

