/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: eschoi $
 * $LastChangedDate: 2014-08-27 20:10:39 +0900 (수, 27 8월 2014) $
 * $LastChangedRevision: 1003 $
 * Description:
 * Note:
 *****************************************************************************/

#ifndef EVENTINFO_H_
#define EVENTINFO_H_

#include <string>
#include <list>
#include <vector>
#include "Element.h"

using namespace std;

typedef enum {
    NO_INFO_DOLBY = 0, NOT_DOLBY, DOLBY_SUPPORTED,
} DOLBY_SCHEME;

typedef enum {
    NO_INFO_AUDIO = 0, MONO, STEREO, AC3,
} AUDIO_SCHEME;

typedef enum {
    NO_INFO_RESOLUTION = 0, SD, HD,
} RESOLUTION_SCHEME;

typedef enum {
    NO_CAPTION = 0, CAPTION,
} CAPTION_SCHEME;

typedef enum {
    NO_DVS = 0, DVS,
} DVS_SCHEME;

class Element;

typedef struct _ProgramLinkedInfo {
    /**
     * @brief
     */
	uint8_t linked_service_flag;

	/**
	 * @brief
	 */
	string linked_service_text;

	/**
	 * @brief
	 */
	uint8_t button_type;

	/**
	 * @brief
	 */
	uint32_t c_menu;

	/**
	 * @brief button image filename
	 */
	string button_image_filename;

	/**
	 * @brief display start time - UTC - second
	 */
	uint32_t display_start_time;

	/**
	 * @brief display end time - UTC - second
	 */
	uint32_t display_end_time;

	/**
	 * @brief
	 */
	string vas_itemid;

	/**
	 * @brief
	 */
	string vas_path;

	/**
	 * @brief
	 */
	string vas_serviceid;
} ProgramLinkedInfo;

/**
 * @brief Program정보를 담고 있는 class
 * @class EventInfo
 */
class EventInfo {
public:

    /**
     * @brief 생성자
     */
	EventInfo();

	EventInfo(const EventInfo& eInfo);
	EventInfo(string channel_uid, uint32_t channel_id, vector<string> element, int version);
	/**
	 * @brief 소멸자
	 */
	virtual ~EventInfo();

	/**
	 * @brief 초기화 - channel uid생성시킴
	 */
	void init();

	/**
	 * @brief XML형식의 string으로 변경
	 * @return string XML string
	 */
	string toXMLString();

	/**
	 * @brief XML형식의 string으로 변경 for pure IPTV UI
	 * @return string XML string
	 */
	string toXMLString_EPG();

	/**
	 * @brief 데이터 저장
	 * @param elm Element instance
	 */
	void saveData(Element elm);

	/**
	 * @brief service unique id : 채널 정보와의 연결 고리
	 */
	string service_uid;

	/**
	 * @brief service id
	 */
	uint32_t service_id;
	/**
	 * @brief event id : program id
	 */
	uint32_t event_id;

	/**
	 * @brief name of program
	 */
	string event_name;

	/**
	 * @brief short name of program
	 */
	string event_short_description;

	/**
	 * @brief description of program
	 */
	string event_description;

	/**
	 * @brief
	 */
	uint8_t running_status;

    /**
     * @brief
     */
	uint8_t free_CA_mode;
    /**
     * @brief
     */
	uint8_t rating;

    /**
     * @brief 프로그램 시작 시간 - UTC second
     */
	uint32_t start_time;

    /**
     * @brief 프로그램 종료 시간 - UTC second
     */
	uint32_t end_time;

    /**
     * @brief running time - second
     */
	uint16_t duration;

    /**
     * @brief
     */
	string image_url;

    /**
     * @brief name of director
     */
	string director_name;

    /**
     * @brief name of actors
     */
	string actors_name;

    /**
     * @brief
     */
	string price_text;

    /**
     * @brief dolby status
     *        0: no info, 'N': not dolby, 'D': dolby support
     */
	uint8_t dolby;

    /**
     * @brief audio mode
     *        0: no info, 'M': mono, 'S': stereo, 'A': AC-3
     */
	uint8_t audio;

	/**
	 * @brief resolution
	 *        0: no info, 'S': SD, 'H': HD
	 */
	uint8_t resolution;

    /**
     * @brief
     */
	uint8_t content_nibble_level_1;

    /**
     * @brief
     */
	uint8_t content_nibble_level_2;

    /**
     * @brief
     */
	uint16_t user_nibble_1; // loneowl_20140821 | used to Season for WEPG v2, 0 ~ 999

    /**
     * @brief
     */
	uint16_t user_nibble_2; // loneowl_20140821 | used to EpisodeNo for WEPG v2

    /**
     * @brief
     */
	uint8_t caption;

    /**
     * @brief
     */
	uint8_t dvs;

	list<ProgramLinkedInfo> listProgramLinkedInfo;

    /**
     * @brief
     */
	uint32_t transport_stream_id;

    /**
     * @brief
     */
	uint32_t original_network_id;

	/**
	 * @brief
	 */
	uint8_t ETM_location;

	/**
	 * @brief toString
	 * @return string
	 */
	string toString();

	/**
	 * @brief
	 * @param info
	 * @return void
	 */
	void update(EventInfo info);

    /**
     * @brief
     */
	string getValue(string name);

	uint32_t getSize();
private:

    /**
     * @brief
     */
	bool bCompound;

    /**
     * @brief
     */
	list<Element> m_listElm;

	/**
	 * @brief make Unique ID
	 * @return void
	 */
	void makeUid();
	uint32_t size;

	uint32_t convertEventIDFromString(string str);
	uint16_t convertSeasonFromString(string str);
};

#endif /* EVENTINFO_H_ */
