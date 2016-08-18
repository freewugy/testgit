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

#ifndef DVB_ECM_REPETITION_RATE_DESCRIPTOR_H_
#define DVB_ECM_REPETITION_RATE_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef std::vector<uint8_t> ECMRepetitionPrivateByteVector;
typedef ECMRepetitionPrivateByteVector::iterator ECMRepetitionPrivateByteIterator;
typedef ECMRepetitionPrivateByteVector::const_iterator ECMRepetitionPrivateByteConstIterator;

class DVB_ECM_Repetition_Rate_Descriptor : public Descriptor
{
    protected:
        unsigned m_ca_system_id         :16;
        unsigned m_repetition_rate         :16;

        ECMRepetitionPrivateByteVector m_private_data_bytes;

    public:
        DVB_ECM_Repetition_Rate_Descriptor(const uint8_t* const buffer);
        virtual ~DVB_ECM_Repetition_Rate_Descriptor();

        uint16_t getCaSystemId(void) const;
        uint16_t getRepetitionRate(void) const;

        const ECMRepetitionPrivateByteVector* getPrivateDataBytes() const;
};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_ECM_REPETITION_RATE_DESCRIPTOR_H_ */



