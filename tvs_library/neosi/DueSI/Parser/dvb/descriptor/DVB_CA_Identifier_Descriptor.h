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

#ifndef DVB_CA_IDENTIFIER_DESCRIPTOR_H_
#define DVB_CA_IDENTIFIER_DESCRIPTOR_H_


////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef std::list<uint16_t> CaSystemIdList;
typedef CaSystemIdList::iterator CaSystemIdIterator;
typedef CaSystemIdList::const_iterator CaSystemIdConstIterator;

class DVB_CA_Identifier_Descriptor : public Descriptor
{
    protected:
        CaSystemIdList m_ca_system_ids;

    public:
        DVB_CA_Identifier_Descriptor(const uint8_t * const buffer);

        const CaSystemIdList *getCaSystemIds(void) const;
};

////////////////////////////////////////////////////////////////////////////////



#endif /* DVB_CA_IDENTIFIER_DESCRIPTOR_H_ */



