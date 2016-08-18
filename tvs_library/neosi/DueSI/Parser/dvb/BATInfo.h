/*
 * BATInfo.h
 *
 *  Created on: Jan 19, 2015
 *      Author: wjpark
 */

#ifndef BATINFO_H_
#define BATINFO_H_

/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * Description:
 * Note:
 *****************************************************************************/

#include <string>
#include <list>
#include "Element.h"

using namespace std;

class Element;

/**
 * @brief BAT정보를 담고 있는 class
 * @class BATInfo
 */
class BATInfo {
public:
    BATInfo();
    virtual ~BATInfo();

    uint16_t bouquet_id;
    uint16_t transport_stream_id;
    string bouquet_name;
    uint16_t original_network_id;
    uint16_t service_id;
    uint8_t linkage_type;
    uint8_t service_type;
    uint16_t local_channel_number;

    /**
     * @brief toString
     * @return string
     */
    string toString();
    string toXMLString();
};

#endif /* BATINFO_H_ */
