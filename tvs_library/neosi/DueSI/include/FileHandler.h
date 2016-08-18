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

#ifndef FILEHANDLER_H_
#define FILEHANDLER_H_

#include <string>
#include <list>
#include <map>
#include <fstream>

#include "ServiceInfo.h"
#include "EventInfo.h"

/**
 * @class FileHandler
 * @brief handle to save/load XML data to File\n
 * singleton class
 */
class FileHandler {
public:
	virtual ~FileHandler();

	/**
	 * @brief instance를 생성하여서 반환
	 */
	static FileHandler& getInstance() {
		static FileHandler instance;

		return instance;
	}

	/**
	 * @brief 채널 정보를 파일로 저장
	 * @param mapServiceInfo
	 * @return int
	 */
	int saveServiceInfo(map<string, ServiceInfo> mapServiceInfo);

	/**
	 * @brief 프로그램 정보를 파일로 저장
	 * @param mapEventInfo
	 * @return int
	 */
	int saveEventInfo(map<string, list<EventInfo> > mapEventInfo);

	/**
	 * @brief 채널/프로그램 정보를 파일로 저장
	 * @param mapEventInfo
	 * @return int
	 */
	int saveEPGInfo(map<string, ServiceInfo> mapServiceInfo,map<string, list<EventInfo> > mapEventInfo);

private:

	/**
	 * @brief 생성자
	 */
	FileHandler() {
	};                   // Constructor? (the {} brackets) are needed here.
	// Dont forget to declare these two. You want to make sure they
    // are unaccessable otherwise you may accidently get copies of
    // your singleton appearing.
	FileHandler(FileHandler const&);            // Don't Implement
    void operator=(FileHandler const&); 		// Don't implement

};

#endif /* FILEHANDLER_H_ */
