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

#ifndef MPEG_MULTIPLEXBUFUTIL_DESCRIPTOR_H_
#define MPEG_MULTIPLEXBUFUTIL_DESCRIPTOR_H_



////////////////////////////////////////////////////////////////////////////////
#include "BaseDescriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
struct MPEG_MultiplexBufUtil_Descriptor: public BaseDescriptor
{
    MPEG_MultiplexBufUtil_Descriptor();
	virtual ~MPEG_MultiplexBufUtil_Descriptor();
	virtual bool parse(BitStream* bs);

    uint8_t m_bound_valid_flag;
    uint16_t m_LTW_offset_lower_bound;
    uint16_t m_LTW_offset_upper_bound;

};

////////////////////////////////////////////////////////////////////////////////



#endif /* MPEG_MULTIPLEXBUFUTIL_DESCRIPTOR_H_ */



