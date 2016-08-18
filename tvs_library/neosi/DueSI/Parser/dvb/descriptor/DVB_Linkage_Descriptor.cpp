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

#include "DVB_Linkage_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DVB_Linkage_Descriptor::DVB_Linkage_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    size_t headerLength = 7;
    ASSERT_MIN_DLEN(headerLength);

    m_transport_stream_id = r16(&buffer[2]);
    m_original_network_id = r16(&buffer[4]);
    m_service_id = r16(&buffer[6]);
    m_linkage_type = buffer[8];

    if (m_linkage_type != 0x08) {
        m_private_data_bytes.resize(mLength - headerLength);
        memcpy(&m_private_data_bytes[0], &buffer[9], mLength - headerLength);
    } else {
        uint8_t offset = 0;

        headerLength++;
        ASSERT_MIN_DLEN(headerLength);

        m_hand_over_type = (buffer[9] >> 4) & 0x0f;
        m_origin_type = buffer[9] & 0x01;

        if ((m_hand_over_type >= 0x01) && (m_hand_over_type <= 0x03)) {
            headerLength += 2;
            ASSERT_MIN_DLEN(headerLength);

            m_network_id = r16(&buffer[10]);
            offset += 2;
        }

        if (m_origin_type == 0x00) {
            headerLength += 2;
            ASSERT_MIN_DLEN(headerLength);

            m_initial_service_id = r16(&buffer[offset + 10]);
            offset += 2;
        }

        m_private_data_bytes.resize(mLength - headerLength);
        memcpy(&m_private_data_bytes[0], &buffer[offset + 10], mLength - headerLength);
    }
}

uint16_t DVB_Linkage_Descriptor::getTransportStreamId(void) const
{
    return m_transport_stream_id;
}

uint16_t DVB_Linkage_Descriptor::getOriginalNetworkId(void) const
{
    return m_original_network_id;
}

uint16_t DVB_Linkage_Descriptor::getServiceId(void) const
{
    return m_service_id;
}

uint8_t DVB_Linkage_Descriptor::getLinkageType(void) const
{
    return m_linkage_type;
}

const PrivateDataByteVector *DVB_Linkage_Descriptor::getPrivateDataBytes(void) const
{
    return &m_private_data_bytes;
}

uint8_t DVB_Linkage_Descriptor::getHandOverType(void) const
{
    return m_hand_over_type;
}

uint8_t DVB_Linkage_Descriptor::getOriginType(void) const
{
    return m_origin_type;
}

uint16_t DVB_Linkage_Descriptor::getNetworkId(void) const
{
    return m_network_id;
}

uint16_t DVB_Linkage_Descriptor::getInitialServiceId(void) const
{
    return m_initial_service_id;
}

////////////////////////////////////////////////////////////////////////////////

