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

#ifndef MPEG_VIDEOSTREAM_DESCRIPTOR_H_
#define MPEG_VIDEOSTREAM_DESCRIPTOR_H_



////////////////////////////////////////////////////////////////////////////////
#include "BaseDescriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
struct MPEG_VideoStream_Descriptor: public BaseDescriptor
{
    MPEG_VideoStream_Descriptor();
	virtual ~MPEG_VideoStream_Descriptor();
	virtual bool parse(BitStream* bs);

    uint8_t m_multiple_frame_rate_flag;
    uint8_t m_frame_rate_code;
    uint8_t m_MPEG_1_only_flag;
    uint8_t m_constrained_parameter_flag;
    uint8_t m_still_picture_flag;
    uint8_t m_profile_and_level_indication;
    uint8_t m_chroma_format;
    uint8_t m_frame_rate_extension_flag;

};

////////////////////////////////////////////////////////////////////////////////



#endif /* MPEG_VIDEOSTREAM_DESCRIPTOR_H_ */



