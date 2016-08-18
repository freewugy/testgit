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

#include "DVB_Multilingual_Component_Name_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MultilingualComponent::MultilingualComponent(const uint8_t * const buffer)
{
    mISO639LanguageCode.assign((char *)&buffer[0], 3);
    mTextDescriptionLength = buffer[3];
    mText.assign((char *)&buffer[4], mTextDescriptionLength);
}

DVB_Multilingual_Component_Name_Descriptor::DVB_Multilingual_Component_Name_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    mComponentTag = buffer[2];

    for (size_t i = 0; i < mLength - 1; i += buffer[i + 6] + 4)
        mMultilingualComponentList.push_back(new MultilingualComponent(&buffer[i + 3]));
}

DVB_Multilingual_Component_Name_Descriptor::~DVB_Multilingual_Component_Name_Descriptor(void)
{
    for (MultilingualComponentIterator i = mMultilingualComponentList.begin(); i != mMultilingualComponentList.end(); ++i)
        delete *i;
}

////////////////////////////////////////////////////////////////////////////////

