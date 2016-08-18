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

#ifndef ELEMENT_H_
#define ELEMENT_H_

#include <stdint.h>
#include <list>
#include <string>

using namespace std;

#define STRING_LENGTH 512

/**
 * @class SiConf
 * @brief information about SI Table
 */
typedef struct {
	/**
	 * @brief Table Name (PAT, PMT)
	 */
	string table;

	/**
	 * @brief
	 */
	string type;

	/**
	 * @brief ONE_SHOT/VERSION_CHANGED/CONTINUOUS
	 */
	string mode;

	/**
	 * @brief time of time out
	 */
	int timeout;

	/**
	 * @brief order of this Table
	 */
	int priority;
	int tableId;
	int pid;
	string schema;
	int extid;
} SiConf;

typedef struct {
	int type; // 0 : Air, 1 : Satellite, 2 : Cable, 3 : Terrestrial, etc
	int frequency;
	int symbol_rate;
	int modulation;
	int polarization;
	int pilot;
	int fec;
} TunerInfo;

/**
 * @class elementary_stream
 * @brief Elementary Stream Information Structure
 */
typedef struct elementary_stream_t {
	/**
	 * @brief PID (Audio, Video)
	 */
	uint16_t stream_pid;

	/**
	 * @brief stream type ( 0x02, 0x81 )
	 */
	uint8_t stream_type;

	/**
	 * @brief ISO_639_language_code ( exL "kor", "eng" )
	 */
	uint32_t language_code;
} elementary_stream;

/**
 * @class Element
 * @brief information about Element DOMNode
 */
class Element {
public:

    /**
     * @brief 생성자
     */
	Element() {
	}

	/**
	 * @brief 생성자
	 * @param sName     element name
	 * @param sValue    element value
	 */
	Element(string sName, string sValue) {
		name = sName;
		value = sValue;
	}

	/**
	 * @brief name
	 */
	string name;

	/**
	 * @brief value
	 */
	string value;

};

#endif /* ELEMENT_H_ */
