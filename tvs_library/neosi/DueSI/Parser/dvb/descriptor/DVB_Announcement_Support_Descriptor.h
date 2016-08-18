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

#ifndef __DVB_ANNOUNCEMENT_SUPPORT_DESCRIPTOR_H__
#define __DVB_ANNOUNCEMENT_SUPPORT_DESCRIPTOR_H__

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class Announcement {
protected:
    uint8_t mAnnouncementType;
    uint8_t mReferenceType;
    uint16_t mOriginalNetworkID;
    uint16_t mTransportStreamID;
    uint16_t mServiceID;
    uint8_t mComponentTag;

public:
    Announcement(const uint8_t * const buffer);

    uint8_t getAnnouncementType(void) const
    {
        return mAnnouncementType;
    }

    uint8_t getReferenceType(void) const
    {
        return mReferenceType;
    }

    uint16_t getOriginalNetworkId(void) const
    {
        return mOriginalNetworkID;
    }

    uint16_t getTransportStreamId(void) const
    {
        return mTransportStreamID;
    }

    uint16_t getServiceId(void) const
    {
        return mServiceID;
    }

    uint8_t getComponentTag(void) const
    {
        return mComponentTag;
    }

};

typedef list<Announcement *> AnnouncementList;
typedef AnnouncementList::iterator AnnouncementIterator;
typedef AnnouncementList::const_iterator AnnouncementConstIterator;

class DVB_Announcement_Support_Descriptor: public Descriptor {
protected:
    uint16_t m_announcement_support_indicator;
    AnnouncementList m_announcements;

public:
    DVB_Announcement_Support_Descriptor(const uint8_t * const buffer);
    ~DVB_Announcement_Support_Descriptor(void);

    uint16_t getAnnouncementSupportIndicator(void) const
    {
        return m_announcement_support_indicator;
    }

    const AnnouncementList* getAnnouncements(void) const
    {
        return &m_announcements;
    }

};

////////////////////////////////////////////////////////////////////////////////

#endif /* __DVB_ANNOUNCEMENT_SUPPORT_DESCRIPTOR_H__ */

