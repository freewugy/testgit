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

#include "DVB_Service_Move_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DVB_Service_Move_Descriptor::DVB_Service_Move_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    ASSERT_MIN_DLEN(6);

    m_new_original_network_id = r16(&buffer[2]);
    m_new_transport_stream_id = r16(&buffer[4]);
    m_new_service_id = r16(&buffer[6]);
}

uint16_t DVB_Service_Move_Descriptor::getNewOriginalNetworkId(void) const
{
    return m_new_original_network_id;
}

uint16_t DVB_Service_Move_Descriptor::getNewTransportStreamId(void) const
{
    return m_new_transport_stream_id;
}

uint16_t DVB_Service_Move_Descriptor::getNewServiceId(void) const
{
    return m_new_service_id;
}

////////////////////////////////////////////////////////////////////////////////

