#ifndef _EXEC_SERVICE_H_
#define _EXEC_SERVICE_H_

#include <utils/Thread.h>
#include <utils/StrongPointer.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "cutils/properties.h"

using namespace android;

class CheckLknHomeThread : public Thread
{
public:
    void onFirstRef();
    virtual status_t readyToRun();
    virtual bool threadLoop();

	int IsDocsisEnvSetupAlive();
	int IsHddCtrlAlive();
};

class TVSFirewallRunThread : public Thread
{
public:
    void onFirstRef();
    virtual status_t readyToRun();
    virtual bool threadLoop();
};

class TVSCheckRouteTableThread : public Thread
{
    static bool needToCheckEth;
    static bool needToCheckWlan;
public:
    void onFirstRef();
    virtual status_t readyToRun();
    virtual bool threadLoop();
};

class CheckDevModeThread : public Thread
{
public:
    void onFirstRef();
    virtual status_t readyToRun();
    virtual bool threadLoop();

    int prepare_sus_test(void);
};

#endif	//_EXEC_SERVICE_H_
