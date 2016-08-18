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

#ifndef DVB_NETWORK_NAME_DESCRIPTOR_H_
#define DVB_NETWORK_NAME_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class DVB_Network_Name_Descriptor : public Descriptor
{
    protected:
        std::string m_network_name;

    public:
        DVB_Network_Name_Descriptor(const uint8_t * const buffer);

        const std::string &getNetworkName(void) const;
};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_NETWORK_NAME_DESCRIPTOR_H_ */



