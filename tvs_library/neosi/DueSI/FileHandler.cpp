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

#include "FileHandler.h"
#include "Logger.h"

FileHandler::~FileHandler() {
}

int FileHandler::saveEPGInfo(map<string, ServiceInfo> mapServiceInfo,map<string, list<EventInfo> > mapEventInfo)
{
	int res = 0;

	// 예외인 경우를 처리해야 함
	if(mapServiceInfo.size() == 0) {

	}
	ofstream epg_file("epg.xml");

	epg_file << "<EPG>" << endl;

	map<string, ServiceInfo>::iterator itr;
	for(itr = mapServiceInfo.begin(); itr != mapServiceInfo.end(); itr++)
	{
		ServiceInfo serviceInfo = itr->second;
		epg_file << "<CHANNEL" << serviceInfo.toXMLString_EPG() << ">" << endl;

		map<string, list<EventInfo> >::iterator ii;
		ii = mapEventInfo.find(serviceInfo.service_uid.c_str());

		if(ii == mapEventInfo.end())
		{
			epg_file << "</CHANNEL>" << endl;
			break;
		}

		list<EventInfo> listEventInfo = ii->second;
		// find service_uid from m_map_eventInfo
		list<EventInfo>::iterator itr;
		for(itr = listEventInfo.begin(); itr != listEventInfo.end(); itr++) {
			epg_file << itr->toXMLString_EPG() << endl;
		}
		epg_file << "</CHANNEL>" << endl;
	}

	epg_file << "</EPG>" << endl;
	epg_file.close();
	return res;
}


int FileHandler::saveServiceInfo(map<string, ServiceInfo> mapServiceInfo) {
	int res = 0;

	// 예외인 경우를 처리해야 함
	if(mapServiceInfo.size() == 0) {

	}
#ifdef ANDROID
	ofstream channels_file("/data/si/channels.xml");
#else
	ofstream channels_file("channels.xml");
#endif

	channels_file << "<Channels_Info>" << endl;
	map<string, ServiceInfo>::iterator itr;
	for(itr = mapServiceInfo.begin(); itr != mapServiceInfo.end(); itr++) {
		ServiceInfo serviceInfo = itr->second;

#ifdef ANDROID
		channels_file << serviceInfo.toString() << endl;
		channels_file << serviceInfo.toXMLString() << endl;
#else
		channels_file << serviceInfo.toString() << endl;
#endif
	}

	channels_file << "</Channels_Info>" << endl;
	channels_file.close();
	return res;
}

int FileHandler::saveEventInfo(map<string, list<EventInfo> > mapEventInfo) {
#ifdef ANDROID
	ofstream programs_file("/data/si/programs.xml");
#else
	ofstream programs_file("programs.xml");
#endif

	programs_file << "<Programs_Info>" << endl;
	map<string, list<EventInfo> >::iterator ii;
	for(ii = mapEventInfo.begin(); ii != mapEventInfo.end(); ii++) {
		list<EventInfo> listEventInfo = ii->second;
		// find service_uid from m_map_eventInfo
		list<EventInfo>::iterator itr;
		for(itr = listEventInfo.begin(); itr != listEventInfo.end(); itr++) {
#ifdef ANDROID
			programs_file << itr->toString() << endl;
			programs_file << itr->toXMLString() << endl;
#else
			programs_file << itr->toString() << endl;
#endif
		}
	}
	programs_file << "</Programs_Info>" << endl;
	programs_file.close();
	return -1;
}
