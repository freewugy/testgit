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

#include "DVB_Time_Shifted_Event_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DVB_Time_Shifted_Event_Descriptor::DVB_Time_Shifted_Event_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    ASSERT_MIN_DLEN(2);

    m_reference_service_id = r16(&buffer[2]);
    m_reference_event_id = r16(&buffer[4]);
}

uint16_t DVB_Time_Shifted_Event_Descriptor::getReferenceServiceId(void) const
{
    return m_reference_service_id;
}

uint16_t DVB_Time_Shifted_Event_Descriptor::getReferenceEventId(void) const
{
    return m_reference_event_id;
}

////////////////////////////////////////////////////////////////////////////////

