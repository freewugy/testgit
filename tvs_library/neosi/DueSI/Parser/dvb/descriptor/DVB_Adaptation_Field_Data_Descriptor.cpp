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

#include "DVB_Adaptation_Field_Data_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DVB_Adaptation_Field_Data_Descriptor::DVB_Adaptation_Field_Data_Descriptor(const uint8_t* const buffer) : Descriptor(buffer)
{
    ASSERT_MIN_DLEN(1);

    m_adaptation_field_data_identifier = buffer[2];
}

DVB_Adaptation_Field_Data_Descriptor::~DVB_Adaptation_Field_Data_Descriptor()
{
}

uint8_t DVB_Adaptation_Field_Data_Descriptor::getAdaptationFieldDataIdentifier(void) const
{
    return m_adaptation_field_data_identifier;
}



////////////////////////////////////////////////////////////////////////////////

