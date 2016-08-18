/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 * PSIMonitor.h
 *
 * LastChangedBy: owl
 * LastChangedDate: 2015. 3. 4.
 * Description:
 * Note:
 *****************************************************************************/
#ifndef PSIMONITOR_PSIMONITOR_H_
#define PSIMONITOR_PSIMONITOR_H_

#include <vector>
#include <list>
#include <string.h>
#include <string>
#include "Parser/mpeg/PMTInfo.h"
#include "PMTGetter.h"

#ifdef ANDROID
#include <pthread.h>
#endif

using namespace std;

// callback
typedef void (*PSIMonitorCallback)(int id, string channel_uid, vector<PMTInfo*> listPMTInfo);

typedef struct _MMF_Message {
    int what;
    int extra;
    string uri;
} MMF_Message;

/*
 *
 */
class PSIMonitor {
public:

    virtual ~PSIMonitor();

    static PSIMonitor& getInstance()
    {
        static PSIMonitor instance;

        return instance;
    }

    void setCallback(PSIMonitorCallback callback) {
        mCallback_func = callback;
    }

    void start();
    static pthread_mutex_t m_mutex;

private:
    PSIMonitor();

    PSIMonitor(PSIMonitor const&); // Don't Implement
    void operator=(PSIMonitor const&); // Don't implement

    pthread_t mTask;
    bool bStart;
    PSIMonitorCallback mCallback_func;

    static void* psiMonitorThread(void* param);

    static void PSIMonitorLock()
    {
        pthread_mutex_lock(&m_mutex);
    }

    static void PSIMonitorUnlock()
    {
        pthread_mutex_unlock(&m_mutex);
    }
};

#endif /* PSIMONITOR_PSIMONITOR_H_ */
