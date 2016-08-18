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

#ifndef DVB_TVA_ID_DESCRIPTOR_H_
#define DVB_TVA_ID_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class TVAIdentifier
{
    protected:
        unsigned m_id             :16;
        unsigned m_running_status          : 3;

    public:
        TVAIdentifier(const uint8_t* const buffer);
        ~TVAIdentifier();

        uint16_t getId() const;
        uint8_t getRunningStatus() const;
};

typedef std::list<TVAIdentifier*> TVAIdentifierList;
typedef TVAIdentifierList::iterator TVAIdentifierIterator;
typedef TVAIdentifierList::const_iterator TVAIdentifierConstIterator;

class DVB_TVA_ID_Descriptor : public Descriptor
{
    protected:
        TVAIdentifierList m_identifiers;

    public:
        DVB_TVA_ID_Descriptor(const uint8_t* const buffer);
        virtual ~DVB_TVA_ID_Descriptor();

        const TVAIdentifierList* getIdentifier() const;
};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_TVA_ID_DESCRIPTOR_H_ */



