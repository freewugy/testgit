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

#ifndef DVB_STREAM_IDENTIFIER_DESCRIPTOR_H_
#define DVB_STREAM_IDENTIFIER_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class DVB_Stream_Identifier_Descriptor : public Descriptor
{
    protected:
        unsigned m_component_tag               : 8;

    public:
        DVB_Stream_Identifier_Descriptor(const uint8_t * const buffer);

        uint8_t getComponentTag(void) const;
};

////////////////////////////////////////////////////////////////////////////////



#endif /* DVB_STREAM_IDENTIFIER_DESCRIPTOR_H_ */



