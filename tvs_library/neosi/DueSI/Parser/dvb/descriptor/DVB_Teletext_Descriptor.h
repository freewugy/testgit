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

#ifndef DVB_TELETEXT_DESCRIPTOR_H_
#define DVB_TELETEXT_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"
#include "DVB_VBI_Teletext_Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class DVB_Teletext_Descriptor : public DVB_VBI_Teletext_Descriptor
{
    public:
    DVB_Teletext_Descriptor(const uint8_t * const buffer);
};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_TELETEXT_DESCRIPTOR_H_ */



