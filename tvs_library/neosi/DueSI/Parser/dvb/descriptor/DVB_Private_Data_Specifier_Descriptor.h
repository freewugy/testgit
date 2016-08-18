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

#ifndef DVB_PRIVATE_DATA_SPECIFIER_DESCRIPTOR_H_
#define DVB_PRIVATE_DATA_SPECIFIER_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class DVB_Private_Data_Specifier_Descriptor : public Descriptor
{
    protected:
        unsigned m_private_data_specifier           : 32;

    public:
        DVB_Private_Data_Specifier_Descriptor(const uint8_t * const buffer);

        uint32_t getPrivateDataSpecifier(void) const;
};

////////////////////////////////////////////////////////////////////////////////



#endif /* DVB_PRIVATE_DATA_SPECIFIER_DESCRIPTOR_H_ */



