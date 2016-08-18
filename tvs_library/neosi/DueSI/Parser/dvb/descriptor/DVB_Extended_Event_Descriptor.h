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

#ifndef DVB_EXTENDED_EVENT_DESCRIPTOR_H_
#define DVB_EXTENDED_EVENT_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class ExtendedEvent
{
    protected:
        unsigned m_item_description_length          : 8;
        std::string m_item_description;
        unsigned m_item_length             : 8;
        std::string m_item;

    public:
        ExtendedEvent(const uint8_t * const buffer);

        const std::string &getItemDescription(void) const;
        const std::string &getItem(void) const;

    friend class DVB_Extended_Event_Descriptor;
};

typedef std::list<ExtendedEvent *> ExtendedEventList;
typedef ExtendedEventList::iterator ExtendedEventIterator;
typedef ExtendedEventList::const_iterator ExtendedEventConstIterator;

class DVB_Extended_Event_Descriptor : public Descriptor
{
    protected:
        unsigned m_descriptor_number           : 4;
        unsigned m_last_descriptor_number           : 4;
        std::string m_iso639_language_code;
        unsigned m_length_of_items              : 8;
        ExtendedEventList m_items;
        unsigned m_text_length             : 8;
        std::string m_text;

    public:
        DVB_Extended_Event_Descriptor(const uint8_t * const buffer);
        ~DVB_Extended_Event_Descriptor(void);

        uint8_t getDescriptorNumber(void) const;
        uint8_t getLastDescriptorNumber(void) const;
        const std::string &getIso639LanguageCode(void) const;
        const ExtendedEventList *getItems(void) const;
        const std::string &getText(void) const;
};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_EXTENDED_EVENT_DESCRIPTOR_H_ */



