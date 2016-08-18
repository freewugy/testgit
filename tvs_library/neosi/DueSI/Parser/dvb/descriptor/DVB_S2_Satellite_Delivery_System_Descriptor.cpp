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

#include "DVB_S2_Satellite_Delivery_System_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DVB_S2_Satellite_Delivery_System_Descriptor::DVB_S2_Satellite_Delivery_System_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    size_t headerLength = 1;
    ASSERT_MIN_DLEN(headerLength);

    m_scrambling_sequence_selector = (buffer[2] >> 7) & 0x01;
    m_multiple_input_stream_flag = (buffer[2] >> 6) & 0x01;
    m_backwards_compatibility_indicator = (buffer[2] >> 5) & 0x01;

    size_t i = 3;
    if (m_scrambling_sequence_selector == 1) {
        headerLength += 3;
        ASSERT_MIN_DLEN(headerLength);

        m_scrambling_sequence_index = (buffer[i++] & 0x3) << 16;
        m_scrambling_sequence_index |= buffer[i++] << 8;
        m_scrambling_sequence_index |= buffer[i++];
    } else {
        m_scrambling_sequence_index = 0;
    }

    if (m_multiple_input_stream_flag == 1) {
        headerLength++;
        ASSERT_MIN_DLEN(headerLength);

        m_input_stream_identifier = buffer[i];
    } else {
        m_input_stream_identifier = 0;
    }
}

DVB_S2_Satellite_Delivery_System_Descriptor::~DVB_S2_Satellite_Delivery_System_Descriptor()
{
}

uint8_t DVB_S2_Satellite_Delivery_System_Descriptor::getScramblingSequenceSelector() const
{
    return m_scrambling_sequence_selector;
}

uint8_t DVB_S2_Satellite_Delivery_System_Descriptor::getMultipleInputStreamFlag() const
{
    return m_multiple_input_stream_flag;
}

uint8_t DVB_S2_Satellite_Delivery_System_Descriptor::getBackwardsCompatibilityIndicator() const
{
    return m_backwards_compatibility_indicator;
}

uint32_t DVB_S2_Satellite_Delivery_System_Descriptor::getScramblingSequenceIndex() const
{
    return m_scrambling_sequence_index;
}

uint8_t DVB_S2_Satellite_Delivery_System_Descriptor::getInputStreamIdentifier() const
{
    return m_input_stream_identifier;
}

////////////////////////////////////////////////////////////////////////////////

