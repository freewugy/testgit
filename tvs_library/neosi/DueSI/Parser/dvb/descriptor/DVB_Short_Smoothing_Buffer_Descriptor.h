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

#ifndef DVB_SHORT_SMOOTHING_BUFFER_DESCRIPTOR_H_
#define DVB_SHORT_SMOOTHING_BUFFER_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef std::vector<uint8_t> PrivateDataByteVector;
typedef PrivateDataByteVector::iterator PrivateDataByteIterator;
typedef PrivateDataByteVector::const_iterator PrivateDataByteConstIterator;

class DVB_Short_Smoothing_Buffer_Descriptor : public Descriptor
{
    protected:
        unsigned m_sb_size         : 2;
        unsigned m_sb_leak_rate     : 6;
        PrivateDataByteVector m_private_data_bytes;

    public:
        DVB_Short_Smoothing_Buffer_Descriptor(const uint8_t* const buffer);
        virtual ~DVB_Short_Smoothing_Buffer_Descriptor();

        uint8_t getSbSize(void) const;
        uint8_t getSbLeakRate(void) const;
        const PrivateDataByteVector *getPrivateDataBytes(void) const;
};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_SHORT_SMOOTHING_BUFFER_DESCRIPTOR_H_ */



