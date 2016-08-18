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

#ifndef DVB_DTS_DESCRIPTOR_H_
#define DVB_DTS_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef std::vector<uint8_t> AdditionalInfoByteVector;
typedef AdditionalInfoByteVector::iterator AdditionalByteIterator;
typedef AdditionalInfoByteVector::const_iterator AdditionalByteConstIterator;

class DVB_DTS_Descriptor : public Descriptor
{
    protected:
        unsigned m_sample_rate     : 4;
        unsigned m_bit_rate        : 6;
        unsigned m_number_of_blocks     : 7;
        unsigned m_frame_size      :14;
        unsigned m_surround_mode       : 6;
        unsigned m_lfe_flag        : 1;
        unsigned m_extended_surround_flag   : 2;

        AdditionalInfoByteVector m_additional_info_bytes;
    public:
        DVB_DTS_Descriptor(const uint8_t* const buffer);
        virtual ~DVB_DTS_Descriptor();

        uint8_t getSampleRate() const;
        uint8_t getBitRate() const;
        uint8_t getNumberOfBlocks() const;
        uint16_t getFrameSize() const;
        uint8_t getLfeFlag() const;
        uint8_t getExtendedSurroundFlag() const;

        const AdditionalInfoByteVector* getAdditionalInfoBytes() const;
};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_DTS_DESCRIPTOR_H_ */



