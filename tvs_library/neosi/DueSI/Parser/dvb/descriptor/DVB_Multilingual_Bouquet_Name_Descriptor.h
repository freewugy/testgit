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

#ifndef DVB_MULTILINGUAL_BOUQUET_NAME_DESCRIPTOR_H_
#define DVB_MULTILINGUAL_BOUQUET_NAME_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class MultilingualBouquetName {
protected:
    string mISO639LanguageCode;
    uint8_t mBouquetNameLength;
    string mBouquetName;

public:
    MultilingualBouquetName(const uint8_t * const buffer);

    const string &getISO639LanguageCode(void) const
    {
        return mISO639LanguageCode;
    }

    const string &getBouquetName(void) const
    {
        return mBouquetName;
    }

};

typedef list<MultilingualBouquetName *> MultilingualBouquetNameList;
typedef MultilingualBouquetNameList::iterator MultilingualBouquetNameIterator;
typedef MultilingualBouquetNameList::const_iterator MultilingualBouquetNameConstIterator;

class DVB_Multilingual_Bouquet_Name_Descriptor: public Descriptor {
protected:
    MultilingualBouquetNameList mListMultilingualBouquetName;

public:
    DVB_Multilingual_Bouquet_Name_Descriptor(const uint8_t * const buffer);
    ~DVB_Multilingual_Bouquet_Name_Descriptor(void);

    const MultilingualBouquetNameList *getMultilingualBouquetNameList(void) const;
};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_MULTILINGUAL_BOUQUET_NAME_DESCRIPTOR_H_ */

