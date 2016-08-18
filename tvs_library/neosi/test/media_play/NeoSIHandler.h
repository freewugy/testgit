/*
 * NeoSIHandler.h
 *
 *  Created on: 2013. 7. 1.
 *      Author: owl
 */

#ifndef _NEOSI_HANDLER_H_
#define _NEOSI_HANDLER_H_

#include "SIChangeEvent.h"
#include <SIChangeEventListener.h>
#include <SIManager.h>
#include <ServiceInfo.h>
#include <EventInfo.h>

#include <string>
//#include "TVI_types.h"
#include "TVS_DVBSI_Interface.h"

using namespace std;

class SIChangeEvent;
class ServiceInfo;
class EventInfo;
/*
 *
 */
class NeoSIHandler : public SIChangeEventListener
{
public:
	NeoSIHandler();
	virtual ~NeoSIHandler();

	/**
	 * @brief NeoSIHandler 초기화하는 함수
	 * @param si
	 * @param provider
	 */
	void init(string strProvider);
	void start(int nProvider);
	void stop();
	void scanStart(int nProvider);
	void scanStop();

	TDSResult getChannelList(list<ServiceInfo*> &listServiceInfo); // 0 : All, 1 : TV, 2 : Radio
	TDSResult getDefaultChannel(string *channelUid);
	TDSResult getProgramInfo(uint16_t channel_number, int offset, EventInfo** eventInfo, int* pPrevPresent, int* pNextPresent);
	TDSResult getProgramList(string channelUid, list<EventInfo*> &listEventInfo);
	TDSResult getCurrentProgramInfo(uint16_t channel_number, EventInfo** eventInfo);
	TDSResult getMultiChannelProgramList(list<string> channelList, map<string, list<EventInfo*> > &mapEventInfo);
	TDSResult getMultiChannelProgramListByTime(list<string> channelList, map<string, list<EventInfo*> > &mapEventInfo, int startTime, int endTime);
	TDSResult getMultiChannelProgramListByIndex(list<string> channelList, map<string, list<EventInfo*> > &mapEventInfo, int startIdx, int reqCount);
	TDSResult getProgramListByIndex(string channelUid, list<EventInfo*> &listEventInfo, int startIdx, int reqCount);
	TDSResult getProgramListByTime(string channelUid, list<EventInfo*> &listEventInfo, int startTime, int endTime);

    TDSResult getServiceIDByChannelNumber(uint16_t channel_number, uint16_t *service_id);
    TDSResult getChannelNumberByServiceID(uint16_t service_id, uint16_t *channel);

	void setCurrentService(string channelUid) {
		mCurrentService = channelUid;
	}

	void setNetwork();
	virtual void receiveSIChangeEvent(SIChangeEvent* e);

	void printProductInfo();
	void printchannellist(bool pProgram);
    void printChannelInfo(list<ServiceInfo *> listChannel, bool bProgram);
    void printProgramInfo(list<EventInfo *> listProgram);

private:
	bool bFirstProgram;
	string mProvider;
	string mCurrentService;

	map<uint16_t, uint16_t> mMapChannelNo;
    map<uint16_t, string> mMapChannelNoServiceUID;

	string getTime();
	void makeMapChannelNo();
    TDSResult getServiceUIDByChannelNumber(uint16_t channel_number, string *service_uid);
};

#endif /* _NEOSI_HANDLER_H_ */
