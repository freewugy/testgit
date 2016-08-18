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

#ifndef DVB_VBI_TELETEXT_DESCRIPTOR_H_
#define DVB_VBI_TELETEXT_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class VbiTeletext
{
    protected:
        std::string m_iso639_language_code;
        unsigned m_teletext_type               : 5;
        unsigned m_teletext_magazine_number         : 3;
        unsigned m_teletext_page_number         : 8;

    public:
        VbiTeletext(const uint8_t * const buffer);

        const std::string &getIso639LanguageCode(void) const;
        uint8_t getTeletextType(void) const;
        uint8_t getTeletextMagazineNumber(void) const;
        uint8_t getTeletextPageNumber(void) const;
};

typedef std::list<VbiTeletext *> VbiTeletextList;
typedef VbiTeletextList::iterator VbiTeletextIterator;
typedef VbiTeletextList::const_iterator VbiTeletextConstIterator;

class DVB_VBI_Teletext_Descriptor : public Descriptor
{
    protected:
        VbiTeletextList m_vbi_teletexts;

    public:
        DVB_VBI_Teletext_Descriptor(const uint8_t * const buffer);
        ~DVB_VBI_Teletext_Descriptor(void);

        const VbiTeletextList *getVbiTeletexts(void) const;
};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_VBI_TELETEXT_DESCRIPTOR_H_ */



