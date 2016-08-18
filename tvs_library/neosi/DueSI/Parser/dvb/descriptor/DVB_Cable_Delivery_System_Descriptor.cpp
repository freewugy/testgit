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

#include "DVB_Cable_Delivery_System_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DVB_Cable_Delivery_System_Descriptor::DVB_Cable_Delivery_System_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    ASSERT_MIN_DLEN(11);

    m_frequency =
    (
        ((buffer[2] >> 4)   * 10000000) +
        ((buffer[2] & 0x0F) * 1000000) +
        ((buffer[3] >> 4)   * 100000) +
        ((buffer[3] & 0x0F) * 10000) +
        ((buffer[4] >> 4)   * 1000) +
        ((buffer[4] & 0x0F) * 100) +
        ((buffer[5] >> 4)   * 10) +
        ((buffer[5] & 0x0F) * 1)
    );

    m_fec_outer = buffer[7] & 0x0F;
    m_modulation = buffer[8];

    m_symbol_rate =
    (
        ((buffer[9] >> 4)   * 1000000) +
        ((buffer[9] & 0x0F) * 100000) +
        ((buffer[10] >> 4)  * 10000) +
        ((buffer[10] & 0x0F)    * 1000) +
        ((buffer[11] >> 4)  * 100) +
        ((buffer[11] & 0x0F)    * 10) +
        ((buffer[12] >> 4)  * 1)
    );

    m_fec_inner = buffer[12] & 0x0F;
}

uint32_t DVB_Cable_Delivery_System_Descriptor::getFrequency(void) const
{
    return m_frequency;
}

uint8_t DVB_Cable_Delivery_System_Descriptor::getFecOuter(void) const
{
    return m_fec_outer;
}

uint8_t DVB_Cable_Delivery_System_Descriptor::getModulation(void) const
{
    return m_modulation;
}

uint32_t DVB_Cable_Delivery_System_Descriptor::getSymbolRate(void) const
{
    return m_symbol_rate;
}

uint8_t DVB_Cable_Delivery_System_Descriptor::getFecInner(void) const
{
    return m_fec_inner;
}

////////////////////////////////////////////////////////////////////////////////

