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

#include "DVB_Satellite_Delivery_System_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DVB_Satellite_Delivery_System_Descriptor::DVB_Satellite_Delivery_System_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
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

    m_orbital_position = r16(&buffer[6]);
    m_west_east_flag = (buffer[8] >> 7) & 0x01;
    m_polarization = (buffer[8] >> 5) & 0x03;
    m_roll_off = (buffer[8] >> 3) & 0x03;
    m_modulation_system = (buffer[8] >> 2) & 0x01;
    m_modulation_type = (buffer[8]) & 0x03;

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

uint32_t DVB_Satellite_Delivery_System_Descriptor::getFrequency(void) const
{
    return m_frequency;
}

uint16_t DVB_Satellite_Delivery_System_Descriptor::getOrbitalPosition(void) const
{
    return m_orbital_position;
}

uint8_t DVB_Satellite_Delivery_System_Descriptor::getWestEastFlag(void) const
{
    return m_west_east_flag;
}

uint8_t DVB_Satellite_Delivery_System_Descriptor::getPolarization(void) const
{
    return m_polarization;
}

uint8_t DVB_Satellite_Delivery_System_Descriptor::getModulation(void) const
{
    return m_modulation_type;
}

uint8_t DVB_Satellite_Delivery_System_Descriptor::getRollOff(void) const
{
    return m_roll_off;
}

uint8_t DVB_Satellite_Delivery_System_Descriptor::getModulationSystem(void) const
{
    return m_modulation_system;
}

uint32_t DVB_Satellite_Delivery_System_Descriptor::getSymbolRate(void) const
{
    return m_symbol_rate;
}

uint8_t DVB_Satellite_Delivery_System_Descriptor::getFecInner(void) const
{
    return m_fec_inner;
}

////////////////////////////////////////////////////////////////////////////////

