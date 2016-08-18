/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: loneowl $
 * $LastChangedDate: 2014-08-27 13:24:32 +0900 (수, 27 8월 2014) $
 * $LastChangedRevision: 1002 $
 * Description:
 * Note:
 *****************************************************************************/

#include <sstream>
#include <string.h>
#include "include/EventInfo.h"
#include "Logger.h"
#include "StringUtil.h"
#include "TimeConvertor.h"

static const char* TAG = "EventInfo";

EventInfo::EventInfo()
        : service_uid(""), service_id(0), event_id(0), event_name(""), event_short_description(""), event_description(""), running_status(0), free_CA_mode(
                0), rating(0), start_time(0), end_time(0), duration(0), image_url(""), director_name(""), actors_name(""), price_text(""), dolby(
                NO_INFO_DOLBY), // 0: no info, 'N': not dolby, 'D': dolby support
        audio(NO_INFO_AUDIO), // 0: no info, 'M': mono, 'S': stereo, 'A': AC-3
        resolution(NO_INFO_RESOLUTION), // 0: no info, 'S': SD, 'H': HD
        content_nibble_level_1(0), content_nibble_level_2(0), user_nibble_1(0), user_nibble_2(0), caption(NO_CAPTION), dvs(NO_DVS), bCompound(false)
{
    size = 0;
    transport_stream_id = 0;
    original_network_id = 0;
    ETM_location = 0;
}

EventInfo::~EventInfo()
{
    // TODO Auto-generated destructor stub
}

EventInfo::EventInfo(const EventInfo& eInfo)
{
    size = 0;
    ETM_location = eInfo.ETM_location;
    actors_name = eInfo.actors_name;
    audio = eInfo.audio;
    caption = eInfo.caption;
    content_nibble_level_1 = eInfo.content_nibble_level_1;
    content_nibble_level_2 = eInfo.content_nibble_level_2;
    director_name = eInfo.director_name;
    dolby = eInfo.dolby;
    duration = eInfo.duration;
    dvs = eInfo.dvs;
    end_time = eInfo.end_time;
    event_description = eInfo.event_description;
    event_id = eInfo.event_id;
    event_name = eInfo.event_name;
    event_short_description = eInfo.event_short_description;
    free_CA_mode = eInfo.free_CA_mode;
    image_url = eInfo.image_url;
    service_id = eInfo.service_id;
    service_uid = eInfo.service_uid;
    running_status = eInfo.running_status;
    rating = eInfo.rating;
    start_time = eInfo.start_time;
    price_text = eInfo.price_text;
    resolution = eInfo.resolution;
    transport_stream_id = eInfo.transport_stream_id;
    original_network_id = eInfo.original_network_id;
    bCompound = eInfo.bCompound;

    user_nibble_1 = eInfo.user_nibble_1;
    user_nibble_2 = eInfo.user_nibble_2;
    if(eInfo.listProgramLinkedInfo.size() > 0) {
        for (list<ProgramLinkedInfo>::const_iterator p_itr = eInfo.listProgramLinkedInfo.begin(); p_itr != eInfo.listProgramLinkedInfo.end();
                p_itr++) {
            listProgramLinkedInfo.push_back(*p_itr);
        }
    }
}

EventInfo::EventInfo(string channel_uid, uint32_t channel_id, vector<string> element, int version)
        : event_id(0), event_name(""), event_short_description(""), event_description(""), running_status(0), free_CA_mode(0), rating(0), start_time(
                0), end_time(0), duration(0), image_url(""), director_name(""), actors_name(""), price_text(""), dolby(NO_INFO_DOLBY), // 0: no info, 'N': not dolby, 'D': dolby support
        audio(NO_INFO_AUDIO), // 0: no info, 'M': mono, 'S': stereo, 'A': AC-3
        resolution(NO_INFO_RESOLUTION), // 0: no info, 'S': SD, 'H': HD
        content_nibble_level_1(0), content_nibble_level_2(0), user_nibble_1(0), user_nibble_2(0), caption(NO_CAPTION), dvs(NO_DVS), bCompound(false)
{
    transport_stream_id = 0;
    original_network_id = 0;
    ETM_location = 0;

    if(version == 1) {
        /*
         0: ANIMAX
         1: #01:30:00
         2: #Fairy Tail (Season 4)
         3: #-
         4: #Lucy attempts to make up for Fairy Tail's low ranking from “Hidden”, fighting fiercely against Flare, although the two stand on equal grounds at first
         5: #52
         6: #1800
         7: #03/19/2014
         */
        service_uid = channel_uid;
        service_id = channel_id;

        if(element.size() < 8) {
            L_ERROR(TAG, "ERROR element.size() is SMALL [%u]\n", element.size());
            return;
        }

        event_name = element[2];
        event_description = element[4];
        start_time = TimeConvertor::getInstance().convertStrToSeconds(element[1], element[7]);

        int running_time = convertStringToInt(element[6]);
        if(running_time <= 0 || running_time > 86400 /* 60 * 60 * 24 */) {
            duration = 0;
        } else {
            duration = running_time;
        }
        end_time = start_time + duration;
    } else if(version == 2) {
        /*
         0: ANIMAX
         1: #01:30:00
         2: #Fairy Tail (Season 4)
         3: #-
         4: #Lucy attempts to make up for Fairy Tail's low ranking from “Hidden”, fighting fiercely against Flare, although the two stand on equal grounds at first
         5: #52
         6: #1800
         7: #03/19/2014
         */
        service_uid = channel_uid;
        service_id = channel_id;

        if(element.size() < 15) {
            L_ERROR(TAG, "ERROR element.size() is SMALL [%u]\n", element.size());
            return;
        }

        event_name = element[2];
        event_short_description = element[3];
        event_description = element[4];
        actors_name = element[9];
        director_name = element[10];

        user_nibble_1 = convertSeasonFromString(element[5].c_str());
        user_nibble_2 = convertStringToInt(element[6].c_str());
        start_time = TimeConvertor::getInstance().convertStrToSeconds(element[1], element[14]);

        int running_time = convertStringToInt(element[12]);
        if(running_time <= 0 || running_time > 86400 /* 60 * 60 * 24 */) {
            duration = 0;
        } else {
            duration = running_time;
        }
        end_time = start_time + duration;
        event_id = convertEventIDFromString(element[13]);
    }

    bCompound = true;
}

uint16_t EventInfo::convertSeasonFromString(string str) {
    uint16_t season = 0;
    const char* chr;
    char num_chr[64];

    memset(num_chr, '\0', 64);

    chr = str.c_str();
    int j = 0;
    for(int i = 0; i < str.length(); i++) {
        if(chr[i] >= '0' && chr[i] <= '9') {
            num_chr[j++] = chr[i];
        }
    }

    season = atoi(num_chr);

    return season;
}

uint32_t EventInfo::convertEventIDFromString(string str) {
    uint32_t eventId = 0;
    const char* chr;
    char num_chr[64];
    uint8_t len = 0;

    chr = str.c_str();
    for(int i = 0; i < str.length(); i++) {
        if(chr[i] >= '0' && chr[i] <= '9') {
        } else {
            len = i;
            break;
        }
    }

    memset(num_chr, '\0', 64);
    memcpy(num_chr, &chr[4], len);
    eventId = atoi(num_chr);

    return eventId;
}

void EventInfo::makeUid()
{
    char chr[32];
    sprintf(chr, "%u.%u.%u", original_network_id, transport_stream_id, service_id);

    service_uid.clear();
    service_uid.append(chr);
}

string EventInfo::toXMLString_EPG()
{
    init();

    char chr[4096];
    ostringstream stringStream;
    stringStream << "<PROGRAM";
    bool bsetHD = false;
    list<Element>::iterator itr;
    for (itr = m_listElm.begin(); itr != m_listElm.end(); itr++) {
        Element data = *itr;

        if(strcmp(data.name.c_str(), "start_time_MJD") == 0) {
            sprintf(chr, " MJD=\"%d\"", atoi(data.value.c_str()));
            stringStream << chr;
        } else if(strcmp(data.name.c_str(), "start_time") == 0) {
            sprintf(chr, " start_time=\"%x\"", atoi(data.value.c_str()));
            stringStream << chr;
        } else if(strcmp(data.name.c_str(), "duration") == 0) {
            sprintf(chr, " duration=\"%x\"", atoi(data.value.c_str()));
            stringStream << chr;
        } else if(strcmp(data.name.c_str(), "rating") == 0) {
            sprintf(chr, " rating=\"%d\"", atoi(data.value.c_str()));
            stringStream << chr;
        } else if(strcmp(data.name.c_str(), "event_name") == 0) {
            sprintf(chr, " name=\"%s\"", data.value.c_str());
            stringStream << chr;
        } else if(strcmp(data.name.c_str(), "Rsl") == 0 && !bsetHD) {
            if(strcmp(data.value.c_str(), "H") == 0)
                sprintf(chr, " hd=\"%s\"", "1");
            else
                sprintf(chr, " hd=\"%s\"", "0");
            stringStream << chr;
            bsetHD = true;
        }
    }
    stringStream << "/>";
    return stringStream.str();
}

string EventInfo::toXMLString()
{
    init();

    ostringstream stringStream;
    char chr[4096];
    sprintf(chr, "\t<EventInfo event_id=\"%u\" ", event_id);
    stringStream << chr;

    sprintf(chr, "service_uid=\"%s\" ", service_uid.c_str());  stringStream << chr;

    sprintf(chr, "service_id=\"%u\" ", service_id);
    stringStream << chr;
    sprintf(chr, "event_name=\"%s\" ", convertUTFStringToHexaString(event_name).c_str());
    stringStream << chr;
    sprintf(chr, "event_short_description=\"%s\" ", convertUTFStringToHexaString(event_short_description).c_str());
    stringStream << chr;
    sprintf(chr, "event_description=\"%s\" ", convertUTFStringToHexaString(event_description).c_str());
    stringStream << chr;
    sprintf(chr, "running_status=\"%u\" ", running_status);
    stringStream << chr;
    sprintf(chr, "free_CA_mode=\"%u\" ", free_CA_mode);
    stringStream << chr;
    sprintf(chr, "rating=\"%u\" ", rating);
    stringStream << chr;
    sprintf(chr, "start_time=\"%u\" ", start_time);
    stringStream << chr;
    sprintf(chr, "end_time=\"%u\" ", end_time);
    stringStream << chr;
    sprintf(chr, "duration=\"%u\" ", duration);
    stringStream << chr;
    sprintf(chr, "image_url=\"%s\" ", convertUTFStringToHexaString(image_url).c_str());
    stringStream << chr;
    sprintf(chr, "director_name=\"%s\" ", convertUTFStringToHexaString(director_name).c_str());
    stringStream << chr;
    sprintf(chr, "actors_name=\"%s\" ", convertUTFStringToHexaString(actors_name).c_str());
    stringStream << chr;
    sprintf(chr, "price_text=\"%s\" ", convertUTFStringToHexaString(price_text).c_str());
    stringStream << chr;
    sprintf(chr, "dolby=\"%u\" ", dolby);
    stringStream << chr;
    sprintf(chr, "audio=\"%u\" ", audio);
    stringStream << chr;
    sprintf(chr, "resolution=\"%u\" ", resolution);
    stringStream << chr;
    sprintf(chr, "content_nibble_level_1=\"%u\" ", content_nibble_level_1);
    stringStream << chr;
    sprintf(chr, "content_nibble_level_2=\"%u\" ", content_nibble_level_2);
    stringStream << chr;
    sprintf(chr, "user_nibble_1=\"%u\" ", user_nibble_1);
    stringStream << chr;
    sprintf(chr, "user_nibble_2=\"%u\" ", user_nibble_2);
    stringStream << chr;
    sprintf(chr, "dvs=\"%u\" ", dvs);
    stringStream << chr;

    list<Element>::iterator itr;
    for (itr = m_listElm.begin(); itr != m_listElm.end(); itr++) {
        Element data = *itr;
        sprintf(chr, "%s=\"%s\" ", data.name.c_str(), data.value.c_str());
        stringStream << chr;
    }

    stringStream << ">\n";
    if(!listProgramLinkedInfo.empty()) {
        for (list<ProgramLinkedInfo>::iterator itr = listProgramLinkedInfo.begin(); itr != listProgramLinkedInfo.end(); itr++) {
            ProgramLinkedInfo info = *itr;
            stringStream << "\t\t<ProgramLinkedInfo ";
            sprintf(chr, "linked_service_flag=\"%u\" ", info.linked_service_flag);
            stringStream << chr;
            sprintf(chr, "linked_service_text=\"%s\" ", convertUTFStringToHexaString(info.linked_service_text).c_str());
            stringStream << chr;
            sprintf(chr, "button_type=\"%u\" ", info.button_type);
            stringStream << chr;
            sprintf(chr, "c_menu=\"%u\" ", info.c_menu);
            stringStream << chr;
            sprintf(chr, "button_image_filename=\"%s\" ", convertUTFStringToHexaString(info.button_image_filename).c_str());
            stringStream << chr;
            sprintf(chr, "display_start_time=\"%u\" ", info.display_start_time);
            stringStream << chr;
            sprintf(chr, "display_end_time=\"%u\" ", info.display_end_time);
            stringStream << chr;
            sprintf(chr, "vas_itemid=\"%s\" ", convertUTFStringToHexaString(info.vas_itemid).c_str());
            stringStream << chr;
            sprintf(chr, "vas_path=\"%s\" ", convertUTFStringToHexaString(info.vas_path).c_str());
            stringStream << chr;
            sprintf(chr, "vas_serviceid=\"%s\" />\n", convertUTFStringToHexaString(info.vas_serviceid).c_str());
            stringStream << chr;
        }
    }

    stringStream << "\t</EventInfo>";
    return stringStream.str();
}

void EventInfo::saveData(Element elm)
{
    string name = elm.name;
    string value = elm.value;

    if(name.compare("url") == 0) {
        image_url = value;
        return;
    } else if(name.compare("dlb") == 0) {
        dolby = NOT_DOLBY;
        if(value.compare("D") == 0)
            dolby = DOLBY_SUPPORTED;
        return;
    } else if(name.compare("ste") == 0 || name.compare("Ste") == 0) { // 실제 Ste, 대문자를 소문자로 교환
        if(value.compare("M") == 0)
            audio = MONO;
        else if(value.compare("S") == 0)
            audio = STEREO;
        else if(value.compare("A") == 0)
            audio = AC3;
        return;
    } else if(name.compare("rsl") == 0 || name.compare("Rsl") == 0) { // 실제 Rsl, 대문자를 소문자로 교환
        if(value.compare("H") == 0)
            resolution = HD;
        else
            resolution = SD;
        return;
    } else if(name.compare("prc") == 0) {
        price_text = value;
        return;
    } else if(name.compare("dir") == 0) {
        director_name = value;
        return;
    } else if(name.compare("act") == 0) {
        actors_name = value;
        return;
    } else if(name.compare("cap") == 0) {
        if(value.compare("Y") == 0)
            caption = CAPTION;
        return;
    } else if(name.compare("dvs") == 0) {
        if(value.compare("Y") == 0)
            dvs = DVS;
        return;
    } else {
        L_ERROR(TAG, "=== OOPS : NOT SUPPRORTED : %s:%s\n", name.c_str(), value.c_str());
    }

    m_listElm.push_back(elm);
}

string EventInfo::toString()
{
    char chr[4096];
    ostringstream stringStream;

    sprintf(chr, "%u>", event_id);
    stringStream << chr;
    sprintf(chr, "service_uid<%s>", service_uid.c_str());
    stringStream << chr;
    sprintf(chr, "service_id<%u>", service_id);
    stringStream << chr;
    sprintf(chr, "event_name<%s>", event_name.c_str());
    stringStream << chr;
    sprintf(chr, "event_short_description<%s>", event_short_description.c_str());
    stringStream << chr;
    sprintf(chr, "event_description<%s>", event_description.c_str());
    stringStream << chr;
    sprintf(chr, "running_status<%u>", running_status);
    stringStream << chr;
    sprintf(chr, "free_CA_mode<%u>", free_CA_mode);
    stringStream << chr;
    sprintf(chr, "rating<%u>", rating);
    stringStream << chr;
    sprintf(chr, "start_time<%u>", start_time);
    stringStream << chr;
    sprintf(chr, "end_time<%u>", end_time);
    stringStream << chr;
    sprintf(chr, "duration<%u>", duration);
    stringStream << chr;
    sprintf(chr, "image_url<%s>", image_url.c_str());
    stringStream << chr;
    sprintf(chr, "director_name<%s>", director_name.c_str());
    stringStream << chr;
    sprintf(chr, "actors_name<%s>", actors_name.c_str());
    stringStream << chr;
    sprintf(chr, "price_text<%s>", price_text.c_str());
    stringStream << chr;
    sprintf(chr, "dolby<%u>", dolby);
    stringStream << chr;
    sprintf(chr, "audio<%u>", audio);
    stringStream << chr;
    sprintf(chr, "resolution<%u>", resolution);
    stringStream << chr;
    sprintf(chr, "content_nibble_level_1<%u>", content_nibble_level_1);
    stringStream << chr;
    sprintf(chr, "content_nibble_level_2<%u>", content_nibble_level_2);
    stringStream << chr;
    sprintf(chr, "user_nibble_1<%u>", user_nibble_1);
    stringStream << chr;
    sprintf(chr, "user_nibble_2<%u>", user_nibble_2);
    stringStream << chr;
    sprintf(chr, "dvs<%u>", dvs);
    stringStream << chr;
    sprintf(chr, "caption<%u>", caption);
    stringStream << chr;

    for (list<Element>::iterator itr = m_listElm.begin(); itr != m_listElm.end(); itr++) {
        Element data = *itr;

        sprintf(chr, "%s<%s>", data.name.c_str(), data.value.c_str());
        stringStream << chr;
    }
    stringStream << "\n";
    if(listProgramLinkedInfo.size() > 0) {
        for (list<ProgramLinkedInfo>::iterator itr = listProgramLinkedInfo.begin(); itr != listProgramLinkedInfo.end(); itr++) {
            ProgramLinkedInfo info = *itr;
            sprintf(chr, "linked_service_flag<%u>", info.linked_service_flag);
            stringStream << chr;
            sprintf(chr, "linked_service_text<%s>", info.linked_service_text.c_str());
            stringStream << chr;
            sprintf(chr, "button_type<%u>", info.button_type);
            stringStream << chr;
            sprintf(chr, "c_menu<%u>", info.c_menu);
            stringStream << chr;
            sprintf(chr, "button_image_filename<%s>", info.button_image_filename.c_str());
            stringStream << chr;
            sprintf(chr, "display_start_time<%u>", info.display_start_time);
            stringStream << chr;
            sprintf(chr, "display_end_time<%u>", info.display_end_time);
            stringStream << chr;
            sprintf(chr, "vas_itemid<%s>", info.vas_itemid.c_str());
            stringStream << chr;
            sprintf(chr, "vas_path<%s>", info.vas_path.c_str());
            stringStream << chr;
            sprintf(chr, "vas_serviceid<%s>", info.vas_serviceid.c_str());
            stringStream << chr;
            stringStream << "\n";
        }
    }

    return stringStream.str();
}

void EventInfo::init()
{
    if(bCompound == false) {
        makeUid();

        bCompound = true;
    }
}

string EventInfo::getValue(string name)
{
    list<Element>::iterator itr;
    for (itr = m_listElm.begin(); itr != m_listElm.end(); itr++) {
        Element data = *itr;

        if(data.name.compare(name) == 0)
            return data.value;
    }

    return "";
}

uint32_t EventInfo::getSize()
{
    size = 0;
    size += service_uid.length();
    size += 4; //    , service_id(0),
    size += 4; //event_id(0),
    size += event_name.length();
    size += event_short_description.length();
    size += event_description.length();
    size++; //, running_status(0)
    size++; //, free_CA_mode(0),
    size++; //           rating(0),
    size += 4; //           start_time(0),
    size += 4; // end_time(0),
    size += 4; // duration(0),
    size += image_url.length();
    size += director_name.length();
    size += actors_name.length();
    size += price_text.length();
    size++; //, dolby(_INFO_DOLBY)
    size++; //       audio(NO_INFO_AUDIO), // 0: no info, 'M': mono, 'S': stereo, 'A': AC-3
    size++; //       resolution(NO_INFO_RESOLUTION), // 0: no info, 'S': SD, 'H': HD
    size++; //      content_nibble_level_1(0),
    size++; // content_nibble_level_2(0),
    size++; // user_nibble_1(0),
    size++; // user_nibble_2(0),
    size++; // caption(NO_CAPTION),
    size++; // dvs(NO_DVS),
    size++; // bCompound(false)
    return size;
}
