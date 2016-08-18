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

#include "DVB_Data_Broadcast_ID_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DVB_Data_Broadcast_ID_Descriptor::DVB_Data_Broadcast_ID_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    ASSERT_MIN_DLEN(2);

    m_data_broadcast_id = r16(&buffer[2]);

    for (size_t i = 0; i < mLength - 2; ++i)
        m_id_selector_bytes.push_back(buffer[i + 4]);
}

uint16_t DVB_Data_Broadcast_ID_Descriptor::getDataBroadcastId(void) const
{
    return m_data_broadcast_id;
}

const IdSelectorByteList *DVB_Data_Broadcast_ID_Descriptor::getIdSelectorBytes(void) const
{
    return &m_id_selector_bytes;
}

////////////////////////////////////////////////////////////////////////////////

