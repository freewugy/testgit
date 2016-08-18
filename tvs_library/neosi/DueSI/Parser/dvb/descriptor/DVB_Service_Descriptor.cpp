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

#include "DVB_Service_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DVB_Service_Descriptor::DVB_Service_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    size_t headerLength = 3;
    ASSERT_MIN_DLEN(headerLength);

    m_service_type = buffer[2];
    m_service_provider_name_length = buffer[3];

    headerLength += m_service_provider_name_length;
    ASSERT_MIN_DLEN(headerLength);

    m_service_provider_name.assign((char *)&buffer[4], m_service_provider_name_length);
    m_service_name_length = buffer[m_service_provider_name_length + 4];

    headerLength += m_service_name_length;
    ASSERT_MIN_DLEN(headerLength);

    m_service_name.assign((char *)&buffer[m_service_provider_name_length + 5], m_service_name_length);
}

uint8_t DVB_Service_Descriptor::getServiceType(void) const
{
    return m_service_type;
}

const std::string &DVB_Service_Descriptor::getServiceProviderName(void) const
{
    return m_service_provider_name;
}

const std::string &DVB_Service_Descriptor::getServiceName(void) const
{
    return m_service_name;
}

////////////////////////////////////////////////////////////////////////////////

