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

#ifndef DVB_SERVICE_MOVE_DESCRIPTOR_H_
#define DVB_SERVICE_MOVE_DESCRIPTOR_H_



////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class DVB_Service_Move_Descriptor : public Descriptor
{
    protected:
        unsigned m_new_original_network_id           : 16;
        unsigned m_new_transport_stream_id           : 16;
        unsigned m_new_service_id               : 16;

    public:
        DVB_Service_Move_Descriptor(const uint8_t * const buffer);

        uint16_t getNewOriginalNetworkId(void) const;
        uint16_t getNewTransportStreamId(void) const;
        uint16_t getNewServiceId(void) const;
};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_SERVICE_MOVE_DESCRIPTOR_H_ */



