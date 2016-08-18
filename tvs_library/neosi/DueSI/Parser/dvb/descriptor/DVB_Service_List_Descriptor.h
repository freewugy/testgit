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

#ifndef DVB_SERVICE_LIST_DESCRIPTOR_H_
#define DVB_SERVICE_LIST_DESCRIPTOR_H_



////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class ServiceListItem
{
    protected:
        unsigned m_service_id              : 16;
        unsigned m_service_type                : 8;

    public:
        ServiceListItem(const uint8_t * const buffer);

        uint16_t getServiceId(void) const;
        uint8_t getServiceType(void) const;
};

typedef std::list<ServiceListItem *> ServiceListItemList;
typedef ServiceListItemList::iterator ServiceListItemIterator;
typedef ServiceListItemList::const_iterator ServiceListItemConstIterator;

class DVB_Service_List_Descriptor : public Descriptor
{
    protected:
        ServiceListItemList m_service_list;

    public:
        DVB_Service_List_Descriptor(const uint8_t * const buffer);
        ~DVB_Service_List_Descriptor(void);

        const ServiceListItemList *getServiceList(void) const;
};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_SERVICE_LIST_DESCRIPTOR_H_ */



