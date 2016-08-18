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

#include "DVB_Bouquet_Name_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DVB_Bouquet_Name_Descriptor::DVB_Bouquet_Name_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    m_bouquet_name.assign((char *)&buffer[2], mLength);
}

DVB_Bouquet_Name_Descriptor::~DVB_Bouquet_Name_Descriptor(void)
{
}

const std::string &DVB_Bouquet_Name_Descriptor::getBouquetName(void) const
{
    return m_bouquet_name;
}

////////////////////////////////////////////////////////////////////////////////

