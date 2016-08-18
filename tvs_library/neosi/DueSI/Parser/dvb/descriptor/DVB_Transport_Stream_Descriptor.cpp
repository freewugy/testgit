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

#include "DVB_Transport_Stream_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DVB_Transport_Stream_Descriptor::DVB_Transport_Stream_Descriptor(const uint8_t * const buffer) : Descriptor(buffer), m_private_data_bytes(mLength)
{
    memcpy(&m_private_data_bytes[0], &buffer[2], mLength);
}

DVB_Transport_Stream_Descriptor::~DVB_Transport_Stream_Descriptor()
{
}

const PrivateDataByteVector *DVB_Transport_Stream_Descriptor::getPrivateDataBytes(void) const
{
    return &m_private_data_bytes;
}

////////////////////////////////////////////////////////////////////////////////

