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

#include "DVB_PDC_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DVB_PDC_Descriptor::DVB_PDC_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    ASSERT_MIN_DLEN(3);

    m_program_identification_label = ((buffer[2] & 0x0f) << 16) | r16(&buffer[3]);
}

uint32_t DVB_PDC_Descriptor::getProgramIdentificationLabel(void) const
{
    return m_program_identification_label;
}

////////////////////////////////////////////////////////////////////////////////

