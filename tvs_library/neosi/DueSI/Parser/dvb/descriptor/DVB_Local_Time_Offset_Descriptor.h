/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: loneowl $
 * $LastChangedDate: 2014-04-23 12:02:06 +0900 (수, 23 4월 2014) $
 * $LastChangedRevision: 684 $
 * Description:
 * Note:
 *****************************************************************************/

#ifndef DVB_LOCAL_TIME_OFFSET_DESCRIPTOR_H_
#define DVB_LOCAL_TIME_OFFSET_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class LocalTimeOffset {
protected:
    string mCountryCode;
    uint8_t mCountryRegionID;
    uint8_t mLocalTimeOffsetPolarity;
    uint16_t mLocalTimeOffset;
    uint32_t mTimeOfChange;
    uint16_t mNextTimeOffset;

public:
    LocalTimeOffset(const uint8_t * const buffer);

    const string &getCountryCode(void) const
    {
        return mCountryCode;
    }

    uint8_t getCountryRegionId(void) const
    {
        return mCountryRegionID;
    }

    uint8_t getLocalTimeOffsetPolarity(void) const
    {
        return mLocalTimeOffsetPolarity;
    }

    uint16_t getLocalTimeOffset(void) const
    {
        return mLocalTimeOffset;
    }

    uint32_t getTimeOfChange(void) const
    {
        return mTimeOfChange;
    }

    uint16_t getNextTimeOffset(void) const
    {
        return mNextTimeOffset;
    }
};

typedef list<LocalTimeOffset *> LocalTimeOffsetList;
typedef LocalTimeOffsetList::iterator LocalTimeOffsetIterator;
typedef LocalTimeOffsetList::const_iterator LocalTimeOffsetConstIterator;

class DVB_Local_Time_Offset_Descriptor: public Descriptor {
protected:
    LocalTimeOffsetList mLocalTimeOffsets;

public:
    DVB_Local_Time_Offset_Descriptor(const uint8_t * const buffer);
    ~DVB_Local_Time_Offset_Descriptor(void);

    const LocalTimeOffsetList *getLocalTimeOffsets(void) const;
};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_LOCAL_TIME_OFFSET_DESCRIPTOR_H_ */

