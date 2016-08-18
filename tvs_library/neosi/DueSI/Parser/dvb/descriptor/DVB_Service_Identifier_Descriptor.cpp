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

#include "DVB_Service_Identifier_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DVB_Service_Identifier_Descriptor::DVB_Service_Identifier_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    m_textual_service_identifier.assign((char *)&buffer[2], mLength);
}

const std::string &DVB_Service_Identifier_Descriptor::getTextualServiceIdentifier(void) const
{
    return m_textual_service_identifier;
}

////////////////////////////////////////////////////////////////////////////////

