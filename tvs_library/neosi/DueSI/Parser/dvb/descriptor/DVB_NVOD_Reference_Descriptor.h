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

#ifndef DVB_NVOD_REFERENCE_DESCRIPTOR_H_
#define DVB_NVOD_REFERENCE_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class NvodReference
{
    protected:
        unsigned m_transport_stream_id          : 16;
        unsigned m_original_network_id          : 16;
        unsigned m_service_id              : 16;

    public:
        NvodReference(const uint8_t * const buffer);

        uint16_t getTransportStreamId(void) const;
        uint16_t getOriginalNetworkId(void) const;
        uint16_t getServiceId(void) const;
};

typedef std::list<NvodReference *> NvodReferenceList;
typedef NvodReferenceList::iterator NvodReferenceIterator;
typedef NvodReferenceList::const_iterator NvodReferenceConstIterator;

class DVB_NVOD_Reference_Descriptor : public Descriptor
{
    protected:
        NvodReferenceList m_nvod_references;

    public:
        DVB_NVOD_Reference_Descriptor(const uint8_t * const buffer);
        ~DVB_NVOD_Reference_Descriptor(void);

        const NvodReferenceList* getNvodReferences(void) const;
};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_NVOD_REFERENCE_DESCRIPTOR_H_ */



