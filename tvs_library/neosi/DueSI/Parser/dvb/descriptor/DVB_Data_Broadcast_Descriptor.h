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

#ifndef DVB_DATA_BROADCAST_DESCRIPTOR_H_
#define DVB_DATA_BROADCAST_DESCRIPTOR_H_



////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef std::list<uint8_t> selectorByteList;
typedef selectorByteList::iterator selectorByteIterator;
typedef selectorByteList::const_iterator selectorByteConstIterator;

class DVB_Data_Broadcast_Descriptor : public Descriptor
{
    protected:
        unsigned m_data_broadcast_id            : 16;
        unsigned m_component_tag               : 8;
        unsigned m_selector_length             : 8;
        selectorByteList m_selector_bytes;
        std::string m_iso639_language_code;
        unsigned m_text_length             : 8;
        std::string m_text;

    public:
        DVB_Data_Broadcast_Descriptor(const uint8_t * const buffer);

        uint16_t getDataBroadcastId(void) const;
        uint8_t getComponentTag(void) const;
        const selectorByteList *getSelectorBytes(void) const;
        const std::string &getIso639LanguageCode(void) const;
        const std::string &getText(void) const;
};

////////////////////////////////////////////////////////////////////////////////



#endif /* DVB_DATA_BROADCAST_DESCRIPTOR_H_ */



