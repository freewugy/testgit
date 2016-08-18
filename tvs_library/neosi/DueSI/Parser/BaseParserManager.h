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

#ifndef BASEPARSERMANAGER_H_
#define BASEPARSERMANAGER_H_



////////////////////////////////////////////////////////////////////////////////
#include "Uncopyable.h"
#include "tvs_common.h"



////////////////////////////////////////////////////////////////////////////////
class BaseParser;



////////////////////////////////////////////////////////////////////////////////
class BaseParserManager: private Uncopyable
{
public:
	static BaseParserManager* createInstance();
	static BaseParserManager* getInstance() { return m_instance; }
	static void destroyInstance();

	virtual TBool init() = 0;
	virtual TBool start() = 0;
	virtual TBool start(int index) = 0;
	virtual TBool stop() = 0;

	TBool add(BaseParser* parser);
	TBool remove(BaseParser* parser);
	void clear();
	TInt32 count() const;

	BaseParser* operator[](TInt32 index);
	const BaseParser* operator[](TInt32 index) const;

protected:
	BaseParserManager();
	virtual ~BaseParserManager();

private:
	static BaseParserManager* m_instance;
	std::vector<BaseParser*> m_parserList;
};



////////////////////////////////////////////////////////////////////////////////



#endif /* BASEPARSERMANAGER_H_ */



