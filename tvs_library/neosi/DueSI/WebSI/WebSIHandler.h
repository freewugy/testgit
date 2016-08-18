/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 * WebSIHandler.h
 *
 * LastChangedBy: owl
 * LastChangedDate: 2015. 3. 4.
 * Description:
 * Note:
 *****************************************************************************/
#ifndef WEBSI_WEBSIHANDLER_H_
#define WEBSI_WEBSIHANDLER_H_

#include <string>
#include <vector>
#include <map>
#include <string.h>
#include <stdint.h>
#include "ServiceInfo.h"
#include "EventInfo.h"
#include "Listener.h"
#include "SIChangeEvent.h"
#include <sys/system_properties.h>

#ifdef ANDROID
#include <pthread.h>
#endif

using namespace std;

#define WEB_SI_COMPLETED   -99
#define WEB_SI_UPDATED     -101
#define WEB_SI_GET_FAILURE    -103

#define WEB_SI_RETRY_TIME  1
#define WEB_PRG_RETRY_TIME  3

#define WEB_SI_TIMER_PERIOD	 2;
#define WEB_SI_TIMER_PERIOD_FAILD	1;

typedef struct WebSIChannelInfoFile_t {
    uint16_t channel_number;
    string channel_name;
    string program_info_url;
    string channel_genre;
    uint32_t update_seconds;
    uint8_t version;
} WebSIChannelInfoFile;

/*
 *
 */
class WebSIHandler: public Notifier {
public:
    virtual ~WebSIHandler();

    static WebSIHandler& getInstance()
    {
        static WebSIHandler instance;

        return instance;
    }


    static pthread_mutex_t m_mutex;

    void start();

    void update();

    void stop();
    void clear();

    void setChannelXMLUrl(string channelXMLUrl) {
        mChannelXMLUrl = channelXMLUrl;
    }

    void setChannelMapTxtUrl(string channelMapTxtUrl) {
        mChannelMapTxtUrl = channelMapTxtUrl;
    }

    bool checkConnectionToServer();
    void startTimer();
private:

    WebSIHandler();

    WebSIHandler(WebSIHandler const&); // Don't Implement
    void operator=(WebSIHandler const&); // Don't implement

    pthread_t mTask;
    string mChannelMapTxtUrl;
    string mChannelXMLUrl;
    bool bStartTimer;
    string mStrVersion;
    map<string, WebSIChannelInfoFile> mMapWepgChannelInfo;

    static void* webSIThread(void* param);
    int PropertyCheck();
    void dummyDefault_ch();
    string  versionSplit(string strVersion);
    void updateLocalChXML(string xml);
    void updateProcess(int proValue);
    void parseChannelMapTxt(string str_channel_map);
    bool getChannelUid(uint16_t channel_number, string *channel_uid);
    uint32_t getServiceId(string channel_uid);

    void sendEventProgramMapCompleted();

    static void WebSILock()
    {
        pthread_mutex_lock(&m_mutex);
    }

    static void WebSIUnlock()
    {
        pthread_mutex_unlock(&m_mutex);
    }
    void NetWorkStatusFailed();
    void saveChannelInfo(list<ServiceInfo*> listChannelInfo, int mVersion, int default_ch);
    void saveProgramInfo(string channel_uid, vector<EventInfo *> listProgramInfo);
    void updateWebEPG(int proValue);
    void saveMapChannelInfo(map<string, WebSIChannelInfoFile> mapWepgChannelInfo);
    void makeNeedToUpdateMapWepgChannelInfo(map<string, WebSIChannelInfoFile> mapWepgChannelInfo, map<string, WebSIChannelInfoFile> &updateMapChannel);

    bool downloadAndMakeProgramInfosFromServer(map<string, WebSIChannelInfoFile> updateMapChannel);
};

#endif /* WEBSI_WEBSIHANDLER_H_ */
