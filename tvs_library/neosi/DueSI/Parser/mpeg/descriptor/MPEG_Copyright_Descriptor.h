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

#ifndef MPEG_COPYRIGHT_DESCRIPTOR_H_
#define MPEG_COPYRIGHT_DESCRIPTOR_H_



////////////////////////////////////////////////////////////////////////////////
#include "BaseDescriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
struct MPEG_Copyright_Descriptor: public BaseDescriptor
{
    MPEG_Copyright_Descriptor();
	virtual ~MPEG_Copyright_Descriptor();
	virtual bool parse(BitStream* bs);

    uint32_t m_Copyright_identifier;
    vector<uint8_t> m_additional_copyright_info_data;
};

////////////////////////////////////////////////////////////////////////////////



#endif /* MPEG_COPYRIGHT_DESCRIPTOR_H_ */



