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

#include "Descriptor.h"

Descriptor::Descriptor(const uint8_t * const buffer)
{
	mTag = buffer[0];
	mLength = buffer[1];

	mBytes.resize(mLength);
	memcpy(&mBytes[0], &buffer[2], mLength);

	mValid = true;
}

uint8_t Descriptor::writeToBytes(uint8_t * const buffer) const
{
	buffer[0] = mTag;
	buffer[1] = mLength;
	memcpy(&buffer[2], &mBytes[0], mLength);

	return mLength + 2;
}

