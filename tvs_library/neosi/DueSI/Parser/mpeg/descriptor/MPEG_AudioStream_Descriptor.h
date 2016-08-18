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

#ifndef MPEG_AUDIOSTREAM_DESCRIPTOR_H_
#define MPEG_AUDIOSTREAM_DESCRIPTOR_H_



////////////////////////////////////////////////////////////////////////////////
#include "BaseDescriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
struct MPEG_AudioStream_Descriptor: public BaseDescriptor
{
    MPEG_AudioStream_Descriptor();
	virtual ~MPEG_AudioStream_Descriptor();
	virtual bool parse(BitStream* bs);
    uint8_t m_byte_data;
    uint8_t m_free_format_flag;
    uint8_t m_ID;
    uint8_t m_layer;
    uint8_t m_variable_rate_audio_indicator;
};

////////////////////////////////////////////////////////////////////////////////



#endif /* MPEG_AUDIOSTREAM_DESCRIPTOR_H_ */



