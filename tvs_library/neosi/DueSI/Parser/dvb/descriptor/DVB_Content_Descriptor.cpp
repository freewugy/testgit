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

#include "DVB_Content_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
ContentClassification::ContentClassification(const uint8_t * const buffer)
{
    mContentNibbleLevel1 = (buffer[0] >> 4) & 0x0f;
    mContentNibbleLevel2 = buffer[0] & 0x0f;
    mUserNibble1 = (buffer[1] >> 4) & 0x0f;
    mUserNibble2 = buffer[1] & 0x0f;
}

DVB_Content_Descriptor::DVB_Content_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    for (size_t i = 0; i < mLength; i += 2) {
        ASSERT_MIN_DLEN(i + 2);
        mListClassification.push_back(new ContentClassification(&buffer[i + 2]));
    }
}

DVB_Content_Descriptor::~DVB_Content_Descriptor(void)
{
    for (ContentClassificationList::iterator i = mListClassification.begin(); i != mListClassification.end(); ++i)
        delete *i;
}

const ContentClassificationList *DVB_Content_Descriptor::getClassifications(void) const
{
    return &mListClassification;
}

////////////////////////////////////////////////////////////////////////////////

