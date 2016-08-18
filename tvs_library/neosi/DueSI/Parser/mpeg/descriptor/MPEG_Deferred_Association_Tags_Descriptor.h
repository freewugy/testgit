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

#ifndef MPEG_DEFERRED_ASSOCIATION_TAGS_DESCRIPTOR_H_
#define MPEG_DEFERRED_ASSOCIATION_TAGS_DESCRIPTOR_H_



////////////////////////////////////////////////////////////////////////////////
#include "BaseDescriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
struct MPEG_Deferred_Association_Tags_Descriptor: public BaseDescriptor
{
    MPEG_Deferred_Association_Tags_Descriptor();
	virtual ~MPEG_Deferred_Association_Tags_Descriptor();
	virtual bool parse(BitStream* bs);
};

////////////////////////////////////////////////////////////////////////////////



#endif /* MPEG_DEFERRED_ASSOCIATION_TAGS_DESCRIPTOR_H_ */



