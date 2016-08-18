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

#include "DVB_Local_Time_Offset_Descriptor.h"
#include "TimeConvertor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
LocalTimeOffset::LocalTimeOffset(const uint8_t * const buffer)
{
    uint16_t mjd;
    uint32_t bcd;
    mCountryCode.assign((char *)&buffer[0], 3);
    mCountryRegionID = (buffer[3] >> 2) & 0x3f;
    mLocalTimeOffsetPolarity = buffer[3] & 0x01;
    mLocalTimeOffset = r16(&buffer[4]);
    mjd = r16(&buffer[6]);
    bcd = (buffer[8] << 16) | r16(&buffer[9]);
    mTimeOfChange = TimeConvertor::getInstance().convertTimeMJDBCD2Seconds(mjd, bcd);
    mNextTimeOffset = r16(&buffer[11]);
}

DVB_Local_Time_Offset_Descriptor::DVB_Local_Time_Offset_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    for (int i = 0; i < mLength; i += 13) {
        mLocalTimeOffsets.push_back(new LocalTimeOffset(&buffer[i + 2]));
    }
}


DVB_Local_Time_Offset_Descriptor::~DVB_Local_Time_Offset_Descriptor(void)
{
    LocalTimeOffsetIterator itr;
    for (itr = mLocalTimeOffsets.begin(); itr != mLocalTimeOffsets.end(); ++itr)
        delete *itr;
}

const LocalTimeOffsetList *DVB_Local_Time_Offset_Descriptor::getLocalTimeOffsets(void) const
{
    return &mLocalTimeOffsets;
}

////////////////////////////////////////////////////////////////////////////////

