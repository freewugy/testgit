/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: eschoi $
 * $LastChangedDate: 2014-06-03 13:58:35 +0900 (화, 03 6월 2014) $
 * $LastChangedRevision: 837 $
 * Description:
 * Note:
 *****************************************************************************/

#ifndef BASEPARSER_H_
#define BASEPARSER_H_

////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <list>
#include "BitStream.h"
#include "Uncopyable.h"
#include "Table.h"
#include "Section.h"

#include "Logger.h"

////////////////////////////////////////////////////////////////////////////////
class BitStream;



////////////////////////////////////////////////////////////////////////////////
struct BaseSection: private Uncopyable
{
	BaseSection();
	virtual ~BaseSection();
	virtual bool parse(BitStream* bs) = 0;
};



////////////////////////////////////////////////////////////////////////////////
class BaseParser
{
public:
	BaseParser();
	virtual ~BaseParser();

	virtual bool init() = 0;
	virtual bool parse(BitStream* bs) = 0;
    virtual bool process(void* table) = 0;
};



////////////////////////////////////////////////////////////////////////////////



#endif /* BASEPARSER_H_ */



