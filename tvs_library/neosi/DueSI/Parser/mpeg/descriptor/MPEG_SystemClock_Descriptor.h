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

#ifndef MPEG_SYSTEMCLOCK_DESCRIPTOR_H_
#define MPEG_SYSTEMCLOCK_DESCRIPTOR_H_



////////////////////////////////////////////////////////////////////////////////
#include "BaseDescriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
struct MPEG_SystemClock_Descriptor: public BaseDescriptor
{
    MPEG_SystemClock_Descriptor();
	virtual ~MPEG_SystemClock_Descriptor();
	virtual bool parse(BitStream* bs);

    uint8_t m_external_clock_reference_indicator;
    uint8_t m_clock_accuracy_integer;
    uint8_t m_clock_accuracy_exponent;

};

////////////////////////////////////////////////////////////////////////////////



#endif /* MPEG_SYSTEMCLOCK_DESCRIPTOR_H_ */



