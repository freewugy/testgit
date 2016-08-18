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

#include "DVB_Ancillary_Data_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DVB_Ancillary_Data_Descriptor::DVB_Ancillary_Data_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    ASSERT_MIN_DLEN(1);

    m_ancillary_data_identifier = buffer[2];
}

uint8_t DVB_Ancillary_Data_Descriptor::getAncillaryDataIdentifier(void) const
{
    return m_ancillary_data_identifier;
}


////////////////////////////////////////////////////////////////////////////////

