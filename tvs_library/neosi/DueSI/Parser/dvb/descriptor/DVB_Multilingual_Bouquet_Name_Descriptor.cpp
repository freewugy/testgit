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

#include "DVB_Multilingual_Bouquet_Name_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MultilingualBouquetName::MultilingualBouquetName(const uint8_t * const buffer)
{
    mISO639LanguageCode.assign((char *)&buffer[0], 3);
    mBouquetNameLength = buffer[3];
    mBouquetName.assign((char *)&buffer[4], mBouquetNameLength);
}

DVB_Multilingual_Bouquet_Name_Descriptor::DVB_Multilingual_Bouquet_Name_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    for (size_t i = 0; i < mLength; i += buffer[i + 5] + 4)
        mListMultilingualBouquetName.push_back(new MultilingualBouquetName(&buffer[i + 2]));
}

DVB_Multilingual_Bouquet_Name_Descriptor::~DVB_Multilingual_Bouquet_Name_Descriptor(void)
{
    for (MultilingualBouquetNameIterator i = mListMultilingualBouquetName.begin(); i != mListMultilingualBouquetName.end(); ++i)
        delete *i;
}

const MultilingualBouquetNameList *DVB_Multilingual_Bouquet_Name_Descriptor::getMultilingualBouquetNameList(void) const
{
    return &mListMultilingualBouquetName;
}

////////////////////////////////////////////////////////////////////////////////

