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

#include "DVB_Announcement_Support_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
Announcement::Announcement(const uint8_t * const buffer)
{
    mAnnouncementType = (buffer[0] >> 4) & 0x0f;
    mReferenceType = buffer[0] & 0x07;

    if ((mReferenceType >= 0x01) && (mReferenceType <= 0x03)) {
        mOriginalNetworkID = r16(&buffer[1]);
        mTransportStreamID = r16(&buffer[3]);
        mServiceID = r16(&buffer[5]);
        mComponentTag = buffer[7];
    }
}

DVB_Announcement_Support_Descriptor::DVB_Announcement_Support_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    Announcement *a;
    size_t headerLength = 2;
    ASSERT_MIN_DLEN(headerLength);

    m_announcement_support_indicator = r16(&buffer[2]);

    for (size_t i = 0; i < mLength - 2; ++i) {
        headerLength++;
        ASSERT_MIN_DLEN(headerLength);

        a = new Announcement(&buffer[i + 4]);
        m_announcements.push_back(a);
        switch (a->getReferenceType()) {
        case 0x01:
        case 0x02:
        case 0x03:
            // FIXME: might already have parsed beyond end
            // of memory in Announcement()
            headerLength += 7;
            ASSERT_MIN_DLEN(headerLength);

            i += 7;
            break;
        default:
            break;
        }
    }
}

DVB_Announcement_Support_Descriptor::~DVB_Announcement_Support_Descriptor(void)
{
    for (AnnouncementIterator itr = m_announcements.begin(); itr != m_announcements.end(); ++itr)
        delete *itr;
}

////////////////////////////////////////////////////////////////////////////////

