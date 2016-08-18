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

#ifndef MPEG_HIERARCHY_DESCRIPTOR_H_
#define MPEG_HIERARCHY_DESCRIPTOR_H_



////////////////////////////////////////////////////////////////////////////////
#include "BaseDescriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
struct MPEG_Hierarchy_Descriptor: public BaseDescriptor
{
    MPEG_Hierarchy_Descriptor();
	virtual ~MPEG_Hierarchy_Descriptor();
	virtual bool parse(BitStream* bs);

    uint8_t m_hierarchy_type;
    uint8_t m_hierarchy_layer_index;
    uint8_t m_hierarchy_embedded_layer_index;
    uint8_t m_hierarchy_channel;

};

////////////////////////////////////////////////////////////////////////////////



#endif /* MPEG_AUDIOSTREAM_DESCRIPTOR_H_ */



