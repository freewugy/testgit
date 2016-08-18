/******************************************************************************
 *
 * Author : hcsung
 * $LastChangedDate: 2014-05-30 22:21:57 +0900 (금, 30 5월 2014) $
 * $LastChangedRevision: 824 $
 * Description:
 * Note:
 *****************************************************************************/

// ksh_20140530 | modify include path for si
//#include <tvstorm/export/OhPlayerClientSIWrapper.h>
//#include "../IOhPlayerClient.h"
#include "OhPlayerClientSIWrapper.h"
#include "IOhPlayerClient.h"

#include <stdio.h>
#include <dlfcn.h>
#include <string.h>
#include <android/log.h>

namespace tvstorm {

#define LOG_TAG "OhPlayerClientSIWrapper"
#define ALOGD(...)	__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define ALOGE(...)	__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

OhPlayerClientSIWrapper::OhPlayerClientSIWrapper() : mCore(NULL)
{
	mLibHandle = dlopen("libtvs_mediaclient_si.so", RTLD_NOW);
	if(mLibHandle == NULL) {
		ALOGE("%s[%d] ERROR : dlopen failed!!!!", __FUNCTION__, __LINE__);
		return;
	}

	OPEN open = (OPEN)dlsym(mLibHandle, "open");
	if(open == NULL) {
		ALOGD("%s[%d] dlsym(open()) failed!!!!!", __FUNCTION__, __LINE__);
		return;
	}

	mCore = (*open)(TVSMEDIA_CLIENT_TYPE_SI);
	if(mCore == NULL) {
		ALOGD("%s[%d] open failed!!!!!", __FUNCTION__, __LINE__);
	}
}

OhPlayerClientSIWrapper::~OhPlayerClientSIWrapper()
{
//	if(mCore)
//		delete mCore;
	CLOSE close = (CLOSE)dlsym(mLibHandle, "close");
	if(close == NULL) {
		ALOGD("%s[%d] dlsym(close()) failed!!!!!", __FUNCTION__, __LINE__);
	}
	(*close)(mCore);
	dlclose(mLibHandle);
	mLibHandle = NULL;
}

int OhPlayerClientSIWrapper::startTune(const char *aUrl)
{
    if(!mCore)
        return -1;
	return mCore->tuneStart(aUrl);
}

int OhPlayerClientSIWrapper::stopTune()
{
    if(!mCore)
        return -1;
    return mCore->tuneStop();
}

int OhPlayerClientSIWrapper::createSectionFilter(SectionFilterParameter_t *aParam, SectionCallback aCallback, void *aUserData, unsigned int aTimeoutUSec, int aFrequencyKhz)
{
    if(!mCore)
        return -1;
    return mCore->createSectionFilter(aParam, aCallback, aUserData, aTimeoutUSec, aFrequencyKhz);
}

int OhPlayerClientSIWrapper::destroySectionFilter(int aFilterId, int aFrequencyKhz)
{
    if(!mCore)
        return -1;
	return mCore->destroySectionFilter(aFilterId, aFrequencyKhz);
}

void OhPlayerClientSIWrapper::enableSectionFilter(int aFilterId, bool aEnable, int aFrequencyKhz)
{
    if(!mCore)
        return;
	mCore->setEnableSectionFilter(aFilterId, aEnable, aFrequencyKhz);
}

void OhPlayerClientSIWrapper::setMediaServerCallback(TVSMediaServerCallback aCallback)
{
    if(!mCore)
        return;
    mCore->setCallback(aCallback);
}


} /* namespace TVSTORM */
