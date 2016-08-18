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

#ifndef MPEG_MPEG4_VIDEO_DESCRIPTOR_H_
#define MPEG_MPEG4_VIDEO_DESCRIPTOR_H_



////////////////////////////////////////////////////////////////////////////////
#include "BaseDescriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
struct MPEG_MPEG4_Video_Descriptor: public BaseDescriptor
{
    MPEG_MPEG4_Video_Descriptor();
	virtual ~MPEG_MPEG4_Video_Descriptor();
	virtual bool parse(BitStream* bs);

	uint8_t m_MPEG4_visual_profile_and_level;
};

////////////////////////////////////////////////////////////////////////////////



#endif /* MPEG_MPEG4_VIDEO_DESCRIPTOR_H_ */



