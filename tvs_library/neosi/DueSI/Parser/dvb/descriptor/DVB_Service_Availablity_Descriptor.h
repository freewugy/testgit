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

#ifndef DVB_SERVICE_AVAILABLITY_DESCRIPTOR_H_
#define DVB_SERVICE_AVAILABLITY_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef std::list<uint16_t> CellIdList;
typedef CellIdList::iterator CellIdIterator;
typedef CellIdList::const_iterator CellIdConstIterator;

class DVB_Service_Availablity_Descriptor : public Descriptor
{
    protected:
        unsigned m_availability_flag       : 1;
        CellIdList m_cell_ids;

    public:
        DVB_Service_Availablity_Descriptor(const uint8_t* const buffer);

        uint8_t getAvailabilityFlag() const;
        const CellIdList *getCellIds(void) const;
};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_SERVICE_AVAILABLITY_DESCRIPTOR_H_ */



