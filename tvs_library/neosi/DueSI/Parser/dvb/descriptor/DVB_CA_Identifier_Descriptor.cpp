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

#include "DVB_CA_Identifier_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DVB_CA_Identifier_Descriptor::DVB_CA_Identifier_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    for (size_t i = 0; i < mLength; i += 2)
        m_ca_system_ids.push_back(r16(&buffer[i + 2]));
}

const CaSystemIdList *DVB_CA_Identifier_Descriptor::getCaSystemIds(void) const
{
    return &m_ca_system_ids;
}

////////////////////////////////////////////////////////////////////////////////

