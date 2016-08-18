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

#ifndef DVB_SUBTITLING_DESCRIPTOR_H_
#define DVB_SUBTITLING_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class Subtitling
{
    protected:
        std::string m_iso639_language_code;
        unsigned m_subtitling_type             : 8;
        unsigned m_composition_page_id          : 16;
        unsigned m_ancillary_page_id            : 16;

    public:
        Subtitling(const uint8_t * const buffer);

        const std::string &getIso639LanguageCode(void) const;
        uint8_t getSubtitlingType(void) const;
        uint16_t getCompositionPageId(void) const;
        uint16_t getAncillaryPageId(void) const;
};

typedef std::list<Subtitling *> SubtitlingList;
typedef SubtitlingList::iterator SubtitlingIterator;
typedef SubtitlingList::const_iterator SubtitlingConstIterator;

class DVB_Subtitling_Descriptor : public Descriptor
{
    protected:
        SubtitlingList m_subtitlings;

    public:
        DVB_Subtitling_Descriptor(const uint8_t * const buffer);
        ~DVB_Subtitling_Descriptor(void);

        const SubtitlingList *getSubtitlings(void) const;
};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_SUBTITLING_DESCRIPTOR_H_ */



