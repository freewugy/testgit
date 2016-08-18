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

#include "BaseParserManager.h"
#include "BaseParser.h"

// util.
#include "MutexLock.h"
#include "AutoLock.h"



////////////////////////////////////////////////////////////////////////////////
BaseParserManager* BaseParserManager::m_instance = NULL;
static MutexLock sg_lock; // lock {m_parserList}



////////////////////////////////////////////////////////////////////////////////
BaseParserManager::BaseParserManager()
{
}



BaseParserManager::~BaseParserManager()
{
	clear();
}



////////////////////////////////////////////////////////////////////////////////
TBool BaseParserManager::add(BaseParser* parser)
{
	AutoLock<MutexLock> lock(sg_lock);
	m_parserList.push_back(parser);
	return TRUE;
}



////////////////////////////////////////////////////////////////////////////////
TBool BaseParserManager::remove(BaseParser* parser)
{
	AutoLock<MutexLock> lock(sg_lock);

	std::vector<BaseParser*>::iterator it;
	for (it = m_parserList.begin(); it != m_parserList.end(); it++)
	{
		if ((*it) == parser)
		{
			delete (*it);
			m_parserList.erase(it);
			return TRUE;
		}
	}

	return FALSE;
}



////////////////////////////////////////////////////////////////////////////////
void BaseParserManager::clear()
{
	AutoLock<MutexLock> lock(sg_lock);

	std::vector<BaseParser*>::iterator it;
	for (it = m_parserList.begin(); it != m_parserList.end(); it++)
	{
		delete (*it);
	}
	m_parserList.clear();
}



////////////////////////////////////////////////////////////////////////////////
TInt32 BaseParserManager::count() const
{
	AutoLock<MutexLock> lock(sg_lock);
	return m_parserList.size();
}



////////////////////////////////////////////////////////////////////////////////
BaseParser* BaseParserManager::operator[](TInt32 index)
{
	AutoLock<MutexLock> lock(sg_lock);
	return m_parserList[index];
}



////////////////////////////////////////////////////////////////////////////////
const BaseParser* BaseParserManager::operator[](TInt32 index) const
{
	AutoLock<MutexLock> lock(sg_lock);
	return m_parserList[index];
}



////////////////////////////////////////////////////////////////////////////////



