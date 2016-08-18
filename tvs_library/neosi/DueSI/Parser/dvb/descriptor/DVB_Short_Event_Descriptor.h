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

#ifndef DVB_SHORT_EVENT_DESCRIPTOR_H_
#define DVB_SHORT_EVENT_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class DVB_Short_Event_Descriptor : public Descriptor
{
    protected:
        std::string m_iso639_language_code;
        unsigned m_event_name_length            : 8;
        std::string m_event_name;
        unsigned m_text_length             : 8;
        std::string m_text;

    public:
        DVB_Short_Event_Descriptor(const uint8_t * const buffer);

        const std::string &getIso639LanguageCode(void) const;
        const std::string &getEventName(void) const;
        const std::string &getText(void) const;
};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_SHORT_EVENT_DESCRIPTOR_H_ */



