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

#ifndef MPEG_VIDEOWINDOW_DESCRIPTOR_H_
#define MPEG_VIDEOWINDOW_DESCRIPTOR_H_



////////////////////////////////////////////////////////////////////////////////
#include "BaseDescriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
struct MPEG_VideoWindow_Descriptor: public BaseDescriptor
{
    MPEG_VideoWindow_Descriptor();
	virtual ~MPEG_VideoWindow_Descriptor();
	virtual bool parse(BitStream* bs);

    uint16_t m_horizontal_offset;
    uint16_t m_vertical_offset;
    uint8_t m_window_priority;

};

////////////////////////////////////////////////////////////////////////////////



#endif /* MPEG_VIDEOWINDOW_DESCRIPTOR_H_ */



