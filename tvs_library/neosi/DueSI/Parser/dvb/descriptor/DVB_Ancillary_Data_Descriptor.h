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

#ifndef DVB_ANCILLARY_DATA_DESCRIPTOR_H_
#define DVB_ANCILLARY_DATA_DESCRIPTOR_H_



////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class DVB_Ancillary_Data_Descriptor: public Descriptor
{
protected:
    unsigned m_ancillary_data_identifier        : 8;

public:
    DVB_Ancillary_Data_Descriptor(const uint8_t * const buffer);

    uint8_t getAncillaryDataIdentifier(void) const;

};

////////////////////////////////////////////////////////////////////////////////



#endif /* DVB_ANCILLARY_DATA_DESCRIPTOR_H_ */



