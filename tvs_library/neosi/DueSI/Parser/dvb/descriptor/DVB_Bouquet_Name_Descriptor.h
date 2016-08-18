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

#ifndef DVB_BOUQUET_NAME_DESCRIPTOR_H_
#define DVB_BOUQUET_NAME_DESCRIPTOR_H_



////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class DVB_Bouquet_Name_Descriptor: public Descriptor
{
    DVB_Bouquet_Name_Descriptor();
protected:
    std::string m_bouquet_name;

public:
    DVB_Bouquet_Name_Descriptor(const uint8_t * const buffer);
    ~DVB_Bouquet_Name_Descriptor(void);

    const std::string &getBouquetName(void) const;

};

////////////////////////////////////////////////////////////////////////////////



#endif /* DVB_BOUQUET_NAME_DESCRIPTOR_H_ */



