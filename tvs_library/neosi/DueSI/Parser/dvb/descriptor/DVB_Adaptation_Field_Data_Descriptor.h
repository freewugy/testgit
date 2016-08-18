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

#ifndef DVB_ADAPTATION_FIELD_DATA_DESCRIPTOR_H_
#define DVB_ADAPTATION_FIELD_DATA_DESCRIPTOR_H_



////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class DVB_Adaptation_Field_Data_Descriptor: public Descriptor
{
protected:
    unsigned m_adaptation_field_data_identifier      : 8;

public:
    DVB_Adaptation_Field_Data_Descriptor(const uint8_t* const buffer);
    virtual ~DVB_Adaptation_Field_Data_Descriptor();

    uint8_t getAdaptationFieldDataIdentifier(void) const;
};

////////////////////////////////////////////////////////////////////////////////



#endif /* DVB_ADAPTATION_FIELD_DATA_DESCRIPTOR_H_ */



