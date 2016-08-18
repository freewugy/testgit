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

#ifndef DVB_SERVICE_DESCRIPTOR_H_
#define DVB_SERVICE_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class DVB_Service_Descriptor : public Descriptor
{
    protected:
        unsigned m_service_type                : 8;
        unsigned m_service_provider_name_length      : 8;
        std::string m_service_provider_name;
        unsigned m_service_name_length          : 8;
        std::string m_service_name;

    public:
        DVB_Service_Descriptor(const uint8_t * const buffer);

        uint8_t getServiceType(void) const;
        const std::string &getServiceProviderName(void) const;
        const std::string &getServiceName(void) const;
};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_SERVICE_DESCRIPTOR_H_ */



