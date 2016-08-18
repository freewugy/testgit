/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 * PMTGetter.cpp
 *
 * LastChangedBy: owl
 * LastChangedDate: 2015. 3. 12.
 * Description:
 * Note:
 *****************************************************************************/
#include "PMTGetter.h"
#include "MMF/MMFInterface.h"
#include "Logger.h"
#include "Parser/mpeg/PMTParser.h"
#include "SIChangeEvent.h"
#include "SIDataManager.h"
#include "Timer.h"

#include "ByteStream.h"

static const char* TAG = "PMTGetter";
static pthread_mutex_t gQueueLock = PTHREAD_MUTEX_INITIALIZER;
static list<Section_Message> gQueueSectionMessage;

void sectionCB(const int sectionFilterHandle, const int eventId, const int pid, const unsigned char *sectionData, const unsigned int length,
        void *userParam)
{
    L_DEBUG(TAG, "SectionCallback : sectionFilterHandle[%d], eventId[%d], pid[0x%x], length[%d]\n", sectionFilterHandle, eventId, pid, length);

    Section_Message msg;
    msg.sectionFilterHandle = sectionFilterHandle;
    msg.eventId = eventId;
    msg.pid = pid;
    uint16_t sectionLength = r16(&sectionData[1]) & 0x0fff;

    if(length < sectionLength) {
        L_ERROR(TAG, "TOO SHORT DATA : Expected[%u], Received[%u]\n", sectionLength, length);
        return;
    }

    msg.sectionData = new uint8_t[sectionLength];//(uint8_t *) malloc(sectionLength);
    memcpy(msg.sectionData, sectionData, sectionLength);
    msg.length = sectionLength;
    msg.userParam = userParam;

    pthread_mutex_lock(&gQueueLock);
    gQueueSectionMessage.push_back(msg);
    pthread_mutex_unlock(&gQueueLock);
}

PMTGetter& PMTGetter::getInstance()
{
    static PMTGetter instance;
    return instance;
}

PMTGetter::PMTGetter()
{
    mHandlePAT = -1;

    mListPMTPid.clear();

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&mTask, &attr, &PMTGetter::pmtGetterThread, this);
    pthread_attr_destroy(&attr);
}

PMTGetter::~PMTGetter()
{
    // TODO Auto-generated destructor stub
}

void PMTGetter::setUri(string uri)
{
    mUri = uri;
    mHandlePAT = -1;
}

void PMTGetter::start()
{
    requestPAT();
}

void PMTGetter::requestPAT()
{
    L_DEBUG(TAG, "Request PAT\n");

    MMF_SectionFilterHandle handle;

    uint8_t mask[12] = { 0, };
    uint8_t mode[12] = { 0, };
    uint8_t comp[12] = { 0, };

    mask[0] = 0xff;
    comp[0] = 0x0;

    MMF_Filter_Start(&handle, (uint8_t*) mUri.c_str(), 0, mask, mode, comp, sectionCB, this, SECTION_MODE_ONE_SHOT);

    mHandlePAT = handle;
}

void PMTGetter::parsePAT(const int sectionFilterHandle, const unsigned char *sectionData, const unsigned int length)
{
    if(mHandlePAT != sectionFilterHandle) {
        L_INFO(TAG, "PAT FILTER HANDLE DIFFERENT : Expected[%d], Received[%d]\n", mHandlePAT, sectionFilterHandle);
    }

    //	stop first.
    MMF_Filter_Stop(sectionFilterHandle);
    mHandlePAT = -1;

    uint8_t* buffer = (uint8_t *) sectionData;
    uint16_t section_length = r16(&buffer[1]) & 0x0fff;
    uint32_t pos = 3;

    uint16_t transport_stream_id = r16(&buffer[pos]);
    pos += 2;
    pos++;
    uint8_t section_number = buffer[pos++];
    uint8_t last_section_number = buffer[pos++];

    while ((pos + 4) < section_length) {
        uint16_t program_number = r16(&buffer[pos]);
        pos += 2;
        uint16_t program_map_PID = r16(&buffer[pos]) & 0x1fff;
        pos += 2;

        if(program_number != 0) {
            mListPMTPid.push_back(program_map_PID);
        }
    }

    for (list<uint16_t>::iterator itr = mListPMTPid.begin(); itr != mListPMTPid.end(); itr++) {
        requestPMT((*itr));
    }
}

void PMTGetter::requestPMT(uint16_t pid)
{
    L_DEBUG(TAG, "Request PMT : Pid[0x%x]\n", pid);
    MMF_SectionFilterHandle handle;

    uint8_t mask[12] = { 0, };
    uint8_t mode[12] = { 0, };
    uint8_t comp[12] = { 0, };

    mask[0] = 0xff;
    comp[0] = 0x02;

    MMF_Filter_Start(&handle, (uint8_t*) mUri.c_str(), pid, mask, mode, comp, sectionCB, this, SECTION_MODE_ONE_SHOT);
    mListHandlePMT.push_back(handle);
}

void PMTGetter::parsePMT(const int sectionFilterHandle, const int pid, const unsigned char *sectionData, const unsigned int length)
{
    MMF_Filter_Stop(sectionFilterHandle);

    for (list<int>::iterator itr = mListHandlePMT.begin(); itr != mListHandlePMT.end(); itr++) {
        int handle = (int) (*itr);
        if(handle == sectionFilterHandle) {
            mListHandlePMT.erase(itr);
            break;
        }
    }

    //	만약에 sectionFilterHandle이 위에서 찾아지지 않았으면??

    for (list<uint16_t>::iterator itr = mListPMTPid.begin(); itr != mListPMTPid.end(); itr++) {
        uint16_t pmtPid = (int) (*itr);
        if(pmtPid == (uint16_t) pid) {
            mListPMTPid.erase(itr);
            break;
        }
    }

    PMTParser pmtParser;
    pmtParser.process((uint8_t*) sectionData, (int) length);
    list<PMTInfo*> listPMTInfo = pmtParser.getPMTList();

    if(listPMTInfo.empty() == false) {
        SIChangeEvent event;
        event.setReceived(0);
        event.setToBeReceived(0);
        event.setEventType(SIChangeEvent::CHANNEL_PMT);

        SIDataManager::getInstance().savePMTInfo(listPMTInfo, event);
    }
}

void PMTGetter::stop()
{
    if(mHandlePAT > 0)
        MMF_Filter_Stop(mHandlePAT);

    for (list<int>::iterator itr = mListHandlePMT.begin(); itr != mListHandlePMT.end(); itr++) {
        int handle = (int) (*itr);
        L_INFO(TAG, "Request Stop PMT Handle [%d]\n", handle);
        MMF_Filter_Stop(handle);
    }

    mListHandlePMT.clear();
    mListPMTPid.clear();
}

void* PMTGetter::pmtGetterThread(void* param)
{
    while (1) {
    	Section_Message msg;
    	bool messageReceived = false;

        pthread_mutex_lock(&gQueueLock);
        if(!gQueueSectionMessage.empty())
        {
            msg = gQueueSectionMessage.front();
            gQueueSectionMessage.pop_front();
        	messageReceived = true;
        }
        pthread_mutex_unlock(&gQueueLock);

        if(!messageReceived) {
            siMiliSleep(50);
            continue;
        }

		switch(msg.sectionData[0])
		{
			case 0:	//	pat
				((PMTGetter *) param)->parsePAT(msg.sectionFilterHandle, msg.sectionData, msg.length);
				break;
			case 2:	//	pmt
				((PMTGetter *) param)->parsePMT(msg.sectionFilterHandle, msg.pid, msg.sectionData, msg.length);
				break;
			default:
				L_ERROR(TAG, "=== NOT SUPPORT TABLE : %d ===\n", msg.sectionData[0]);
				break;
		}

		delete [] msg.sectionData;

		siMiliSleep(10);
    }
}
