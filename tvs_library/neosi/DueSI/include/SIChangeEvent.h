/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: eschoi $
 * $LastChangedDate: 2014-05-19 18:18:02 +0900 (월, 19 5월 2014) $
 * $LastChangedRevision: 780 $
 * Description:
 * Note:
 *****************************************************************************/

#ifndef SICHANGEEEVENT_H_
#define SICHANGEEEVENT_H_
#include <string>
#include <stdio.h>
#include <stdint.h>

using namespace std;

/**
 * @class	SIChangeEvent
 * @brief	service change event 를 정의
 * 채널 변경이나 프로그램 변경시 전달되는 이벤트 \n
 * SIChangeEventListener 인스턴스 receiveSIChangeEvent메서드의 인자로 전달된다.
 *
 */
class SIChangeEvent {
public:
	enum Type {
		UNKNOWN,

		CHANNEL_COMPLETED = 1000,
		CHANNEL_UPDATED = 1001,
		CHANNEL_ADD_EVENT = 1002,
		CHANNEL_MODIFY_EVENT = 1003,
		CHANNEL_REMOVE_EVENT = 1004,
		CHANNEL_MAP_COMPLETED = 1005,
		CHANNEL_MAP_READY = 1006,
		CHANNEL_PROGRAM_MAP_READY = 1007,
		CHANNEL_PMT = 1008,

		PROGRAM_COMPLETED = 2000,
		PROGRAM_UPDATED = 2001,
		PROGRAM_ADD_EVENT = 2002,
		PROGRAM_MODIFY_EVENT = 2003,
		PROGRAM_REMOVE_EVENT = 2004,
		PROGRAM_MAP_COMPLETED = 2005,
		PROGRAM_PROCESSING = 2006,
		PROGRAM_MAP_READY = 2007,

		SCAN_READY = 3001,
		SCAN_PROCESSING = 3002,
		SCAN_COMPLETED = 3003,
		SCAN_STOP = 3004,

		DATA_READING = 4001,
		DATA_EMPTY = 4002,

		TDT_DATE = 5001,

		SET_DEFAULT_CH = 6001,

		ERROR_WEPG_CONNECTION = 9001,
		ERROR_HAPPENED = 9999,
	};

	/* @brief 	SIChangeEevent class 상수 */
	static const int RETURN_OK = 1;
	/* @brief 	SIChangeEevent class 상수 */
	static const int RETURN_FAIL = 2;
	/* @brief 	SIChangeEevent class 상수 */
	static const int RETURN_UNKNOWN = 3;

	SIChangeEvent() {
		mReceived = 0;
		mToBeReceived = 0;
		mFoundChannel = 0;
		mTDTDate = 0;
	};

	/**
	 * @brief 	SIChangeEvent 타입 반환
	 * @param 	[IN] None
	 * @return	SIChangeEvent class에 정의된 int를 반환
	 */
	int getEventType() {
		return m_event_type;
	}

	/**
	 * @brief 	SIChangeEvent 타입 저장
	 * @param 	[IN] type : SIChangeEvent class에 정의된 int
	 * @return	void
	 */
	void setEventType(int type) {
		m_event_type = type;
	}

	/**
	 * @brief toString()
	 * @return [OUT] string
	 */
	string toString() {
		string res;
		char chr[128];
		res.append("EventType[");
		if (m_event_type == SIChangeEvent::CHANNEL_COMPLETED)
			res.append("CHANNEL_COMPLETED]");
		else if (m_event_type == SIChangeEvent::CHANNEL_UPDATED)
			res.append("CHANNEL_UPDATED]");
		else if (m_event_type == SIChangeEvent::CHANNEL_ADD_EVENT)
			res.append("CHANNEL_ADD_EVENT]");
		else if (m_event_type == SIChangeEvent::CHANNEL_MODIFY_EVENT)
			res.append("CHANNEL_MODIFY_EVENT]");
		else if (m_event_type == SIChangeEvent::CHANNEL_REMOVE_EVENT)
			res.append("CHANNEL_REMOVE_EVENT]");
		else if (m_event_type == SIChangeEvent::CHANNEL_MAP_COMPLETED)
			res.append("CHANNEL_MAP_COMPLETED]");
		else if (m_event_type == SIChangeEvent::CHANNEL_MAP_READY)
			res.append("CHANNEL_MAP_READY]");
		else if (m_event_type == SIChangeEvent::CHANNEL_PROGRAM_MAP_READY)
			res.append("CHANNEL_PROGRAM_MAP_READY]");
		else if (m_event_type == SIChangeEvent::CHANNEL_PMT)
			res.append("CHANNEL_PMT]");
		else if (m_event_type == SIChangeEvent::PROGRAM_COMPLETED)
			res.append("PROGRAM_COMPLETED]");
		else if (m_event_type == SIChangeEvent::PROGRAM_UPDATED)
			res.append("PROGRAM_UPDATED]");
		else if (m_event_type == SIChangeEvent::PROGRAM_ADD_EVENT)
			res.append("PROGRAM_ADD_EVENT]");
		else if (m_event_type == SIChangeEvent::PROGRAM_MODIFY_EVENT)
			res.append("PROGRAM_MODIFY_EVENT]");
		else if (m_event_type == SIChangeEvent::PROGRAM_REMOVE_EVENT)
			res.append("PROGRAM_REMOVE_EVENT]");
		else if (m_event_type == SIChangeEvent::PROGRAM_MAP_COMPLETED) {
			sprintf(chr, "PROGRAM_MAP_COMPLETED], Process[%d/%d]", mReceived,
					mToBeReceived);
			res.append(chr);
		} else if (m_event_type == SIChangeEvent::PROGRAM_PROCESSING)
			res.append("PROGRAM_PROCESSING]");
		else if (m_event_type == SIChangeEvent::PROGRAM_MAP_READY)
			res.append("PROGRAM_MAP_READY]");
		else if (m_event_type == SIChangeEvent::SCAN_READY) {
			sprintf(chr, "SCAN_READY], Process[%d/%d]", mReceived,
					mToBeReceived);
			res.append(chr);
		} else if (m_event_type == SIChangeEvent::SCAN_PROCESSING) {
			sprintf(chr, "SCAN_PROCESSING], Process[%d/%d]", mReceived,
					mToBeReceived);
			res.append(chr);
		} else if (m_event_type == SIChangeEvent::SCAN_COMPLETED) {
			sprintf(chr, "SCAN_COMPLETED], Found Channel[%d]", mFoundChannel);
			res.append(chr);
		} else if (m_event_type == SIChangeEvent::SCAN_STOP) {
			res.append("SCAN_STOP]");
		} else if (m_event_type == SIChangeEvent::DATA_READING)
			res.append("DATA_READING]");
		else if (m_event_type == SIChangeEvent::DATA_EMPTY)
			res.append("DATA_EMPTY]");
		else if (m_event_type == SIChangeEvent::TDT_DATE) {
			sprintf(chr, "TDT_DATE], Received Time[%u]", mTDTDate);
			res.append(chr);
		} else if (m_event_type == SIChangeEvent::ERROR_WEPG_CONNECTION)
			res.append("ERROR_WEPG_CONNECTION]");
		else if (m_event_type == SIChangeEvent::ERROR_HAPPENED)
			res.append("ERROR_HAPPENED]");
		else {
			sprintf(chr, "UNKNOWN:%d]", m_event_type);
			res.append(chr);
		}

		return res;
	}

	/**
	 * @brief
	 */
	string getChannelUID() {
		return mChannelUid;
	}

	/**
	 * @brief
	 */
	void setChannelUID(string uid) {
		mChannelUid = uid;
	}

	/**
	 * @brief get number of scan frequencies at this time
	 * @return [OUT] int number of frequencies
	 */
	int getReceived() {
		return mReceived;
	}

	/**
	 * @brief 현재까지 스캔한 주파수 갯수 set
	 * @param [IN] count 현재까지 스캔한 주파수 갯수
	 */
	void setReceived(int count) {
		mReceived = count;
	}

	/**
	 * @brief 총 주파수 갯수 set
	 * @param [IN] totalCount 총 주파수 갯수
	 */
	void setToBeReceived(int totalCount) {
		mToBeReceived = totalCount;
	}

	/**
	 * @brief 총 주파수 갯수
	 * @return [OUT] int 총 주파수 갯수
	 */
	int getToBeReceived() {
		return mToBeReceived;
	}

	/**
	 * @brief 받은 채널 갯수 get
	 * @return mFoundChannel 받은 채널 갯수
	 */
	int getFoundChannel() {
		return mFoundChannel;
	}

	/**
	 * @brief 받은 채널 갯수 set
	 * @param [IN] foundChannel 받은 채널 갯수
	 */
	void setFoundChannel(int foundChannel) {
		mFoundChannel = foundChannel;
	}

	uint32_t getTDTDate() {
		return mTDTDate;
	}

	void setTDTDate(uint32_t tdt_date) {
		mTDTDate = tdt_date;
	}

	int getChannelVersion() {
		return mVersion;
	}

	void setChannelVersion(int chVersion) {
		mVersion = chVersion;
	}

	int getDefaultChannel() {
		return mDefault_ch;
	}

	void setDefaultChannel(int default_ch) {
		mDefault_ch = default_ch;
	}

private:
	/**
	 * @brief EVENT TYPE
	 */
	int m_event_type;

	/**
	 * @brief SI TYPE
	 */
	int m_si_type;

	int errorSource;
	int errorRange;
	int errorCode;

	/**
	 * @brief 진행정도를 나타냄 0 ~ 99
	 */
	int progress;

	/**
	 * @brief channel or program count in Scanning
	 */
	int mReceived;

	/**
	 * @brief 받아야 하는 채널 또는 프로그램 갯수
	 */
	int mToBeReceived;

	/**
	 * @brief PMT를 받아서 es_info를 구성한 채널 수
	 */
	int mFoundChannel;

	/**
	 * @brief
	 */
	string mChannelUid;

	/**
	 * @brief TDT Time (utc second)
	 */
	uint32_t mTDTDate;

	int mVersion;
	int mDefault_ch;

};

#endif /* SICHANGEEEVENT_H_ */
