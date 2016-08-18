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

#ifndef _PMT_PARSER_H_
#define _PMT_PARSER_H_

#include <list>

#include "PMTInfo.h"
#include "BaseParser.h"

class PMTInfo;

/**
 * @class PMTParser
 * @brief
 */
class PMTParser : public BaseParser {
public:
    PMTParser();
	virtual ~PMTParser();

    virtual bool init();
    virtual bool parse(BitStream* bs); /** @override BaseParser */
    virtual bool process(void* sectionData); /** @override BaseParser */
    bool process(uint8_t* sectionData, int length);

	list<PMTInfo*> getPMTList() {
		return listTotalPMTInfo;
	};

private:
	list<PMTInfo*> listTotalPMTInfo;
};

#endif /* _PMT_PARSER_H_ */
