/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 * WebXMLParser.h
 *
 * LastChangedBy: owl
 * LastChangedDate: 2015. 3. 5.
 * Description:
 * Note:
 *****************************************************************************/
#ifndef WEBSI_WEBXMLPARSER_H_
#define WEBSI_WEBXMLPARSER_H_

#include <vector>
#include "ServiceInfo.h"
#include "WebSIHandler.h"
#include <string.h>

using namespace std;

/*
 *
 */
class WebXMLParser {
public:
    virtual ~WebXMLParser();

    /**
     * @brief instance를 생성하여서 반환
     */
    static WebXMLParser& getInstance() {
        static WebXMLParser instance;

        return instance;
    }
    string getDefaultChannel(string xml);
    string getVersion(string xml);
    bool parseChannelXML(string xml, list<ServiceInfo *> &listChannelInfo);
    bool parseChannelMapTxt(string str_channel_map, map<string, WebSIChannelInfoFile> &mapChannel);
    int parseProgramList(string channel_uid, uint16_t channel_number, string program_info_url, uint8_t version,
            vector<EventInfo*> &listProgramInfo);

    void setLimitedDate();

private:
    WebXMLParser();

    WebXMLParser(WebXMLParser const&); // Don't Implement
    void operator=(WebXMLParser const&); // Don't implement

    int mTotalProgramListSize;
    int mTotalWrongFormatProgramListSize;
    int mTotalReceivedProgramListSize;
    uint32_t mLimitedDate;

    void makeChannelUidMap(list<ServiceInfo*> listServiceInfo);

    bool getChannelUid(uint16_t channel_number, string *channel_uid);
    uint32_t getServiceId(string channel_uid);

    void makeProgramInfoListByTextV1(string channel_uid, uint16_t channel_number, string xml_string, vector<EventInfo*>& listProgramInfo);
    void makeProgramInfoListByTextV2(string channel_uid, uint16_t channel_number, string xml_string, vector<EventInfo*>& listProgramInfo);

    map<uint16_t, string> mMapChannelUid;
    bool bLimitedDateSet;
};

#endif /* WEBSI_WEBXMLPARSER_H_ */
