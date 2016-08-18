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

#ifndef DVB_CONTENT_DESCRIPTOR_H_
#define DVB_CONTENT_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class ContentClassification {
protected:
    uint8_t mContentNibbleLevel1;
    uint8_t mContentNibbleLevel2;
    uint8_t mUserNibble1;
    uint8_t mUserNibble2;

public:
    ContentClassification(const uint8_t * const buffer);

    uint8_t getContentNibbleLevel1(void) const
    {
        return mContentNibbleLevel1;
    }

    uint8_t getContentNibbleLevel2(void) const
    {
        return mContentNibbleLevel2;
    }

    uint8_t getUserNibble1(void) const
    {
        return mUserNibble1;
    }

    uint8_t getUserNibble2(void) const
    {
        return mUserNibble2;
    }

};

typedef list<ContentClassification *> ContentClassificationList;
typedef ContentClassificationList::iterator ContentClassificationIterator;
typedef ContentClassificationList::const_iterator ContentClassificationConstIterator;

class DVB_Content_Descriptor: public Descriptor {
protected:
    ContentClassificationList mListClassification;

public:
    DVB_Content_Descriptor(const uint8_t * const buffer);
    ~DVB_Content_Descriptor(void);

    const ContentClassificationList *getClassifications(void) const;
};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_CONTENT_DESCRIPTOR_H_ */

