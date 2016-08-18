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

#ifndef _DUMMY_EPG_HANDLER_H_
#define _DUMMY_EPG_HANDLER_H_

#include <string>
#include <list>
#include <vector>
#include <map>
#include <string.h>
#include <stdint.h>
#include "EventInfo.h"
#include "tinyxml/tinyxml.h"
#ifdef ANDROID
#include <pthread.h>
#endif

using namespace std;

/*
 *
 */
class DummyEPGHandler {
public:
    DummyEPGHandler();
    virtual ~DummyEPGHandler();

    void start();

    void update();

    void stop();
    void clear();
    bool isRunning();
    bool checkXMLFile();

    static pthread_mutex_t m_mutex;

private:
    pthread_t mTask;

    static void* dummyEPGHandlerThread(void* param);

    void startProcess(bool bUpdated);

    static void DummyEPGHandlerLock()
    {
        pthread_mutex_lock(&m_mutex);
    }

    static void DummyEPGHandlerUnlock()
    {
        pthread_mutex_unlock(&m_mutex);
    }

    void loadChannels();
    void loadPrograms();

    void changeTimeInfoByToday(vector<EventInfo*> &listEventInfo, uint32_t oldest_time);
    int getDifference(uint32_t oldest_time);

    bool isValidSystemTime();
};



#endif /* _DUMMY_EPG_HANDLER_H_ */
