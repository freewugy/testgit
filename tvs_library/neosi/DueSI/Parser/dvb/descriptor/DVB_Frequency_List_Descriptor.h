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

#ifndef DVB_FREQUENCY_LIST_DESCRIPTOR_H_
#define DVB_FREQUENCY_LIST_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef std::list<uint32_t> CentreFrequencyList;
typedef CentreFrequencyList::iterator CentreFrequencyIterator;
typedef CentreFrequencyList::const_iterator CentreFrequencyConstIterator;

class DVB_Frequency_List_Descriptor : public Descriptor
{
    protected:
        unsigned m_coding_type             : 2;
        CentreFrequencyList m_centre_frequencies;

    public:
        DVB_Frequency_List_Descriptor(const uint8_t * const buffer);

        uint8_t getCodingType(void) const;
        const CentreFrequencyList *getCentreFrequencies(void) const;
};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_FREQUENCY_LIST_DESCRIPTOR_H_ */



