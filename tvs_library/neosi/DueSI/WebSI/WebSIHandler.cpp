/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 * WebSIHandler.cpp
 *
 * LastChangedBy: owl
 * LastChangedDate: 2015. 3. 4.
 * Description:
 * Note:
 *****************************************************************************/
#include "WebSIHandler.h"
#include <sstream>
#include <set>
#include <stdint.h>
#include "SIDataManager.h"
#include "tinyxml/tinyxml.h"
#include "HttpHandler_curl.h"
#include "WebXMLParser.h"
#include "TimeConvertor.h"
#include "StringUtil.h"
#include "Logger.h"
#include "PSIMonitor/PSIMonitor.h"
#include "Timer.h"
#include "xmlUtil.h"
#include <unistd.h>

/////////////////////////////////////////////////////////

static int bWEPGStatus = 0; // : NOT READY, 1 : FIRST START, 2 : UPDATE
static bool bStop = false;
bool bSuccessToMakeChannelMap = false;
string DummyChXML = "";

char tvstorm_eth_valid_prop[10] = { '\0', };

pthread_mutex_t WebSIHandler::m_mutex =
#ifdef ANDROID
		PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
#else
		PTHREAD_MUTEX_INITIALIZER;
#endif

static const char* TAG = "WebSIHandler";

WebSIHandler::WebSIHandler() {
	bWEPGStatus = 0;
	bStartTimer = false;
	mStrVersion = "";
	bSuccessToMakeChannelMap = false;

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&mTask, &attr, &WebSIHandler::webSIThread, this);
	pthread_attr_destroy(&attr);
	return;
}

WebSIHandler::~WebSIHandler() {
	// TODO Auto-generated destructor stub
}

int WebSIHandler::PropertyCheck()
{
	int proValue = 0;

	for (int i = 0; i < 10; i++) {
		memset(tvstorm_eth_valid_prop, 0, 10);
		proValue = __system_property_get("tvstorm.eth.valid",tvstorm_eth_valid_prop);

		if (!strcmp(tvstorm_eth_valid_prop, "1")){
			return proValue;
		}
		sleep(1);
	}
	return proValue;
}

void WebSIHandler::dummyDefault_ch()
{
	string default_ch = "";

	char ch[1000];

	ifstream xmlFile;
	xmlFile.open("/data/lkn_home/run/channel.xml", ios::in | ios::binary);

	while (!xmlFile.eof()) {
		xmlFile.getline(ch, 1000);
		DummyChXML.append(ch);
	}
	xmlFile.close();

	default_ch = WebXMLParser::getInstance().getDefaultChannel(DummyChXML);
	if(default_ch == "")
		default_ch = "0";

	L_DEBUG(TAG, "default_Ch : %s\n", default_ch.c_str());
	SIDataManager::getInstance().setDefaultChannelUid("0.0." + default_ch);
}

void* WebSIHandler::webSIThread(void* param) {
	((WebSIHandler *) param)->dummyDefault_ch();
	while (1) {
		int result_proValue = ((WebSIHandler *) param)->PropertyCheck();
		while (bWEPGStatus > 0) {
			WebSILock();
			if (bWEPGStatus == 1) {
				((WebSIHandler *) param)->updateProcess(result_proValue);
				((WebSIHandler *) param)->updateWebEPG(result_proValue);
				((WebSIHandler *) param)->startTimer();
#ifdef ANDROID
				PSIMonitor::getInstance().start();
#endif
			} else if (bWEPGStatus == 2) {
				((WebSIHandler *) param)->updateProcess(result_proValue);
				((WebSIHandler *) param)->updateWebEPG(result_proValue);
			}
			bWEPGStatus = -1;
			WebSIUnlock();
		}
		siMiliSleep(500);
		if (!strcmp(tvstorm_eth_valid_prop, "1") && bSuccessToMakeChannelMap == false){
			bWEPGStatus = 1;
		}

	}

	return NULL;
}

void updateTimer(unsigned int id) {
	static int nTimes = 0;
	L_DEBUG(TAG, "==== [%d] Update Process START ====\n", ++nTimes);
	bWEPGStatus = 2;
}

void WebSIHandler::startTimer() {
	if (bStartTimer || bSuccessToMakeChannelMap == false)
		return;

	bStartTimer = true;
	long period = 1000 * 60 * 10;

	if (init_timer() < 0) {
		L_ERROR(TAG, "\n\n=== init_time() Error!!! ===\n\n\n");
		return;
	}

	if (set_timer(ID_TIMER_CHECK_WEBSI_UPDATE, period,
			(TimerHandler *) updateTimer) < 0) {
		L_ERROR(TAG,
				"\n\n=== set_timer() failed!! ID[ID_TIMER_CHECK_WEBSI_UPDATE] ===\n\n");
		return;
	} else {
		L_INFO(TAG,
				"=== success set_timer() : [ID_TIMER_CHECK_WEBSI_UPDATE] Period [%ld] seconds ===\n\n",
				period / 1000);
	}
}

void WebSIHandler::saveChannelInfo(list<ServiceInfo*> listChannelInfo,
		int mVersion, int default_ch) {
	SIDataManager& dataManager = SIDataManager::getInstance();
	dataManager.ready();

	SIChangeEvent event;
	event.setEventType(SIChangeEvent::CHANNEL_MAP_COMPLETED);
	event.setChannelVersion(mVersion);
	event.setDefaultChannel(default_ch);
	if (!listChannelInfo.empty()) {
		event.setFoundChannel(-1);
		dataManager.saveChannelInfo(listChannelInfo, event, mVersion);
	}
}

void WebSIHandler::saveProgramInfo(string channel_uid,
		vector<EventInfo *> listProgramInfo) {
	SIDataManager& dataManager = SIDataManager::getInstance();

	SIChangeEvent event;
	event.setEventType(SIChangeEvent::PROGRAM_COMPLETED);
	if (listProgramInfo.empty() == false) {
		event.setFoundChannel(-1);
		dataManager.saveProgramInfo(channel_uid, listProgramInfo, event, false,
				true);
	}
}

string WebSIHandler::versionSplit(string strVersion) {
	string str;
	int count = 0;
	vector<string> element;
	count = splitStringByDelimiter(strVersion, ".", element);

	for (int i = 0; i < element.size(); i++) {
		str.append(element[i]);
	}

	if (count == 2)
		str.append("0");

	return str.c_str();
}

void WebSIHandler::updateLocalChXML(string xml) {

	L_DEBUG(TAG, "channel.xml change Version -> new Version Down \n");
	int ret = 0;

	ofstream outFile;
	outFile.open("/data/lkn_home/run/channel.xml", ios::trunc);

	if (outFile) {
		L_DEBUG(TAG, "channel.xml File OPEN mode :: trunc \n");
		outFile << xml;
		L_DEBUG(TAG, "channel.xml File WRITE \n");
		outFile.close();
	} else {
		L_ERROR(TAG, "channel.xml File OPEN ERROR \n");
		return;
	}

}

void WebSIHandler::updateProcess(int proValue) {

	L_DEBUG(TAG, "WEB SI UPDATE : URI[%s]\n", mChannelXMLUrl.c_str());

	string strChannelsXML = "";
	string default_ch = "";
	int nTry = 0;

	string Local_Version = "";

	Local_Version = versionSplit(WebXMLParser::getInstance().getVersion(DummyChXML)); //2015.02

	L_DEBUG(TAG, "property Get : %d\n", proValue);
	L_DEBUG(TAG, "property eth_valid_Value : %s\n", tvstorm_eth_valid_prop);


	while (nTry++ < WEB_SI_RETRY_TIME) {
			if (socket_httpget(mChannelXMLUrl.c_str(), &strChannelsXML) == 200) {
				L_DEBUG(TAG, "WEB SI Update : START ::::::::::::::::::: \n");

				string Web_Version = versionSplit(WebXMLParser::getInstance().getVersion(strChannelsXML));

				default_ch = WebXMLParser::getInstance().getDefaultChannel(	strChannelsXML);
				if(default_ch == "")
					default_ch = "0";
				SIDataManager::getInstance().setDefaultChannelUid("0.0." + default_ch);
				bSuccessToMakeChannelMap = true;
				if (Web_Version.compare(mStrVersion) == 0  ) {
						L_INFO(TAG, "WebSI XML : NOT CHANGED [%s]\n", Web_Version.c_str());
						return;
				} else {
					mStrVersion = Web_Version;
				}

				list<ServiceInfo*> listChannelInfo;
				if (WebXMLParser::getInstance().parseChannelXML(strChannelsXML,listChannelInfo)) {
					saveChannelInfo(listChannelInfo, atoi(Web_Version.c_str()), atoi(default_ch.c_str()));
				}

				for (list<ServiceInfo*>::iterator itr = listChannelInfo.begin();
						itr != listChannelInfo.end(); itr++) {
					delete (*itr);
				}
				listChannelInfo.clear();

				return;
			}
	}

	bSuccessToMakeChannelMap = false;
//	SIDataManager::getInstance().sendChannelReady(atoi(Local_Version.c_str()));

	L_ERROR(TAG, "\n\n=== OOPS : Cannot Get ChannelMap [%s] ===\n\n",mChannelXMLUrl.c_str());

}

bool WebSIHandler::downloadAndMakeProgramInfosFromServer(map<string, WebSIChannelInfoFile> updateMapChannel) {
	string channel_uid = "";
	int i = 0;
	long response_code;
	bool bInsert = false;
	vector<EventInfo*> listProgramInfo;
	for (map<string, WebSIChannelInfoFile>::iterator itr = updateMapChannel.begin(); itr != updateMapChannel.end(); itr++) {
		channel_uid = itr->first;
		WebSIChannelInfoFile info = itr->second;

		if (WebXMLParser::getInstance().parseProgramList(channel_uid,info.channel_number, info.program_info_url, info.version,	listProgramInfo) >= 0) {

			bInsert = true;
			saveProgramInfo(channel_uid, listProgramInfo);

			L_DEBUG(TAG,
					"[%03d] : channel : No[%03d], Uid[%s], Name[%s], Genre[%s], ProgramInfo size[%u], update_date[%u]\n",
					++i, info.channel_number, channel_uid.c_str(),
					info.channel_name.c_str(), info.channel_genre.c_str(),
					listProgramInfo.size(), info.update_seconds);
		}

		for (vector<EventInfo*>::iterator itr = listProgramInfo.begin(); itr != listProgramInfo.end(); itr++) {
			delete (*itr);
		}
		listProgramInfo.clear();
	}

	return bInsert;
}

void WebSIHandler::updateWebEPG(int proValue) {
	L_INFO(TAG, "WEB EPG UPDATE : URI[%s]\n", mChannelMapTxtUrl.c_str());
	uint32_t starttime = TimeConvertor::getInstance().getCurrentSecond();

	string strChannelMapText = "";
	int nTry = 0;
	WebXMLParser::getInstance().setLimitedDate();

	L_DEBUG(TAG, "property Get : %d\n", proValue);
	L_DEBUG(TAG, "property eth_valid_Value : %s\n", tvstorm_eth_valid_prop);

	while (nTry++ < WEB_SI_RETRY_TIME) {
		if (socket_httpget(mChannelMapTxtUrl.c_str(), &strChannelMapText) == 200) {
				L_DEBUG(TAG, "WEB EPG Update : START ::::::::::::::::::: \n");

				map<string, WebSIChannelInfoFile> mapChannel;
				WebXMLParser::getInstance().parseChannelMapTxt(strChannelMapText, mapChannel);

				map<string, WebSIChannelInfoFile> updateMapChannel;
				makeNeedToUpdateMapWepgChannelInfo(mapChannel,updateMapChannel);

				L_DEBUG(TAG, "mapChannel Received[%u], Updated[%u]\n",mapChannel.size(), updateMapChannel.size());

				if (downloadAndMakeProgramInfosFromServer(updateMapChannel)) {
					sendEventProgramMapCompleted();

					uint32_t endtime =TimeConvertor::getInstance().getCurrentSecond();

					L_DEBUG(TAG, "WEB EPG Update : Running Time [%d]sec\n",	endtime - starttime);
				} else {
					L_INFO(TAG, "WEB EPG Update : NO UPDATED\n");
				}

				mapChannel.clear();
				updateMapChannel.clear();

				return;
			}
		}

	L_ERROR(TAG, "\n\n=== OOPS : Cannot Get ChannelMap Text[%s] ===\n\n",	mChannelMapTxtUrl.c_str());
}

void WebSIHandler::sendEventProgramMapCompleted() {
	SIDataManager& dataManager = SIDataManager::getInstance();
	vector<EventInfo*> listProgramInfo;
	listProgramInfo.clear();

	SIChangeEvent event;
	event.setEventType(SIChangeEvent::PROGRAM_MAP_COMPLETED);
	dataManager.saveProgramInfo("0.0.0", listProgramInfo, event, false, true);
}

void WebSIHandler::saveMapChannelInfo(
		map<string, WebSIChannelInfoFile> mapWepgChannelInfo) {
	for (map<string, WebSIChannelInfoFile>::iterator itr =
			mapWepgChannelInfo.begin(); itr != mapWepgChannelInfo.end();
			itr++) {
		string channel_uid = itr->first;
		WebSIChannelInfoFile info = itr->second;

		mMapWepgChannelInfo.insert(make_pair(channel_uid, info));
	}
}

void WebSIHandler::makeNeedToUpdateMapWepgChannelInfo(
		map<string, WebSIChannelInfoFile> mapWepgChannelInfo,
		map<string, WebSIChannelInfoFile> &updateMapChannel) {
	for (map<string, WebSIChannelInfoFile>::iterator itr =
			mapWepgChannelInfo.begin(); itr != mapWepgChannelInfo.end();
			itr++) {
		string channel_uid = itr->first;
		WebSIChannelInfoFile info = itr->second;

		map<string, WebSIChannelInfoFile>::iterator ii =
				mMapWepgChannelInfo.find(channel_uid);
		if (ii != mMapWepgChannelInfo.end()) {
			WebSIChannelInfoFile savedInfo = ii->second;

			if (savedInfo.update_seconds != info.update_seconds) {
				updateMapChannel.insert(make_pair(channel_uid, info));
				mMapWepgChannelInfo.erase(ii);
				mMapWepgChannelInfo.insert(make_pair(channel_uid, info));
			} else {
#ifdef __DEBUG_LOG_WEPG__
				L_INFO(TAG, "=== channelUid[%s] SAME VERSION [%s] === skipped ===\n",
						channel_uid.c_str(),
						TimeConvertor::getInstance().convertSeconds2Date(info.update_seconds).c_str());
#endif
			}
		} else {
			updateMapChannel.insert(make_pair(channel_uid, info));
			mMapWepgChannelInfo.insert(make_pair(channel_uid, info));
		}
	}
}

void WebSIHandler::start() {
	L_DEBUG(TAG, "=== CALLED ===\n");
	if (bWEPGStatus == 1 || bWEPGStatus == 2) {
		L_INFO(TAG, "=== WEB EPG PROCESSING : WAIT TO FINISH ===\n");
		return;
	}

	if (bStartTimer && bSuccessToMakeChannelMap) {
		L_INFO(TAG, "=== ALREADY STARTED ===\n");
		return;
	}

	bWEPGStatus = 1;
}

void WebSIHandler::stop() {
	bStop = true;
	bWEPGStatus = 0;
}

void WebSIHandler::update() {
	if (bWEPGStatus == 1 || bWEPGStatus == 2) {
		L_INFO(TAG, "=== WEB EPG PROCESSING : WAIT TO FINISH ===\n");
		return;
	}

	bWEPGStatus = 2;
}

void WebSIHandler::clear() {

}

bool WebSIHandler::checkConnectionToServer() {
	bool bConnection = false;
	int nTry = 0;
	string str_channel_map = "";
	long rc = -1;
	while (nTry++ < WEB_SI_RETRY_TIME) {
		if (httpget(mChannelXMLUrl.c_str(), &str_channel_map) == 200) {
			bConnection = true;
			L_INFO(TAG, "[%s] Connection OK\n", mChannelXMLUrl.c_str());
			break;
		}
	}

	if (bConnection == false) {
		L_INFO(TAG, "\n=== [%s] Connection FAILED ===\n\n",
				mChannelMapTxtUrl.c_str());
	}

	return bConnection;
}
