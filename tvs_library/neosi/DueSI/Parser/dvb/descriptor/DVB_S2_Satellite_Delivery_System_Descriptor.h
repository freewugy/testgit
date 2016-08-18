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

#ifndef DVB_S2_SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR_H_
#define DVB_S2_SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class DVB_S2_Satellite_Delivery_System_Descriptor : public Descriptor
{
    protected:
        unsigned m_scrambling_sequence_selector  : 1;
        unsigned m_multiple_input_stream_flag     : 1;
        unsigned m_backwards_compatibility_indicator : 1;
        unsigned m_scrambling_sequence_index     :18;
        unsigned m_input_stream_identifier       : 8;

    public:
        DVB_S2_Satellite_Delivery_System_Descriptor(const uint8_t* const buffer);
        virtual ~DVB_S2_Satellite_Delivery_System_Descriptor();

        uint8_t getScramblingSequenceSelector() const;
        uint8_t getMultipleInputStreamFlag() const;
        uint8_t getBackwardsCompatibilityIndicator() const;
        uint32_t getScramblingSequenceIndex() const;
        uint8_t getInputStreamIdentifier() const;
};


////////////////////////////////////////////////////////////////////////////////



#endif /* DVB_S2_SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR_H_ */



