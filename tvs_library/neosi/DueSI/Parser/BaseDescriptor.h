/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: eschoi $
 * $LastChangedDate: 2014-06-03 13:58:35 +0900 (화, 03 6월 2014) $
 * $LastChangedRevision: 837 $
 * Description:
 * Note:
 *****************************************************************************/

#ifndef BASEDESCRIPTOR_H_
#define BASEDESCRIPTOR_H_



////////////////////////////////////////////////////////////////////////////////
#include <unistd.h>
#include <stdint.h>
#include <vector>
#include "Uncopyable.h"



////////////////////////////////////////////////////////////////////////////////
class BitStream;



////////////////////////////////////////////////////////////////////////////////
struct BaseDescriptor: private Uncopyable
{
	BaseDescriptor();
	virtual ~BaseDescriptor();
	virtual bool parse(BitStream* bs);

	uint8_t m_descriptor_tag;
	uint8_t m_descriptor_length;
};



////////////////////////////////////////////////////////////////////////////////



#endif /* BASEDESCRIPTOR_H_ */



