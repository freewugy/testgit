/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 * PMTGetter.h
 *
 * LastChangedBy: owl
 * LastChangedDate: 2015. 3. 12.
 * Description:
 * Note:
 *****************************************************************************/
#ifndef PSIMONITOR_PMTGETTER_H_
#define PSIMONITOR_PMTGETTER_H_

#include <string>
#include <vector>
#include <list>
#include <stdint.h>

#ifdef ANDROID
#include <pthread.h>
#endif

using namespace std;

//typedef struct Section_Message_t {
//    int sectionFilterHandle;
//    int eventId;
//    int pid;
//    unsigned char *sectionData;
//    unsigned int length;
//    void *userParam;
//} Section_Message;

/*
 *
 */
class PMTGetter {
public:
    virtual ~PMTGetter();

    static PMTGetter& getInstance();

    void setUri(string uri);
    void start();

    void stop();

    static pthread_mutex_t m_mutex;

private:
    PMTGetter();

    PMTGetter(PMTGetter const&); // Don't Implement
    void operator=(PMTGetter const&); // Don't implement

    pthread_t mTask;

    string mUri;
    int mHandlePAT;
    list<int> mListHandlePMT;
    list<uint16_t> mListPMTPid;

    void requestPAT();
    void parsePAT(const int sectionFilterHandle, const unsigned char *sectionData, const unsigned int length);
    void requestPMT(uint16_t pid);
    void parsePMT(const int sectionFilterHandle, const int pid, const unsigned char *sectionData, const unsigned int length);

    static void* pmtGetterThread(void* param);

    static void PMTGetterLock()
    {
        pthread_mutex_lock(&m_mutex);
    }

    static void PMTGetterUnlock()
    {
        pthread_mutex_unlock(&m_mutex);
    }
};

#endif /* PSIMONITOR_PMTGETTER_H_ */
