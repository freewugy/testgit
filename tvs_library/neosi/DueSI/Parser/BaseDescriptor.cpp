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

#include "BaseDescriptor.h"
#include "BitStream.h"
#include "Logger.h"

static const char* TAG = "BaseDescriptor";

////////////////////////////////////////////////////////////////////////////////
BaseDescriptor::BaseDescriptor()
	: m_descriptor_tag(0)
	, m_descriptor_length(0)
{
}



BaseDescriptor::~BaseDescriptor()
{
}



////////////////////////////////////////////////////////////////////////////////
bool BaseDescriptor::parse(BitStream* bs)
{
	if (
            (false == bs->readBits(8, &m_descriptor_tag)) ||
            (false == bs->readBits(8, &m_descriptor_length)) ||
			(m_descriptor_length >= bs->getRemainBytes())
		)
	{
        L_ERROR(TAG, "parse(), failed\n");
		return false;
	}
    return true;
}



////////////////////////////////////////////////////////////////////////////////



