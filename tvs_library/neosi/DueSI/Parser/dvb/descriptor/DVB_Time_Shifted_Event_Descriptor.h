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

#ifndef DVB_TIME_SHIFTED_EVENT_DESCRIPTOR_H_
#define DVB_TIME_SHIFTED_EVENT_DESCRIPTOR_H_



////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class DVB_Time_Shifted_Event_Descriptor : public Descriptor
{
    protected:
        unsigned m_reference_service_id         : 16;
        unsigned m_reference_event_id         : 16;

    public:
        DVB_Time_Shifted_Event_Descriptor(const uint8_t * const buffer);

        uint16_t getReferenceServiceId(void) const;
        uint16_t getReferenceEventId(void) const;
};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_TIME_SHIFTED_EVENT_DESCRIPTOR_H_ */



