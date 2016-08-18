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

#ifndef DVB_MULTILINGUAL_COMPONENT_NAME_DESCRIPTOR_H_
#define DVB_MULTILINGUAL_COMPONENT_NAME_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class MultilingualComponent {
protected:
    string mISO639LanguageCode;
    uint8_t mTextDescriptionLength;
    string mText;

public:
    MultilingualComponent(const uint8_t * const buffer);

    const string &getIso639LanguageCode(void) const
    {
        return mISO639LanguageCode;
    }

    const string &getText(void) const
    {
        return mText;
    }

};

typedef list<MultilingualComponent *> MultilingualComponentList;
typedef MultilingualComponentList::iterator MultilingualComponentIterator;
typedef MultilingualComponentList::const_iterator MultilingualComponentConstIterator;

class DVB_Multilingual_Component_Name_Descriptor: public Descriptor {
protected:
    uint8_t mComponentTag;
    MultilingualComponentList mMultilingualComponentList;

public:
    DVB_Multilingual_Component_Name_Descriptor(const uint8_t * const buffer);
    ~DVB_Multilingual_Component_Name_Descriptor(void);

    uint8_t getComponentTag(void) const
    {
        return mComponentTag;
    }

    const MultilingualComponentList *getMultilingualComponents(void) const
    {
        return &mMultilingualComponentList;
    }

};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_MULTILINGUAL_COMPONENT_NAME_DESCRIPTOR_H_ */

