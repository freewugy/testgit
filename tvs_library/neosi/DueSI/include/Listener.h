/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: loneowl $
 * $LastChangedDate: 2014-04-21 17:32:52 +0900 (월, 21 4월 2014) $
 * $LastChangedRevision: 665 $
 * Description:
 * Note:
 *****************************************************************************/

#if !defined(EA_C4D3F146_FA75_4714_BEBF_61AF2A403063__INCLUDED_)
#define EA_C4D3F146_FA75_4714_BEBF_61AF2A403063__INCLUDED_

#include <list>
#include <iostream>
#include <set>
#include <algorithm>

using namespace std;

/**
 * @class IListener
 * @brief table Section filter의 listener interface class
 */

class ISiListener {
public:

	virtual void receivedEvent( void* e)=0;
};

/**
 * @class Notifier
 * @brief 등록된 IListener를 table filter 관련 event 발생시에 호출하는 역할
 */
class Notifier {
protected:

	virtual void notifyEvent(void* e) {
		set<ISiListener*>::iterator itr;
		for (itr = m_listeners.begin(); itr != m_listeners.end(); itr++)
			(*itr)->receivedEvent(e);
	}


public:
	/**
	 * @brief listener 등록
	 */
	virtual void registerListener(ISiListener *l) {
		m_listeners.insert(l);
	}

	/**
	 * @brief listener 해제
	 */
	virtual void unregisterListener(ISiListener *l) {
		std::set<ISiListener *>::const_iterator iter = m_listeners.find(l);
		if (iter != m_listeners.end()) {
			m_listeners.erase(iter);
		} else {
			// handle the case
			std::cout
				<< "Could not unregister the specified listener object as it is not registered."
				<< std::endl;
		}
	}

private:
	/**
	 * @brief set of IListener
	 */
	std::set<ISiListener*> m_listeners;
};

#endif // !defined(EA_C4D3F146_FA75_4714_BEBF_61AF2A403063__INCLUDED_)
