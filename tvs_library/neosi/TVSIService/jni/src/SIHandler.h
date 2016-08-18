/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: eschoi $
 * $LastChangedDate: 2014-01-19 02:02:18 +0900 (일, 19 1월 2014) $
 * $LastChangedRevision: 11136 $
 * Description:
 * Note:
 *****************************************************************************/

#ifndef _SI_HANDLER_H_
#define _SI_HANDLER_H_

#include "SIChangeEvent.h"
#include "SIChangeEventListener.h"
#include "SIManager.h"
#include "ServiceInfo.h"
#include "EventInfo.h"

#include "JNI_Utils.h"
#include "TVS_DVBSI_Interface.h"
#include <string>

using namespace std;

class SIManager;
class SIDataBase;
class SIChangeEvent;
class SIChangeEventListener;
class ServiceInfo;
class EventInfo;

class SIHandler: public SIChangeEventListener {
public:
    SIHandler();
    SIHandler(jobject obj)
            : mJni(obj)
    {
        SIManager& siManager = SIManager::getInstance();
        siManager.addUpdateEventListener(this);

        siManager.initialize();
    }
    ;

    virtual ~SIHandler();

    /**
     * @brief SIHandler 초기화하는 함수
     */
    void start(string uri, int mode); // 1: normal 2: abnormal
    void stop();

        void autoScanStart(string uri, int startFreqMHz, int endFreqMHz, int tunerId);
        void manualScanStart(string uri, int freqMHz, int tunerId);
        void manualChannelScanStart(string uri, int freqMHz, int networkId);
        void scanStart(string uri, int mode); // 1:fti(first-time-installation) 2: button event
        void scanStop();
        void setNetworkConnectionInfo(bool isConnected);
        void setCurrentService(string channelUid);

    TDSResult getDefaultChannel(string *channelUid);
    TDSResult getChannelList(int tvRadioMode, list<ServiceInfo*> &listServiceInfo); // 0: All 1: DTV 2: DRadio
    TDSResult getCurrentProgram(string channelUid, list<EventInfo*> &listEventInfo);
    TDSResult getProgramList(string channelUid, list<EventInfo*> &listEventInfo);
    TDSResult getProgramListByIndex(string channelUid, list<EventInfo*> &listEventInfo, int startIdx, int reqCount);
    TDSResult getProgramListByName(string channelUid, list<EventInfo*> &listEventInfo, int reqTime, string reqName, bool descFlag);
    TDSResult getProgramListBySeries(string channelUid, list<EventInfo*> &listEventInfo, int reqTime, string reqName, int seasonNo, int episodeNo);
    TDSResult getProgramListByTime(string channelUid, list<EventInfo*> &listEventInfo, int startTime, int endTime);
    TDSResult getMultiChannelProgramList(list<string> channelList, map<string, list<EventInfo*> > &mapEventInfo);
    TDSResult getMultiChannelProgramListByIndex(list<string> channelList, map<string, list<EventInfo*> > &mapEventInfo, int startIdx, int reqCount);
    TDSResult getMultiChannelProgramListByTime(list<string> channelList, map<string, list<EventInfo*> > &mapEventInfo, int startTime, int endTime);

        string getProvName()
        {
                return mProvName;
        }

        int getProvSymbolrate()
        {
                return mProvSymbolrate;
        }

        int getProvBandwidth()
        {
                return mProvBandwidth;
        }

        int getProvTunerId()
        {
                return mProvTunerId;
        }

        virtual void receiveSIChangeEvent(SIChangeEvent* e);
private:
    jobject mJni;
    string mUri;

    bool bFirstProgram;
    string mCurrentService;

        //
        string mProvName;
        int mProvSymbolrate;
        int mProvBandwidth;
        int mProvTunerId;

    void notifyChannelReady(int sdtVersion);
    void notifyDefaultChannel(string default_channel);
    void notifyChannelUpdated(int sdtVersion, int default_ch);
    void notifyProgramUpdated();
    void notifyChangeProgram(int chNum);
    void notifyScanReady(int total);
    void notifyScanProcessing(int progress, int found);
    void notifyScanCompleted(int found);
    void notifyScanStop();
    void notifyTDTDate(uint32_t timeInSec);
    void notifyChannelPMT();
    void notifyErrorWepgConn();

    void addTunerInfoToChannel(ServiceInfo *sInfo);

    void setProvUri(string uri, int tunerId);
    bool splitKeyAndValue(const std::string str, const std::string delimiter, std::string& key, std::string& value);
};

#endif /* _SI_HANDLER_H_ */
