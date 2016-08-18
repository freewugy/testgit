/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 * WebXMLParser.cpp
 *
 * LastChangedBy: owl
 * LastChangedDate: 2015. 3. 5.
 * Description:
 * Note:
 *****************************************************************************/
#include "WebXMLParser.h"
#include "Logger.h"
#include "tinyxml/tinyxml.h"
#include "xmlUtil.h"
#include "StringUtil.h"
#include "TimeConvertor.h"
#include "StringUtil.h"
#include "Timer.h"
#include "HttpHandler_curl.h"
#include <sstream>
#include <stdint.h>

static const char* TAG = "WebXMLParser";

WebXMLParser::WebXMLParser()
{
    bLimitedDateSet = false;
    mLimitedDate = 0;
}

WebXMLParser::~WebXMLParser()
{
}

string WebXMLParser::getDefaultChannel(string xml)
{
    if(xml.length() < 10) {
        return "";
    }

    TiXmlDocument doc;
    doc.Parse(xml.c_str());
    TiXmlNode* channels = doc.FirstChild("channels");

    return getStringFromXMLElement(channels->ToElement(), "default_ch");
}

string WebXMLParser::getVersion(string xml)
{
    if(xml.length() < 10) {
        return "";
    }

    TiXmlDocument doc;
    doc.Parse(xml.c_str());
    TiXmlNode* channels = doc.FirstChild("channels");

    return getStringFromXMLElement(channels->ToElement(), "version");
}

bool WebXMLParser::parseChannelXML(string xml, list<ServiceInfo *> &listChannelInfo)
{
    if(xml.length() < 10) {
        return false;
    }

    TiXmlDocument doc;
    doc.Parse(xml.c_str());
    TiXmlNode* channels = doc.FirstChild("channels");

    // <channel
    // sid="2"
    // ch="2"
    // cname="ANTV"
    // ip="4022350087"
    // ip_address="224.11.11.9"
    // port="1000"
    // genre="100"
    // vpid="225"
    // vst="27"
    // apid="1" ast="129" aplang="ind"
    // apid2="0" ast2="0" aplang2="ind"
    // ppid="225"
    // res="1"
    // ca_id="6245" ca_pid="304"/>
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
        sInfo->genre_code = getIntFromXMLElement(elm, "genre");
        sInfo->running_status = getIntFromXMLElement(elm, "running_status");
        sInfo->local_area_code = getIntFromXMLElement(elm, "local_area_code");

        elementary_stream esInfo;
        esInfo.stream_pid = getIntFromXMLElement(elm, "vpid");
        esInfo.stream_type = getIntFromXMLElement(elm, "vst");
        sInfo->list_elementary_stream.push_back(esInfo);

        esInfo.stream_pid = getIntFromXMLElement(elm, "apid");
        esInfo.stream_type = getIntFromXMLElement(elm, "ast");
        esInfo.language_code = getIntFromXMLElement(elm, "aplang");
        sInfo->list_elementary_stream.push_back(esInfo);

        esInfo.stream_pid = getIntFromXMLElement(elm, "apid2");
        esInfo.stream_type = getIntFromXMLElement(elm, "ast2");
        esInfo.language_code = getIntFromXMLElement(elm, "aplang2");
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

        if(sInfo->service_type == 0) {
            if(getIntFromXMLElement(elm, "vpid") != 0) {
                if(getIntFromXMLElement(elm, "apid") != 0) {
                    sInfo->service_type = 1;
                }
            } else {
                if(getIntFromXMLElement(elm, "apid") != 0) {
                    sInfo->service_type = 2;
                }
            }
        }

        if(sInfo->running_status == 0) {
            if(sInfo->service_type != 0) {
                sInfo->running_status = 4;
            }
        }

        sInfo->init();
        listChannelInfo.push_back(sInfo);
    }

    L_DEBUG(TAG, "END : channel list size [%u]\n", listChannelInfo.size());

    makeChannelUidMap(listChannelInfo);
    return true;
}

bool WebXMLParser::parseChannelMapTxt(string str_channel_map, map<string, WebSIChannelInfoFile> &mapChannel)
{
    std::istringstream f(str_channel_map);
    std::string line = "";
    int i = 0;
    int count = 0;

    /*
     * 0: 1
     * 1: #MIX
     * 2: #http://139.228.191.243/EPG/SDCHANNEL/MIX_normal.png
     * 3: #http://139.228.191.243/EPG/SDCHANNEL/MIX_focus.png
     * 4: #http://139.228.191.243/EPG/SDCHANNEL/MIX_reverse.png
     * 5: #MIX
     * 6: #http://139.228.191.243/EPG/schedule/MIX.xml
     * 7: #INHOUSE FREE VIEW
     * 8: #03/14/2014T04:00:00
     */

    /*
     * 0: 1
     * 1: #BERITASATU
     * 2: #http://139.228.191.244/EPG/schedule/BERITASATU.xml
     * 3: #INHOUSE FREE VIEW
     * 4: #08/14/2014T19:55:05
     */
    while (std::getline(f, line)) {
        vector < string > element;
        vector < string > subElement;

        count = splitStringByDelimiter(line, "#", element);
        if(count != 8 && count != 4) {
            L_ERROR(TAG, " === OOPS : WRONG FORMAT CHANNEL ==== %d ====\n", count);
            element.clear();
            continue;
        }

        if(count == 8) {
            string channel_uid = "";
            uint16_t channel_number = atoi(element[0].c_str());
            if(getChannelUid(channel_number, &channel_uid) == true) {
                WebSIChannelInfoFile info;

                info.channel_number = channel_number;
                info.program_info_url = element[6];
                info.channel_genre = element[7];
                info.channel_name = element[1];

                count = splitStringByDelimiter(element[8], "T", subElement);

                if(count == 1) {
                    info.update_seconds = TimeConvertor::getInstance().convertStrToSeconds(subElement[1], subElement[0]);
                }

                info.version = 1;
                mapChannel.insert(make_pair(channel_uid, info));
            }
        } else if(count == 4) {
            string channel_uid = "";
            uint16_t channel_number = atoi(element[0].c_str());
            if(getChannelUid(channel_number, &channel_uid) == true) {
                WebSIChannelInfoFile info;

                info.channel_number = channel_number;
                info.program_info_url = element[2];
                info.channel_genre = element[3];
                info.channel_name = element[1];

                count = splitStringByDelimiter(element[4], "T", subElement);

                if(count == 1) {
                    info.update_seconds = TimeConvertor::getInstance().convertStrToSeconds(subElement[1], subElement[0]);
                }

                info.version = 2;

                mapChannel.insert(make_pair(channel_uid, info));
            }
        }

        element.clear();
        subElement.clear();
    }
}

int WebXMLParser::parseProgramList(string channel_uid, uint16_t channel_number, string program_info_url, uint8_t version,
        vector<EventInfo*> &listProgramInfo)
{
    int nTry = 0;
    string xml_string = "";
    //long response_code;
    int response_code;
    while (nTry++ < WEB_PRG_RETRY_TIME) {
    	//response_code = httpget(program_info_url.c_str(), &xml_string);
    	response_code = socket_httpget(program_info_url.c_str(), &xml_string);

       if(response_code == 200) {
            if(xml_string.length() < 10) {
                L_DEBUG(TAG, "xml too SHORT, RETRY length[%u]\n", xml_string.length());
                continue;
            }

            if(version == 1) {
                makeProgramInfoListByTextV1(channel_uid, channel_number, xml_string, listProgramInfo);
            } else if(version == 2) {
                makeProgramInfoListByTextV2(channel_uid, channel_number, xml_string, listProgramInfo);
            }
            return listProgramInfo.size();
        }
       L_DEBUG(TAG, " [%d] count receive Failed :: Re-Connection \n", nTry);
    }

    //L_ERROR(TAG, "=== OOPS : ChannelUid[%s] InfoURL[%s] Response code:  %ld ===\n", channel_uid.c_str(), program_info_url.c_str(), response_code);
    L_ERROR(TAG, "=== OOPS : ChannelUid[%s] InfoURL[%s] Response code:  %d ===\n", channel_uid.c_str(), program_info_url.c_str(), response_code);
    return -1;

}

bool WebXMLParser::getChannelUid(uint16_t channel_number, string *channel_uid)
{
    map<uint16_t, string>::iterator itr = mMapChannelUid.find(channel_number);
    if(itr == mMapChannelUid.end()) {
        return false;
    } else {
        (*channel_uid) = itr->second;
        return true;
    }
}

uint32_t WebXMLParser::getServiceId(string channel_uid)
{
    int count = 0;
    uint32_t service_id = 0;
    vector < string > element;
    count = splitStringByDelimiter(channel_uid, ".", element);
    if(count == 2) {
        service_id = convertStringToInt(element[2]);
    } else {
        L_ERROR(TAG, "=== Wrong Format channel_uid [%s] ===\n", channel_uid.c_str());
        service_id = convertStringToInt(channel_uid);
    }

    return service_id;
}

void WebXMLParser::makeProgramInfoListByTextV1(string channel_uid, uint16_t channel_number, string xml_string, vector<EventInfo*>& listProgramInfo)
{
    istringstream f(xml_string.c_str());
    string line;

    vector < string > element;
    vector < string > sub_element;

    int count = 0;
    uint32_t service_id = getServiceId(channel_uid);
    string partial_line;
    bool bLineSplitted = false;
    bool bCompleted = false;

    while (std::getline(f, line)) {
        trim(line);
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        bCompleted = false;
        if(line.length() == 0)
            continue;

        if(hasPrefix("<anyType", line) && hasSuffix(line, "anyType>")) {
            bCompleted = true;
        } else {
            if(hasPrefix("<", line) && hasSuffix(line, ">")) {
            } else {
                if(hasPrefix("<anyType>", line)) {
                    bLineSplitted = true;
                    if(partial_line.length() > 0) {
                        L_ERROR(TAG, "WRONG FORMAT XML[%s]\n", line.c_str());
                        L_ERROR(TAG, "WRONG FORMAT XML[%s]\n", partial_line.c_str());
                        partial_line = "";
                    }

                    bCompleted = false;
                }

                if(hasSuffix(line, "</anyType>") || (line.find("</anyType>") != std::string::npos)) {
                    partial_line.append(line);
                    line = partial_line;
                    bLineSplitted = false;
                    bCompleted = true;
                    partial_line = "";
                }

                if(bLineSplitted) {
#ifdef __DEBUG_LOG_WEPG__
                    L_DEBUG(TAG, "Splitted[%s]\n", line.c_str());
#endif
                    partial_line.append(line);
                } else {
                    if(bCompleted == false) {
                        L_ERROR(TAG, "WRONG FORMAT XML[%s]\n", line.c_str());
                        L_ERROR(TAG, "WRONG FORMAT XML[%s]\n", partial_line.c_str());
                    } else {
#ifdef __DEBUG_LOG_WEPG__
                        L_DEBUG(TAG, "Finished[%s]\n", line.c_str());
#endif
                    }
                }
            }
        }

        if(bCompleted) {
            bCompleted = false;
            element.clear();
            sub_element.clear();

            splitStringByDelimiter(line, ">", element);
            splitStringByDelimiter(element[1], "<", sub_element);

            element.clear();
            convertSpecialChInXML (sub_element[0]);
            count = splitStringByDelimiter(sub_element[0], "#", element);

            mTotalReceivedProgramListSize++;
            if(count != 7) {
                mTotalWrongFormatProgramListSize++;
                L_ERROR(TAG, "WRONG FORMAT[%d] : channel_uid[%s] : [%s]\n", count, channel_uid.c_str(), sub_element[0].c_str());
                continue;
            }

            uint16_t number = convertStringToInt(element.at(5));
            if(number != channel_number) {
                mTotalWrongFormatProgramListSize++;
                L_ERROR(TAG, "WRONG CHANNEL_NUMBER : Received [%u] : Expected[%u] [%s]\n", number, channel_number, sub_element[0].c_str());
                continue;
            }

            EventInfo* eventInfo = new EventInfo(channel_uid, service_id, element, 1);
            if(bLimitedDateSet == false) {
                mLimitedDate = eventInfo->start_time + 7 * 24 * 60 * 60;
                bLimitedDateSet = true;
                L_INFO(TAG, "Network Time is Not Correct : USE start time of First Program [%u]\n", mLimitedDate);
            }

            if(eventInfo->duration == 0 || eventInfo->start_time > mLimitedDate) {
#ifdef __DEBUG_LOG_WEPG__
                L_INFO(TAG, "Skipped because start_time is bigger than 7 days : ServiceID[%u], Name[%s], duration[%d], StartTime[%s]\n"
                        " or Duration is ZERO\n",
                        eventInfo->service_id, element[2].c_str(), eventInfo->duration,
                        TimeConvertor::getInstance().convertSeconds2Date(eventInfo->start_time).c_str());
#endif
                delete eventInfo;
            } else {
                // eschoi_20140614 | supported the issue for wepg's event_id
                // eschoi_20140614 | used the end_time for unique id
                eventInfo->event_id = eventInfo->end_time / 60; //event_id++;

                listProgramInfo.push_back(eventInfo);
            }
            // eschoi_20140811 |
            //siMiliSleep(1);
        }
    }
}

void WebXMLParser::makeProgramInfoListByTextV2(string channel_uid, uint16_t channel_number, string xml_string, vector<EventInfo*>& listProgramInfo)
{
    istringstream f(xml_string.c_str());
    string line;

    vector < string > element;
    vector < string > sub_element;

    int count = 0;
    uint32_t service_id = getServiceId(channel_uid);
    string partial_line;
    bool bLineSplitted = false;
    bool bCompleted = false;

    while (std::getline(f, line)) {
        trim(line);
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        bCompleted = false;
        if(line.length() == 0)
            continue;

        if(hasPrefix("<TvGuideData", line) && hasSuffix(line, "TvGuideData>")) {
            bCompleted = true;
        } else {
            if(hasPrefix("<", line) && hasSuffix(line, ">")) {
            } else {
                if(hasPrefix("<TvGuideData>", line)) {
                    bLineSplitted = true;
                    if(partial_line.length() > 0) {
                        L_ERROR(TAG, "WRONG FORMAT XML[%s]\n", line.c_str());
                        L_ERROR(TAG, "WRONG FORMAT XML[%s]\n", partial_line.c_str());
                        partial_line = "";
                    }
                }

                if(hasSuffix(line, "</TvGuideData>") || (line.find("</TvGuideData>") != std::string::npos)) {
                    partial_line.append(line);
                    line = partial_line;
                    bLineSplitted = false;
                    bCompleted = true;
                    partial_line = "";
                }

                if(bLineSplitted) {
#ifdef __DEBUG_LOG_WEPG__
                    L_DEBUG(TAG, "Splitted [%s]\n", line.c_str());
#endif
                    partial_line.append(line);
                } else {
                    if(bCompleted == false) {
                        L_ERROR(TAG, "WRONG FORMAT XML[%s]\n", line.c_str());
                        L_ERROR(TAG, "WRONG FORMAT XML[%s]\n", partial_line.c_str());

                    } else {
#ifdef __DEBUG_LOG_WEPG__
                        L_DEBUG(TAG, "Finished[%s]\n", line.c_str());
#endif
                    }
                }
            }
        }

        if(bCompleted) {
            bCompleted = false;
            element.clear();
            sub_element.clear();

            splitStringByDelimiter(line, ">", element);
            splitStringByDelimiter(element[1], "<", sub_element);

            element.clear();
            convertSpecialChInXML (sub_element[0]);
            count = splitStringByDelimiter(sub_element[0], "#", element);

            mTotalReceivedProgramListSize++;
            if(count != 14) {
                mTotalWrongFormatProgramListSize++;
                L_ERROR(TAG, "WRONG FORMAT[%d] : channel_uid[%s] : [%s]\n", count, channel_uid.c_str(), sub_element[0].c_str());
                continue;
            }

            uint16_t number = convertStringToInt(element.at(11));
            if(number != channel_number) {
                mTotalWrongFormatProgramListSize++;
                L_ERROR(TAG, "WRONG CHANNEL_NUMBER : Received [%u] : Expected[%u]\n", number, channel_number);
                continue;
            }

            EventInfo* eventInfo = new EventInfo(channel_uid, service_id, element, 2);
            if(bLimitedDateSet == false) {
                mLimitedDate = eventInfo->start_time + 7 * 24 * 60 * 60;
                bLimitedDateSet = true;
                L_INFO(TAG, "Network Time is Not Correct : USE start time of First Program [%u]\n", mLimitedDate);
            }

            if(eventInfo->duration == 0 || eventInfo->start_time > mLimitedDate) {
#ifdef __DEBUG_LOG_WEPG__
                L_INFO(TAG, "Skipped because start_time is bigger than 7 days : ServiceID[%u], Name[%s], duration[%d], StartTime[%s]\n"
                        " or Duration is ZERO\n",
                        eventInfo->service_id, element[2].c_str(), eventInfo->duration,
                        TimeConvertor::getInstance().convertSeconds2Date(eventInfo->start_time).c_str());
#endif
                delete eventInfo;
            } else {
                listProgramInfo.push_back(eventInfo);
            }
        }
    }
}

void WebXMLParser::makeChannelUidMap(list<ServiceInfo*> listServiceInfo)
{
    mMapChannelUid.clear();
    for (list<ServiceInfo*>::iterator itr = listServiceInfo.begin(); itr != listServiceInfo.end(); itr++) {
        ServiceInfo* info = (ServiceInfo *) (*itr);
        mMapChannelUid.insert(make_pair(info->channel_number, info->service_uid));
#ifdef __DEBUG_LOG_WEPG__
        L_INFO(TAG, "channel_number[%u], UID[%s]\n", info->channel_number, info->service_uid.c_str());
#endif
    }
    L_INFO(TAG, "ChannelMap UID list size[%u]\n", mMapChannelUid.size());

    return;
}

void WebXMLParser::setLimitedDate()
{
    if(TimeConvertor::getInstance().isValidSystemTime() == false) {
        L_INFO(TAG, "\n\n=== NOT YET GET Network Time ===\n\n\n");
        bLimitedDateSet = false;

        mLimitedDate = 0;
        return;
    }

    uint32_t current_second = TimeConvertor::getInstance().getCurrentSecond();
    bLimitedDateSet = true;

    mLimitedDate = current_second + 7 * 24 * 60 * 60;
}

