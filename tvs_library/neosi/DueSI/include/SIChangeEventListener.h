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

#ifndef SICHANGEEEVENTLISTENER_H_
#define SICHANGEEEVENTLISTENER_H_

#include "SIChangeEvent.h"

/**
 * @class	SIChangeEventListener
 * @brief	SIChangeEventListener 인터페이스
 *
*/
class SIChangeEventListener
{
public:
	/**
	 * @brief 	SIChangeEvent가 전달되는 가상 함수
	 * @param	[IN] e	SIChangeEvent 인스턴스
	 * @return	void
	 */
	virtual void receiveSIChangeEvent(SIChangeEvent* e)= 0;

};



#endif /* SICHANGEEEVENTLISTENER_H_ */
