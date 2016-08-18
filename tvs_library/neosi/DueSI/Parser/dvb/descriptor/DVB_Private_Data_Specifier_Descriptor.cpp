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

#include "DVB_Private_Data_Specifier_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DVB_Private_Data_Specifier_Descriptor::DVB_Private_Data_Specifier_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    ASSERT_MIN_DLEN(4);

    m_private_data_specifier = r32(&buffer[2]);
}

uint32_t DVB_Private_Data_Specifier_Descriptor::getPrivateDataSpecifier(void) const
{
    return m_private_data_specifier;
}


////////////////////////////////////////////////////////////////////////////////

