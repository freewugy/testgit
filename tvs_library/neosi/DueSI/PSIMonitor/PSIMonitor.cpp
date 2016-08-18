/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 * PSIMonitor.cpp
 *
 * LastChangedBy: owl
 * LastChangedDate: 2015. 3. 4.
 * Description:
 * Note:
 *****************************************************************************/
#include "PSIMonitor.h"
#include "Logger.h"
#include "TimeConvertor.h"
#include "StringUtil.h"
#include "Timer.h"
#include "MMF/MMFInterface.h"
#include <unistd.h>

static const char* TAG = "PSIMonitor";
static pthread_mutex_t gListLock = PTHREAD_MUTEX_INITIALIZER;
static list<MMF_Message> listMMFMessage;
static string gCurrentUri;

pthread_mutex_t PSIMonitor::m_mutex =
#ifdef ANDROID
        PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
#else
        PTHREAD_MUTEX_INITIALIZER;
#endif

void mmf_callback(int what, int extra, const uint8_t* data, size_t size)
{
    string uri;
    uri.assign((char*) data, size);

    MMF_Message msg;
    msg.what = what;
    msg.extra = extra;
    msg.uri = uri;

    pthread_mutex_lock(&gListLock);
    listMMFMessage.push_back(msg);
    pthread_mutex_unlock(&gListLock);
}

PSIMonitor::PSIMonitor()
{
    mCallback_func = NULL;
    bStart = false;
    gCurrentUri = "";

    return;
}

PSIMonitor::~PSIMonitor()
{
    // TODO Auto-generated destructor stub
}

void PSIMonitor::start()
{
    if(bStart)
        return;

    bStart = true;

    MMF_SetCallback(mmf_callback);

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&mTask, &attr, &PSIMonitor::psiMonitorThread, this);
    pthread_attr_destroy(&attr);

    return;
}

void* PSIMonitor::psiMonitorThread(void* param)
{
    while (1)
    {
    	MMF_Message msg;
    	bool messageReceived = false;

        pthread_mutex_lock(&gListLock);
        if(!listMMFMessage.empty())
        {
            msg = listMMFMessage.front();
            listMMFMessage.pop_front();
        	messageReceived = true;
        }
        pthread_mutex_unlock(&gListLock);

        if(!messageReceived) {
            siMiliSleep(50);
            continue;
        }

		L_DEBUG(TAG, "=== what(%d), extra(%d), URI[%s] ===\n", msg.what, msg.extra, msg.uri.c_str());
		switch(msg.what)
		{
			case PLAY_INFO_STOPPED:                     // 25000
				if(gCurrentUri.compare(msg.uri) == 0) {
					PMTGetter::getInstance().stop();
				} else {
					L_ERROR(TAG, "WRONG URI : Expected[%s], Received[%s]\n", gCurrentUri.c_str(), msg.uri.c_str());
				}
				break;

			case PLAY_INFO_STARTED:                     // 25001
				gCurrentUri = msg.uri;
				PMTGetter::getInstance().setUri(gCurrentUri);
				PMTGetter::getInstance().start();
				break;

			case PLAY_INFO_START_OF_STREAM:             // 25002
			case PLAY_INFO_END_OF_STREAM:               // 25003
			case PLAY_INFO_FIRST_FRAME_DISPLAYED:       // 25004
			case PLAY_INFO_PLAY_GOOD:       // 25004
				break;
		}

		siMiliSleep(10);
    }

    return NULL;
}
