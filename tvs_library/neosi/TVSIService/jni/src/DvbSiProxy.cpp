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

#include <stdio.h>
#include <string.h>
#include <jni.h>
#include <list>
#include <map>
#include <string>

#include "JNI_Utils.h"
#include "JNIConvertor.h"
#include "DvbSiProxy.h"
#include "SIHandler.h"

#include "Logger.h"
static const char* TAG = "DVBSIProxy";

SIHandler *g_pSIHandler = NULL;
DVBSI_STATUS g_DvbSiActive = DVBSI_STS_INACTIVE;

static jclass gPrgmsCls = 0;
static jclass gPrdtInfoCls = 0;

bool isDvbSiActive()
{
    if(g_DvbSiActive == DVBSI_STS_ACTIVE) {
        return true;
    }

    return false;
}

void setDvbSiStatus(DVBSI_STATUS sts)
{
    g_DvbSiActive = sts;
}

////////////////////////////////////////////////////////////////////////////
// JNI Setters/Getters API
////////////////////////////////////////////////////////////////////////////
// setters
static jint JNIStart(JNIEnv* env, jobject thiz, jstring providerUri, jint mode)
{
    L_DEBUG(TAG, "Perform JNI [%s].....\n", __FUNCTION__);

    jint result = -1;
    if(env == NULL || thiz == NULL) {
        L_ERROR(TAG, "ERROR - %s has INVALID parameter...\n", __FUNCTION__);
        return result;
    }
    if(providerUri == NULL || env->GetStringLength(providerUri) <= 0) {
        L_ERROR(TAG, "ERROR - %s has INVALID parameter...\n", __FUNCTION__);
        return result;
    }

    if(g_pSIHandler == NULL) {
        g_pSIHandler = new SIHandler(env->NewGlobalRef(thiz));
    }

    if(g_pSIHandler != NULL && isDvbSiActive() == false) {
        string uri = JNI_javaStringToStdString(env, providerUri);

        g_pSIHandler->start(uri, mode);
        L_INFO(TAG, "call_start(uri=%s, mode=%d)\n", uri.c_str(), mode);

        setDvbSiStatus(DVBSI_STS_ACTIVE);
        result = 0;
    }
    return result;
}

static jint JNIStop(JNIEnv* env, jobject thiz)
{
    L_DEBUG(TAG, "Perform JNI [%s].....\n", __FUNCTION__);

    jint result = -1;
    if(env == NULL || thiz == NULL) {
        L_ERROR(TAG, "ERROR - %s has INVALID parameter...\n", __FUNCTION__);
        return result;
    }

    if(g_pSIHandler != NULL && isDvbSiActive() == true) {
        setDvbSiStatus(DVBSI_STS_INACTIVE);
        result = 0;

        g_pSIHandler->stop();
        L_INFO(TAG, "call_stop\n");
    }
    return result;
}

static jint JNIRestart(JNIEnv* env, jobject thiz, jstring providerUri, jint mode)
{
    L_DEBUG(TAG, "Perform JNI [%s].....\n", __FUNCTION__);

    jint result = -1;
    if(env == NULL || thiz == NULL) {
        L_ERROR(TAG, "ERROR - %s has INVALID parameter...\n", __FUNCTION__);
        return result;
    }
    if(providerUri == NULL || env->GetStringLength(providerUri) <= 0) {
        L_ERROR(TAG, "ERROR - %s has INVALID parameter...\n", __FUNCTION__);
        return result;
    }

    if(g_pSIHandler == NULL) {
        g_pSIHandler = new SIHandler(env->NewGlobalRef(thiz));
    }

    if(g_pSIHandler != NULL && isDvbSiActive() == true) {
        JNIStop(env, thiz);
        return JNIStart(env, thiz, providerUri, mode);
    }
    return result;
}

static jint JNIStartAutoScan(JNIEnv* env, jobject thiz, jstring providerUri, jint startFreqMHz, jint endFreqMHz, jint tunerId)
{
        L_DEBUG(TAG, "Perform JNI [%s].....", __FUNCTION__);

        jint result = -1;
        if(env == NULL || thiz == NULL) {
                L_DEBUG(TAG, "ERROR - %s has INVALID parameter...", __FUNCTION__);
                return result;
        }
        if(providerUri == NULL || env->GetStringLength(providerUri) <= 0) {
                L_DEBUG(TAG, "ERROR - %s has INVALID parameter...", __FUNCTION__);
                return result;
        }

        if(g_pSIHandler == NULL) {
                g_pSIHandler = new SIHandler(env->NewGlobalRef(thiz));
        }

        if(g_pSIHandler != NULL) {
                string uri = JNI_javaStringToStdString(env, providerUri);

                g_pSIHandler->autoScanStart(uri, startFreqMHz, endFreqMHz, tunerId);
                L_INFO(TAG, "call_autoScanStart(uri=%s, startFreqMHz=%d, endFreqMHz=%d, tunerId=%d)", uri.c_str(), startFreqMHz, endFreqMHz, tunerId);

                setDvbSiStatus(DVBSI_STS_ACTIVE);
                result = 0;
        }
        return result;
}

static jint JNIStartManualScan(JNIEnv* env, jobject thiz, jstring providerUri, jint freqMHz, jint tunerId)
{
        L_DEBUG(TAG, "Perform JNI [%s].....", __FUNCTION__);

        jint result = -1;
        if(env == NULL || thiz == NULL) {
                L_DEBUG(TAG, "ERROR - %s has INVALID parameter...", __FUNCTION__);
                return result;
        }
        if(providerUri == NULL || env->GetStringLength(providerUri) <= 0) {
                L_DEBUG(TAG, "ERROR - %s has INVALID parameter...", __FUNCTION__);
                return result;
        }

        if(g_pSIHandler == NULL) {
                g_pSIHandler = new SIHandler(env->NewGlobalRef(thiz));
        }

        if(g_pSIHandler != NULL) {
                string uri = JNI_javaStringToStdString(env, providerUri);

                g_pSIHandler->manualScanStart(uri, freqMHz, tunerId);
                L_INFO(TAG, "manualScanStart(uri=%s, freqMHz=%d, tunerId=%d)", uri.c_str(), freqMHz, tunerId);

                setDvbSiStatus(DVBSI_STS_ACTIVE);
                result = 0;
        }
        return result;
}

static jint JNIStartManualChannelScan(JNIEnv* env, jobject thiz, jstring providerUri, jint freqMHz, jint networkId)
{
        L_DEBUG(TAG, "Perform JNI [%s].....", __FUNCTION__);

        jint result = -1;
        if(env == NULL || thiz == NULL) {
                L_DEBUG(TAG, "ERROR - %s has INVALID parameter...", __FUNCTION__);
                return result;
        }
        if(providerUri == NULL || env->GetStringLength(providerUri) <= 0) {
                L_DEBUG(TAG, "ERROR - %s has INVALID parameter...", __FUNCTION__);
                return result;
        }

        if(g_pSIHandler == NULL) {
                g_pSIHandler = new SIHandler(env->NewGlobalRef(thiz));
        }

        if(g_pSIHandler != NULL) {
                string uri = JNI_javaStringToStdString(env, providerUri);

                g_pSIHandler->manualChannelScanStart(uri, freqMHz, networkId);
                L_INFO(TAG, "manualChannelScanStart(uri=%s, freqMHz=%d, networkId=%d)", uri.c_str(), freqMHz, networkId);

                setDvbSiStatus(DVBSI_STS_ACTIVE);
                result = 0;
        }
        return result;
}

static jint JNIStartScan(JNIEnv* env, jobject thiz, jstring providerUri, jint mode)
{
        L_DEBUG(TAG, "Perform JNI [%s].....", __FUNCTION__);

        jint result = -1;
        if(env == NULL || thiz == NULL) {
                L_DEBUG(TAG, "ERROR - %s has INVALID parameter...", __FUNCTION__);
                return result;
        }
        if(providerUri == NULL || env->GetStringLength(providerUri) <= 0) {
                L_DEBUG(TAG, "ERROR - %s has INVALID parameter...", __FUNCTION__);
                return result;
        }

        if(g_pSIHandler == NULL) {
                g_pSIHandler = new SIHandler(env->NewGlobalRef(thiz));
        }

        if(g_pSIHandler != NULL) {
                string uri = JNI_javaStringToStdString(env, providerUri);

                g_pSIHandler->scanStart(uri, mode);
                L_INFO(TAG, "call_scanStart(uri=%s, mode=%d)", uri.c_str(), mode);

                setDvbSiStatus(DVBSI_STS_ACTIVE);
                result = 0;
        }
        return result;
}

static jint JNIStopScan(JNIEnv* env, jobject thiz)
{
        L_DEBUG(TAG, "Perform JNI [%s].....", __FUNCTION__);

        jint result = -1;
        if(env == NULL || thiz == NULL) {
                L_DEBUG(TAG, "ERROR - %s has INVALID parameter...", __FUNCTION__);
                return result;
        }

        if(g_pSIHandler != NULL) {
                setDvbSiStatus(DVBSI_STS_ACTIVE);
                result = 0;

                g_pSIHandler->scanStop();
                L_INFO(TAG, "call_scanStop");
        }
        return result;
}

static jint JNISetCurrentChannel(JNIEnv* env, jobject thiz, jstring channelUid)
{
    L_DEBUG(TAG, "Perform JNI [%s].....\n", __FUNCTION__);

    jint result = -1;
    if(env == NULL || thiz == NULL) {
        L_ERROR(TAG, "ERROR - %s has INVALID parameter...\n", __FUNCTION__);
        return result;
    }
    if(channelUid == NULL || env->GetStringLength(channelUid) <= 0) {
        L_ERROR(TAG, "ERROR - %s has INVALID parameter...\n", __FUNCTION__);
        return result;
    }
    if(g_pSIHandler == NULL) {
        L_ERROR(TAG, "ERROR - %s has GENERAL failure...\n", __FUNCTION__);
        return result;
    }

    g_pSIHandler->setCurrentService(JNI_javaStringToStdString(env, channelUid));
    return 0;
}

static jint JNISetNetworkConnectionInfo(JNIEnv* env, jobject thiz, jboolean isConnected)
{
    L_DEBUG(TAG, "Perform JNI [%s].....\n", __FUNCTION__);

    jint result = -1;
    if(env == NULL || thiz == NULL) {
        L_ERROR(TAG, "ERROR - %s has INVALID parameter...\n", __FUNCTION__);
        return result;
    }
    if(g_pSIHandler == NULL) {
        L_ERROR(TAG, "ERROR - %s has GENERAL failure...\n", __FUNCTION__);
        return result;
    }

    g_pSIHandler->setNetworkConnectionInfo(isConnected);
    return 0;
}

// getters
static jstring JNIGetDefaultChannel(JNIEnv* env, jobject thiz)
{
    L_DEBUG(TAG, "Perform JNI [%s].....\n", __FUNCTION__);

    if(env == NULL || thiz == NULL) {
        L_ERROR(TAG, "ERROR - %s has INVALID parameter...\n", __FUNCTION__);
        return NULL;
    }
    if(g_pSIHandler == NULL) {
        L_ERROR(TAG, "ERROR - %s has GENERAL failure...\n", __FUNCTION__);
        return NULL;
    }

    jstring defaultChannelUid;

    string strChannel;
    TDSResult result;

    result = g_pSIHandler->getDefaultChannel(&strChannel);
    defaultChannelUid = env->NewStringUTF(strChannel.c_str());
    return defaultChannelUid;
}

static jobject JNIGetChannelList(JNIEnv* env, jobject thiz, jint tvRadioMode)
{
    L_DEBUG(TAG, "Perform JNI [%s].....\n", __FUNCTION__);

    if(env == NULL || thiz == NULL) {
        L_ERROR(TAG, "ERROR - %s has INVALID parameter...\n", __FUNCTION__);
        return NULL;
    }
    if(g_pSIHandler == NULL) {
        L_ERROR(TAG, "ERROR - %s has GENERAL failure...\n", __FUNCTION__);
        return NULL;
    }

    jclass mRetChListCls;
    jmethodID mid;
    jobject retChannelObject;

    list<ServiceInfo*> listChannelInfo;
    TDSResult result;

    result = g_pSIHandler->getChannelList((int) tvRadioMode, listChannelInfo);

    mRetChListCls = env->FindClass("com/tvstorm/tv/si/channel/RetChannelList");
    if(mRetChListCls == NULL) {
        L_ERROR(TAG, "ERROR - Not found RetChannelList class...");
        return NULL;
    }

    mid = env->GetMethodID(mRetChListCls, "<init>", "()V");
    if(mid == NULL) {
        L_ERROR(TAG, "ERROR - Not found Constructor of RetChannelList ...");
        return NULL;
    }

    retChannelObject = env->NewObject(mRetChListCls, mid);
    if(retChannelObject == NULL) {
        L_ERROR(TAG, "Fail to create new RetChannelList object....");
        return NULL;
    }

        if(result == TDS_OK) {
                string name = g_pSIHandler->getProvName();
                convertJNIChannelInfo(env, mRetChListCls, retChannelObject, listChannelInfo,
                                strstr(name.c_str(), "lkn") == NULL ? CONST_JAVA_SI_EPG_PROVIDER_SKB : CONST_JAVA_SI_EPG_PROVIDER_LKN,
                                g_pSIHandler->getProvSymbolrate(), g_pSIHandler->getProvBandwidth(), g_pSIHandler->getProvTunerId());
        }

    setResultOfRetObjectList(env, mRetChListCls, retChannelObject, result);

    // memory free
    for (list<ServiceInfo *>::iterator itr = listChannelInfo.begin(); itr != listChannelInfo.end(); itr++) {
        delete (*itr);
    }
    listChannelInfo.clear();

    return retChannelObject;
}

static jobject JNIGetCurrentProgram(JNIEnv* env, jobject thiz, jstring channelUid)
{
    L_DEBUG(TAG, "Perform JNI [%s].....\n", __FUNCTION__);

    if(env == NULL || thiz == NULL) {
        L_ERROR(TAG, "ERROR - %s has INVALID parameter...\n", __FUNCTION__);
        return NULL;
    }
    if(channelUid == NULL || env->GetStringLength(channelUid) <= 0) {
        L_ERROR(TAG, "ERROR - %s has INVALID parameter...\n", __FUNCTION__);
        return NULL;
    }
    if(g_pSIHandler == NULL) {
        L_ERROR(TAG, "ERROR - %s has GENERAL failure...\n", __FUNCTION__);
        return NULL;
    }

    jclass mRetPrgmListCls;
    jclass mPrgmsCls;
    jmethodID mid;
    jobject retProgramListObject;
    jobject programsObject;
    jfieldID fidChannelUid;

    list<EventInfo*> eventList;
    TDSResult result;

    result = g_pSIHandler->getCurrentProgram(JNI_javaStringToStdString(env, channelUid), eventList);

    //////////////////////////////////////////////////////////////////////////////////
    // Creat RetProgramList object
    //////////////////////////////////////////////////////////////////////////////////
    mRetPrgmListCls = env->FindClass("com/tvstorm/tv/si/program/RetProgramList");
    if(mRetPrgmListCls == NULL) {
        L_ERROR(TAG, "ERROR - Not found RetChannelList class...\n");
        return NULL;
    }

    mid = env->GetMethodID(mRetPrgmListCls, "<init>", "()V");
    if(mid == NULL) {
        L_ERROR(TAG, "ERROR - Not found Constructor of RetChannelList ...\n");
        return NULL;
    }

    retProgramListObject = env->NewObject(mRetPrgmListCls, mid);
    if(retProgramListObject == NULL) {
        L_ERROR(TAG, "Fail to create new RetEventList object....\n");
        return NULL;
    }

    if(result == TDS_OK) {
        //////////////////////////////////////////////////////////////////////////////////
        // Create Programs object
        //////////////////////////////////////////////////////////////////////////////////
        if(gPrgmsCls == 0) {
            mPrgmsCls = env->FindClass("com/tvstorm/tv/si/program/Programs");
            gPrgmsCls = (jclass) env->NewGlobalRef(mPrgmsCls);
        }
        if(gPrgmsCls == NULL) {
            L_ERROR(TAG, "ERROR - Not found Programs class...\n");
            return NULL;
        }

        mid = env->GetMethodID(gPrgmsCls, "<init>", "()V");
        if(mid == NULL) {
            L_ERROR(TAG, "ERROR - Not found Constructor of Programs ...\n");
            return NULL;
        }

        programsObject = env->NewObject(gPrgmsCls, mid);
        if(programsObject == NULL) {
            L_ERROR(TAG, "Fail to create new Programs object....\n");
            return NULL;
        }

        fidChannelUid = env->GetFieldID(gPrgmsCls, "mChannelUid", "Ljava/lang/String;");
        if(fidChannelUid == NULL) {
            L_ERROR(TAG, "Fail to find channelUid field of Programs\n");
            env->DeleteLocalRef(programsObject);
            return NULL;
        } else {
            env->SetObjectField(programsObject, fidChannelUid, channelUid);
            env->DeleteLocalRef(channelUid);
        }

        //////////////////////////////////////////////////////////////////////////////////
        // Convert ProgramInfo and add it to Programs list
        //////////////////////////////////////////////////////////////////////////////////
        convertJNIPrograms(env, gPrgmsCls, programsObject, eventList, channelUid);

        //////////////////////////////////////////////////////////////////////////////////
        // Add Programs to RetProgramList
        //////////////////////////////////////////////////////////////////////////////////
        convertJRetProgramList(env, mRetPrgmListCls, retProgramListObject, programsObject);
        env->DeleteLocalRef(programsObject);
    }

    //////////////////////////////////////////////////////////////////////////////////
    // Set Result
    //////////////////////////////////////////////////////////////////////////////////
    setResultOfRetObjectList(env, mRetPrgmListCls, retProgramListObject, result);
    // memory free
    for (list<EventInfo *>::iterator itr = eventList.begin(); itr != eventList.end(); itr++) {
        delete (*itr);
    }
    eventList.clear();

    return retProgramListObject;
}

static jobject JNIGetProgramList(JNIEnv* env, jobject thiz, jstring channelUid)
{
    L_DEBUG(TAG, "Perform JNI [%s].....\n", __FUNCTION__);

    if(env == NULL || thiz == NULL) {
        L_ERROR(TAG, "ERROR - %s has INVALID parameter...\n", __FUNCTION__);
        return NULL;
    }
    if(channelUid == NULL || env->GetStringLength(channelUid) <= 0) {
        L_ERROR(TAG, "ERROR - %s has INVALID parameter...\n", __FUNCTION__);
        return NULL;
    }
    if(g_pSIHandler == NULL) {
        L_ERROR(TAG, "ERROR - %s has GENERAL failure...\n", __FUNCTION__);
        return NULL;
    }

    jclass mRetPrgmListCls;
    jclass mPrgmsCls;
    jmethodID mid;
    jobject retProgramListObject;
    jobject programsObject;
    jfieldID fidChannelUid;

    list<EventInfo*> eventList;
    TDSResult result;

    result = g_pSIHandler->getProgramList(JNI_javaStringToStdString(env, channelUid), eventList);

    //////////////////////////////////////////////////////////////////////////////////
    // Creat RetProgramList object
    //////////////////////////////////////////////////////////////////////////////////
    mRetPrgmListCls = env->FindClass("com/tvstorm/tv/si/program/RetProgramList");
    if(mRetPrgmListCls == NULL) {
        L_ERROR(TAG, "ERROR - Not found RetProgramList class...\n");
        return NULL;
    }

    mid = env->GetMethodID(mRetPrgmListCls, "<init>", "()V");
    if(mid == NULL) {
        L_ERROR(TAG, "ERROR - Not found Constructor of RetProgramList ...\n");
        return NULL;
    }

    retProgramListObject = env->NewObject(mRetPrgmListCls, mid);
    if(retProgramListObject == NULL) {
        L_ERROR(TAG, "Fail to create new RetProgramList object....\n");
        return NULL;
    }

    if(result == TDS_OK) {
        //////////////////////////////////////////////////////////////////////////////////
        // Create Programs object
        //////////////////////////////////////////////////////////////////////////////////
        if(gPrgmsCls == 0) {
            mPrgmsCls = env->FindClass("com/tvstorm/tv/si/program/Programs");
            gPrgmsCls = (jclass) env->NewGlobalRef(mPrgmsCls);
        }
        if(gPrgmsCls == NULL) {
            L_ERROR(TAG, "ERROR - Not found Programs class...\n");
            return NULL;
        }

        mid = env->GetMethodID(gPrgmsCls, "<init>", "()V");
        if(mid == NULL) {
            L_ERROR(TAG, "ERROR - Not found Constructor of Programs ...\n");
            return NULL;
        }

        programsObject = env->NewObject(gPrgmsCls, mid);
        if(programsObject == NULL) {
            L_ERROR(TAG, "Fail to create new Programs object....\n");
            return NULL;
        }

        fidChannelUid = env->GetFieldID(gPrgmsCls, "mChannelUid", "Ljava/lang/String;");
        if(fidChannelUid == NULL) {
            L_ERROR(TAG, "Fail to find channelUid field of Programs\n");
            env->DeleteLocalRef(programsObject);
            return NULL;
        } else {
            env->SetObjectField(programsObject, fidChannelUid, channelUid);
            env->DeleteLocalRef(channelUid);
        }

        //////////////////////////////////////////////////////////////////////////////////
        // Convert ProgramInfo and add it to Programs list
        //////////////////////////////////////////////////////////////////////////////////
        convertJNIPrograms(env, gPrgmsCls, programsObject, eventList, channelUid);

        //////////////////////////////////////////////////////////////////////////////////
        // Add Programs to RetProgramList
        //////////////////////////////////////////////////////////////////////////////////
        convertJRetProgramList(env, mRetPrgmListCls, retProgramListObject, programsObject);
        env->DeleteLocalRef(programsObject);
    }

    //////////////////////////////////////////////////////////////////////////////////
    // Set Result
    //////////////////////////////////////////////////////////////////////////////////
    setResultOfRetObjectList(env, mRetPrgmListCls, retProgramListObject, result);
    // memory free
    for (list<EventInfo *>::iterator itr = eventList.begin(); itr != eventList.end(); itr++) {
        delete (*itr);
    }
    eventList.clear();

    return retProgramListObject;
}

static jobject JNIGetProgramListByIndex(JNIEnv* env, jobject thiz, jstring channelUid, jint startIdx, jint reqCount)
{
    L_DEBUG(TAG, "Perform JNI [%s].....\n", __FUNCTION__);

    if(env == NULL || thiz == NULL) {
        L_ERROR(TAG, "ERROR - %s has INVALID parameter...\n", __FUNCTION__);
        return NULL;
    }
    if(channelUid == NULL || env->GetStringLength(channelUid) <= 0) {
        L_ERROR(TAG, "ERROR - %s has INVALID parameter...\n", __FUNCTION__);
        return NULL;
    }
    if(g_pSIHandler == NULL) {
        L_ERROR(TAG, "ERROR - %s has GENERAL failure...\n", __FUNCTION__);
        return NULL;
    }

    jclass mRetPrgmListCls;
    jclass mPrgmsCls;
    jmethodID mid;
    jobject retProgramListObject;
    jobject programsObject;
    jfieldID fidChannelUid;

    list<EventInfo*> eventList;
    TDSResult result;

    result = g_pSIHandler->getProgramListByIndex(JNI_javaStringToStdString(env, channelUid), eventList, startIdx, reqCount);

    //////////////////////////////////////////////////////////////////////////////////
    // Creat RetProgramList object
    //////////////////////////////////////////////////////////////////////////////////
    mRetPrgmListCls = env->FindClass("com/tvstorm/tv/si/program/RetProgramList");
    if(mRetPrgmListCls == NULL) {
        L_ERROR(TAG, "ERROR - Not found RetProgramList class...\n");
        return NULL;
    }

    mid = env->GetMethodID(mRetPrgmListCls, "<init>", "()V");
    if(mid == NULL) {
        L_ERROR(TAG, "ERROR - Not found Constructor of RetProgramList ...\n");
        return NULL;
    }

    retProgramListObject = env->NewObject(mRetPrgmListCls, mid);
    if(retProgramListObject == NULL) {
        L_ERROR(TAG, "Fail to create new RetProgramList object....\n");
        return NULL;
    }

    if(result == TDS_OK) {
        //////////////////////////////////////////////////////////////////////////////////
        // Create Programs object
        //////////////////////////////////////////////////////////////////////////////////
        if(gPrgmsCls == 0) {
            mPrgmsCls = env->FindClass("com/tvstorm/tv/si/program/Programs");
            gPrgmsCls = (jclass) env->NewGlobalRef(mPrgmsCls);
        }
        if(gPrgmsCls == NULL) {
            L_ERROR(TAG, "ERROR - Not found Programs class...\n");
            return NULL;
        }

        mid = env->GetMethodID(gPrgmsCls, "<init>", "()V");
        if(mid == NULL) {
            L_ERROR(TAG, "ERROR - Not found Constructor of Programs ...\n");
            return NULL;
        }

        programsObject = env->NewObject(gPrgmsCls, mid);
        if(programsObject == NULL) {
            L_ERROR(TAG, "Fail to create new Programs object....\n");
            return NULL;
        }

        fidChannelUid = env->GetFieldID(gPrgmsCls, "mChannelUid", "Ljava/lang/String;");
        if(fidChannelUid == NULL) {
            L_ERROR(TAG, "Fail to find channelUid field of Programs\n");
            env->DeleteLocalRef(programsObject);
            return NULL;
        } else {
            env->SetObjectField(programsObject, fidChannelUid, channelUid);
            env->DeleteLocalRef(channelUid);
        }

        //////////////////////////////////////////////////////////////////////////////////
        // Convert ProgramInfo and add it to Programs list
        //////////////////////////////////////////////////////////////////////////////////
        convertJNIPrograms(env, gPrgmsCls, programsObject, eventList, channelUid);

        //////////////////////////////////////////////////////////////////////////////////
        // Add Programs to RetProgramList
        //////////////////////////////////////////////////////////////////////////////////
        convertJRetProgramList(env, mRetPrgmListCls, retProgramListObject, programsObject);
        env->DeleteLocalRef(programsObject);
    }

    //////////////////////////////////////////////////////////////////////////////////
    // Set Result
    //////////////////////////////////////////////////////////////////////////////////
    setResultOfRetObjectList(env, mRetPrgmListCls, retProgramListObject, result);
    // memory free
    for (list<EventInfo *>::iterator itr = eventList.begin(); itr != eventList.end(); itr++) {
        delete (*itr);
    }
    eventList.clear();

    return retProgramListObject;
}

static jobject JNIGetProgramListByName(JNIEnv* env, jobject thiz, jstring channelUid, jint reqTime, jstring reqName, jboolean descFlag)
{
    L_DEBUG(TAG, "Perform JNI [%s].....\n", __FUNCTION__);

    if(env == NULL || thiz == NULL) {
        L_ERROR(TAG, "ERROR - %s has INVALID parameter...\n", __FUNCTION__);
        return NULL;
    }
    if(channelUid == NULL || env->GetStringLength(channelUid) <= 0) {
        L_ERROR(TAG, "ERROR - %s has INVALID parameter...\n", __FUNCTION__);
        return NULL;
    }
    if(reqName == NULL || env->GetStringLength(reqName) <= 0) {
        L_ERROR(TAG, "ERROR - %s has INVALID parameter...\n", __FUNCTION__);
        return NULL;
    }
    if(g_pSIHandler == NULL) {
        L_ERROR(TAG, "ERROR - %s has GENERAL failure...\n", __FUNCTION__);
        return NULL;
    }

    jclass mRetPrgmListCls;
    jclass mPrgmsCls;
    jmethodID mid;
    jobject retProgramListObject;
    jobject programsObject;
    jfieldID fidChannelUid;

    list<EventInfo*> eventList;
    TDSResult result;

    result = g_pSIHandler->getProgramListByName(JNI_javaStringToStdString(env, channelUid), eventList, reqTime,
            JNI_javaStringToStdString(env, reqName), descFlag);

    //////////////////////////////////////////////////////////////////////////////////
    // Creat RetProgramList object
    //////////////////////////////////////////////////////////////////////////////////
    mRetPrgmListCls = env->FindClass("com/tvstorm/tv/si/program/RetProgramList");
    if(mRetPrgmListCls == NULL) {
        L_ERROR(TAG, "ERROR - Not found RetProgramList class...\n");
        return NULL;
    }

    mid = env->GetMethodID(mRetPrgmListCls, "<init>", "()V");
    if(mid == NULL) {
        L_ERROR(TAG, "ERROR - Not found Constructor of RetProgramList ...\n");
        return NULL;
    }

    retProgramListObject = env->NewObject(mRetPrgmListCls, mid);
    if(retProgramListObject == NULL) {
        L_ERROR(TAG, "Fail to create new RetProgramList object....\n");
        return NULL;
    }

    if(result == TDS_OK) {
        //////////////////////////////////////////////////////////////////////////////////
        // Create Programs object
        //////////////////////////////////////////////////////////////////////////////////
        if(gPrgmsCls == 0) {
            mPrgmsCls = env->FindClass("com/tvstorm/tv/si/program/Programs");
            gPrgmsCls = (jclass) env->NewGlobalRef(mPrgmsCls);
        }
        if(gPrgmsCls == NULL) {
            L_ERROR(TAG, "ERROR - Not found Programs class...\n");
            return NULL;
        }

        mid = env->GetMethodID(gPrgmsCls, "<init>", "()V");
        if(mid == NULL) {
            L_ERROR(TAG, "ERROR - Not found Constructor of Programs ...\n");
            return NULL;
        }

        programsObject = env->NewObject(gPrgmsCls, mid);
        if(programsObject == NULL) {
            L_ERROR(TAG, "Fail to create new Programs object....\n");
            return NULL;
        }

        fidChannelUid = env->GetFieldID(gPrgmsCls, "mChannelUid", "Ljava/lang/String;");
        if(fidChannelUid == NULL) {
            L_ERROR(TAG, "Fail to find channelUid field of Programs\n");
            env->DeleteLocalRef(programsObject);
            return NULL;
        } else {
            env->SetObjectField(programsObject, fidChannelUid, channelUid);
            env->DeleteLocalRef(channelUid);
        }

        //////////////////////////////////////////////////////////////////////////////////
        // Convert ProgramInfo and add it to Programs list
        //////////////////////////////////////////////////////////////////////////////////
        convertJNIPrograms(env, gPrgmsCls, programsObject, eventList, channelUid);

        //////////////////////////////////////////////////////////////////////////////////
        // Add Programs to RetProgramList
        //////////////////////////////////////////////////////////////////////////////////
        convertJRetProgramList(env, mRetPrgmListCls, retProgramListObject, programsObject);
        env->DeleteLocalRef(programsObject);
    }

    //////////////////////////////////////////////////////////////////////////////////
    // Set Result
    //////////////////////////////////////////////////////////////////////////////////
    setResultOfRetObjectList(env, mRetPrgmListCls, retProgramListObject, result);
    // memory free
    for (list<EventInfo *>::iterator itr = eventList.begin(); itr != eventList.end(); itr++) {
        delete (*itr);
    }
    eventList.clear();

    return retProgramListObject;
}

static jobject JNIGetProgramListBySeries(JNIEnv* env, jobject thiz, jstring channelUid, jint reqTime, jstring reqName, jint seasonNo, jint episodeNo)
{
    L_DEBUG(TAG, "Perform JNI [%s].....\n", __FUNCTION__);

    if(env == NULL || thiz == NULL) {
        L_ERROR(TAG, "ERROR - %s has INVALID parameter...\n", __FUNCTION__);
        return NULL;
    }
    if(channelUid == NULL || env->GetStringLength(channelUid) <= 0) {
        L_ERROR(TAG, "ERROR - %s has INVALID parameter...\n", __FUNCTION__);
        return NULL;
    }
    if(reqName == NULL || env->GetStringLength(reqName) <= 0) {
        L_ERROR(TAG, "ERROR - %s has INVALID parameter...\n", __FUNCTION__);
        return NULL;
    }
    if(g_pSIHandler == NULL) {
        L_ERROR(TAG, "ERROR - %s has GENERAL failure...\n", __FUNCTION__);
        return NULL;
    }

    jclass mRetPrgmListCls;
    jclass mPrgmsCls;
    jmethodID mid;
    jobject retProgramListObject;
    jobject programsObject;
    jfieldID fidChannelUid;

    list<EventInfo*> eventList;
    TDSResult result;

    result = g_pSIHandler->getProgramListBySeries(JNI_javaStringToStdString(env, channelUid), eventList, reqTime,
            JNI_javaStringToStdString(env, reqName), seasonNo, episodeNo);

    //////////////////////////////////////////////////////////////////////////////////
    // Creat RetProgramList object
    //////////////////////////////////////////////////////////////////////////////////
    mRetPrgmListCls = env->FindClass("com/tvstorm/tv/si/program/RetProgramList");
    if(mRetPrgmListCls == NULL) {
        L_ERROR(TAG, "ERROR - Not found RetProgramList class...\n");
        return NULL;
    }

    mid = env->GetMethodID(mRetPrgmListCls, "<init>", "()V");
    if(mid == NULL) {
        L_ERROR(TAG, "ERROR - Not found Constructor of RetProgramList ...\n");
        return NULL;
    }

    retProgramListObject = env->NewObject(mRetPrgmListCls, mid);
    if(retProgramListObject == NULL) {
        L_ERROR(TAG, "Fail to create new RetProgramList object....\n");
        return NULL;
    }

    if(result == TDS_OK) {
        //////////////////////////////////////////////////////////////////////////////////
        // Create Programs object
        //////////////////////////////////////////////////////////////////////////////////
        if(gPrgmsCls == 0) {
            mPrgmsCls = env->FindClass("com/tvstorm/tv/si/program/Programs");
            gPrgmsCls = (jclass) env->NewGlobalRef(mPrgmsCls);
        }
        if(gPrgmsCls == NULL) {
            L_ERROR(TAG, "ERROR - Not found Programs class...\n");
            return NULL;
        }

        mid = env->GetMethodID(gPrgmsCls, "<init>", "()V");
        if(mid == NULL) {
            L_ERROR(TAG, "ERROR - Not found Constructor of Programs ...\n");
            return NULL;
        }

        programsObject = env->NewObject(gPrgmsCls, mid);
        if(programsObject == NULL) {
            L_ERROR(TAG, "Fail to create new Programs object....\n");
            return NULL;
        }

        fidChannelUid = env->GetFieldID(gPrgmsCls, "mChannelUid", "Ljava/lang/String;");
        if(fidChannelUid == NULL) {
            L_ERROR(TAG, "Fail to find channelUid field of Programs\n");
            env->DeleteLocalRef(programsObject);
            return NULL;
        } else {
            env->SetObjectField(programsObject, fidChannelUid, channelUid);
            env->DeleteLocalRef(channelUid);
        }

        //////////////////////////////////////////////////////////////////////////////////
        // Convert ProgramInfo and add it to Programs list
        //////////////////////////////////////////////////////////////////////////////////
        convertJNIPrograms(env, gPrgmsCls, programsObject, eventList, channelUid);

        //////////////////////////////////////////////////////////////////////////////////
        // Add Programs to RetProgramList
        //////////////////////////////////////////////////////////////////////////////////
        convertJRetProgramList(env, mRetPrgmListCls, retProgramListObject, programsObject);
        env->DeleteLocalRef(programsObject);
    }

    //////////////////////////////////////////////////////////////////////////////////
    // Set Result
    //////////////////////////////////////////////////////////////////////////////////
    setResultOfRetObjectList(env, mRetPrgmListCls, retProgramListObject, result);
    // memory free
    for (list<EventInfo *>::iterator itr = eventList.begin(); itr != eventList.end(); itr++) {
        delete (*itr);
    }
    eventList.clear();

    return retProgramListObject;
}

static jobject JNIGetProgramListByTime(JNIEnv* env, jobject thiz, jstring channelUid, jint startTime, jint endTime)
{
    L_DEBUG(TAG, "Perform JNI [%s].....\n", __FUNCTION__);

    if(env == NULL || thiz == NULL) {
        L_ERROR(TAG, "ERROR - %s has INVALID parameter...\n", __FUNCTION__);
        return NULL;
    }
    if(channelUid == NULL || env->GetStringLength(channelUid) <= 0) {
        L_ERROR(TAG, "ERROR - %s has INVALID parameter...\n", __FUNCTION__);
        return NULL;
    }
    if(g_pSIHandler == NULL) {
        L_ERROR(TAG, "ERROR - %s has GENERAL failure...\n", __FUNCTION__);
        return NULL;
    }

    jclass mRetPrgmListCls;
    jclass mPrgmsCls;
    jmethodID mid;
    jobject retProgramListObject;
    jobject programsObject;
    jfieldID fidChannelUid;

    list<EventInfo*> eventList;
    TDSResult result;

    result = g_pSIHandler->getProgramListByTime(JNI_javaStringToStdString(env, channelUid), eventList, startTime, endTime);

    //////////////////////////////////////////////////////////////////////////////////
    // Creat RetProgramList object
    //////////////////////////////////////////////////////////////////////////////////
    mRetPrgmListCls = env->FindClass("com/tvstorm/tv/si/program/RetProgramList");
    if(mRetPrgmListCls == NULL) {
        L_ERROR(TAG, "ERROR - Not found RetProgramList class...\n");
        return NULL;
    }

    mid = env->GetMethodID(mRetPrgmListCls, "<init>", "()V");
    if(mid == NULL) {
        L_ERROR(TAG, "ERROR - Not found Constructor of RetProgramList ...\n");
        return NULL;
    }

    retProgramListObject = env->NewObject(mRetPrgmListCls, mid);
    if(retProgramListObject == NULL) {
        L_ERROR(TAG, "Fail to create new RetProgramList object....\n");
        return NULL;
    }

    if(result == TDS_OK) {
        //////////////////////////////////////////////////////////////////////////////////
        // Create Programs object
        //////////////////////////////////////////////////////////////////////////////////
        if(gPrgmsCls == 0) {
            mPrgmsCls = env->FindClass("com/tvstorm/tv/si/program/Programs");
            gPrgmsCls = (jclass) env->NewGlobalRef(mPrgmsCls);
        }
        if(gPrgmsCls == NULL) {
            L_ERROR(TAG, "ERROR - Not found Programs class...\n");
            return NULL;
        }

        mid = env->GetMethodID(gPrgmsCls, "<init>", "()V");
        if(mid == NULL) {
            L_ERROR(TAG, "ERROR - Not found Constructor of Programs ...\n");
            return NULL;
        }

        programsObject = env->NewObject(gPrgmsCls, mid);
        if(programsObject == NULL) {
            L_ERROR(TAG, "Fail to create new Programs object....\n");
            return NULL;
        }

        fidChannelUid = env->GetFieldID(gPrgmsCls, "mChannelUid", "Ljava/lang/String;");
        if(fidChannelUid == NULL) {
            L_ERROR(TAG, "Fail to find channelUid field of Programs\n");
            env->DeleteLocalRef(programsObject);
            return NULL;
        } else {
            env->SetObjectField(programsObject, fidChannelUid, channelUid);
            env->DeleteLocalRef(channelUid);
        }

        //////////////////////////////////////////////////////////////////////////////////
        // Convert ProgramInfo and add it to Programs list
        //////////////////////////////////////////////////////////////////////////////////
        convertJNIPrograms(env, gPrgmsCls, programsObject, eventList, channelUid);

        //////////////////////////////////////////////////////////////////////////////////
        // Add Programs to RetProgramList
        //////////////////////////////////////////////////////////////////////////////////
        convertJRetProgramList(env, mRetPrgmListCls, retProgramListObject, programsObject);
        env->DeleteLocalRef(programsObject);
    }

    //////////////////////////////////////////////////////////////////////////////////
    // Set Result
    //////////////////////////////////////////////////////////////////////////////////
    setResultOfRetObjectList(env, mRetPrgmListCls, retProgramListObject, result);
    // memory free
    for (list<EventInfo *>::iterator itr = eventList.begin(); itr != eventList.end(); itr++) {
        delete (*itr);
    }
    eventList.clear();

    return retProgramListObject;
}

static jobject JNIGetMultiChannelProgramList(JNIEnv* env, jobject thiz, jobjectArray channelUidArray)
{
    L_DEBUG(TAG, "Perform JNI [%s].....\n", __FUNCTION__);

    if(env == NULL || thiz == NULL) {
        L_ERROR(TAG, "ERROR - %s has INVALID parameter...\n", __FUNCTION__);
        return NULL;
    }
    if(channelUidArray == NULL || env->GetArrayLength(channelUidArray) <= 0) {
        L_ERROR(TAG, "ERROR - %s has INVALID parameter...\n", __FUNCTION__);
        return NULL;
    }
    if(g_pSIHandler == NULL) {
        L_ERROR(TAG, "ERROR - %s has GENERAL failure...\n", __FUNCTION__);
        return NULL;
    }

    jclass mRetPrgmListCls;
    jclass mPrgmsCls;
    jmethodID mid;
    jobject retProgramListObject;
    jobject programsObject;
    jfieldID fidChannelUid;

    //////////////////////////////////////////////////////////////////////////////////
    // Tokenize channel list
    //////////////////////////////////////////////////////////////////////////////////
    list < string > listChannelUid;
    int size = env->GetArrayLength(channelUidArray);
    for (int i = 0; i < size; i++) {
        jstring ChannelUid = (jstring) env->GetObjectArrayElement(channelUidArray, i);

        if(ChannelUid != NULL && env->GetStringLength(ChannelUid) > 0) {
            listChannelUid.push_back(JNI_javaStringToStdString(env, ChannelUid));
        }
    }

    L_DEBUG(TAG, "%s - channelCount[%d]", __FUNCTION__, listChannelUid.size());

    map<string, list<EventInfo*> > mapEventInfo;
    TDSResult result;

    result = g_pSIHandler->getMultiChannelProgramList(listChannelUid, mapEventInfo);

    //////////////////////////////////////////////////////////////////////////////////
    // Creat RetProgramList object
    //////////////////////////////////////////////////////////////////////////////////
    mRetPrgmListCls = env->FindClass("com/tvstorm/tv/si/program/RetProgramList");

    if(mRetPrgmListCls == NULL) {
        L_ERROR(TAG, "ERROR - Not found RetProgramList class...\n");
        return NULL;
    }

    mid = env->GetMethodID(mRetPrgmListCls, "<init>", "()V");
    if(mid == NULL) {
        L_ERROR(TAG, "ERROR - Not found Constructor of RetProgramList ...\n");
        return NULL;
    }

    retProgramListObject = env->NewObject(mRetPrgmListCls, mid);
    if(retProgramListObject == NULL) {
        L_ERROR(TAG, "Fail to create new RetProgramList object....\n");
        return NULL;
    }

    if(result == TDS_OK) {
        string channelUid;
        int eventCount = 0;

        //////////////////////////////////////////////////////////////////////////////////
        // Create Programs object
        //////////////////////////////////////////////////////////////////////////////////
        if(gPrgmsCls == 0) {
            mPrgmsCls = env->FindClass("com/tvstorm/tv/si/program/Programs");
            gPrgmsCls = (jclass) env->NewGlobalRef(mPrgmsCls);
        }
        if(gPrgmsCls == NULL) {
            L_ERROR(TAG, "ERROR - Not found Programs class...\n");
            return NULL;
        }

        for (map<string, list<EventInfo*> >::iterator itr = mapEventInfo.begin(); itr != mapEventInfo.end(); itr++) {
            channelUid = itr->first;
            list<EventInfo *> eventList = itr->second;
            eventCount = eventList.size();
            L_DEBUG(TAG, "getMultiChannel - Ch[%s]/EventCount[%d]\n", channelUid.c_str(), eventCount);

            //////////////////////////////////////////////////////////////////////////////////
            // Create Programs object
            //////////////////////////////////////////////////////////////////////////////////
            mid = env->GetMethodID(gPrgmsCls, "<init>", "()V");
            if(mid == NULL) {
                L_ERROR(TAG, "ERROR - Not found Constructor of Programs ...\n");
                return NULL;
            }

            programsObject = env->NewObject(gPrgmsCls, mid);
            if(programsObject == NULL) {
                L_ERROR(TAG, "Fail to create new Programs object....\n");
                return NULL;
            }

            //////////////////////////////////////////////////////////////////////////////////
            // Set Channel Uid
            //////////////////////////////////////////////////////////////////////////////////
            fidChannelUid = env->GetFieldID(gPrgmsCls, "mChannelUid", "Ljava/lang/String;");
            if(fidChannelUid == NULL) {
                L_ERROR(TAG, "Fail to find channelUid field of Programs\n");
                env->DeleteLocalRef(programsObject);
                return NULL;
            } else {
                setStringToJavaParam(env, programsObject, fidChannelUid, channelUid);
            }

            //////////////////////////////////////////////////////////////////////////////////
            // Convert ProgramInfo and add it to Programs list
            //////////////////////////////////////////////////////////////////////////////////
            convertJNIPrograms(env, gPrgmsCls, programsObject, eventList, env->NewStringUTF(channelUid.c_str()));

            //////////////////////////////////////////////////////////////////////////////////
            // Add Programs to RetProgramList
            //////////////////////////////////////////////////////////////////////////////////
            convertJRetProgramList(env, mRetPrgmListCls, retProgramListObject, programsObject);
            env->DeleteLocalRef(programsObject);
        }
    }

    //////////////////////////////////////////////////////////////////////////////////
    // Set Result
    //////////////////////////////////////////////////////////////////////////////////
    setResultOfRetObjectList(env, mRetPrgmListCls, retProgramListObject, result);
    // memory free
    for (map<string, list<EventInfo*> >::iterator itr = mapEventInfo.begin(); itr != mapEventInfo.end(); itr++) {
        list<EventInfo *> eventList = itr->second;
        for (list<EventInfo *>::iterator ii = eventList.begin(); ii != eventList.end(); ii++) {
            delete (*ii);
        }
        eventList.clear();
    }
    mapEventInfo.clear();

    return retProgramListObject;
}

static jobject JNIGetMultiChannelProgramListByIndex(JNIEnv* env, jobject thiz, jobjectArray channelUidArray, jint startIndex, jint reqCount)
{
    L_DEBUG(TAG, "Perform JNI [%s].....\n", __FUNCTION__);

    if(env == NULL || thiz == NULL) {
        L_ERROR(TAG, "ERROR - %s has INVALID parameter...\n", __FUNCTION__);
        return NULL;
    }
    if(channelUidArray == NULL || env->GetArrayLength(channelUidArray) <= 0) {
        L_ERROR(TAG, "ERROR - %s has INVALID parameter...\n", __FUNCTION__);
        return NULL;
    }
    if(g_pSIHandler == NULL) {
        L_ERROR(TAG, "ERROR - %s has GENERAL failure...\n", __FUNCTION__);
        return NULL;
    }

    jclass mRetPrgmListCls;
    jclass mPrgmsCls;
    jmethodID mid;
    jobject retProgramListObject;
    jobject programsObject;
    jfieldID fidChannelUid;

    //////////////////////////////////////////////////////////////////////////////////
    // Tokenize channel list
    //////////////////////////////////////////////////////////////////////////////////
    list < string > listChannelUid;
    int size = env->GetArrayLength(channelUidArray);
    for (int i = 0; i < size; i++) {
        jstring ChannelUid = (jstring) env->GetObjectArrayElement(channelUidArray, i);

        if(ChannelUid != NULL && env->GetStringLength(ChannelUid) > 0) {
            listChannelUid.push_back(JNI_javaStringToStdString(env, ChannelUid));
        }
    }

    L_DEBUG(TAG, "%s - channelCount[%d]\n", __FUNCTION__, listChannelUid.size());

    map<string, list<EventInfo*> > mapEventInfo;
    TDSResult result;

    result = g_pSIHandler->getMultiChannelProgramListByIndex(listChannelUid, mapEventInfo, startIndex, reqCount);

    //////////////////////////////////////////////////////////////////////////////////
    // Creat RetProgramList object
    //////////////////////////////////////////////////////////////////////////////////
    mRetPrgmListCls = env->FindClass("com/tvstorm/tv/si/program/RetProgramList");

    if(mRetPrgmListCls == NULL) {
        L_ERROR(TAG, "ERROR - Not found RetProgramList class...\n");
        return NULL;
    }

    mid = env->GetMethodID(mRetPrgmListCls, "<init>", "()V");
    if(mid == NULL) {
        L_ERROR(TAG, "ERROR - Not found Constructor of RetProgramList ...\n");
        return NULL;
    }

    retProgramListObject = env->NewObject(mRetPrgmListCls, mid);
    if(retProgramListObject == NULL) {
        L_ERROR(TAG, "Fail to create new RetProgramList object....\n");
        return NULL;
    }

    if(result == TDS_OK) {
        string channelUid;
        int eventCount = 0;

        //////////////////////////////////////////////////////////////////////////////////
        // Create Programs object
        //////////////////////////////////////////////////////////////////////////////////
        if(gPrgmsCls == 0) {
            mPrgmsCls = env->FindClass("com/tvstorm/tv/si/program/Programs");
            gPrgmsCls = (jclass) env->NewGlobalRef(mPrgmsCls);
        }
        if(gPrgmsCls == NULL) {
            L_ERROR(TAG, "ERROR - Not found Programs class...\n");
            return NULL;
        }

        for (map<string, list<EventInfo*> >::iterator itr = mapEventInfo.begin(); itr != mapEventInfo.end(); itr++) {
            channelUid = itr->first;
            list<EventInfo *> eventList = itr->second;
            eventCount = eventList.size();
            L_DEBUG(TAG, "getMultiChannel - Ch[%s]/EventCount[%d]\n", channelUid.c_str(), eventCount);

            //////////////////////////////////////////////////////////////////////////////////
            // Create Programs object
            //////////////////////////////////////////////////////////////////////////////////
            mid = env->GetMethodID(gPrgmsCls, "<init>", "()V");
            if(mid == NULL) {
                L_ERROR(TAG, "ERROR - Not found Constructor of Programs ...\n");
                return NULL;
            }

            programsObject = env->NewObject(gPrgmsCls, mid);
            if(programsObject == NULL) {
                L_ERROR(TAG, "Fail to create new Programs object....\n");
                return NULL;
            }

            //////////////////////////////////////////////////////////////////////////////////
            // Set Channel Uid
            //////////////////////////////////////////////////////////////////////////////////
            fidChannelUid = env->GetFieldID(gPrgmsCls, "mChannelUid", "Ljava/lang/String;");
            if(fidChannelUid == NULL) {
                L_ERROR(TAG, "Fail to find channelUid field of Programs\n");
                env->DeleteLocalRef(programsObject);
                return NULL;
            } else {
                setStringToJavaParam(env, programsObject, fidChannelUid, channelUid);
            }

            //////////////////////////////////////////////////////////////////////////////////
            // Convert ProgramInfo and add it to Programs list
            //////////////////////////////////////////////////////////////////////////////////
            convertJNIPrograms(env, gPrgmsCls, programsObject, eventList, env->NewStringUTF(channelUid.c_str()));

            //////////////////////////////////////////////////////////////////////////////////
            // Add Programs to RetProgramList
            //////////////////////////////////////////////////////////////////////////////////
            convertJRetProgramList(env, mRetPrgmListCls, retProgramListObject, programsObject);
            env->DeleteLocalRef(programsObject);
        }
    }

    //////////////////////////////////////////////////////////////////////////////////
    // Set Result
    //////////////////////////////////////////////////////////////////////////////////
    setResultOfRetObjectList(env, mRetPrgmListCls, retProgramListObject, result);
    // memory free
    for (map<string, list<EventInfo*> >::iterator itr = mapEventInfo.begin(); itr != mapEventInfo.end(); itr++) {
        //channelUid = itr->first;
        list<EventInfo *> eventList = itr->second;
        for (list<EventInfo *>::iterator ii = eventList.begin(); ii != eventList.end(); ii++) {
            delete (*ii);
        }
        eventList.clear();
    }
    mapEventInfo.clear();

    return retProgramListObject;
}

static jobject JNIGetMultiChannelProgramListByTime(JNIEnv* env, jobject thiz, jobjectArray channelUidArray, jint startTime, jint endTime)
{
    L_DEBUG(TAG, "Perform JNI [%s].....\n", __FUNCTION__);

    if(env == NULL || thiz == NULL) {
        L_ERROR(TAG, "ERROR - %s has INVALID parameter...\n", __FUNCTION__);
        return NULL;
    }
    if(channelUidArray == NULL || env->GetArrayLength(channelUidArray) <= 0) {
        L_ERROR(TAG, "ERROR - %s has INVALID parameter...\n", __FUNCTION__);
        return NULL;
    }
    if(g_pSIHandler == NULL) {
        L_ERROR(TAG, "ERROR - %s has GENERAL failure...\n", __FUNCTION__);
        return NULL;
    }

    jclass mRetPrgmListCls;
    jclass mPrgmsCls;
    jmethodID mid;
    jobject retProgramListObject;
    jobject programsObject;
    jfieldID fidChannelUid;

    //////////////////////////////////////////////////////////////////////////////////
    // Tokenize channel list
    //////////////////////////////////////////////////////////////////////////////////
    list < string > listChannelUid;
    int size = env->GetArrayLength(channelUidArray);
    for (int i = 0; i < size; i++) {
        jstring ChannelUid = (jstring) env->GetObjectArrayElement(channelUidArray, i);

        if(ChannelUid != NULL && env->GetStringLength(ChannelUid) > 0) {
            listChannelUid.push_back(JNI_javaStringToStdString(env, ChannelUid));
        }
    }

    L_DEBUG(TAG, "%s - channelCount[%d]\n", __FUNCTION__, listChannelUid.size());

    map<string, list<EventInfo*> > mapEventInfo;
    TDSResult result;

    result = g_pSIHandler->getMultiChannelProgramListByTime(listChannelUid, mapEventInfo, startTime, endTime);

    //////////////////////////////////////////////////////////////////////////////////
    // Creat RetProgramList object
    //////////////////////////////////////////////////////////////////////////////////
    mRetPrgmListCls = env->FindClass("com/tvstorm/tv/si/program/RetProgramList");

    if(mRetPrgmListCls == NULL) {
        L_ERROR(TAG, "ERROR - Not found RetProgramList class...\n");
        return NULL;
    }

    mid = env->GetMethodID(mRetPrgmListCls, "<init>", "()V");
    if(mid == NULL) {
        L_ERROR(TAG, "ERROR - Not found Constructor of RetProgramList ...\n");
        return NULL;
    }

    retProgramListObject = env->NewObject(mRetPrgmListCls, mid);
    if(retProgramListObject == NULL) {
        L_ERROR(TAG, "Fail to create new RetProgramList object....\n");
        return NULL;
    }

    if(result == TDS_OK) {
        string channelUid;
        int eventCount = 0;

        //////////////////////////////////////////////////////////////////////////////////
        // Create Programs object
        //////////////////////////////////////////////////////////////////////////////////
        if(gPrgmsCls == 0) {
            mPrgmsCls = env->FindClass("com/tvstorm/tv/si/program/Programs");
            gPrgmsCls = (jclass) env->NewGlobalRef(mPrgmsCls);
        }
        if(gPrgmsCls == NULL) {
            L_ERROR(TAG, "ERROR - Not found Programs class...\n");
            return NULL;
        }

        for (map<string, list<EventInfo*> >::iterator itr = mapEventInfo.begin(); itr != mapEventInfo.end(); itr++) {
            channelUid = itr->first;
            list<EventInfo *> eventList = itr->second;
            eventCount = eventList.size();
            L_DEBUG(TAG, "getMultiChannel - Ch[%s]/EventCount[%d]\n", channelUid.c_str(), eventCount);

            //////////////////////////////////////////////////////////////////////////////////
            // Create Programs object
            //////////////////////////////////////////////////////////////////////////////////
            mid = env->GetMethodID(gPrgmsCls, "<init>", "()V");
            if(mid == NULL) {
                L_ERROR(TAG, "ERROR - Not found Constructor of Programs ...\n");
                return NULL;
            }

            programsObject = env->NewObject(gPrgmsCls, mid);
            if(programsObject == NULL) {
                L_ERROR(TAG, "Fail to create new Programs object....\n");
                return NULL;
            }

            //////////////////////////////////////////////////////////////////////////////////
            // Set Channel Uid
            //////////////////////////////////////////////////////////////////////////////////
            fidChannelUid = env->GetFieldID(gPrgmsCls, "mChannelUid", "Ljava/lang/String;");
            if(fidChannelUid == NULL) {
                L_ERROR(TAG, "Fail to find channelUid field of Programs\n");
                env->DeleteLocalRef(programsObject);
                return NULL;
            } else {
                setStringToJavaParam(env, programsObject, fidChannelUid, channelUid);
            }

            //////////////////////////////////////////////////////////////////////////////////
            // Convert ProgramInfo and add it to Programs list
            //////////////////////////////////////////////////////////////////////////////////
            convertJNIPrograms(env, gPrgmsCls, programsObject, eventList, env->NewStringUTF(channelUid.c_str()));

            //////////////////////////////////////////////////////////////////////////////////
            // Add Programs to RetProgramList
            //////////////////////////////////////////////////////////////////////////////////
            convertJRetProgramList(env, mRetPrgmListCls, retProgramListObject, programsObject);
            env->DeleteLocalRef(programsObject);
        }
    }

    //////////////////////////////////////////////////////////////////////////////////
    // Set Result
    //////////////////////////////////////////////////////////////////////////////////
    setResultOfRetObjectList(env, mRetPrgmListCls, retProgramListObject, result);
    // memory free
    for (map<string, list<EventInfo*> >::iterator itr = mapEventInfo.begin(); itr != mapEventInfo.end(); itr++) {
        //channelUid = itr->first;
        list<EventInfo *> eventList = itr->second;
        for (list<EventInfo *>::iterator ii = eventList.begin(); ii != eventList.end(); ii++) {
            delete (*ii);
        }
        eventList.clear();
    }
    mapEventInfo.clear();

    return retProgramListObject;
}

static const char* classPathName = "com/tvstorm/tv/siservice/TVSIServiceManager";

static JNINativeMethod methods[] = { //
        { "start", "(Ljava/lang/String;I)I", (void*) JNIStart }, // setters
                { "stop", "()I", (void*) JNIStop }, // setters
                { "restart", "(Ljava/lang/String;I)I", (void*) JNIRestart }, // setters
                { "startAutoScan", "(Ljava/lang/String;III)I", (void*) JNIStartAutoScan }, // setters
                { "startManualScan", "(Ljava/lang/String;II)I", (void*) JNIStartManualScan }, // setters
                { "startManualChannelScan", "(Ljava/lang/String;II)I", (void*) JNIStartManualChannelScan }, // setters
                { "startScan", "(Ljava/lang/String;I)I", (void*) JNIStartScan }, // setters
                { "stopScan", "()I", (void*) JNIStopScan }, // setters
                { "setCurrentChannel", "(Ljava/lang/String;)I", (void*) JNISetCurrentChannel }, // setters
                { "setNetworkConnectionInfo", "(Z)I", (void*) JNISetNetworkConnectionInfo }, // setters
                { "getDefaultChannel", "()Ljava/lang/String;", (void*) JNIGetDefaultChannel }, // getters
                { "getChannelList", "(I)Lcom/tvstorm/tv/si/channel/RetChannelList;", (void*) JNIGetChannelList }, // getters
                { "getCurrentProgram", "(Ljava/lang/String;)Lcom/tvstorm/tv/si/program/RetProgramList;", (void*) JNIGetCurrentProgram }, // getters
                { "getProgramList", "(Ljava/lang/String;)Lcom/tvstorm/tv/si/program/RetProgramList;", (void*) JNIGetProgramList }, // getters
                { "getProgramListByIndex", "(Ljava/lang/String;II)Lcom/tvstorm/tv/si/program/RetProgramList;", (void*) JNIGetProgramListByIndex }, // getters
                { "getProgramListByName", "(Ljava/lang/String;ILjava/lang/String;Z)Lcom/tvstorm/tv/si/program/RetProgramList;",
                        (void*) JNIGetProgramListByName }, // getters
                { "getProgramListBySeries", "(Ljava/lang/String;ILjava/lang/String;II)Lcom/tvstorm/tv/si/program/RetProgramList;",
                        (void*) JNIGetProgramListBySeries }, // getters
                { "getProgramListByTime", "(Ljava/lang/String;II)Lcom/tvstorm/tv/si/program/RetProgramList;", (void*) JNIGetProgramListByTime }, // getters
                { "getMultiChannelProgramList", "([Ljava/lang/String;)Lcom/tvstorm/tv/si/program/RetProgramList;",
                        (void*) JNIGetMultiChannelProgramList }, // getters
                { "getMultiChannelProgramListByTime", "([Ljava/lang/String;II)Lcom/tvstorm/tv/si/program/RetProgramList;",
                        (void*) JNIGetMultiChannelProgramListByTime }, // getters
                { "getMultiChannelProgramListByIndex", "([Ljava/lang/String;II)Lcom/tvstorm/tv/si/program/RetProgramList;",
                        (void*) JNIGetMultiChannelProgramListByIndex }, // getters
        };

int registerNativeMethods(JNIEnv* env, const char* className, JNINativeMethod* gMethods, int numMethods)
{
    jclass clazz;

    clazz = env->FindClass(className);
    if(clazz == NULL) {
        L_ERROR(TAG, "Native registration unable to find class '%s'\n", className);
        return JNI_FALSE;
    }

    if(env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        L_ERROR(TAG, "RegisterNatives failed for '%s'\n", className);
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

int registerNatives(JNIEnv* env)
{
    if(!registerNativeMethods(env, classPathName, methods, sizeof(methods) / sizeof(methods[0]))) {
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

typedef union {
    JNIEnv* env;
    void* venv;
} UnionJNIEnvToVoid;

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNI_SetVM(vm);

    UnionJNIEnvToVoid uenv;
    uenv.venv = NULL;
    jint result = -1;
    JNIEnv* env = NULL;

    L_INFO(TAG, "JNI_OnLoad\n");

    if(vm->GetEnv(&uenv.venv, JNI_VERSION_1_6) != JNI_OK) {
        L_ERROR(TAG, "ERROR: GetEnv Failed!\n");
        return result;
    }
    env = uenv.env;

    if(registerNatives(env) != JNI_TRUE) {
        L_ERROR(TAG, "ERROR: registerNatives failed!\n");
        return result;
    }
    result = JNI_VERSION_1_6;

    L_INFO(TAG, "SUCCESS TO REGISTER NATIVE METHOD......\n");
    return result;
}
