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

#include "DVB_Country_Availablity_Descriptor.h"


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

DVB_Country_Availablity_Descriptor::DVB_Country_Availablity_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    size_t headerLength = 1;
    ASSERT_MIN_DLEN(headerLength);

    std::string str_country_code;
    m_country_availability_flag = (buffer[2] >> 7) & 0x01;

    for (size_t i = 0; i < mLength - 1; i += 3) {
        headerLength += 3;
        ASSERT_MIN_DLEN(headerLength);

        str_country_code.assign((char *)&buffer[i + 3], 3);
        m_country_codes.push_back(str_country_code);
    }
}

uint8_t DVB_Country_Availablity_Descriptor::getCountryAvailabilityFlag(void) const
{
    return m_country_availability_flag;
}

const CountryCodeList *DVB_Country_Availablity_Descriptor::getCountryCodes(void) const
{
    return &m_country_codes;
}

////////////////////////////////////////////////////////////////////////////////

