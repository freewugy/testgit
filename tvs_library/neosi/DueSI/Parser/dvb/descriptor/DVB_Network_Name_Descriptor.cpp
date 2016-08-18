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

#include "DVB_Network_Name_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DVB_Network_Name_Descriptor::DVB_Network_Name_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    m_network_name.assign((char *)&buffer[2], mLength);
}

const std::string &DVB_Network_Name_Descriptor::getNetworkName(void) const
{
    return m_network_name;
}

////////////////////////////////////////////////////////////////////////////////

