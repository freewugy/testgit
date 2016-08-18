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

#ifndef DVB_LINKAGE_DESCRIPTOR_H_
#define DVB_LINKAGE_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef std::vector<uint8_t> PrivateDataByteVector;
typedef PrivateDataByteVector::iterator PrivateDataByteIterator;
typedef PrivateDataByteVector::const_iterator PrivateDataByteConstIterator;

class DVB_Linkage_Descriptor : public Descriptor
{
    protected:
        unsigned m_transport_stream_id          : 16;
        unsigned m_original_network_id          : 16;
        unsigned m_service_id              : 16;
        unsigned m_linkage_type                : 8;
        PrivateDataByteVector m_private_data_bytes;
        unsigned m_hand_over_type               : 4;
        unsigned m_origin_type             : 1;
        unsigned m_network_id              : 16;
        unsigned m_initial_service_id           : 16;

    public:
        DVB_Linkage_Descriptor(const uint8_t * const buffer);

        uint16_t getTransportStreamId(void) const;
        uint16_t getOriginalNetworkId(void) const;
        uint16_t getServiceId(void) const;
        uint8_t getLinkageType(void) const;
        const PrivateDataByteVector *getPrivateDataBytes(void) const;
        uint8_t getHandOverType(void) const;
        uint8_t getOriginType(void) const;
        uint16_t getNetworkId(void) const;
        uint16_t getInitialServiceId(void) const;
};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_LINKAGE_DESCRIPTOR_H_ */



