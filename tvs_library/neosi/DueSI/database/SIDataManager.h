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

#ifndef SIDATAMANAGER_H_
#define SIDATAMANAGER_H_

#include <map>
#include <set>
#include <vector>
#include <string>
#include <list>
#include <set>
#include <fstream>
#include <unistd.h>
#include <pthread.h>

#include "Logger.h"
#include "SqliteDB.h"
#include "EventInfo.h"
#include "ServiceInfo.h"
#include "SIChangeEvent.h"
#include "mpeg/PMTInfo.h"
#include "dvb/NITInfo.h"
#include "Listener.h"
//#include "WebSI/WebSIHandler.h"
#include "SIChangeEventListener.h"


enum status_type {
    UNKNOWN,

    SERVICE_DB_COMPLETE = 100, PROGRAM_DB_COMPLETE, DATA_INSERT,
};

typedef enum {
    ALL = 0,
    AUDIO_VIDEO,
    AUDIO_ONLY,

} CHANNEL_TYPE;

enum elementary_stream_type {
    ES_STREAM_PID = 1, ES_STREAM_TYPE, ES_LANGUAGE_CODE,
};


using namespace std;

class SIDataManager: public Notifier {

public:

    virtual ~SIDataManager()
    {
        delete mDatabase;
        mDatabase = NULL;
    }

    static SIDataManager& getInstance()
    {
        static SIDataManager instance;

        return instance;
    }

    static pthread_mutex_t mSqliteMutex;

    void initDB();
    void resetDB();

    int getProgramList(int serviceid, list<EventInfo*> &listEventInfo);

    int getProgramList(string service_uid, list<EventInfo*> &listEventInfo);

    int getChannelInfo(string* str);

    int getChannelList(CHANNEL_TYPE channel_type, list<ServiceInfo*> &listServiceInfo);

    int getChannelsInfo(list<ServiceInfo*> &listServiceInfo);

    int getCurrentProgramInfo(int serviceid, EventInfo** eventinfo);

    int getCurrentProgramInfo(string service_uid, EventInfo** eventinfo);

    int getProgramListbyIndex(int serviceid, int startIndex, int reqCount, list<EventInfo*> &listEventInfo);

    int getProgramListbyIndex(string channelUid, int startIndex, int reqCount, list<EventInfo*> &listEventInfo);

    int getProgramListbyTime(int serviceid, int startTime, int endTime, list<EventInfo*> &listEventInfo);

    int getProgramListbyTime(string channelUid, int startTime, int endTime, list<EventInfo*> &listEventInfo);

    int getProgramListbyName(int serviceid, int currentTime, string programName, bool DesFlag, list<EventInfo*> &listEventInfo);

    int getProgramListbyName(string channelUid, int currentTime, string programName, bool DesFlag, list<EventInfo*> &listEventInfo);

    int getProgramListbySeries(string channelUid, int requestTime, string programName, int seasonNo, int episodeNo, list<EventInfo*> &listEventInfo);

    int getProductInfoList(list<ProductInfo> &listProductInfo);

    int getProgramInfo(string* str);

    int saveChannelInfo(list<ServiceInfo*> ChannelList, SIChangeEvent event, int mVersion,bool bdummyEpgMode);

    int saveProgramInfo(string service_uid, vector<EventInfo*> listEventInfo, SIChangeEvent event, bool bEitPf, bool bWebEPG);

    int saveProgramInfo(EventInfo* pEventInfo, uint8_t size, SIChangeEvent event);

    int savePMTInfo(list<PMTInfo*> listPMTInfo, SIChangeEvent event);

    int saveProductInfo(list<ProductInfo> listProduct);

    void sendChannelReady(int mVersion);

    void setDefaultChannelUid(string channel_uid)
    {
        mDefaultChannelUid = channel_uid;
        saveDefaultChannel(mDefaultChannelUid);
        printf("mDefaultChannelUid [%s]\n",mDefaultChannelUid.c_str());
    }

    string getDefaultChannelUid(void)
    {
        if(mDefaultChannelUid.c_str() == NULL || mDefaultChannelUid.empty()) {
            mDefaultChannelUid = "0.0.0";
        }
        printf("mDefaultChannelUid [%s]\n",mDefaultChannelUid.c_str());
        return mDefaultChannelUid;
    }

    int saveDefaultChannel(string default_ch);

    void setChannelMapReadyFlag(bool flag){
    	mSaveInfo = flag;
    }

    bool getChannelMapReadyFlag(){
        	return mSaveInfo;
	}

    int notifySIEvent(SIChangeEvent event);
    int notifyScanProcessTest(SIChangeEvent event);

    void setLocalAreaCode(Local_area_code_info localareacode)
    {
        m_localareacode = localareacode;
    }

    void setScanMode(int flag){
    	m_scanMode = flag;
    }
    int getScanMode(){
    	return m_scanMode;
    }

    void ready();

    void clear();

    void setProvider(string provname)
    {
        m_provname = provname;
    }

    void setListNITInfo(list<NITInfo*> listNITInfo, SIChangeEvent event);
private:

    /**
     *  @brief sqlite3 db
     */
    SqliteDB* mDatabase;
    map<string, list<elementary_stream> > mMapESInfo;

    /**
     * @brief 	event정보를 가지고 있는 map 변수	\n
     * KEY 		: string : service unique id	\n
     * VALUE	: list<EventInfo*> : service에 해당하는 event정보를 가지고 있는 리스트
     */
    map<string, list<EventInfo*> > mMapEventInfo;
    set<string> mSetEventUid;
    map<uint32_t, string> mMapServiceIdInfo;
    map<string, list<string> > mMapServiceUIDList;

    set<string> mSetChannelUid;

    /**
     *  @brief mso type
     */
    string m_provname;
    /**
     *  @brief listener set
     */
    set<ISiListener*> mSetSiListener;

    /**
     * @brief default channel number (barker channel)
     */
    string mDefaultChannelUid;

    list<ProductInfo> mProductInfoList;

    int mPMTCount;
    int mServiceStatus;
    int mEventStatus;
    bool mSaveInfo;
    int m_scanMode;

    SIDataManager()
    {
    	mDatabase = NULL;
    	mDefaultChannelUid = "0.0.0";
        mPMTCount = 0;
        mEventStatus = UNKNOWN;
        mServiceStatus = UNKNOWN;
        mSaveInfo = false;
    };

    SIDataManager(SIDataManager const&); // Don't Implement
    void operator=(SIDataManager const&); // Don't implement

    Local_area_code_info m_localareacode;
    int createTable();
    int clearTable();
    void makeEventInfoFromDBData(vector<string> iinfo, EventInfo* eventinfo);
    void makeServiceInfoFromDBData(vector<string> iinfo, ServiceInfo* serviceinfo);
    int ischangedPMTinfo(list<PMTInfo*> listPMTInfo, string service_uid);
    int getServiceUidList();
    int backupDB();
    int recoverDB();
    int copyFile(const char* src , const char* des );
    int countCHDB();

    int insertServiceList(list<ServiceInfo*> listServiceInfo);
    int insertPMTList(list<PMTInfo*> listPMTInfo,SIChangeEvent event);

    int updateServiceList(list<ServiceInfo*> listServiceInfo);
    int updateEventList(string service_uid, vector<EventInfo*> listEventInfo, bool bWebEPG);

    int deleteEventList(string service_uid);
    int deleteServiceList(list<ServiceInfo*> listServiceInfo, list<string> &querylist);
    int removeOldServiceList(list<ServiceInfo*> listNewServiceInfo);

    int isVideoStream(int streamType);
    int isAudioStream(int streamType);
    int isDTVChannel(int serviceType);
    int isDRadioChannel(int serviceType);
    int isLocalChannel(int localareacode);

    string getDBFileName();
    bool verifyDBFile();

    void SqliteLock(char const* str)
    {
        pthread_mutex_lock(&mSqliteMutex);
    }

    void SqliteUnlock(char const* str)
    {
        pthread_mutex_unlock(&mSqliteMutex);
    }

    int selectDBEventInfo(char* strQuery, list<EventInfo*> &listEventInfo);
    void deleteDBFile();

    int getServiceUid(uint16_t program_number, string *service_uid);
};

#endif /* SIDATAMANAGER_H_ */
