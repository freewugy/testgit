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

#ifndef DVB_COUNTRY_AVAILABLITY_DESCRIPTOR_H_
#define DVB_COUNTRY_AVAILABLITY_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef std::list<std::string> CountryCodeList;
typedef CountryCodeList::iterator CountryCodeIterator;
typedef CountryCodeList::const_iterator CountryCodeConstIterator;

class DVB_Country_Availablity_Descriptor : public Descriptor
{
    protected:
        unsigned m_country_availability_flag        : 1;
        CountryCodeList m_country_codes;

    public:
        DVB_Country_Availablity_Descriptor(const uint8_t * const buffer);

        uint8_t getCountryAvailabilityFlag(void) const;
        const CountryCodeList *getCountryCodes(void) const;
};

////////////////////////////////////////////////////////////////////////////////



#endif /* DVB_COUNTRY_AVAILABLITY_DESCRIPTOR_H_ */



