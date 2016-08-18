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

#ifndef DVB_TIME_SLICE_FEC_IDENTIFIER_DESCRIPTOR_H_
#define DVB_TIME_SLICE_FEC_IDENTIFIER_DESCRIPTOR_H_



////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef std::vector<uint8_t> TimeSliceFecIdentifierByteVector;
typedef TimeSliceFecIdentifierByteVector::iterator TimeSliceFecIdentifierByteIterator;
typedef TimeSliceFecIdentifierByteVector::const_iterator TimeSliceFecIdentifierByteConstIterator;

class DVB_Time_Slice_FEC_Identifier_Descriptor : public Descriptor
{
    protected:
        unsigned m_time_slicing            : 1;
        unsigned m_mpe_fec             : 2;
        // 2 bits reserved
        unsigned m_frame_size          : 3;
        unsigned m_max_burst_duration       : 8;
        unsigned m_max_average_rate         : 4;
        unsigned m_time_slice_fec_id         : 4;

        TimeSliceFecIdentifierByteVector m_id_selector_bytes;

    public:
        DVB_Time_Slice_FEC_Identifier_Descriptor(const uint8_t* const buffer);
        virtual ~DVB_Time_Slice_FEC_Identifier_Descriptor();

        uint8_t getTimeSlicing() const;
        uint8_t getMpeFec() const;
        uint8_t getFrameSize() const;
        uint8_t getMaxBurstDuration() const;
        uint8_t getMaxAverageRate() const;
        uint8_t getTimeSliceFecId() const;

        const TimeSliceFecIdentifierByteVector* getIdSelectorBytes() const;
};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_TIME_SLICE_FEC_IDENTIFIER_DESCRIPTOR_H_ */



