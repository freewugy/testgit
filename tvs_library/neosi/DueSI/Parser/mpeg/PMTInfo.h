/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: loneowl $
 * $LastChangedDate: 2014-04-21 14:23:57 +0900 (월, 21 4월 2014) $
 * $LastChangedRevision: 661 $
 * Description:
 * Note:
 *****************************************************************************/

#ifndef PMTINFO_H_
#define PMTINFO_H_

#include <list>
#include "ServiceInfo.h"
#include "Element.h"

class Element;
/*
 *
 */
class PMTInfo {
public:
	PMTInfo();
	virtual ~PMTInfo();

    uint16_t 	program_number;
    uint16_t 	pcr_pid;

    list<elementary_stream> list_elementary_stream;

    /**
     * @brief toString
     * @return string
     */
    string toString();
    string toXMLString();
};

#endif /* PMTINFO_H_ */
