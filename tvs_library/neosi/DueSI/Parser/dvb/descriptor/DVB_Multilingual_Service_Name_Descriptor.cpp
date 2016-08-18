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

#include "DVB_Multilingual_Service_Name_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MultilingualServiceName::MultilingualServiceName(const uint8_t * const buffer)
{
    m_iso639_language_code.assign((char *)&buffer[0], 3);
    m_service_provider_name_length = buffer[3];
    m_service_provider_name.assign((char *)&buffer[4], m_service_provider_name_length);
    m_service_name_length = buffer[m_service_provider_name_length + 4];
    m_service_name.assign((char *)&buffer[m_service_provider_name_length + 5], m_service_name_length);
}

DVB_Multilingual_Service_Name_Descriptor::DVB_Multilingual_Service_Name_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    MultilingualServiceName *name;

    for (size_t i = 0; i < mLength; i += name->m_service_provider_name_length + name->m_service_name_length + 5) {
        name = new MultilingualServiceName(&buffer[i + 2]);
        mMultilingualServiceNameList.push_back(name);
    }
}

DVB_Multilingual_Service_Name_Descriptor::~DVB_Multilingual_Service_Name_Descriptor(void)
{
    for (MultilingualServiceNameIterator i = mMultilingualServiceNameList.begin(); i != mMultilingualServiceNameList.end(); ++i)
        delete *i;
}

const MultilingualServiceNameList *DVB_Multilingual_Service_Name_Descriptor::getMultilingualServiceNames(void) const
{
    return &mMultilingualServiceNameList;
}

////////////////////////////////////////////////////////////////////////////////

