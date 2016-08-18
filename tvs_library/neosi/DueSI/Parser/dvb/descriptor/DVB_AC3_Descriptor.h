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

#ifndef DVB_AC3_DESCRIPTOR_H_
#define DVB_AC3_DESCRIPTOR_H_



////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"
#include <vector>

using namespace std;

typedef std::vector<uint8_t> AdditionalInfoVector;
typedef AdditionalInfoVector::iterator AdditionalInfoIterator;
typedef AdditionalInfoVector::const_iterator AdditionalInfoConstIterator;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class DVB_AC3_Descriptor: public Descriptor
{
public:
    DVB_AC3_Descriptor(const uint8_t * const buffer);

    uint8_t getAc3TypeFlag(void) const;
    uint8_t getBsidFlag(void) const;
    uint8_t getMainidFlag(void) const;
    uint8_t getAsvcFlag(void) const;
    uint8_t getAc3Type(void) const;
    uint8_t getBsid(void) const;
    uint8_t getMainid(void) const;
    uint8_t getAsvc(void) const;
    const AdditionalInfoVector *getAdditionalInfo(void) const;

protected:
    uint8_t m_ac3_type_flag;
    uint8_t m_bsid_flag;
    uint8_t m_mainid_flag;
    uint8_t m_asvc_flag;
    uint8_t m_ac3_type;
    uint8_t m_bsid;
    uint8_t m_mainid;
    uint8_t m_asvc;
    AdditionalInfoVector m_additional_info;
};

////////////////////////////////////////////////////////////////////////////////



#endif /* DVB_AC3_DESCRIPTOR_H_ */



