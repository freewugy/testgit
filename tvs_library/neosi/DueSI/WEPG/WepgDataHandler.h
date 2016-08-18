/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: loneowl $
 * $LastChangedDate: 2014-08-28 13:55:52 +0900 (목, 28 8월 2014) $
 * $LastChangedRevision: 1004 $
 * Description:
 * Note:
 *****************************************************************************/

#ifndef _WEPG_DATA_HANDLER_H_
#define _WEPG_DATA_HANDLER_H_

#include <string>
#include <list>
#include <vector>
#include <map>
#include <string.h>
#include <stdint.h>
#include "EventInfo.h"
#include "Parser/dvb/NITInfo.h"
#ifdef ANDROID
#include <pthread.h>
#endif

using namespace std;

#define WEB_EPG_COMPLETED   -99
#define WEB_EPG_UPDATED     -101
#define WEB_EPG_NOTCHANGE     -102
#define WEB_EPG_GET_CHANNEL_MAP_FAILURE    -103

#define WEB_EPG_RETRY_TIME  1
// callback
typedef void (*WepgDataHandlerCallback)(int id, string channel_uid, vector<EventInfo *> listProgramInfo);

class EventInfo;
typedef struct WepgChanneInfo_t {
    uint16_t channel_number;
    string channel_name;
    string program_info_url;
    string channel_genre;
    uint32_t update_seconds;
    uint8_t version;
} WepgChannelInfo;

/*
 *
 */
class WepgDataHandler {
public:
    WepgDataHandler();
    WepgDataHandler(string channelMapUrl, WepgDataHandlerCallback *callback_func);

    virtual ~WepgDataHandler();

    void setChannelMapUrl(string channelMapUrl)
    {
        mChannelMapUrl = channelMapUrl;
    }
    ;

    void start();

    void update();

    void setListNITInfo(list<NITInfo*> listNITInfo);

    void stop();
    void clear();
    bool isRunning();

    static pthread_mutex_t m_mutex;

private:
    pthread_t mTask;
    string mChannelMapUrl;
    int mTotalProgramListSize;
    int mTotalWrongFormatProgramListSize;
    int mTotalReceivedProgramListSize;
    WepgDataHandlerCallback mCallback_func;
    uint32_t mLimitedDate;

    static void* wepgDataThread(void* param);

    int EPG_Retry_update(map<string, WepgChannelInfo> mMapChannel_Lost);
    void startProcess();
    void updateProcess();
    void parseChannelMapTxt(string str_channel_map);
    int parseProgramList(string channel_uid, uint16_t channel_number, string program_info_url, uint8_t version, vector<EventInfo*>& listProgramInfo);
    void makeProgramInfoListByXMLV1(string channel_uid, uint16_t channel_number, string xml_string, vector<EventInfo*>& listProgramInfo);
    void makeProgramInfoListByXMLV2(string channel_uid, uint16_t channel_number, string xml_string, vector<EventInfo*>& listProgramInfo);
    void makeProgramInfoListByTextV1(string channel_uid, uint16_t channel_number, string xml_string, vector<EventInfo*>& listProgramInfo);
    void makeProgramInfoListByTextV2(string channel_uid, uint16_t channel_number, string xml_string, vector<EventInfo*>& listProgramInfo);
    void makeWebProgramInfoFromData(string channel_uid, vector<string> element);
    bool getChannelUid(uint16_t channel_number, string *channel_uid);
    uint32_t getServiceId(string channel_uid);

    void setLimitedDate();

    map<string, WepgChannelInfo> mMapChannel;
    map<string, WepgChannelInfo> mUpdateMapChannel;
    uint64_t mReceivedBytes;

    static void WEPGLock()
    {
        pthread_mutex_lock(&m_mutex);
    }

    static void WEPGUnlock()
    {
        pthread_mutex_unlock(&m_mutex);
    }
};

#endif /* _WEPG_DATA_HANDLER_H_ */
