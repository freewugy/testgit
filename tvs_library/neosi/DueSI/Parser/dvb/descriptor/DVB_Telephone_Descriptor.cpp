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

#include "DVB_Telephone_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DVB_Telephone_Descriptor::DVB_Telephone_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    size_t headerLength = 3;
    ASSERT_MIN_DLEN(headerLength);

    m_foreign_availability = (buffer[2] >> 5) & 0x01;
    m_connection_type = buffer[2] & 0x1f;
    m_country_prefix_length = (buffer[3] >> 5) & 0x03;
    m_international_area_code_length = (buffer[3] >> 2) & 0x07;
    m_operator_code_length = buffer[3] & 0x03;
    m_national_area_code_length = (buffer[4] >> 4) & 0x07;
    m_core_number_length = buffer[4] & 0x0f;

    headerLength += m_country_prefix_length;
    headerLength += m_international_area_code_length;
    headerLength += m_operator_code_length;
    headerLength += m_national_area_code_length;
    headerLength += m_core_number_length;
    ASSERT_MIN_DLEN(headerLength);

    uint16_t offset = 5;
    m_country_prefix.assign((char *)&buffer[offset], m_country_prefix_length);
    offset += m_country_prefix_length;
    m_international_area_code.assign((char *)&buffer[offset], m_international_area_code_length);
    offset += m_international_area_code_length;
    m_operator_code.assign((char *)&buffer[offset], m_operator_code_length);
    offset += m_operator_code_length;
    m_national_area_code.assign((char *)&buffer[offset], m_national_area_code_length);
    offset += m_national_area_code_length;
    m_core_number.assign((char *)&buffer[offset], m_core_number_length);
}

uint8_t DVB_Telephone_Descriptor::getForeignAvailability(void) const
{
    return m_foreign_availability;
}

uint8_t DVB_Telephone_Descriptor::getConnectionType(void) const
{
    return m_connection_type;
}

const std::string &DVB_Telephone_Descriptor::getCountryPrefix(void) const
{
    return m_country_prefix;
}

const std::string &DVB_Telephone_Descriptor::getInternationalAreaCode(void) const
{
    return m_international_area_code;
}

const std::string &DVB_Telephone_Descriptor::getOperatorCode(void) const
{
    return m_operator_code;
}

const std::string &DVB_Telephone_Descriptor::getNationalAreaCode(void) const
{
    return m_national_area_code;
}

const std::string &DVB_Telephone_Descriptor::getCoreNumber(void) const
{
    return m_core_number;
}


////////////////////////////////////////////////////////////////////////////////

