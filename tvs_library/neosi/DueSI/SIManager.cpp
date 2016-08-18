/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: eschoi $
 * $LastChangedDate: 2014-10-20 18:00:23 +0900 (월, 20 10월 2014) $
 * $LastChangedRevision: 1032 $
 * Description:
 * Note:
 *****************************************************************************/

#include "SIManager.h"
#include "Logger.h"
#include "SIDataManager.h"
#include "Parser.h"
#include "TimeConvertor.h"
#include "StringUtil.h"
#include <unistd.h>
#include <stdio.h>
#include <algorithm>
#include "version.h"
#include "./DummyEPG/DummyEPGHandler.h"
//#include "WebSI/WebSIHandler.h"
#include "ScanManager.h"

static bool bSIFactoryTestMode = false;
static const char* TAG = "SIManager";

#define SCAN_PROCESSING 1

DummyEPGHandler* pDummyEPGHandler = NULL;

SIManager& SIManager::getInstance()
{
    static SIManager instance;
    return instance;
}

void SIManager::initialize()
{
    L_INFO(TAG, "########################################################\n");
    L_INFO(TAG, "### DVBSI Version Check : v%s.r%s ###\n", VERSION, REVISION());
    L_INFO(TAG, "########################################################\n");
}

list<ServiceInfo*> SIManager::getChannelList(int channel_type)
{
    list<ServiceInfo*> listServiceInfo;
    if(bSIFactoryTestMode) {
        ScanManager& scan = ScanManager::getInstance();
        listServiceInfo = scan.getChList();
    } else {
		SIDataManager& data = SIDataManager::getInstance();
		data.getChannelList((CHANNEL_TYPE) channel_type, listServiceInfo);

		L_INFO(TAG, "Channel Type[%d], Size[%u]\n", channel_type, listServiceInfo.size());
	}
    return listServiceInfo;
}

list<ServiceInfo*> SIManager::getChannelList()
{
    list<ServiceInfo*> listServiceInfo;
    if(bSIFactoryTestMode) {
        ScanManager& scan = ScanManager::getInstance();
        listServiceInfo = scan.getChList();
    } else {
        SIDataManager& data = SIDataManager::getInstance();
        data.getChannelsInfo(listServiceInfo);
    }
#ifdef __DEBUG_LOG_SIM__
    L_INFO(TAG, "Channel List Size is %u\n", listServiceInfo.size());
#endif
    DL_INFO(TAG, "L=%u, P=%s\n", listServiceInfo.size(), "all");
    return listServiceInfo;
}

EventInfo* SIManager::getCurrentProgram(int channelNumber)
{
    EventInfo* eventInfo = new EventInfo;
    SIDataManager& data = SIDataManager::getInstance();
    int result = data.getCurrentProgramInfo(channelNumber, &eventInfo);
    if(result > 0) {
        L_DEBUG(TAG, "channelNo[%d] : event id[%u] starttime[%s], endtime[%s]\n", channelNumber, eventInfo->event_id,
                TimeConvertor::getInstance().convertSeconds2Date(eventInfo->start_time).c_str(),
                TimeConvertor::getInstance().convertSeconds2Date(eventInfo->end_time).c_str());
        return eventInfo;
    }

    return NULL;
}

EventInfo* SIManager::getCurrentProgram(string channelUid)
{
    EventInfo* eventInfo = new EventInfo;
    SIDataManager& data = SIDataManager::getInstance();
    int result = data.getCurrentProgramInfo(channelUid, &eventInfo);
    if(result > 0) {
        L_DEBUG(TAG, "channelUid[%s] : event id[%u] starttime[%s], endtime[%s]\n", channelUid.c_str(), eventInfo->event_id,
                TimeConvertor::getInstance().convertSeconds2Date(eventInfo->start_time).c_str(),
                TimeConvertor::getInstance().convertSeconds2Date(eventInfo->end_time).c_str());
        return eventInfo;
    }
	else
		delete eventInfo;

    return NULL;
}

list<EventInfo*> SIManager::getProgramList(int channelNumber)
{
    list<EventInfo*> listEventInfo;
    SIDataManager& data = SIDataManager::getInstance();
    data.getProgramList(channelNumber, listEventInfo);
    L_INFO(TAG, "Channel No[%d], Program Size [%u]\n", channelNumber, listEventInfo.size());
    return listEventInfo;
}

list<EventInfo*> SIManager::getProgramList(string channelUid)
{
    list<EventInfo*> listEventInfo;
    SIDataManager& data = SIDataManager::getInstance();
    data.getProgramList(channelUid, listEventInfo);
    L_INFO(TAG, "Channel UID[%s], Program Size [%u]\n", channelUid.c_str(), listEventInfo.size());

    return listEventInfo;
}

list<EventInfo*> SIManager::getProgramListByIndex(int channelNumber, int startIndex, int reqCount)
{
    list<EventInfo*> listEventInfo;
    SIDataManager& data = SIDataManager::getInstance();
    data.getProgramListbyIndex(channelNumber, startIndex, reqCount, listEventInfo);
    L_INFO(TAG, "Channel Number[%d], Program Size [%u], startIndex[%d], reqCount[%d]\n", channelNumber, listEventInfo.size(), startIndex, reqCount);
    return listEventInfo;

}

list<EventInfo*> SIManager::getProgramListByIndex(string channelUid, int startIndex, int reqCount)
{
    list<EventInfo*> listEventInfo;
    SIDataManager& data = SIDataManager::getInstance();
    data.getProgramListbyIndex(channelUid, startIndex, reqCount, listEventInfo);
    L_INFO(TAG, "Channel UID[%s] : Program Size [%u], startIndex[%d], reqCount[%d]\n", channelUid.c_str(), listEventInfo.size(), startIndex,
            reqCount);
    return listEventInfo;

}

list<EventInfo*> SIManager::getProgramListByTime(int channelNumber, int startTime, int endTime)
{
    list<EventInfo*> listEventInfo;
    SIDataManager& data = SIDataManager::getInstance();
    data.getProgramListbyTime(channelNumber, startTime, endTime, listEventInfo);
    L_INFO(TAG, "Channel Number[%d], Program Size [%u], Time[%s ~ %s]\n", channelNumber, listEventInfo.size(),
            TimeConvertor::getInstance().convertSeconds2Date(startTime).c_str(), TimeConvertor::getInstance().convertSeconds2Date(endTime).c_str());
    return listEventInfo;

}

list<EventInfo*> SIManager::getProgramListByTime(string channelUid, int startTime, int endTime)
{
    list<EventInfo*> listEventInfo;
    SIDataManager& data = SIDataManager::getInstance();
    data.getProgramListbyTime(channelUid, startTime, endTime, listEventInfo);

    L_INFO(TAG, "Channel UID[%s], Program Size [%u], Time[%s ~ %s]\n", channelUid.c_str(), listEventInfo.size(),
            TimeConvertor::getInstance().convertSeconds2Date(startTime).c_str(), TimeConvertor::getInstance().convertSeconds2Date(endTime).c_str());
    return listEventInfo;
}

list<EventInfo*> SIManager::getProgramListByName(int channelNumber, int currentTime, string programName, bool DesFlag)
{
    list<EventInfo*> listEventInfo;
    SIDataManager& data = SIDataManager::getInstance();
    data.getProgramListbyName(channelNumber, currentTime, programName, DesFlag, listEventInfo);
    L_INFO(TAG, "Channel Number[%d], Name[%s] Program Size [%u]\n", channelNumber, programName.c_str(), listEventInfo.size());
    return listEventInfo;
}

list<EventInfo*> SIManager::getProgramListByName(string channelUid, int currentTime, string programName, bool DesFlag)
{
    list<EventInfo*> listEventInfo;
    SIDataManager& data = SIDataManager::getInstance();
    data.getProgramListbyName(channelUid, currentTime, programName, DesFlag, listEventInfo);
    L_INFO(TAG, "Channel UID[%s], Name[%s] Program Size [%u]\n", channelUid.c_str(), programName.c_str(), listEventInfo.size());
    return listEventInfo;
}

list<EventInfo*> SIManager::getProgramListBySeries(string channelUid, int requestTime, string programName, int seasonNo, int episodeNo)
{
    list<EventInfo*> listEventInfo;
    SIDataManager& data = SIDataManager::getInstance();
    data.getProgramListbySeries(channelUid, requestTime, programName, seasonNo, episodeNo, listEventInfo);
    L_INFO(TAG, "Channel UID[%s], Program Size [%u] [%s s.%u ep.%u]\n", channelUid.c_str(), listEventInfo.size(),
            TimeConvertor::getInstance().convertSeconds2Date(requestTime).c_str(), seasonNo, episodeNo);
    return listEventInfo;
}

list<ProductInfo> SIManager::getProductInfoList()
{
    list<ProductInfo> listProductInfo;
    SIDataManager& data = SIDataManager::getInstance();
    data.getProductInfoList(listProductInfo);
    L_INFO(TAG, "ProductInfo Size [%u]\n", listProductInfo.size());
    return listProductInfo;
}

void SIManager::parsingEpgUri(string epgUri)
{
    L_DEBUG(TAG, "epgUri : [%s]\n", epgUri.c_str());
    vector < string > listElm;
    splitStringByDelimiter(epgUri, "?", listElm);

    if(listElm.size() != 2) {
        L_ERROR(TAG, "\n=== WRONG EPG URI FORMAT ===\n");
        return;
    }

    m_provuri.bandwidth = 0;
    m_provuri.symbolrate = 0;
    string header = listElm.at(0);
    string val = listElm.at(1);

    listElm.clear();
    splitStringByDelimiter(header, "://", listElm);

    m_provuri.wepgChannelMapFile = "";
    m_provuri.ap = listElm.at(0);
    string token = listElm.at(1);
    if(m_provuri.ap.compare("i") == 0) { // ip
        vector < string > listData;
        splitStringByDelimiter(token, ":", listData);
        if(listData.size() != 2) {
            L_ERROR(TAG, "WRONG FORMAT : %s\n", token.c_str());
        } else {
            m_provuri.provIP = listData.at(0);
            m_provuri.provPort = atoi(listData.at(1).c_str());
        }
    } else if(m_provuri.ap.compare("c") == 0) { // cable
        vector < string > listData;
        splitStringByDelimiter(token, ":", listData);
        if(listData.size() != 2) {
            L_ERROR(TAG, "WRONG FORMAT : %s\n", token.c_str());
        }
        m_provuri.demodType = listData.at(0);
        m_provuri.freqkHz = atoi(listData.at(1).c_str());

        L_DEBUG(TAG, "demod[%s], frequency[%d]\n", m_provuri.demodType.c_str(), m_provuri.freqkHz);
    }

    listElm.clear();
    splitStringByDelimiter(val, "&", listElm);
    map < string, string > mapValue;
    for (vector<string>::iterator itr = listElm.begin(); itr != listElm.end(); itr++) {
        string token = (string)(*itr);
        vector < string > listStr;
        splitStringByDelimiter(token, "=", listStr);
        if(listStr.size() != 2) {
            L_ERROR(TAG, "WRONG FORMAT : %s\n", token.c_str());
            continue;
        }
        mapValue.insert(make_pair(listStr.at(0), listStr.at(1)));
    }

    for (map<string, string>::iterator itr = mapValue.begin(); itr != mapValue.end(); itr++) {
        string key = itr->first;
        string value = itr->second;
        if(key.compare("pn") == 0) {
            m_provuri.provName = value;
            L_DEBUG(TAG, "provName[%s]\n", m_provuri.provName.c_str());
        } else if(key.compare("la") == 0) {
            vector < string > la;
            splitStringByDelimiter(value, "|", la);
            if(la.size() < 3) {
                L_ERROR(TAG, "OOPS : LONG DATA : WRONG LOCAL AREA CODE FORMAT : %s\n", value.c_str());
                continue;
            }

            m_provuri.localAreaCode.local_area_code_1 = atoi(la.at(0).c_str());
            m_provuri.localAreaCode.local_area_code_2 = atoi(la.at(1).c_str());
            m_provuri.localAreaCode.local_area_code_3 = atoi(la.at(2).c_str());
        } else if(key.compare("wepg") == 0) {
            m_provuri.wepgChannelMapFile = value;
        } else if(key.compare("websi") == 0) {
            m_provuri.webSIURI = value;
        } else if(key.compare("sr") == 0) {
            m_provuri.symbolrate = atoi(value.c_str());
        } else if(key.compare("bw") == 0) {
            m_provuri.bandwidth = atoi(value.c_str());
        } else if(key.compare("utc") == 0) {
            uint8_t utc_offset = atoi(value.c_str());
//            ParserFactory::createInstance("")->setUTCOffset(utc_offset);
        } else if(key.compare("xml") == 0) {
            if(value.compare("y") == 0) {
                m_provuri.bUseXML = true;
            } else {
                m_provuri.bUseXML = false;
            }
        }
    }

    if(m_provuri.ap.compare("i") == 0) { // ip
        L_INFO(TAG, "%s\n : IP[%s:%d] : LocalAreaCode[%u|%u|%u]\n", m_provuri.provName.c_str(), m_provuri.provIP.c_str(), m_provuri.provPort,
                m_provuri.localAreaCode.local_area_code_1, m_provuri.localAreaCode.local_area_code_2, m_provuri.localAreaCode.local_area_code_3);
    } else if(m_provuri.ap.compare("c") == 0) { // cable
        L_INFO(TAG, "provName[%s], Frequency[%s:%d]\n", m_provuri.provName.c_str(), m_provuri.demodType.c_str(), m_provuri.freqkHz);
        L_INFO(TAG, "symbolrate[%d] bandwidth[%d]\n", m_provuri.symbolrate, m_provuri.bandwidth);
    }

    if(m_provuri.wepgChannelMapFile.compare("unknown") != 0) {
        L_INFO(TAG, "WEB EPG URL[%s]\n", m_provuri.wepgChannelMapFile.c_str());
    }

    if(m_provuri.webSIURI.compare("unknown") != 0) {
        L_INFO(TAG, "WEB SI URL[%s]\n", m_provuri.webSIURI.c_str());
    }

}

void SIManager::Start(string epgUri, int mode)
{
	bSIFactoryTestMode = false;
    L_DEBUG(TAG, "=== START === mode [%d]\n",mode);
    parsingEpgUri(epgUri);

    SIDataManager& sidm = SIDataManager::getInstance();
    sidm.initDB();
    sidm.setLocalAreaCode(m_provuri.localAreaCode);

    if(m_provuri.bUseXML) {
        if(pDummyEPGHandler == NULL) {
            pDummyEPGHandler = new DummyEPGHandler();
        }

        if(pDummyEPGHandler->checkXMLFile()) {
            pDummyEPGHandler->start();
        }
    } else {
        if(mode == 1) {
			SIDataManager& sidm = SIDataManager::getInstance();
			sidm.initDB();
			sidm.setLocalAreaCode(m_provuri.localAreaCode);
			ScanManager& scan = ScanManager::getInstance();
			scan.initialize(m_provuri.provName);
			scan.setTuneParam(m_provuri);
			if(m_provuri.wepgChannelMapFile.length() > 10)
				scan.setWepgChannelMapFile(m_provuri.wepgChannelMapFile);
			scan.TuneHomeTS();
			scan.start();
		} else if(mode == 2) {
			SIDataManager& sidm = SIDataManager::getInstance();
			sidm.initDB();
			sidm.setLocalAreaCode(m_provuri.localAreaCode);
			ScanManager& scan = ScanManager::getInstance();
			scan.initialize(m_provuri.provName);
			scan.setTuneParam(m_provuri);
			if(m_provuri.wepgChannelMapFile.length() > 10)
				scan.setWepgChannelMapFile(m_provuri.wepgChannelMapFile);
			scan.start();
			scan.setChMapNotify();
		}

    }
    L_DEBUG(TAG, "=== END ===\n");
}

void SIManager::AutoScanStart(string epgUri, int startFreqMHz, int endFreqMHz, int tunerId)
{
    bSIFactoryTestMode = true;
    parsingEpgUri(epgUri);

    ScanManager& scan = ScanManager::getInstance();
    scan.initialize(m_provuri.provName);
    scan.setTuneParam(m_provuri);
    scan.setAutoFreq(startFreqMHz, endFreqMHz);

    SIDataManager& sidm = SIDataManager::getInstance();
    sidm.setScanMode(SCAN_PROCESSING);
    if(tunerId)
        scan.setTunerInvert(1);
    else
        scan.setTunerInvert(0);
    scan.scanStart(true, false/*true*/); // eschoi_20140821 | not fixed moduration for auto (64 & 256 QAM)
}

void SIManager::ManualScanStart(string epgUri, int freqMHz, int tunerId)
{
    bSIFactoryTestMode = true;
    parsingEpgUri(epgUri);

    ScanManager& scan = ScanManager::getInstance();
    scan.initialize(m_provuri.provName);
    scan.setTuneParam(m_provuri);
    scan.setManualFreq(freqMHz);

    SIDataManager& sidm = SIDataManager::getInstance();
    sidm.setScanMode(SCAN_PROCESSING);
    if(tunerId)
        scan.setTunerInvert(1);
    else
        scan.setTunerInvert(0);
    scan.scanStart(false, true);
}

void SIManager::ManualChannelScanStart(string epgUri, int freqMHz, int networkId)
{
    bSIFactoryTestMode = true;
    parsingEpgUri(epgUri);

    SIDataManager& sidm = SIDataManager::getInstance();
    sidm.setScanMode(SCAN_PROCESSING);

    ScanManager& scan = ScanManager::getInstance();
    scan.initialize(m_provuri.provName);
    scan.setTuneParam(m_provuri);
    scan.setManualFreq(freqMHz);
    scan.setTunerInvert(0);
    scan.scanStartManualChannel(networkId);
}

void SIManager::CleanUpFactoryTest()
{
    bSIFactoryTestMode = false;
    ScanManager& scan = ScanManager::getInstance();
    scan.initialize(m_provuri.provName);
    scan.cleanupFactoryTest();
}

string SIManager::getDefaultChannel()
{
    SIDataManager& sidm = SIDataManager::getInstance();
    return sidm.getDefaultChannelUid();
}

void SIManager::scanStart(string epgUri, int mode)
{
//		SIChangeEvent event;
//		event.setReceived(0);
//		event.setToBeReceived(0);
//		event.setEventType(SIChangeEvent::SCAN_READY);
//		notifyEvent(&event);
	bSIFactoryTestMode = false;
	parsingEpgUri(epgUri);

	if(mode == 1 || mode == 2) {
		SIDataManager& sidm = SIDataManager::getInstance();
		sidm.setScanMode(SCAN_PROCESSING);
		sidm.initDB();
		sidm.setLocalAreaCode(m_provuri.localAreaCode);


		ScanManager& scan = ScanManager::getInstance();
		scan.initialize(m_provuri.provName);
		scan.setTuneParam(m_provuri);

		if(m_provuri.bUseXML) {
			scan.setDummyEpgMode(true);
		}
		L_DEBUG(TAG, "=== START === wepg length [%d]\n",m_provuri.wepgChannelMapFile.length());

		if(m_provuri.wepgChannelMapFile.length() > 10 && !m_provuri.bUseXML)
		{
			L_DEBUG(TAG,"\n");
			scan.setWepgChannelMapFile(m_provuri.wepgChannelMapFile);
		}

		scan.scanStart();
	}
}

void SIManager::scanStop()
{
    ScanManager::getInstance().scanStop();
}

void SIManager::Stop()
{
    ScanManager::getInstance().stop();
}

void SIManager::receivedEvent(void* e)
{
    SIChangeEvent* event;
    event = (SIChangeEvent*) e;

    switch (event->getEventType()) {
        case SIChangeEvent::CHANNEL_COMPLETED:
        case SIChangeEvent::CHANNEL_UPDATED:
        case SIChangeEvent::CHANNEL_ADD_EVENT:
        case SIChangeEvent::CHANNEL_MODIFY_EVENT:
        case SIChangeEvent::CHANNEL_REMOVE_EVENT:
        case SIChangeEvent::CHANNEL_MAP_READY:
        case SIChangeEvent::CHANNEL_MAP_COMPLETED:
        case SIChangeEvent::PROGRAM_COMPLETED:
        case SIChangeEvent::PROGRAM_UPDATED:
        case SIChangeEvent::PROGRAM_ADD_EVENT:
        case SIChangeEvent::PROGRAM_MODIFY_EVENT:
        case SIChangeEvent::PROGRAM_REMOVE_EVENT:
        case SIChangeEvent::PROGRAM_PROCESSING:
            break;
        case SIChangeEvent::PROGRAM_MAP_COMPLETED: {
            if(pDummyEPGHandler != NULL && m_provuri.bUseXML) {
                delete pDummyEPGHandler;
                pDummyEPGHandler = NULL;
            }
            break;
        }
        case SIChangeEvent::ERROR_HAPPENED:
            break;
        default:
            break;
    }
    notifyEvent(event);

}

void SIManager::addUpdateEventListener(SIChangeEventListener* listener)
{
    m_listeners.insert(listener);
    SIDataManager& data = SIDataManager::getInstance();
    data.registerListener(this);
}

void SIManager::removeUpdateEventListener(SIChangeEventListener* listener)
{
    std::set<SIChangeEventListener *>::const_iterator iter = m_listeners.find(listener);
    if(iter != m_listeners.end()) {
        m_listeners.erase(iter);
    } else {
        // handle the case
        L_INFO(TAG, "Could not unregister the specified listener object as it is not registered.");
    }
    return;
}

void SIManager::notifyEvent(SIChangeEvent* event)
{
    set<SIChangeEventListener*>::iterator itr;
    for (itr = m_listeners.begin(); itr != m_listeners.end(); itr++)
        (*itr)->receiveSIChangeEvent(event);
    return;
}

void SIManager::setNetworkConnectionInfo(bool isConnected)
{
	L_INFO(TAG, "setNetworkConnectionInfo [%d]\n",isConnected);
    Parser::getInstance().setNetworkConnectionInfo(isConnected);
}
