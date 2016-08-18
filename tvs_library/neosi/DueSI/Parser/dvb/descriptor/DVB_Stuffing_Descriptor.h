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

#ifndef DVB_STUFFING_DESCRIPTOR_H_
#define DVB_STUFFING_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class DVB_Stuffing_Descriptor : public Descriptor
{
    protected:
        std::list<uint8_t> m_stuffing_byte;

    public:
        DVB_Stuffing_Descriptor(const uint8_t * const buffer);
};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_STUFFING_DESCRIPTOR_H_ */



