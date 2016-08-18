/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 * MMFInterface.cpp
 *
 * LastChangedBy: owl
 * LastChangedDate: 2015. 3. 3.
 * Description:
 * Note:
 *****************************************************************************/
#include "MMFInterface.h"
#include "Logger.h"
#include "Timer.h"
#include <string.h>
#include <vector>
#include <list>
#include <algorithm>
#include <pthread.h>
#include <queue>
#include "ByteStream.h"


static const char* TAG = "MMF_INTERFACE";
static int mmf_filter_count=0;

list<int64_t> mmf_handle_list;

pthread_mutex_t pidQMutex =
#ifdef ANDROID
		PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
#else
PTHREAD_MUTEX_INITIALIZER;
#endif

typedef struct __TDIPidFilterMsg {
        uint8_t *pBuff;
        int pid;
        void* pParam;
} TDIPidFilterMsg;

#define MAX_FILTER	60
#define MAX_PID_FILTER	16

list<Section_Message> pidfilterQ;

string getMMFErrorMessage(MMF_Error res)
{
    switch(res) {
        case MMF_ERR_OK:
            return "MMF_ERR_OK";
        case MMF_ERR_UNKNOWN:
            return "MMF_ERR_UNKNOWN";
        case MMF_ERR_INVALID_PARAM:
            return "MMF_ERR_INVALID_PARAM";
        case MMF_ERR_HW:
            return "MMF_ERR_HW";
        case MMF_ERR_NOT_INITIALIZED:
            return "MMF_ERR_NOT_INITIALIZED";
        case MMF_ERR_NOT_SUPPORTED:
            return "MMF_ERR_NOT_SUPPORTED";
        case MMF_ERR_NULL_PTR:
            return "MMF_ERR_NULL_PTR";
    }
}

void* tMMFDemux(void* param)
{
	int cnt = 0;
	while (1) {
		pthread_mutex_lock(&pidQMutex);
		if(pidfilterQ.empty()) {
			pthread_mutex_unlock(&pidQMutex);
			siMiliSleep(50);
		} else {
			Section_Message msg;
			msg = pidfilterQ.front();
			pidfilterQ.pop_front();
			pthread_mutex_unlock(&pidQMutex);

			if(msg.sectionData[0] == 0) { // PAT
//				((PMTGetter *) param)->parsePAT(msg.sectionFilterHandle, msg.sectionData, msg.length);
			} else if(msg.sectionData[0] == 0x02) {
//				((PMTGetter *) param)->parsePMT(msg.sectionFilterHandle, msg.pid, msg.sectionData, msg.length);
			} else {
				L_ERROR(TAG, "=== NOT SUPPORT ===\n");
			}
			delete msg.sectionData;
		}
	}
	L_INFO(TAG,"TDI Demux thread close..\n\n");
	pthread_exit (NULL);
	return NULL;

}

string getFilterName(uint16_t pid, uint8_t tid)
{
	switch(pid) {
		case 0x0:
			return "PAT";
			break;

		case 0x10:
			return "NIT";
			break;

		case 0x11:
			if(tid == 0x42)
				return "SDT";
			else
				return "SDT Other";
			break;

		case 0x14:
			return "TDT";
			break;

		default:
			return "PMT";
			break;
	}
}

//MMF_Error MMF_DemuxCreate(MMF_DemuxHandle * demux, const char* uri);
bool MMF_Demux_Create(MMF_DemuxHandle* demux, uint8_t* uri)
{
	MMF_Error res;
	res = MMF_DemuxCreate(demux, (const char*)uri);
	L_DEBUG(TAG, "MMF_Demux_Create : handle : [%lld] [%s]\n", *demux,uri);
	if(res != MMF_ERR_OK) {
	        L_DEBUG(TAG, "MMF_DemuxControlSectionFilter FAILED : READSON[%s]\n", getMMFErrorMessage(res).c_str());
		return false;
	}
	return true;
}

//MMF_Error MMF_DemuxCreate(MMF_DemuxHandle * demux, const char* uri);
bool MMF_Demux_Destroy(MMF_DemuxHandle demux)
{
	MMF_Error res;
	L_DEBUG(TAG, "MMF_DemuxDestroy before\n");
	res = MMF_DemuxDestroy(demux);
	L_DEBUG(TAG, "MMF_DemuxDestroy : handle : %lld\n", demux);
	if(res != MMF_ERR_OK) {
			L_DEBUG(TAG, "MMF_DemuxControlSectionFilter FAILED : READSON[%s]\n", getMMFErrorMessage(res).c_str());
		return false;
	}
	return true;
}

bool MMF_Demux_Open()
{
	pthread_t task;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&task, &attr, tMMFDemux, NULL);
	pthread_attr_destroy(&attr);
	return 1;
}

//static int mmf_filter_count = 0;

bool MMF_Filter_Start(MMF_SectionFilterHandle* handle, uint8_t* uri, uint16_t pid, uint8_t* mask, uint8_t* mode, uint8_t* comp, SectionCallback sectionCallback, void* pReceiver, MMF_SectionFilterMonitoringMode monitormode)
{
    MMF_Error res;

    MMF_DemuxHandle dHandle;
    MMF_SectionFilter sectionfilter;

//    L_DEBUG(TAG,"uri [%s]\n",uri);
    sectionfilter.uri = (uint8_t *)uri;
    sectionfilter.pid = pid;
    sectionfilter.monitormode = monitormode;

#if 1 // version change wjpark 2015-12-07
    sectionfilter.monitormode = SECTION_MODE_CONTINOUS;

     // case PMT only, used version-change mode
	if(comp[0] == 0x02) { // PMT TableId
		sectionfilter.monitormode = SECTION_MODE_VERSION_CHANGE;
		mask[0] = 0xFF;
	}
	else if(comp[0] == 0x0)
	{
		sectionfilter.monitormode = SECTION_MODE_ONE_SHOT;
		mask[0] = 0xFF;
	}
#if 1
	else if(comp[0] == 0x14)
	{
		L_DEBUG(TAG,"TDT one shot ]\n");
		sectionfilter.monitormode = SECTION_MODE_ONE_SHOT;
	}
#endif
#endif
    memcpy(sectionfilter.mask, mask, 12);
    memcpy(sectionfilter.mode, mode, 12);
    memcpy(sectionfilter.comp, comp, 12);

    sectionfilter.masksize = 12;
    sectionfilter.modesize = 12;
    sectionfilter.compsize = 12;
    sectionfilter.callback = sectionCallback;
    sectionfilter.userParam = pReceiver;

    string name = getFilterName(pid,comp[0] );

    res = MMF_DemuxAddSectionFilter(handle, sectionfilter);

    if(res != MMF_ERR_OK) {
        L_DEBUG(TAG, "MMF_DemuxAddSectionFilter FAILED : READSON[%s] , handle [%lld]\n", getMMFErrorMessage(res).c_str(),*handle);
        return false;
    }


    res = MMF_DemuxControlSectionFilter(*handle, true);

    if(res != MMF_ERR_OK) {
        L_DEBUG(TAG, "MMF_DemuxControlSectionFilter FAILED : READSON[%s] , handle [%lld]\n", getMMFErrorMessage(res).c_str(),*handle);
        MMF_DemuxRemoveSectionFilter(*handle);
        return false;
    }
    mmf_handle_list.push_back(*handle);
    L_DEBUG(TAG, "Name [%s],  MMF_DemuxAddSectionFilter : handle : %lld \n", name.c_str(), *handle);

    return true;
}

bool MMF_Filter_Stop(MMF_SectionFilterHandle handle)
{
    MMF_Error res = MMF_ERR_UNKNOWN;

	list<int64_t>::iterator itor;
	itor=find(mmf_handle_list.begin(), mmf_handle_list.end(),handle);
	if(itor != mmf_handle_list.end()) {
		mmf_handle_list.erase(itor);
		res = MMF_DemuxRemoveSectionFilter(handle);
		if(res != MMF_ERR_OK) {
			L_DEBUG(TAG, "MMF_DemuxControlSectionFilter FAILED [%lld] : READSON[%s]\n",handle, getMMFErrorMessage(res).c_str());
			return false;
		} else {
			L_DEBUG(TAG, "MMF_Filter_Stop : mmf_handle_list size[%d] \n", mmf_handle_list.size());
			return true;
		}
	} else {
		return false;
	}
}
