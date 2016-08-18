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

#include "DVB_DSNG_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DVB_DSNG_Descriptor::DVB_DSNG_Descriptor(const uint8_t * const buffer) : Descriptor(buffer), m_private_data_bytes(mLength)
{
    memcpy(&m_private_data_bytes[0], &buffer[2], mLength);
}

DVB_DSNG_Descriptor::~DVB_DSNG_Descriptor()
{
}

const PrivateDataByteVector *DVB_DSNG_Descriptor::getPrivateDataBytes(void) const
{
    return &m_private_data_bytes;
}

////////////////////////////////////////////////////////////////////////////////

