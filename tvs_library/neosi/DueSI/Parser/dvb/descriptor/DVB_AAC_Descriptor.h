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

#ifndef DVB_AAC_DESCRIPTOR_H_
#define DVB_AAC_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

typedef std::vector<uint8_t> AdditionalInfoByteVector;
typedef AdditionalInfoByteVector::iterator AdditionalByteIterator;
typedef AdditionalInfoByteVector::const_iterator AdditionalByteConstIterator;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class DVB_AAC_Descriptor: public Descriptor
{
protected:
    uint8_t m_profile_level;
    uint8_t m_aac_type_flag;

    uint8_t m_aac_type;

    AdditionalInfoByteVector m_additional_info_bytes;

public:
	DVB_AAC_Descriptor(const uint8_t* const buffer);
    virtual ~DVB_AAC_Descriptor();

    uint8_t getProfileLevel() const;
    uint8_t getAACTypeFlag() const;
    uint8_t getAACType() const;

    const AdditionalInfoByteVector* getAdditionalInfoBytes() const;
};

////////////////////////////////////////////////////////////////////////////////



#endif /* DVB_AAC_DESCRIPTOR_H_ */



