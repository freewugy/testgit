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

#ifndef DVB_MULTILINGUAL_SERVICE_NAME_DESCRIPTOR_H_
#define DVB_MULTILINGUAL_SERVICE_NAME_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class MultilingualServiceName {
protected:
    string m_iso639_language_code;
    uint8_t m_service_provider_name_length;
    string m_service_provider_name;
    uint8_t m_service_name_length;
    string m_service_name;

public:
    MultilingualServiceName(const uint8_t * const buffer);

    const string &getIso639LanguageCode(void) const
    {
        return m_iso639_language_code;
    }

    const string &getServiceProviderName(void) const
    {
        return m_service_provider_name;
    }

    const string &getServiceName(void) const
    {
        return m_service_name;
    }

    friend class DVB_Multilingual_Service_Name_Descriptor;
};

typedef list<MultilingualServiceName *> MultilingualServiceNameList;
typedef MultilingualServiceNameList::iterator MultilingualServiceNameIterator;
typedef MultilingualServiceNameList::const_iterator MultilingualServiceNameConstIterator;

class DVB_Multilingual_Service_Name_Descriptor: public Descriptor {
protected:
    MultilingualServiceNameList mMultilingualServiceNameList;

public:
    DVB_Multilingual_Service_Name_Descriptor(const uint8_t * const buffer);
    ~DVB_Multilingual_Service_Name_Descriptor(void);

    const MultilingualServiceNameList *getMultilingualServiceNames(void) const;
};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_MULTILINGUAL_SERVICE_NAME_DESCRIPTOR_H_ */

