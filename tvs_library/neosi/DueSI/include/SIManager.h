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

#ifndef SIMANAGER_H_
#define SIMANAGER_H_

#include <string>
#include <list>
#include "SIChangeEvent.h"
#include "SIChangeEventListener.h"
#include "Listener.h"
#include "ServiceInfo.h"
#include "EventInfo.h"

using namespace std;

typedef struct _EPGProviderUri {
    string ap; // mandatory
    string provIP; // mandatory
    int provPort; // mandatory
    string demodType; // mandatory
    int freqkHz; // mandatory
    string provName; // mandatory
    bool useMemeoryDB; // mandatory
    // for lkn project
    int symbolrate; // optional
    int bandwidth; // optional
    string wepgChannelMapFile; // optional
    // for linknet iptv project
    string webSIURI; // optional

    // for skb project
    Local_area_code_info localAreaCode; // optional

    bool bUseXML;
} EPGProviderUri;

/**
 * @class	SIManager
 * @brief	DTV Component SIDatabase 관리
 *
 * Singleton class \n
 * SIManager Instance 를 제공 \n
 * SIDatabase Instance 를 제공 \n
 */
class SIManager: public ISiListener {

public:

    virtual ~SIManager()
    {
    }

    /**
     * @brief 	SIManager 초기화
     * @return	void
     */
    void initialize();

    /**
     * @brief 	SIManager Class Instance 를 제공
     * @param 	[IN] None
     * @return	SIManager Class Instance
     */
    static SIManager& getInstance();
    /**
     * @brief 	전체 채널중 Audio 채널/ Not Audio채널 정보 획득
     * @param 	[IN] isAudioch : Audio 채널 유무
     * @return	[OUT] ServiceInfo list 채널정보 데이터
     */
    list<ServiceInfo*> getChannelList(int channel_type);

    /**
     * @brief 	전체 채널정보 획득
     * @return	[OUT] ServiceInfo list 채널정보 데이터
     */
    list<ServiceInfo*> getChannelList();

    /**
     * @brief 	현재서비스의 이벤트정보 획득
     * @param 	[IN] 서비스(채널) id
     * @return	[OUT] EventInfo 이벤트정보 데이터
     */
    EventInfo* getCurrentProgram(int channelNumber);

    /**
     * @brief 	현재서비스의 이벤트정보 획득
     * @param 	[IN] 서비스(채널) unique id
     * @return	[OUT] EventInfo 이벤트정보 데이터
     */
    EventInfo* getCurrentProgram(string channelUid);

    /**
     * @brief 	각서비스의 이벤트정보 획득
     * @param 	[IN] channelNumber : 서비스(채널) id
     * @return	[OUT] EventInfo list 이벤트정보 데이터
     */
    list<EventInfo*> getProgramList(int channelNumber);

    /**
     * @brief 	각서비스의 이벤트정보 획득
     * @param 	[IN] channelUid : 서비스(채널) unique id
     * @return	[OUT] EventInfo list 이벤트정보 데이터
     */
    list<EventInfo*> getProgramList(string channelUid);

    /**
     * @brief 	각서비스의 이벤트정보 획득
     * @param 	[IN] channelNumber : 서비스(채널) id
     * @param 	[IN] startIndex : 시작 index (0 : 첫번째)
     * @param 	[IN] reqCount : event 개수
     * @return	[OUT] EventInfo list 이벤트정보 데이터
     */
    list<EventInfo*> getProgramListByIndex(int channelNumber, int startIndex, int reqCount);

    /**
     * @brief 	각서비스의 이벤트정보 획득
     * @param 	[IN] channelUid : 서비스(채널) unique id
     * @param 	[IN] startIndex : 시작 index (0 : 첫번째)
     * @param 	[IN] reqCount : event 개수
     * @return	[OUT] EventInfo list 이벤트정보 데이터
     */
    list<EventInfo*> getProgramListByIndex(string channelUid, int startIndex, int reqCount);

    /**
     * @brief 	각서비스의 이벤트정보 획득
     * @param 	[IN] channelNumber : 서비스(채널) id
     * @param 	[IN] startTime : 시작시간(sec단위)
     * @param 	[IN] endTime : 끝시간(sec단위)
     * @return	[OUT] EventInfo list 이벤트정보 데이터
     */
    list<EventInfo*> getProgramListByTime(int channelNumber, int startTime, int endTime);

    /**
     * @brief 	각서비스의 이벤트정보 획득
     * @param 	[IN] channelNumber : 서비스(채널) id
     * @param 	[IN] startTime : 시작시간(sec단위)
     * @param 	[IN] endTime : 끝시간(sec단위)
     * @return	[OUT] EventInfo list 이벤트정보 데이터
     */
    list<EventInfo*> getProgramListByTime(string channelUid, int startTime, int endTime);

    /**
     * @brief 	각서비스의 이벤트정보 획득
     * @param 	[IN] channelNumber : 서비스(채널) id
     * @param 	[IN] programName : program name
     * @param 	[IN] DesFlag : true? prgramName, program Description까지 검색, false? programName만 검색.
     * @return	[OUT] EventInfo list 이벤트정보 데이터
     */
    list<EventInfo*> getProgramListByName(int channelNumber, int currentTime, string programName, bool DesFlag);

    /**
     * @brief 	각서비스의 이벤트정보 획득
     * @param 	[IN] channelUid : 서비스(채널) uid
     * @param    [IN] currentTime : request time to query (fixed to current time)
     * @param 	[IN] programName : program name
     * @param 	[IN] DesFlag : true? prgramName, program Description까지 검색, false? programName만 검색.
     * @return	[OUT] EventInfo list 이벤트정보 데이터
     */
    list<EventInfo*> getProgramListByName(string channelUid, int currentTime, string programName, bool DesFlag);

    /**
     * @brief       각서비스의 이벤트정보 획득
     * @param       [IN] channelUid : 서비스(채널) uid
     * @param       [IN] requestTime : request time (sec단위)
     * @param       [IN] programName : program name
     * @param       [IN] seasonNo : season number
     * @param       [IN] episodeNo : episode number
     * @return      [OUT] EventInfo list 이벤트정보 데이터
     */
    list<EventInfo*> getProgramListBySeries(string channelUid, int requestTime, string programName, int seasonNo, int episodeNo);

    /**
     * @brief Product 정보 획득
     * @return [OUT] ProductInfo list Product 정보 데이터
     */
    list<ProductInfo> getProductInfoList();

    string getDefaultChannel();

    void setNetworkConnectionInfo(bool isConnected);

    /**
     * @brief si start to HomeTS
     * @param 	[IN] EPG Provider Uri
     * @param 	[IN] mode : 0: not used 1: normal(HomeTS), 2: abnormal(e,g watchingTV)
     *
     */
    void Start(string epgUri, int mode);

    /**
     * @brief scan start
     * @param       [IN] EPG Provider Uri
     * @param       [IN] mode : 0: not used 1: fti(first-time-installation), 2 button event by setting
     */
    void scanStart(string epgUri, int mode);

    void AutoScanStart(string epgUri, int startFreqMHz, int endFreqMHz, int tunerId);

    void ManualScanStart(string epgUri, int freqMHz, int tunerId);

    void ManualChannelScanStart(string epgUri, int freqMHz, int networkId);

    void CleanUpFactoryTest();

    /**
     * @brief ScanManager scan stop
     */
    void scanStop();

    /**
     * @brief NeoSI stop
     */
    void Stop();

    virtual void receivedEvent(void* e);

    /**
     * @brief 	SIChangeEventListener 등록
     * @param 	[IN] listener SIChangeEventListener 인스턴스
     * @return	void
     */
    void addUpdateEventListener(SIChangeEventListener* listener);

    /**
     * @brief 	SIChangeEventListener 제거
     * @param 	[IN] listener SIChangeEeventListener 인스턴스
     * @return	void
     */
    void removeUpdateEventListener(SIChangeEventListener* listener);

private:
    SIManager()
    {
        m_provuri.ap = "i";
        m_provuri.provIP = "239.192.60.3";
        m_provuri.provPort = 49200;
        m_provuri.demodType = "qam64";
        m_provuri.freqkHz = 466000;

        m_provuri.provName = "lkn";
        m_provuri.useMemeoryDB = true;

        m_provuri.symbolrate = 6875;
        m_provuri.bandwidth = 8;
        m_provuri.wepgChannelMapFile = "unknown";
        m_provuri.webSIURI = "unknown";

        m_provuri.localAreaCode.local_area_code_1 = 1;
        m_provuri.localAreaCode.local_area_code_2 = 41;
        m_provuri.localAreaCode.local_area_code_3 = 61;

        m_provuri.bUseXML = false;
    }
    ; // Constructor? (the {} brackets) are needed here.
    // Dont forget to declare these two. You want to make sure they
    // are unaccessable otherwise you may accidently get copies of
    // your singleton appearing.
    SIManager(SIManager const&); // Don't Implement
    void operator=(SIManager const&); // Don't implement

    EPGProviderUri m_provuri;
    /**
     *  @brief listener set
     */
    set<SIChangeEventListener*> m_listeners;

    /**
     * @brief 	시스템에서 전달된 이벤트를 리스너에 등록된 객체에 전달
     * @param event 현재 SIChangeEvent 상태
     * @return	void
     */
    void notifyEvent(SIChangeEvent* event);
    /**
     * @brief parsing epgUri
     */
    void parsingEpgUri(string epguri);
};

#endif /* SIMANAGER_H_ */
