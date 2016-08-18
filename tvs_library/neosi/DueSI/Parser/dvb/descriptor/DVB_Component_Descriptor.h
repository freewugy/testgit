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

#ifndef DVB_COMPONENT_DESCRIPTOR_H_
#define DVB_COMPONENT_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class DVB_Component_Descriptor: public Descriptor
{
protected:
    unsigned m_stream_content              : 4;
    unsigned m_component_type              : 8;
    unsigned m_component_tag               : 8;
    std::string m_iso639_language_code;
    std::string m_text;

public:
    DVB_Component_Descriptor(const uint8_t * const buffer);

    uint8_t getStreamContent(void) const;
    uint8_t getComponentType(void) const;
    uint8_t getComponentTag(void) const;
    const std::string &getIso639LanguageCode(void) const;
    const std::string &getText(void) const;

};

////////////////////////////////////////////////////////////////////////////////



#endif /* DVB_COMPONENT_DESCRIPTOR_H_ */



