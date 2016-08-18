/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: eschoi $
 * $LastChangedDate: 2015-03-30 18:53:26 +0900 (월, 30 3월 2015) $
 * $LastChangedRevision: 1102 $
 * Description:
 * Note:
 *****************************************************************************/

#include <sstream>
#include <set>
#include <stdint.h>
#include "WepgDataHandler.h"
#ifdef __USE_TINYXML_2__
#include "tinyxml2/tinyxml2.h"
#else
#include "tinyxml/tinyxml.h"
#endif
#include "HttpHandler_curl.h"
#include "TimeConvertor.h"
#include "StringUtil.h"
#include "Logger.h"
#include "Timer.h"
#include <unistd.h>

#ifdef __USE_TINYXML_2__
using namespace tinyxml2;
#endif

static int bWEPGStatus = 0; // : NOT READY, 1 : FIRST START, 2 : UPDATE
static bool bStop = false;
static map<uint16_t, string> mMapChannelUid;

pthread_mutex_t WepgDataHandler::m_mutex =
#ifdef ANDROID
        PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
#else
        PTHREAD_MUTEX_INITIALIZER;
#endif

static const char* TAG = "WepgDataHandler";

WepgDataHandler::WepgDataHandler()
{
    mReceivedBytes = 0;
    mTotalReceivedProgramListSize = 0;
    mTotalProgramListSize = 0;
    mTotalWrongFormatProgramListSize = 0;
    mCallback_func = NULL;
    bWEPGStatus = 0;

    mMapChannel.clear();
    mUpdateMapChannel.clear();

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&mTask, &attr, &WepgDataHandler::wepgDataThread, this);
    pthread_attr_destroy(&attr);
}

WepgDataHandler::WepgDataHandler(string channelMapUrl, WepgDataHandlerCallback *callback_func)
{
    mReceivedBytes = 0;
    mTotalReceivedProgramListSize = 0;
    mTotalProgramListSize = 0;
    mTotalWrongFormatProgramListSize = 0;
    mChannelMapUrl = channelMapUrl;
    mCallback_func = (WepgDataHandlerCallback) callback_func;
    bWEPGStatus = 0;

    mMapChannel.clear();
    mUpdateMapChannel.clear();

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&mTask, &attr, &WepgDataHandler::wepgDataThread, this);
    pthread_attr_destroy(&attr);
}

WepgDataHandler::~WepgDataHandler()
{
    // TODO Auto-generated destructor stub
}

void WepgDataHandler::makeProgramInfoListByXMLV1(string channel_uid, uint16_t channel_number, string xml_string, vector<EventInfo*>& listProgramInfo)
{
#ifdef __USE_TINYXML_2__
    XMLDocument doc;
#else
    TiXmlDocument doc;
#endif
    doc.Parse(xml_string.c_str());
#ifdef __USE_TINYXML_2__
    XMLElement* root = doc.FirstChildElement("ArrayOfAnyType");
#else
    TiXmlNode* root = doc.FirstChild("ArrayOfAnyType");
#endif
    int event_id = 1;
    int count = 0;

    vector < string > element;
    int nLoop = 0;
    uint32_t service_id = getServiceId(channel_uid);

#ifdef __USE_TINYXML_2__
    for(XMLElement* elm = root->FirstChildElement("anyType"); elm; elm = elm->NextSiblingElement("anyType")) {
#else
    for (TiXmlNode* node = root->FirstChild("anyType"); node; node = node->NextSibling("anyType")) {
#endif
#ifdef __USE_TINYXML_2__
        const char* value = elm->GetText();
#else
        const char* value = node->ToElement()->GetText();
#endif
        count = splitStringByDelimiter(value, "#", element);

        mTotalReceivedProgramListSize++;
        if(count != 7) {
            mTotalWrongFormatProgramListSize++;
            L_ERROR(TAG, "OOPS : channel_uid[%s] : WRONG FORMAT[%d] : [%s]\n", channel_uid.c_str(), count, value);
            continue;
        }

        uint16_t number = convertStringToInt(element.at(5));
        if(number != channel_number) {
            mTotalWrongFormatProgramListSize++;
            L_ERROR(TAG, "OOPS[%03d] : [%s] WRONG CHANNEL_NUMBER Received [%u] : Expected[%u]\n",
                    mTotalWrongFormatProgramListSize, value, number, channel_number);
            continue;
        }

        EventInfo* eventInfo = new EventInfo(channel_uid, service_id, element, 1);
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
        element.clear();

        if(++nLoop > 100) {
            siMiliSleep(10);
            nLoop = 0;
        }
    }
}

void WepgDataHandler::makeProgramInfoListByXMLV2(string channel_uid, uint16_t channel_number, string xml_string, vector<EventInfo*>& listProgramInfo)
{
#ifdef __USE_TINYXML_2__
    XMLDocument doc;
#else
    TiXmlDocument doc;
#endif
    doc.Parse(xml_string.c_str());
#ifdef __USE_TINYXML_2__
    XMLElement* root = doc.FirstChildElement("ListEpg")->FirstChildElement("EpgDetails");
#else
    TiXmlNode* root = doc.FirstChild("ListEpg")->FirstChild("EpgDetails");
#endif
    int event_id = 1;
    int count = 0;

    vector < string > element;
    int nLoop = 0;
    uint32_t service_id = getServiceId(channel_uid);

#ifdef __USE_TINYXML_2__
    for(XMLElement* elm = root->FirstChildElement("Data"); elm; elm = elm->NextSiblingElement("Data")) {
#else
    for (TiXmlNode* node = root->FirstChild("Data"); node; node = node->NextSibling("Data")) {
#endif
#ifdef __USE_TINYXML_2__
        const char* value = elm->FirstChildElement("TvGuideData")->GetText();
#else
        const char* value = node->FirstChild("TvGuideData")->ToElement()->GetText();
#endif
        count = splitStringByDelimiter(value, "#", element);

        mTotalReceivedProgramListSize++;
        if(count != 14) {
            mTotalWrongFormatProgramListSize++;
            L_ERROR(TAG, "OOPS : channel_uid[%s] : WRONG FORMAT[%d] : [%s]\n", channel_uid.c_str(), count, value);
            continue;
        }

        uint16_t number = convertStringToInt(element.at(11));
        if(number != channel_number) {
            mTotalWrongFormatProgramListSize++;
            L_ERROR(TAG, "OOPS[%03d] : [%s] WRONG CHANNEL_NUMBER Received [%u] : Expected[%u]\n",
                    mTotalWrongFormatProgramListSize, value, number, channel_number);
            continue;
        }

        EventInfo* eventInfo = new EventInfo(channel_uid, service_id, element, 2);
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
        siMiliSleep(1);
    }
}

uint32_t WepgDataHandler::getServiceId(string channel_uid)
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

void WepgDataHandler::makeProgramInfoListByTextV1(string channel_uid, uint16_t channel_number, string xml_string, vector<EventInfo*>& listProgramInfo)
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

void WepgDataHandler::makeProgramInfoListByTextV2(string channel_uid, uint16_t channel_number, string xml_string, vector<EventInfo*>& listProgramInfo)
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
                L_ERROR(TAG, "WRONG CHANNEL_NUMBER : Received [%u] : Expected[%u] [%s]\n", number, channel_number, sub_element[0].c_str());
                continue;
            }

            EventInfo* eventInfo = new EventInfo(channel_uid, service_id, element, 2);
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
//            siMiliSleep(1);
        }
    }

}

void WepgDataHandler::setListNITInfo(list<NITInfo*> listNITInfo)
{
    WEPGLock();
    //////////////////////////////
    mMapChannelUid.clear();

    char chr[32];
    for (list<NITInfo*>::iterator itr = listNITInfo.begin(); itr != listNITInfo.end(); itr++) {
        NITInfo* info = (NITInfo *) (*itr);

        sprintf(chr, "%u.%u.%u", info->original_network_id, info->transport_stream_id, info->service_id);
        mMapChannelUid.insert(make_pair(info->channel_number, chr));
#ifdef __DEBUG_LOG_WEPG__
        L_INFO(TAG, "channel_number[%u], UID[%s]\n", info->channel_number, chr);
#endif
    }

    L_INFO(TAG, "ChannelMap UID list size[%u]\n", mMapChannelUid.size());
    //////////////////////////////
    WEPGUnlock();
}

bool WepgDataHandler::getChannelUid(uint16_t channel_number, string *channel_uid)
{
    if(channel_number <= 0)
        return false;

    map<uint16_t, string>::iterator itr = mMapChannelUid.find(channel_number);
    if(itr == mMapChannelUid.end()) {
        return false;
    } else {
        (*channel_uid) = itr->second;
        return true;
    }
}

void WepgDataHandler::parseChannelMapTxt(string str_channel_map)
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
            L_ERROR(TAG, "WRONG FORMAT[%d] : channel[%s]\n", count, (count > 0 ? element[0].c_str() : ""));
            element.clear();
            continue;
        }

        if(count == 8) {
            string channel_uid = "";
            uint16_t channel_number = convertStringToInt(element[0]);
            if(getChannelUid(channel_number, &channel_uid) == true) {
                WepgChannelInfo info;

                info.channel_number = channel_number;
                info.program_info_url = element[6];
                info.channel_genre = element[7];
                info.channel_name = element[1];

                count = splitStringByDelimiter(element[8], "T", subElement);

                if(count == 1) {
                    info.update_seconds = TimeConvertor::getInstance().convertStrToSeconds(subElement[1], subElement[0]);
                }

                info.version = 1;
                mMapChannel.insert(make_pair(channel_uid, info));
            }
        } else if(count == 4) {
            string channel_uid = "";
            uint16_t channel_number = convertStringToInt(element[0]);
            if(getChannelUid(channel_number, &channel_uid) == true) {
                WepgChannelInfo info;

                info.channel_number = channel_number;
                info.program_info_url = element[2];
                info.channel_genre = element[3];
                info.channel_name = element[1];

                count = splitStringByDelimiter(element[4], "T", subElement);

                if(count == 1) {
                    info.update_seconds = TimeConvertor::getInstance().convertStrToSeconds(subElement[1], subElement[0]);
                }

                info.version = 2;

                mMapChannel.insert(make_pair(channel_uid, info));
            }
        }

        element.clear();
        subElement.clear();
    }

    /*if(mMapChannel.empty()) {
        return false;
    }

    return true;*/
}

int WepgDataHandler::parseProgramList(string channel_uid, uint16_t channel_number, string program_info_url, uint8_t version,
        vector<EventInfo*> &listProgramInfo)
{
    int nTry = 0;
    string xml_string = "";
    long response_code;
    L_DEBUG(TAG, "ProgramInfo RECEIVED START == CH : [%d] , URL : [%s] \n" , channel_number, program_info_url.c_str());
    while (nTry++ < WEB_EPG_RETRY_TIME) {
        response_code = httpget(program_info_url.c_str(), &xml_string);

        if(response_code == 200) {
            mReceivedBytes += xml_string.length();
#ifdef __USE_XML__
            if(version == 1) {
                makeProgramInfoListByXMLV1(channel_uid, channel_number, xml_string, listProgramInfo);
            } else if (version == 2) {
                makeProgramInfoListByXMLV2(channel_uid, channel_number, xml_string, listProgramInfo);
            }
#else
            if(version == 1) {
                makeProgramInfoListByTextV1(channel_uid, channel_number, xml_string, listProgramInfo);
            } else if(version == 2) {
                makeProgramInfoListByTextV2(channel_uid, channel_number, xml_string, listProgramInfo);
            }
#endif
//            siMiliSleep(500);
            if (listProgramInfo.size() > 0)
            	return listProgramInfo.size();
            else
            	return response_code;

        } else { // connection failed , network disconnect
        	L_ERROR(TAG, "=== OOPS : ChannelUid[%s] InfoURL[%s] Response code:  %ld ===\n", channel_uid.c_str(), program_info_url.c_str(), response_code);
        	return response_code;
        }
    }

    L_ERROR(TAG, "=== OOPS : ChannelUid[%s] InfoURL[%s] Response code:  %ld ===\n", channel_uid.c_str(), program_info_url.c_str(), response_code);
    return -1;

}

void WepgDataHandler::start()
{
    if(bWEPGStatus == 1 || bWEPGStatus == 2) {
        L_WARN(TAG, "=== WEB EPG PROCESSING : WAIT TO FINISH ===\n");
        return;
    }
    bWEPGStatus = 1;
}

void WepgDataHandler::startProcess()
{
    L_INFO(TAG, "WEB EPG : ====================\n");
    bStop = false; // set running status
    uint32_t starttime = TimeConvertor::getInstance().getCurrentSecond();
    setLimitedDate();

    string str_channel_map = "";
    int nTry = 0;
    int try_Result = 0;
    while (nTry++ < WEB_EPG_RETRY_TIME) {
        if(bStop) {
        	mMapChannel.clear();
        	L_WARN(TAG, "=== WEB EPG STOPPED ===\n");
            return;
        }

        if(httpget(mChannelMapUrl.c_str(), &str_channel_map) == 200) {
            mMapChannel.clear();
        	parseChannelMapTxt(str_channel_map);

            string channel_uid = "";
            int i = 0;
            long response_code;
            uint32_t received_channel = 0;
            uint32_t received_size = 0;

            mTotalReceivedProgramListSize = 0;
            mTotalProgramListSize = 0;
            mTotalWrongFormatProgramListSize = 0;
            mReceivedBytes = 0;
            map<string, WepgChannelInfo> mMapChannel_Lost;
            for (map<string, WepgChannelInfo>::iterator itr = mMapChannel.begin(); itr != mMapChannel.end(); itr++) {
                channel_uid = itr->first;
                WepgChannelInfo info = itr->second;

                if(channel_uid.length() < 5) {
                    L_ERROR(TAG, "=== OOPS : CHANNEL_UID IS WRONG FORMAT [%s] ===\n", channel_uid.c_str());
                    continue;
                }

                if(bStop) {
                	mMapChannel.clear();
                	L_WARN(TAG, "=== WEB EPG STOPPED ===\n");
                    break;
                }

                vector<EventInfo*> listProgramInfo;
                if(parseProgramList(channel_uid, info.channel_number, info.program_info_url, info.version, listProgramInfo) >= 0) {
                    received_size = listProgramInfo.size();

                    if(mCallback_func != NULL && received_size > 0) {
                        L_DEBUG(TAG, "[%03d] : channel : No[%03d], Uid[%s], Name[%s], ProgramInfo : size[%u], update_date[%u]\n",
                                ++i, info.channel_number, channel_uid.c_str(), info.channel_name.c_str(), received_size, info.update_seconds);

                        mCallback_func(received_size, channel_uid, listProgramInfo);
                        mTotalProgramListSize += received_size;
                        received_channel++;
                      } else {
                    	  mMapChannel_Lost.insert(make_pair(channel_uid, info)); // connection fail
                    	  L_DEBUG(TAG, "Remain ProgramInfo size : [%d] \n",mMapChannel_Lost.size());
                    	  L_DEBUG(TAG, "ProgramInfo RECEIVED Fail channel : No[%03d], Uid[%s], Name[%s] \n\n",info.channel_number, channel_uid.c_str(), info.channel_name.c_str());

                      }
                } else { // receive fail
                    //info.update_seconds = 0;
                    //itr->second = info;
					mMapChannel_Lost.insert(make_pair(channel_uid, info));
					L_DEBUG(TAG, "Remain ProgramInfo size : [%d] \n",mMapChannel_Lost.size());
					L_DEBUG(TAG, "ProgramInfo RECEIVED Fail channel : No[%03d], Uid[%s], Name[%s] \n\n",info.channel_number, channel_uid.c_str(), info.channel_name.c_str());
                }
            }
            vector<EventInfo*> listDummyProgramInfo;
            listDummyProgramInfo.clear();

            if(bStop == false) {
                L_DEBUG(TAG, "WEB EPG : Channel size[%u], Program List Size[Total:%d, Valid:%d, Wrong:%d] : Received Data : [%llu] bytes\n",
                        received_channel, mTotalReceivedProgramListSize, mTotalProgramListSize, mTotalWrongFormatProgramListSize, mReceivedBytes);

                if(mCallback_func != NULL && mMapChannel_Lost.size() == 0) {
                    mCallback_func(WEB_EPG_COMPLETED, "0.0.0", listDummyProgramInfo);
                }
            }
			if (bStop == false && mMapChannel_Lost.size() != 0) {
				try_Result = EPG_Retry_update(mMapChannel_Lost);

				switch(try_Result){
				case -1 :
					mCallback_func(WEB_EPG_GET_CHANNEL_MAP_FAILURE, "0.0.0",listDummyProgramInfo);
				   mMapChannel.clear();
				   break;
				case 0 :
					mMapChannel.clear();
					L_INFO(TAG, "=== WEB EPG STOPPED ===\n");
					break;
				case 1 :
					mCallback_func(WEB_EPG_COMPLETED, "0.0.0",listDummyProgramInfo);
					break;
				}

			}

            uint32_t endtime = TimeConvertor::getInstance().getCurrentSecond();
            L_DEBUG(TAG, "WEB EPG : Running Time [%d]sec\n", endtime - starttime);

            mMapChannel_Lost.clear();
            mTotalReceivedProgramListSize = 0;
            mTotalProgramListSize = 0;
            mTotalWrongFormatProgramListSize = 0;
            mReceivedBytes = 0;
            return;
        }
    }

    L_ERROR(TAG, "=== OOPS : Cannot Get ChannelMap [%s] ===\n", mChannelMapUrl.c_str());

    if (bStop) {
    	mMapChannel.clear();
    	L_INFO(TAG, "=== WEB EPG STOPPED ===\n");
    		return;
    	}
    vector<EventInfo*> listDummyProgramInfo;
    mCallback_func(WEB_EPG_GET_CHANNEL_MAP_FAILURE, "0.0.0", listDummyProgramInfo);

}

int WepgDataHandler::EPG_Retry_update(map<string, WepgChannelInfo> mMapChannel_Lost){

	L_DEBUG(TAG, "\n");
	L_DEBUG(TAG, "### EPG RETRY RECEIVED ### \n\n");
	string channel_uid = "";
	int i = 0;
	int result_response = 0;
	uint32_t received_channel = 0;
	uint32_t received_size = 0;
	while(mMapChannel_Lost.size() != 0){
		L_DEBUG(TAG, "\n");
		L_DEBUG(TAG, "Remain ProgramInfo SIZE : [%d] \n\n", mMapChannel_Lost.size());
		for (map<string, WepgChannelInfo>::iterator itr = mMapChannel_Lost.begin();itr != mMapChannel_Lost.end(); /*itr++*/) {
			channel_uid = itr->first;
			WepgChannelInfo info = itr->second;

			if (channel_uid.length() < 5) {
				L_ERROR(TAG, "=== OOPS : CHANNEL_UID IS WRONG FORMAT [%s]===\n", channel_uid.c_str());
				continue;
			}

			if (bStop) {
				L_INFO(TAG, "=== WEB EPG STOPPED ===\n");
				mMapChannel_Lost.clear();
				return 0;
			}

			vector<EventInfo*> listProgramInfo;
			if ((result_response = parseProgramList(channel_uid, info.channel_number,info.program_info_url, info.version, listProgramInfo))	>= 0) {
				received_size = listProgramInfo.size();

				if (mCallback_func != NULL && received_size > 0) { //receive success
					L_DEBUG(TAG,
							"[%03d] : channel : No[%03d], Uid[%s], Name[%s], ProgramInfo : size[%u], update_date[%u]\n",
							++i, info.channel_number, channel_uid.c_str(),
							info.channel_name.c_str(), received_size,
							info.update_seconds);

					mCallback_func(received_size, channel_uid, listProgramInfo);
					mTotalProgramListSize += received_size;
					received_channel++;

					mMapChannel_Lost.erase(itr++); // delete
					continue;
				}
				if (result_response == 404){
					L_DEBUG(TAG, "=== EPG SERVER ERROR === \n");
					mMapChannel_Lost.erase(itr++); // delete
				} else if (result_response == 200) {
					L_DEBUG(TAG, "=== Exist Not ProgramInfo === \n");
					mMapChannel_Lost.erase(itr++); // delete
				} else {
					//L_DEBUG(TAG, "PROGRAM INFO REMAIN !!!! count : [%d] Remain_size : [%d] \n\n", ++j, mMapChannel_Lost.size());
					L_DEBUG(TAG, "Remain ProgramInfo size : [%d] \n",mMapChannel_Lost.size());
					L_DEBUG(TAG, "ProgramInfo RECEIVED Fail channel : No[%03d], Uid[%s], Name[%s] \n\n",info.channel_number, channel_uid.c_str(), info.channel_name.c_str());
				}

			} else {
				//L_DEBUG(TAG, "PROGRAM INFO REMAIN !!!! count : [%d] Remain_size : [%d] \n\n", ++j, mMapChannel_Lost.size());
				L_DEBUG(TAG, "Remain ProgramInfo size : [%d] \n",mMapChannel_Lost.size());
				L_DEBUG(TAG, "ProgramInfo RECEIVED Fail channel : No[%03d], Uid[%s], Name[%s] \n\n",info.channel_number, channel_uid.c_str(), info.channel_name.c_str());
			}
			//listProgramInfo.clear();
		}

	}
	mMapChannel_Lost.clear();
	return 1;
}

void WepgDataHandler::update()
{
    if(bWEPGStatus == 1 || bWEPGStatus == 2) {
        L_WARN(TAG, "=== WEB EPG PROCESSING : WAIT TO FINISH ===\n");
        return;
    }

    WEPGLock();
    //////////////////////////////
    mUpdateMapChannel.clear();
    mTotalProgramListSize = 0;

    for (map<string, WepgChannelInfo>::iterator itr = mMapChannel.begin(); itr != mMapChannel.end(); itr++) {
        string channel_uid = itr->first;
        WepgChannelInfo info = (WepgChannelInfo) (itr->second);

        mUpdateMapChannel.insert(make_pair(channel_uid, info));
    }

    //mMapChannel.clear();
    bWEPGStatus = 2;
    //////////////////////////////
    WEPGUnlock();
}

void WepgDataHandler::updateProcess()
{
    L_INFO(TAG, "WEB EPG UPDATE : ====================\n");
    bStop = false; // set running status
    uint32_t starttime = TimeConvertor::getInstance().getCurrentSecond();
    setLimitedDate();

    string str_channel_map = "";
    int nTry = 0;
    int try_Result = 0;
    int updateCheck = 0;
    while (nTry++ < WEB_EPG_RETRY_TIME) {
        if(bStop) {
        	mMapChannel.clear();
        	L_WARN(TAG, "=== WEB EPG STOPPED ===\n");
            return;
        }

        if(httpget(mChannelMapUrl.c_str(), &str_channel_map) == 200) {
        	mMapChannel.clear();
        	parseChannelMapTxt(str_channel_map);

            string channel_uid = "";
            int i = 0;
            long response_code;
            uint32_t received_channel = 0;
            uint32_t received_size = 0;

            mTotalReceivedProgramListSize = 0;
            mTotalProgramListSize = 0;
            mTotalWrongFormatProgramListSize = 0;
            mReceivedBytes = 0;
            map<string, WepgChannelInfo> mMapChannel_Lost;
            for (map<string, WepgChannelInfo>::iterator itr = mMapChannel.begin(); itr != mMapChannel.end(); itr++) {
                channel_uid = itr->first;
                WepgChannelInfo info = itr->second;

                if(channel_uid.length() < 5) {
                    L_ERROR(TAG, "=== OOPS : CHANNEL_UID IS WRONG FORMAT [%s] ===\n", channel_uid.c_str());
                    continue;
                }

                if(bStop) {
                	mMapChannel.clear();
                	L_WARN(TAG, "=== WEB EPG STOPPED ===\n");
                    break;
                }

                map<string, WepgChannelInfo>::iterator received_itr = mUpdateMapChannel.find(channel_uid);
                if(received_itr == mUpdateMapChannel.end()) {

                    vector<EventInfo*> listProgramInfo;
                    if(parseProgramList(channel_uid, info.channel_number, info.program_info_url, info.version, listProgramInfo) >= 0) {
                        received_size = listProgramInfo.size();

                        if(mCallback_func != NULL && received_size > 0) {
                            L_DEBUG(TAG, "[%03d] : channel : No[%03d], Uid[%s], Name[%s], ProgramInfo : size[%u], update_date[%u]\n",
                                    ++i, info.channel_number, channel_uid.c_str(), info.channel_name.c_str(), received_size, info.update_seconds);
                            mCallback_func(received_size, channel_uid, listProgramInfo);
                            mTotalProgramListSize += received_size;
                            received_channel++;
                            } else { // network disconnect
                            	mMapChannel_Lost.insert(make_pair(channel_uid, info)); // connection fail
                            	L_DEBUG(TAG, "Remain ProgramInfo size : [%d] \n",mMapChannel_Lost.size());
                            	L_DEBUG(TAG, "ProgramInfo RECEIVED Fail channel : No[%03d], Uid[%s], Name[%s] \n\n",info.channel_number, channel_uid.c_str(), info.channel_name.c_str());

                            }
                    } else { // receive fail
                        //info.update_seconds = 0;
                        //itr->second = info;
						mMapChannel_Lost.insert(make_pair(channel_uid, info));
						L_DEBUG(TAG, "Remain ProgramInfo size : [%d] \n",mMapChannel_Lost.size());
						L_DEBUG(TAG, "ProgramInfo RECEIVED Fail channel : No[%03d], Uid[%s], Name[%s] \n\n",info.channel_number, channel_uid.c_str(), info.channel_name.c_str());
                    }
                } else {
                    WepgChannelInfo received_info = received_itr->second;

                    if(info.update_seconds == received_info.update_seconds) {
#ifdef __DEBUG_LOG_WEPG__
                        L_INFO(TAG, "=== channelUid[%s] SAME VERSION [%s] === skipped ===\n",
                                channel_uid.c_str(),
                                TimeConvertor::getInstance().convertSeconds2Date(info.update_seconds).c_str());
#endif
                        updateCheck = 1;
                    } else {
                        vector<EventInfo*> listProgramInfo;
                        if(parseProgramList(channel_uid, info.channel_number, info.program_info_url, info.version, listProgramInfo) >= 0) {
                            received_size = listProgramInfo.size();

                            if(mCallback_func != NULL && received_size > 0) {
                                L_DEBUG(TAG, "[%03d] : channel : No[%03d], Uid[%s], Name[%s], ProgramInfo : size[%u], update_date[%u<-%u]\n",
                                        ++i, info.channel_number, channel_uid.c_str(), info.channel_name.c_str(), received_size, info.update_seconds, received_info.update_seconds);

                                mCallback_func(received_size, channel_uid, listProgramInfo);
                                mTotalProgramListSize += received_size;
                                received_channel++;
                                } else {
                                	mMapChannel_Lost.insert(make_pair(channel_uid, info)); // connection fail
                                	L_DEBUG(TAG, "Remain ProgramInfo size : [%d] \n",mMapChannel_Lost.size());
                                	L_DEBUG(TAG, "ProgramInfo RECEIVED Fail channel : No[%03d], Uid[%s], Name[%s] \n\n",info.channel_number, channel_uid.c_str(), info.channel_name.c_str());

                                }
                        } else {
                            //info.update_seconds = 0;
                            //itr->second = info;
							mMapChannel_Lost.insert(	make_pair(channel_uid, info));
							L_DEBUG(TAG, "Remain ProgramInfo size : [%d] \n",mMapChannel_Lost.size());
							L_DEBUG(TAG, "ProgramInfo RECEIVED Fail channel : No[%03d], Uid[%s], Name[%s] \n\n",info.channel_number, channel_uid.c_str(), info.channel_name.c_str());
                        }
                    }
                }
            }
            vector<EventInfo*> listDummyProgramInfo;
            listDummyProgramInfo.clear();
            if(bStop == false) {
                L_DEBUG(TAG, "WEB EPG UPDATE : Channel size[%u], Program List Size[Total:%d, Valid:%d, Wrong:%d] : Received Data : [%llu] bytes\n",
                        received_channel, mTotalReceivedProgramListSize, mTotalProgramListSize, mTotalWrongFormatProgramListSize, mReceivedBytes);
                if(mCallback_func != NULL && mTotalProgramListSize > 0 && mMapChannel_Lost.size() == 0) {
                   mCallback_func(WEB_EPG_UPDATED, "0.0.0", listDummyProgramInfo);
                }
                if (updateCheck == 1 && mTotalProgramListSize == 0 && mMapChannel_Lost.size() == 0){
                 mCallback_func(WEB_EPG_NOTCHANGE, "0.0.0", listDummyProgramInfo);
                 L_DEBUG(TAG, "=== EPG UPDATE : NOT CHANGE \n\n");
                }
            }
			if (bStop == false && mMapChannel_Lost.size() != 0) {
				 try_Result = EPG_Retry_update(mMapChannel_Lost);

				 switch (try_Result) {
				case -1:
					mCallback_func(WEB_EPG_GET_CHANNEL_MAP_FAILURE, "0.0.0",listDummyProgramInfo);
					mMapChannel.clear();
					break;
				case 0:
					mMapChannel.clear();
					L_INFO(TAG, "=== WEB EPG STOPPED ===\n");
					break;
				case 1:
					mCallback_func(WEB_EPG_UPDATED, "0.0.0",listDummyProgramInfo);
					break;
				}

			}

            uint32_t endtime = TimeConvertor::getInstance().getCurrentSecond();
            L_DEBUG(TAG, "WEB EPG UPDATE : Running Time [%d]sec\n", endtime - starttime);

            mMapChannel_Lost.clear();
            mUpdateMapChannel.clear();
            mTotalReceivedProgramListSize = 0;
            mTotalProgramListSize = 0;
            mTotalWrongFormatProgramListSize = 0;
            mReceivedBytes = 0;
            return;
        }
    }

    L_ERROR(TAG, "=== OOPS : Cannot Get ChannelMap [%s] ===\n", mChannelMapUrl.c_str());

    if (bStop) {
    	mMapChannel.clear();
    	L_INFO(TAG, "=== WEB EPG STOPPED ===\n");
    	return;
    	}
    vector<EventInfo*> listDummyProgramInfo;
    mCallback_func(WEB_EPG_GET_CHANNEL_MAP_FAILURE, "0.0.0", listDummyProgramInfo);

}

void WepgDataHandler::stop()
{
    bStop = true;
    bWEPGStatus = 0;
}

void WepgDataHandler::clear()
{
    mMapChannel.clear();
}

void* WepgDataHandler::wepgDataThread(void* param)
{
    while (1) {
        while (bWEPGStatus > 0) {

            if(bWEPGStatus == 1) {
            	((WepgDataHandler *) param)->startProcess();
            } else if(bWEPGStatus == 2) {
            	((WepgDataHandler *) param)->updateProcess();
            }
            if(bStop && bWEPGStatus == 1 ){
            	bWEPGStatus = 1;
            } else {
            	bWEPGStatus = 0;
            }
        }
        siMiliSleep(500);
    }

    return NULL;
}

void WepgDataHandler::setLimitedDate()
{
    uint32_t current_second = TimeConvertor::getInstance().getCurrentSecond();

    mLimitedDate = current_second + 7 * 24 * 60 * 60;
}

bool WepgDataHandler::isRunning()
{
    return bStop == false;
}
