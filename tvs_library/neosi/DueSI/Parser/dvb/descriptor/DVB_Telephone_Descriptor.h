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

#ifndef DVB_TELEPHONE_DESCRIPTOR_H_
#define DVB_TELEPHONE_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class DVB_Telephone_Descriptor : public Descriptor
{
    protected:
        unsigned m_foreign_availability            : 1;
        unsigned m_connection_type             : 5;
        unsigned m_country_prefix_length            : 2;
        unsigned m_international_area_code_length        : 3;
        unsigned m_operator_code_length         : 2;
        unsigned m_national_area_code_length         : 3;
        unsigned m_core_number_length           : 4;
        std::string m_country_prefix;
        std::string m_international_area_code;
        std::string m_operator_code;
        std::string m_national_area_code;
        std::string m_core_number;

    public:
        DVB_Telephone_Descriptor(const uint8_t * const buffer);

        uint8_t getForeignAvailability(void) const;
        uint8_t getConnectionType(void) const;
        const std::string &getCountryPrefix(void) const;
        const std::string &getInternationalAreaCode(void) const;
        const std::string &getOperatorCode(void) const;
        const std::string &getNationalAreaCode(void) const;
        const std::string &getCoreNumber(void) const;
};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_TELEPHONE_DESCRIPTOR_H_ */



