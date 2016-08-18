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

#include "DVB_Multilingual_Network_Name_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MultilingualNetworkName::MultilingualNetworkName(const uint8_t * const buffer)
{
    mISO639LanguageCode.assign((char *)&buffer[0], 3);
    mNetworkNameLength = buffer[3];
    mNetworkName.assign((char *)&buffer[4], mNetworkNameLength);
}

DVB_Multilingual_Network_Name_Descriptor::DVB_Multilingual_Network_Name_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    for (size_t i = 0; i < mLength; i += buffer[i + 5] + 4)
        mMultilingualNetworkNameList.push_back(new MultilingualNetworkName(&buffer[i + 2]));
}

DVB_Multilingual_Network_Name_Descriptor::~DVB_Multilingual_Network_Name_Descriptor(void)
{
    for (MultilingualNetworkNameIterator i = mMultilingualNetworkNameList.begin(); i != mMultilingualNetworkNameList.end(); ++i)
        delete *i;
}

const MultilingualNetworkNameList *DVB_Multilingual_Network_Name_Descriptor::getMultilingualNetworkNames(void) const
{
    return &mMultilingualNetworkNameList;
}

////////////////////////////////////////////////////////////////////////////////

