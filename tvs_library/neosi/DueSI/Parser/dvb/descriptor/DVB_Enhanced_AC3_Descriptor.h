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

#ifndef DVB_ENHANCED_AC3_DESCRIPTOR_H_
#define DVB_ENHANCED_AC3_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef vector<uint8_t> AdditionalInfoVector;
typedef AdditionalInfoVector::iterator AdditionalInfoIterator;
typedef AdditionalInfoVector::const_iterator AdditionalInfoConstIterator;

class DVB_Enhanced_AC3_Descriptor: public Descriptor {
protected:
    uint8_t mComponentTypeFlag;
    uint8_t mBSIDFlag;
    uint8_t mMainIDFlag;
    uint8_t mASVCFlag;
    uint8_t mMixInfoExistsFlag;
    uint8_t mSubstream1Flag;
    uint8_t mSubstream2Flag;
    uint8_t mSubstream3Flag;

    uint8_t mComponentType;
    uint8_t mBSID;
    uint8_t mMainID;
    uint8_t mASVC;
    uint8_t mSubstream1;
    uint8_t mSubstream2;
    uint8_t mSubstream3;

    AdditionalInfoVector mAdditionalInfo;

public:
    DVB_Enhanced_AC3_Descriptor(const uint8_t* const buffer);
    virtual ~DVB_Enhanced_AC3_Descriptor();

    uint8_t getComponentTypeFlag() const
    {
        return mComponentTypeFlag;
    }

    uint8_t getBsidFlag() const
    {
        return mBSIDFlag;
    }

    uint8_t getMainidFlag() const
    {
        return mMainIDFlag;
    }

    uint8_t getAsvcFlag() const
    {
        return mASVCFlag;
    }

    uint8_t getMixInfoExistsFlag() const
    {
        return mMixInfoExistsFlag;
    }

    uint8_t getSubstream1Flag() const
    {
        return mSubstream1Flag;
    }

    uint8_t getSubstream2Flag() const
    {
        return mSubstream2Flag;
    }

    uint8_t getSubstream3Flag() const
    {
        return mSubstream3Flag;
    }

    uint8_t getComponentType() const
    {
        return mComponentType;
    }

    uint8_t getBsid() const
    {
        return mBSID;
    }

    uint8_t getMainid() const
    {
        return mMainID;
    }

    uint8_t getAsvc() const
    {
        return mASVC;
    }

    uint8_t getSubstream1() const
    {
        return mSubstream1;
    }

    uint8_t getSubstream2() const
    {
        return mSubstream2;
    }

    uint8_t getSubstream3() const
    {
        return mSubstream3;
    }

    const AdditionalInfoVector *getAdditionalInfo() const
    {
        return &mAdditionalInfo;
    }
};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_ENHANCED_AC3_DESCRIPTOR_H_ */

