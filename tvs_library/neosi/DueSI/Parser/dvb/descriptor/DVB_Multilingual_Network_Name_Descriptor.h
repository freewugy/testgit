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

#ifndef DVB_MULTILINGUAL_NETWORK_NAME_DESCRIPTOR_H_
#define DVB_MULTILINGUAL_NETWORK_NAME_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class MultilingualNetworkName {
protected:
    string mISO639LanguageCode;
    uint8_t mNetworkNameLength;
    string mNetworkName;

public:
    MultilingualNetworkName(const uint8_t * const buffer);

    const string &getIso639LanguageCode(void) const
    {
        return mISO639LanguageCode;
    }

    const string &getNetworkName(void) const
    {
        return mNetworkName;
    }

};

typedef list<MultilingualNetworkName *> MultilingualNetworkNameList;
typedef MultilingualNetworkNameList::iterator MultilingualNetworkNameIterator;
typedef MultilingualNetworkNameList::const_iterator MultilingualNetworkNameConstIterator;

class DVB_Multilingual_Network_Name_Descriptor: public Descriptor {
protected:
    MultilingualNetworkNameList mMultilingualNetworkNameList;

public:
    DVB_Multilingual_Network_Name_Descriptor(const uint8_t * const buffer);
    ~DVB_Multilingual_Network_Name_Descriptor(void);

    const MultilingualNetworkNameList *getMultilingualNetworkNames(void) const;
};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_MULTILINGUAL_NETWORK_NAME_DESCRIPTOR_H_ */

