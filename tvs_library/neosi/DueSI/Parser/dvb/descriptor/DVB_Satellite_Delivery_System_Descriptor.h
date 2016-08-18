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

#ifndef DVB_SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR_H_
#define DVB_SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class DVB_Satellite_Delivery_System_Descriptor : public Descriptor
{
    protected:
        unsigned m_frequency              : 32;
        unsigned m_orbital_position            : 16;
        unsigned m_west_east_flag               : 1;
        unsigned m_polarization               : 2;
        unsigned m_roll_off                : 2;
        unsigned m_modulation_system           : 1;
        unsigned m_modulation_type             : 2;
        unsigned m_symbol_rate             : 28;
        unsigned m_fec_inner               : 4;

    public:
        DVB_Satellite_Delivery_System_Descriptor(const uint8_t * const buffer);

        uint32_t getFrequency(void) const;
        uint16_t getOrbitalPosition(void) const;
        uint8_t getWestEastFlag(void) const;
        uint8_t getPolarization(void) const;
        uint8_t getRollOff(void) const;
        uint8_t getModulationSystem(void) const;
        uint8_t getModulation(void) const;
        uint32_t getSymbolRate(void) const;
        uint8_t getFecInner(void) const;
};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR_H_ */



