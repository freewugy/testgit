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

#include "DVB_Enhanced_AC3_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DVB_Enhanced_AC3_Descriptor::DVB_Enhanced_AC3_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    // EN300468 says that descriptor_length must be >= 1,
    // but it's easy to set sane defaults in this case
    // and some already broadcasters got it wrong.
    if (mLength == 0) {
        mComponentTypeFlag = 0;
        mBSIDFlag = 0;
        mMainIDFlag = 0;
        mASVCFlag = 0;
        mMixInfoExistsFlag = 0;
        mSubstream1Flag = 0;
        mSubstream2Flag = 0;
        mSubstream3Flag = 0;
        return;
    }

    mComponentTypeFlag = (buffer[2] >> 7) & 0x01;
    mBSIDFlag = (buffer[2] >> 6) & 0x01;
    mMainIDFlag = (buffer[2] >> 5) & 0x01;
    mASVCFlag = (buffer[2] >> 4) & 0x01;
    mMixInfoExistsFlag = (buffer[2] >> 3) & 0x01;
    mSubstream1Flag = (buffer[2] >> 2) & 0x01;
    mSubstream2Flag = (buffer[2] >> 1) & 0x01;
    mSubstream3Flag = buffer[2] & 0x01;

    size_t headerLength = 1 + mComponentTypeFlag + mBSIDFlag + mMainIDFlag +
            mASVCFlag + mSubstream1Flag +
        mSubstream2Flag + mSubstream3Flag;
    ASSERT_MIN_DLEN(headerLength);

    size_t i = 3;
    if (mComponentTypeFlag == 0x01)
        mComponentType = buffer[i++];
    if (mBSIDFlag == 0x01)
        mBSID = buffer[i++];
    if (mMainIDFlag == 0x01)
        mMainID = buffer[i++];
    if (mASVCFlag == 0x01)
        mASVC = buffer[i++];
    if (mSubstream1Flag == 0x01)
        mSubstream1 = buffer[i++];
    if (mSubstream2Flag == 0x01)
        mSubstream2 = buffer[i++];
    if (mSubstream3Flag == 0x01)
        mSubstream3 = buffer[i++];

    mAdditionalInfo.resize(mLength - headerLength);
    memcpy(&mAdditionalInfo[0], &buffer[i], mLength - headerLength);
}

DVB_Enhanced_AC3_Descriptor::~DVB_Enhanced_AC3_Descriptor()
{
}

////////////////////////////////////////////////////////////////////////////////

