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

#include "DVB_ECM_Repetition_Rate_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DVB_ECM_Repetition_Rate_Descriptor::DVB_ECM_Repetition_Rate_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    ASSERT_MIN_DLEN(4);

    m_ca_system_id = r16(&buffer[2]);
    m_repetition_rate = r16(&buffer[4]);

    m_private_data_bytes.resize(mLength - 4);
    memcpy(&m_private_data_bytes[0], &buffer[6], mLength - 4);
}

DVB_ECM_Repetition_Rate_Descriptor::~DVB_ECM_Repetition_Rate_Descriptor()
{
}

uint16_t DVB_ECM_Repetition_Rate_Descriptor::getCaSystemId(void) const
{
    return m_ca_system_id;
}

uint16_t DVB_ECM_Repetition_Rate_Descriptor::getRepetitionRate(void) const
{
    return m_repetition_rate;
}

const ECMRepetitionPrivateByteVector *DVB_ECM_Repetition_Rate_Descriptor::getPrivateDataBytes() const
{
    return &m_private_data_bytes;
}

////////////////////////////////////////////////////////////////////////////////

