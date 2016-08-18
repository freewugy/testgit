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

#ifndef MPEG_FMC_DESCRIPTOR_H_
#define MPEG_FMC_DESCRIPTOR_H_



////////////////////////////////////////////////////////////////////////////////
#include "BaseDescriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef struct _FMC {
    uint16_t m_ES_ID;
    uint8_t m_FlexMuxChannel;
} FMC;

struct MPEG_FMC_Descriptor: public BaseDescriptor
{
    MPEG_FMC_Descriptor();
	virtual ~MPEG_FMC_Descriptor();
	virtual bool parse(BitStream* bs);

	vector<FMC> m_FMC_List;
};

////////////////////////////////////////////////////////////////////////////////



#endif /* MPEG_FMC_DESCRIPTOR_H_ */



