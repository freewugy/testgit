/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: eschoi $
 * $LastChangedDate: 2014-11-19 14:16:20 +0900 (수, 19 11월 2014) $
 * $LastChangedRevision: 1039 $
 * Description:
 * Note:
 *****************************************************************************/

#include "DummyEPGHandler.h"
#include <sstream>
#include <set>
#include <stdint.h>
#include "tinyxml/tinyxml.h"
#include "TimeConvertor.h"
#include "StringUtil.h"
#include "Logger.h"
#include "Timer.h"
#include "xmlUtil.h"
#include <unistd.h>
#include "SIDataManager.h"
#include "PSIMonitor/PSIMonitor.h"

static int nDummyEPGStatus = 0; // : NOT READY, 1 : FIRST START, 2 : UPDATE
static bool bStop = false;

pthread_mutex_t DummyEPGHandler::m_mutex =
#ifdef ANDROID
        PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
#else
        PTHREAD_MUTEX_INITIALIZER;
#endif

static const char* TAG = "DummyEPGHandler";

#ifdef ANDROID
static const char* CHANNELS_XML_FILE = "/data/lkn_home/run/si/channels.xml";
static const char* PROGRAMS_XML_FILE = "/data/lkn_home/run/si/programs.xml";
#else
static const char* CHANNELS_XML_FILE = "./out/channels.xml";
static const char* PROGRAMS_XML_FILE = "./out/programs.xml";
#endif

DummyEPGHandler::DummyEPGHandler()
{
    nDummyEPGStatus = 0;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&mTask, &attr, &DummyEPGHandler::dummyEPGHandlerThread, this);
    pthread_attr_destroy(&attr);
    return;
}

DummyEPGHandler::~DummyEPGHandler()
{
    pthread_join(mTask, NULL);
}

bool DummyEPGHandler::checkXMLFile()
{
    if(access(CHANNELS_XML_FILE, R_OK) != 0) {
        L_ERROR(TAG, "\n\n === OOPS : Cannot Access EPG-Channel File[%s] ===\n\n", CHANNELS_XML_FILE);
        return false;
    }

    if(access(PROGRAMS_XML_FILE, R_OK) != 0) {
        L_DEBUG(TAG, "\n\n === OOPS : Cannot Access EPG-Program File[%s] ===\n\n", PROGRAMS_XML_FILE);
        return false;
    }

    L_INFO(TAG, "XML FILE-[channel - %s, program - %s ]Access OK\n", CHANNELS_XML_FILE, PROGRAMS_XML_FILE);
    return true;
}

void DummyEPGHandler::start()
{
    if(nDummyEPGStatus == 1 || nDummyEPGStatus == 2) {
        L_INFO(TAG, "=== DummyEPGHandler PROCESSING : WAIT TO FINISH ===\n");
        return;
    }

    nDummyEPGStatus = 1;
}

void DummyEPGHandler::update()
{
    if(nDummyEPGStatus == 1 || nDummyEPGStatus == 2) {
        L_INFO(TAG, "=== DummyEPGHandler PROCESSING : WAIT TO FINISH ===\n");
        return;
    }

    nDummyEPGStatus = 2;
}

void DummyEPGHandler::stop()
{
    bStop = true;
    if(nDummyEPGStatus == 0) {
        pthread_join(mTask, NULL);
    }
    nDummyEPGStatus = 0;

}

void* DummyEPGHandler::dummyEPGHandlerThread(void* param)
{
    while (1) {
        while (nDummyEPGStatus > 0) {
            if(nDummyEPGStatus == 1) {
                ((DummyEPGHandler *) param)->startProcess(true);
                nDummyEPGStatus = 0;
            } else if(nDummyEPGStatus == 2) {
                ((DummyEPGHandler *) param)->startProcess(false);
                nDummyEPGStatus = 0;
            }
        }
        siMiliSleep(500);
    }

    return NULL;
}

void DummyEPGHandler::startProcess(bool bUpdated)
{
    L_INFO(TAG, "=== DummyEPGHandler START ===\n");
    bStop = false;
    int nTry = 0;

    if(bUpdated)
        loadChannels();

    while(nTry ++ < 20 && TimeConvertor::getInstance().isValidSystemTime() == false) {
        L_INFO(TAG, "=== WAIT FOR RECEIVING NETWORK TIME ===\n");
        usleep(1000 * 1000);
    }

    L_DEBUG(TAG, "Now Time [%s]\n",
            TimeConvertor::getInstance().convertSeconds2Date(TimeConvertor::getInstance().getCurrentSecond()).c_str());

    loadPrograms();
}

void DummyEPGHandler::loadChannels()
{
    L_INFO(TAG, "=== Load Channels ===\n");

    SIDataManager& dataManager = SIDataManager::getInstance();
    dataManager.ready();

    list<ServiceInfo*> listChannelInfo;
    TiXmlDocument doc(CHANNELS_XML_FILE);
    doc.LoadFile();

    TiXmlNode* channels = doc.FirstChild("channels");

    for (TiXmlNode* node = channels->FirstChild("channel"); node; node = node->NextSibling("channel")) {
        ServiceInfo* sInfo = new ServiceInfo();
        TiXmlElement* elm = node->ToElement();
        sInfo->service_id = getIntFromXMLElement(elm, "sid");
        sInfo->service_uid = getStringFromXMLElement(elm, "suid");
        sInfo->service_type = getIntFromXMLElement(elm, "ctype");
        sInfo->channel_number = getIntFromXMLElement(elm, "ch");
        sInfo->service_name = getStringFromXMLElement(elm, "cname");
        sInfo->IP_address = getIPAddressFromXMLElement(elm, "ip_address");
        sInfo->TS_port_number = getIntFromXMLElement(elm, "port");

        sInfo->IP_address = getIPAddressFromXMLElement(elm, "ip_address");
        sInfo->TS_port_number = getIntFromXMLElement(elm, "port");

        sInfo->genre_code = getIntFromXMLElement(elm, "genre");
        sInfo->running_status = 4;//getIntFromXMLElement(elm, "running_status");
        sInfo->local_area_code = getIntFromXMLElement(elm, "local_area_code");
        sInfo->frequency = getIntFromXMLElement(elm, "freq");

        string modulation = getStringFromXMLElement(elm, "demod");
        if(modulation.compare("qam64") == 0) {
        	 sInfo->modulation = 3;
		} else if(modulation.compare("qam256") == 0) {
			sInfo->modulation = 5;
		}
        elementary_stream esInfo;
        esInfo.stream_pid = getIntFromXMLElement(elm, "vpid");
        esInfo.stream_type = getIntFromXMLElement(elm, "vst");
        sInfo->list_elementary_stream.push_back(esInfo);

        esInfo.stream_pid = getIntFromXMLElement(elm, "apid");
        esInfo.stream_type = getIntFromXMLElement(elm, "ast");
        sInfo->list_elementary_stream.push_back(esInfo);

        esInfo.stream_pid = getIntFromXMLElement(elm, "apid2");
        esInfo.stream_type = getIntFromXMLElement(elm, "ast2");
        if(esInfo.stream_pid > 0 && esInfo.stream_pid < 0x1fff)
            sInfo->list_elementary_stream.push_back(esInfo);

        sInfo->pcr_pid = getIntFromXMLElement(elm, "ppid");
        sInfo->resolution = getIntFromXMLElement(elm, "res");
        sInfo->pay_channel = getIntFromXMLElement(elm, "pay");
        sInfo->pay_ch_sample_sec = getIntFromXMLElement(elm, "sample");
        sInfo->channel_image_url = getStringFromXMLElement(elm, "image");

        CAInfo caInfo;
        caInfo.CA_system_ID = getIntFromXMLElement(elm, "ca_id");
        caInfo.CA_PID = getIntFromXMLElement(elm, "ca_pid");
        if(caInfo.CA_system_ID != 0 && caInfo.CA_PID != 0)
            sInfo->listCAInfo.push_back(caInfo);

        listChannelInfo.push_back(sInfo);

        node->Clear();
    }

    channels->Clear();
    L_DEBUG(TAG, "END : channel list size [%u]\n", listChannelInfo.size());

    SIChangeEvent event;
    event.setEventType(SIChangeEvent::CHANNEL_MAP_COMPLETED);
    event.setChannelVersion(1);
    if(!listChannelInfo.empty()) {
        event.setFoundChannel(-1);
        dataManager.saveChannelInfo(listChannelInfo, event, 1,true);
    }

    doc.Clear();
}

int DummyEPGHandler::getDifference(uint32_t oldest_time)
{
    int diff = 0;

    uint32_t now_time = ((TimeConvertor::getInstance().getCurrentSecond()) / (60 * 60)) * (60 * 60);
    // 분초 삭제
    diff = now_time - oldest_time - 3 * 60 * 60;

    return diff;
}

void DummyEPGHandler::changeTimeInfoByToday(vector<EventInfo*> &listEventInfo, uint32_t oldest_time)
{
    if(TimeConvertor::getInstance().isValidSystemTime() == false) {
        L_INFO(TAG, "=== NOT YET GET Network Time ===\n");
        return;
    }

    int diff = getDifference(oldest_time);

    for (vector<EventInfo*>::iterator itr = listEventInfo.begin(); itr != listEventInfo.end(); itr++) {
        EventInfo* eInfo = (EventInfo*) (*itr);

        eInfo->start_time += (uint32_t)(diff);
        eInfo->end_time = eInfo->start_time + eInfo->duration;
    }

    return;
}

struct comp_eventinfo_by_starttime {
    bool operator()(EventInfo * n1, EventInfo * n2)
    {
        return n1->start_time < n2->start_time;
    }
};

void DummyEPGHandler::loadPrograms()
{
    L_INFO(TAG, "=== Load Programs ===\n");
    SIDataManager& dataManager = SIDataManager::getInstance();
    dataManager.ready();

    string strXML = get_file_contents(PROGRAMS_XML_FILE);

    TiXmlDocument doc(PROGRAMS_XML_FILE);
    doc.LoadFile();

    TiXmlNode* programs = doc.FirstChild("programs");

    for (TiXmlNode* node = programs->FirstChild("program"); node; node = node->NextSibling("program")) {
        string service_uid = node->ToElement()->Attribute("service_uid");
        vector<EventInfo*> listEventInfo;

        if(node->NoChildren()) {
            L_DEBUG(TAG, "service_uid[%s] No Children\n", service_uid.c_str());
            continue;
        }

        for (TiXmlNode* pg = node->FirstChild("EventInfo"); pg; pg = pg->NextSibling("EventInfo")) {
            EventInfo* eInfo = new EventInfo();
            TiXmlElement* elm = pg->ToElement();
            eInfo->event_id = getIntFromXMLElement(elm, "event_id");

            eInfo->service_uid = getStringFromXMLElement(elm, "service_uid");
            eInfo->service_id = getIntFromXMLElement(elm, "service_id");
            eInfo->event_name = getStringFromXMLElement(elm, "event_name");

            eInfo->event_short_description = getStringFromXMLElement(elm, "event_short_description");
            eInfo->event_description = getStringFromXMLElement(elm, "event_description");
            eInfo->running_status = getIntFromXMLElement(elm, "running_status");
            eInfo->free_CA_mode = getIntFromXMLElement(elm, "free_CA_mode");
            eInfo->rating = getIntFromXMLElement(elm, "rating");
            eInfo->start_time = getIntFromXMLElement(elm, "start_time");
            eInfo->end_time = getIntFromXMLElement(elm, "end_time");
            eInfo->duration = eInfo->end_time - eInfo->start_time; //getIntFromXMLElement(elm, "duration");

            eInfo->image_url = getStringFromXMLElement(elm, "image_url");

            eInfo->director_name = getStringFromXMLElement(elm, "director_name");

            eInfo->actors_name = getStringFromXMLElement(elm, "actors_name");
            eInfo->price_text = getStringFromXMLElement(elm, "price_text");
            eInfo->dolby = getIntFromXMLElement(elm, "dolby");
            eInfo->audio = getIntFromXMLElement(elm, "audio");
            eInfo->resolution = getIntFromXMLElement(elm, "resolution");
            eInfo->content_nibble_level_1 = getIntFromXMLElement(elm, "content_nibble_level_1");
            eInfo->content_nibble_level_2 = getIntFromXMLElement(elm, "content_nibble_level_2");
            eInfo->user_nibble_1 = getIntFromXMLElement(elm, "user_nibble_1");
            eInfo->user_nibble_2 = getIntFromXMLElement(elm, "user_nibble_2");
            eInfo->dvs = getIntFromXMLElement(elm, "dvs");

            listEventInfo.push_back(eInfo);

            pg->Clear();
        }

        // sort by start_time
        std::sort(listEventInfo.begin(), listEventInfo.end(), comp_eventinfo_by_starttime());

        // get oldest time
        EventInfo* eInfoOldest = listEventInfo.at(0);
        // get difference time between oldest time and now time
        // modify all program running time
        changeTimeInfoByToday(listEventInfo, eInfoOldest->start_time);

        SIChangeEvent event;
        event.setEventType(SIChangeEvent::PROGRAM_COMPLETED);
        if(!listEventInfo.empty()) {
            dataManager.saveProgramInfo(service_uid, listEventInfo, event, false, false);
        }

        node->Clear();
    }

    programs->Clear();

    vector<EventInfo*> listDummyProgramInfo;
    listDummyProgramInfo.clear();
    SIChangeEvent event;
    event.setEventType(SIChangeEvent::PROGRAM_MAP_COMPLETED);
    event.setFoundChannel(-1);
    dataManager.saveProgramInfo("", listDummyProgramInfo, event, false, false);

    doc.Clear();
}
