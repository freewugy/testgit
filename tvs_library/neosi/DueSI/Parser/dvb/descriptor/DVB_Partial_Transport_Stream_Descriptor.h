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

#ifndef DVB_PARTIAL_TRANSPORT_STREAM_DESCRIPTOR_H_
#define DVB_PARTIAL_TRANSPORT_STREAM_DESCRIPTOR_H_



////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
struct DVB_Partial_Transport_Stream_Descriptor: public Descriptor
{
protected:
    uint32_t m_peak_rate;
    uint32_t m_minimum_overall_smoothing_rate;
    uint16_t m_maximum_overall_smoothing_rate;

public:
    DVB_Partial_Transport_Stream_Descriptor(const uint8_t* const buffer);
    virtual ~DVB_Partial_Transport_Stream_Descriptor();

    uint32_t getPeakRate() const;
    uint32_t getMinimumOverallSmoothingRate() const;
    uint16_t getMaximumOverallSmoothingRate() const;
};

////////////////////////////////////////////////////////////////////////////////



#endif /* DVB_PARTIAL_TRANSPORT_STREAM_DESCRIPTOR_H_ */



