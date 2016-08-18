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

#include "DVB_Service_List_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
ServiceListItem::ServiceListItem(const uint8_t * const buffer)
{
    m_service_id = r16(&buffer[0]);
    m_service_type = buffer[2];
}

uint16_t ServiceListItem::getServiceId(void) const
{
    return m_service_id;
}

uint8_t ServiceListItem::getServiceType(void) const
{
    return m_service_type;
}

DVB_Service_List_Descriptor::DVB_Service_List_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    for (size_t i = 0; i < mLength; i += 3) {
        ASSERT_MIN_DLEN(i + 3);
        m_service_list.push_back(new ServiceListItem(&buffer[i + 2]));
    }
}

DVB_Service_List_Descriptor::~DVB_Service_List_Descriptor(void)
{
    for (ServiceListItemIterator i = m_service_list.begin(); i != m_service_list.end(); ++i)
        delete *i;
}

const ServiceListItemList *DVB_Service_List_Descriptor::getServiceList(void) const
{
    return &m_service_list;
}

////////////////////////////////////////////////////////////////////////////////

