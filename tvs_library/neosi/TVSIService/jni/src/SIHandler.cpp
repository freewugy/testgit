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

#include "SIHandler.h"
#include <map>
#include <list>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "Logger.h"
static const char* TAG = "SIHandler";

SIHandler::SIHandler()
{
    SIManager& siManager = SIManager::getInstance();
    siManager.addUpdateEventListener(this);

    siManager.initialize();
}

SIHandler::~SIHandler()
{
    // TODO Auto-generated destructor stub
}

void SIHandler::start(string epgUri, int mode)
{
    L_INFO(TAG, "epgUri=%s, mode=%d\n", epgUri.c_str(), mode);
	int tunerId = 0;
	setProvUri(epgUri, tunerId);
    SIManager& siManager = SIManager::getInstance();
    siManager.Start(epgUri, mode);
}

void SIHandler::stop()
{
    L_DEBUG(TAG, "=== Called\n");
    SIManager& siManager = SIManager::getInstance();
    siManager.Stop();
}

void SIHandler::autoScanStart(string epgUri, int startFreqMHz, int endFreqMHz, int tunerId)
{
    L_DEBUG(TAG, "%s Called", __FUNCTION__);
    setProvUri(epgUri, tunerId);
    SIManager& siManager = SIManager::getInstance();
    siManager.AutoScanStart(epgUri, startFreqMHz, endFreqMHz, tunerId);
}

void SIHandler::manualScanStart(string epgUri, int freqMHz, int tunerId)
{
	L_DEBUG(TAG, "%s Called", __FUNCTION__);
    setProvUri(epgUri, tunerId);

    L_INFO(TAG, "epgUri=%s, freqMHz=%d, tunerId=%d", epgUri.c_str(), freqMHz, tunerId);
    SIManager& siManager = SIManager::getInstance();
    siManager.ManualScanStart(epgUri, freqMHz, tunerId);
}

void SIHandler::manualChannelScanStart(string epgUri, int freqMHz, int networkId)
{
    L_DEBUG(TAG, "%s Called", __FUNCTION__);
    int tunerId = 0;
    setProvUri(epgUri, tunerId);

    L_INFO(TAG, "epgUri=%s, freqMHz=%d, networkId=%d", epgUri.c_str(), freqMHz, networkId);
    SIManager& siManager = SIManager::getInstance();
    siManager.ManualChannelScanStart(epgUri, freqMHz, networkId);
}

void SIHandler::scanStart(string epgUri, int mode)
{
    L_DEBUG(TAG, "%s Called", __FUNCTION__);
    int tunerId = 0;
    setProvUri(epgUri, tunerId);

    L_INFO(TAG, "epgUri=%s, mode=%d", epgUri.c_str(), mode);
    SIManager& siManager = SIManager::getInstance();
    siManager.scanStart(epgUri, mode);
}

void SIHandler::scanStop()
{
    L_DEBUG(TAG, "%s Called", __FUNCTION__);

    SIManager& siManager = SIManager::getInstance();
    siManager.scanStop();
}

void SIHandler::setNetworkConnectionInfo(bool isConnected)
{
    SIManager& siManager = SIManager::getInstance();
    siManager.setNetworkConnectionInfo(isConnected);
}

void SIHandler::setCurrentService(string channelUid)
{
//        L_DEBUG(TAG, "%s Called", __FUNCTION__);
//
//        mCurrentService = channelUid;
//        SIManager& siManager = SIManager::getInstance();
////        siManager.setCurrentService(mCurrentService);
}

void SIHandler::receiveSIChangeEvent(SIChangeEvent* e)
{
//        L_INFO(TAG, "%s Called", e->toString().c_str());

        switch (e->getEventType()) {
                case SIChangeEvent::UNKNOWN:
                        break;
                case SIChangeEvent::CHANNEL_COMPLETED:
                        break;
                case SIChangeEvent::CHANNEL_UPDATED:
                        break;
                case SIChangeEvent::CHANNEL_ADD_EVENT:
                        break;
                case SIChangeEvent::CHANNEL_MODIFY_EVENT:
                        break;
                case SIChangeEvent::CHANNEL_REMOVE_EVENT:
                        break;
                case SIChangeEvent::CHANNEL_MAP_COMPLETED:
                notifyChannelUpdated(e->getChannelVersion(), e->getDefaultChannel());
                        break;
                case SIChangeEvent::CHANNEL_MAP_READY:
				notifyChannelReady(e->getChannelVersion());
                        break;
                case SIChangeEvent::CHANNEL_PMT:
                notifyChannelPMT();
                        break;
                case SIChangeEvent::CHANNEL_PROGRAM_MAP_READY:
                        break;
                case SIChangeEvent::PROGRAM_COMPLETED:
                        break;
                case SIChangeEvent::PROGRAM_UPDATED:
                        break;
                case SIChangeEvent::PROGRAM_ADD_EVENT:
                        break;
                case SIChangeEvent::PROGRAM_MODIFY_EVENT:
                        break;
                case SIChangeEvent::PROGRAM_REMOVE_EVENT:
                        break;
                case SIChangeEvent::PROGRAM_MAP_COMPLETED:
                notifyProgramUpdated();
                        break;
                case SIChangeEvent::PROGRAM_PROCESSING:
                        break;
                case SIChangeEvent::PROGRAM_MAP_READY:
                        break;
                case SIChangeEvent::SCAN_READY:
                notifyScanReady(e->getToBeReceived());
                        break;
                case SIChangeEvent::SCAN_PROCESSING:
                notifyScanProcessing(e->getReceived(), e->getFoundChannel());
                        break;
                case SIChangeEvent::SCAN_COMPLETED:
                notifyScanCompleted(e->getFoundChannel());
                        break;
                case SIChangeEvent::SCAN_STOP:
                notifyScanStop();
                        break;
                case SIChangeEvent::DATA_READING:
                        break;
                case SIChangeEvent::DATA_EMPTY:
                        break;
                case SIChangeEvent::TDT_DATE:
                notifyTDTDate(e->getTDTDate());
                        break;

                case SIChangeEvent::SET_DEFAULT_CH:
				notifyDefaultChannel(e->getChannelUID());
                        break;

                case SIChangeEvent::ERROR_WEPG_CONNECTION:
                notifyErrorWepgConn();
                        break;

                case SIChangeEvent::ERROR_HAPPENED:
                        break;
                default:
                L_INFO(TAG, "NOT SUPPORTED event type : %s", e->toString().c_str());
                        break;
        }
}

TDSResult SIHandler::getDefaultChannel(string *channelUid)
{
    SIManager& siManager = SIManager::getInstance();
    *channelUid = siManager.getDefaultChannel();
    L_DEBUG(TAG, "force_tunning_channel : %s\n", (*channelUid).c_str());
    return TDS_OK;
}

TDSResult SIHandler::getChannelList(int tvRadioMode, list<ServiceInfo *> &listServiceInfo)
{
    SIManager& siManager = SIManager::getInstance();
    listServiceInfo = siManager.getChannelList(tvRadioMode);

    if(listServiceInfo.empty() == false) {
        return TDS_OK;
    } else {
        return TDS_INFO_EMPTY;
    }
}

TDSResult SIHandler::getCurrentProgram(string channelUid, list<EventInfo*> &listEventInfo)
{
    SIManager& siManager = SIManager::getInstance();
    EventInfo* eventInfo = siManager.getCurrentProgram(channelUid);

    if(eventInfo == NULL) {
        return TDS_INFO_EMPTY;
    }

    listEventInfo.push_back(eventInfo);
    return TDS_OK;
}

TDSResult SIHandler::getProgramList(string channelUid, list<EventInfo*> &listEventInfo)
{
    SIManager& siManager = SIManager::getInstance();
    listEventInfo = siManager.getProgramList(channelUid);

    if(listEventInfo.empty() == false) {
        return TDS_OK;
    } else {
        return TDS_INFO_EMPTY;
    }
}

TDSResult SIHandler::getProgramListByIndex(string channelUid, list<EventInfo*> &listEventInfo, int startIdx, int reqCount)
{
    SIManager& siManager = SIManager::getInstance();
    listEventInfo = siManager.getProgramListByIndex(channelUid, startIdx, reqCount);

    if(listEventInfo.empty() == false) {
        return TDS_OK;
    } else {
        return TDS_INFO_EMPTY;
    }
}

TDSResult SIHandler::getProgramListByName(string channelUid, list<EventInfo*> &listEventInfo, int reqTime, string reqName, bool descFlag)
{
    SIManager& siManager = SIManager::getInstance();
    listEventInfo = siManager.getProgramListByName(channelUid, reqTime, reqName, descFlag);

    if(listEventInfo.empty() == false) {
        return TDS_OK;
    } else {
        return TDS_INFO_EMPTY;
    }
}

TDSResult SIHandler::getProgramListBySeries(string channelUid, list<EventInfo*> &listEventInfo, int reqTime, string reqName, int seasonNo,
        int episodeNo)
{
    SIManager& siManager = SIManager::getInstance();
    listEventInfo = siManager.getProgramListBySeries(channelUid, reqTime, reqName, seasonNo, episodeNo);

    if(listEventInfo.empty() == false) {
        return TDS_OK;
    } else {
        return TDS_INFO_EMPTY;
    }
}

TDSResult SIHandler::getProgramListByTime(string channelUid, list<EventInfo*> &listEventInfo, int startTime, int endTime)
{
    SIManager& siManager = SIManager::getInstance();
    listEventInfo = siManager.getProgramListByTime(channelUid, startTime, endTime);

    if(listEventInfo.empty() == false) {
        return TDS_OK;
    } else {
        return TDS_INFO_EMPTY;
    }
}

TDSResult SIHandler::getMultiChannelProgramList(list<string> channelList, map<string, list<EventInfo*> > &mapEventInfo)
{
    SIManager& siManager = SIManager::getInstance();
    for (list<string>::iterator itr = channelList.begin(); itr != channelList.end(); itr++) {
        string channelUid = *itr;
        list<EventInfo *> listEventInfo = siManager.getProgramList(channelUid);

        mapEventInfo.insert(make_pair(channelUid, listEventInfo));
    }

    if(mapEventInfo.empty() == false) {
        return TDS_OK;
    } else {
        return TDS_INFO_EMPTY;
    }
}

TDSResult SIHandler::getMultiChannelProgramListByIndex(list<string> channelList, map<string, list<EventInfo*> > &mapEventInfo, int startIdx,
        int reqCount)
{
    SIManager& siManager = SIManager::getInstance();
    for (list<string>::iterator itr = channelList.begin(); itr != channelList.end(); itr++) {
        string channelUid = *itr;
        list<EventInfo *> listEventInfo = siManager.getProgramListByIndex(channelUid, startIdx, reqCount);

        mapEventInfo.insert(make_pair(channelUid, listEventInfo));
    }

    if(mapEventInfo.empty() == false) {
        return TDS_OK;
    } else {
        return TDS_INFO_EMPTY;
    }
}

TDSResult SIHandler::getMultiChannelProgramListByTime(list<string> channelList, map<string, list<EventInfo*> > &mapEventInfo, int startTime,
        int endTime)
{
    SIManager& siManager = SIManager::getInstance();
    for (list<string>::iterator itr = channelList.begin(); itr != channelList.end(); itr++) {
        string channelUid = *itr;
        list<EventInfo *> listEventInfo = siManager.getProgramListByTime(channelUid, startTime, endTime);

        mapEventInfo.insert(make_pair(channelUid, listEventInfo));
    }

    if(mapEventInfo.empty() == false) {
        return TDS_OK;
    } else {
        return TDS_INFO_EMPTY;
    }
}

void SIHandler::notifyDefaultChannel(string default_channel)
{
    L_DEBUG(TAG, "Perform [%s(%s)].....\n", __FUNCTION__, default_channel.c_str());

    if((JNI_GetVM() == NULL) || (mJni == NULL)) {
        L_ERROR(TAG, "ERROR - VM info or DvbSiMgrObj is not existed...\n");
        return;
    }

    // Get Env info
    JNIEnv * env;
    int getEnvStat = JNI_GetVM()->GetEnv((void **) &env, JNI_VERSION_1_6);
    if(getEnvStat == JNI_EDETACHED) {
        if(JNI_GetVM()->AttachCurrentThread(&env, NULL) != 0) {
            L_ERROR(TAG, "Fail to attach .....\n");
            return;
        }
    } else if(getEnvStat == JNI_EVERSION) {
        L_ERROR(TAG, "ERROR - GetEnv: VERSION IS NOT SUPPORTED.....\n");
        return;
    }

    // Get DvbSiMgr class info
    jclass dvbsiMgrClazz = env->GetObjectClass(mJni);
    if(dvbsiMgrClazz == NULL) {
        L_ERROR(TAG, "ERROR - Not Find DvbSiMgr Class\n");
        return;
    }
    L_DEBUG(TAG, "Perform [%s(%s)].....\n", __FUNCTION__, default_channel.c_str());
    // Get Method ID for onReadyChannel()
    jmethodID defaultChannelMid = env->GetMethodID(dvbsiMgrClazz, "notifyDefaultChannel", "(Ljava/lang/String;)V");
    if(defaultChannelMid == NULL) {
        L_ERROR(TAG, "ERROR - Not Find notifyChannelReady methodID\n");
        return;
    }
    // Call method
    env->CallVoidMethod(mJni, defaultChannelMid,  env->NewStringUTF(default_channel.c_str()));
    L_DEBUG(TAG, "Perform [%s(%s)].....\n", __FUNCTION__, default_channel.c_str());
    env->DeleteLocalRef(dvbsiMgrClazz);
    JNI_GetVM()->DetachCurrentThread();
}

void SIHandler::notifyChannelReady(int sdtVersion)
{
    L_DEBUG(TAG, "Perform [%s(%d)].....\n", __FUNCTION__, sdtVersion);

    if((JNI_GetVM() == NULL) || (mJni == NULL)) {
        L_ERROR(TAG, "ERROR - VM info or DvbSiMgrObj is not existed...\n");
        return;
    }

    // Get Env info
    JNIEnv * env;
    int getEnvStat = JNI_GetVM()->GetEnv((void **) &env, JNI_VERSION_1_6);
    if(getEnvStat == JNI_EDETACHED) {
        if(JNI_GetVM()->AttachCurrentThread(&env, NULL) != 0) {
            L_ERROR(TAG, "Fail to attach .....\n");
            return;
        }
    } else if(getEnvStat == JNI_EVERSION) {
        L_ERROR(TAG, "ERROR - GetEnv: VERSION IS NOT SUPPORTED.....\n");
        return;
    }

    // Get DvbSiMgr class info
    jclass dvbsiMgrClazz = env->GetObjectClass(mJni);
    if(dvbsiMgrClazz == NULL) {
        L_ERROR(TAG, "ERROR - Not Find DvbSiMgr Class\n");
        return;
    }

    // Get Method ID for onReadyChannel()
    jmethodID readyChannelMid = env->GetMethodID(dvbsiMgrClazz, "notifyChannelReady", "(I)V");
    if(readyChannelMid == NULL) {
        L_ERROR(TAG, "ERROR - Not Find notifyChannelReady methodID\n");
        return;
    }

    // Call method
    env->CallVoidMethod(mJni, readyChannelMid, sdtVersion);

    env->DeleteLocalRef(dvbsiMgrClazz);
    JNI_GetVM()->DetachCurrentThread();
}

void SIHandler::notifyChannelUpdated(int sdtVersion, int default_ch)
{
    L_DEBUG(TAG, "Perform [%s(%d)].....\n", __FUNCTION__, sdtVersion);

    if((JNI_GetVM() == NULL) || (mJni == NULL)) {
        L_ERROR(TAG, "ERROR - VM info or DvbSiMgrObj is not existed...\n");
        return;
    }

    // Get Env info
    JNIEnv * env;
    int getEnvStat = JNI_GetVM()->GetEnv((void **) &env, JNI_VERSION_1_6);
    if(getEnvStat == JNI_EDETACHED) {
        if(JNI_GetVM()->AttachCurrentThread(&env, NULL) != 0) {
            L_ERROR(TAG, "Fail to attach .....\n");
            return;
        }
    } else if(getEnvStat == JNI_EVERSION) {
        L_ERROR(TAG, "ERROR - GetEnv: VERSION IS NOT SUPPORTED.....\n");
        return;
    }

    // Get DvbSiMgr class info
    jclass dvbsiMgrClazz = env->GetObjectClass(mJni);
    if(dvbsiMgrClazz == NULL) {
        L_ERROR(TAG, "ERROR - Not Find DvbSiMgr Class\n");
        return;
    }

    // Get Method ID for onUpdateChannel()
    jmethodID updateChannelMid = env->GetMethodID(dvbsiMgrClazz, "notifyChannelUpdated", "(II)V");
    if(updateChannelMid == NULL) {
        L_ERROR(TAG, "ERROR - Not Find notifyChannelUpdated methodID\n");
        return;
    }

    // Call method
    env->CallVoidMethod(mJni, updateChannelMid, sdtVersion, default_ch);

    env->DeleteLocalRef(dvbsiMgrClazz);
    JNI_GetVM()->DetachCurrentThread();
}

void SIHandler::notifyProgramUpdated()
{
    L_DEBUG(TAG, "Perform [%s].....\n", __FUNCTION__);

    // Get Env info
    JNIEnv * env;
    int getEnvStat = JNI_GetVM()->GetEnv((void **) &env, JNI_VERSION_1_6);
    if(getEnvStat == JNI_EDETACHED) {
        if(JNI_GetVM()->AttachCurrentThread(&env, NULL) != 0) {
            L_ERROR(TAG, "Fail to attach .....\n");
            return;
        }
    } else if(getEnvStat == JNI_EVERSION) {
        L_ERROR(TAG, "ERROR - GetEnv: VERSION IS NOT SUPPORTED.....\n");
        return;
    }

    // Get DvbSiMgr class info
    jclass dvbsiMgrClazz = env->GetObjectClass(mJni);
    if(dvbsiMgrClazz == NULL) {
        L_ERROR(TAG, "ERROR - Not Find DvbSiMgr Class\n");
        return;
    }

    // Get Method ID for onUpdateProgram()
    jmethodID updateProgramMid = env->GetMethodID(dvbsiMgrClazz, "notifyProgramUpdated", "()V");
    if(updateProgramMid == NULL) {
        L_ERROR(TAG, "ERROR - Not Find notifyProgramUpdated methodID\n");
        return;
    }

    // Call method
    env->CallVoidMethod(mJni, updateProgramMid);

    env->DeleteLocalRef(dvbsiMgrClazz);
    JNI_GetVM()->DetachCurrentThread();
}

void SIHandler::notifyChangeProgram(int channelNumber)
{
    L_DEBUG(TAG, "Perform [%s].....\n", __FUNCTION__);

    // Get Env info
    JNIEnv * env;
    int getEnvStat = JNI_GetVM()->GetEnv((void **) &env, JNI_VERSION_1_6);
    if(getEnvStat == JNI_EDETACHED) {
        if(JNI_GetVM()->AttachCurrentThread(&env, NULL) != 0) {
            L_ERROR(TAG, "Fail to attach .....\n");
            return;
        }
    } else if(getEnvStat == JNI_EVERSION) {
        L_ERROR(TAG, "ERROR - GetEnv: VERSION IS NOT SUPPORTED.....\n");
        return;
    }

    // Get DvbSiMgr class info
    jclass dvbsiMgrClazz = env->GetObjectClass(mJni);
    if(dvbsiMgrClazz == NULL) {
        L_ERROR(TAG, "ERROR - Not Find DvbSiMgr Class\n");
        return;
    }

    // Get Method ID for onChangeProgram()
    jmethodID changeProgramMid = env->GetMethodID(dvbsiMgrClazz, "notifyChangeProgram", "(I)V");
    if(changeProgramMid == NULL) {
        L_ERROR(TAG, "ERROR - Not Find notifyChangeProgram methodID\n");
        return;
    }

    // Call method
    env->CallVoidMethod(mJni, changeProgramMid, channelNumber);

    env->DeleteLocalRef(dvbsiMgrClazz);
    JNI_GetVM()->DetachCurrentThread();
}

void SIHandler::notifyScanReady(int total)
{
        L_DEBUG(TAG, "Perform [%s].....", __FUNCTION__);

        // Get Env info
        JNIEnv * env;
        int getEnvStat = JNI_GetVM()->GetEnv((void **) &env, JNI_VERSION_1_6);
        if(getEnvStat == JNI_EDETACHED) {
                if(JNI_GetVM()->AttachCurrentThread(&env, NULL) != 0) {
                        L_DEBUG(TAG, "Fail to attach .....");
                        return;
                }
        } else if(getEnvStat == JNI_EVERSION) {
                L_DEBUG(TAG, "ERROR - GetEnv: VERSION IS NOT SUPPORTED.....");
                return;
        }

        // Get DvbSiMgr class info
        jclass dvbsiMgrClazz = env->GetObjectClass(mJni);
        if(dvbsiMgrClazz == NULL) {
                L_DEBUG(TAG, "ERROR - Not Find DvbSiMgr Class");
                return;
        }

        // Get Method ID for onScanReady()
        jmethodID scanReadyMid = env->GetMethodID(dvbsiMgrClazz, "notifyScanReady", "(I)V");
        if(scanReadyMid == NULL) {
                L_DEBUG(TAG, "ERROR - Not Find notifyScanReady methodID");
                return;
        }

        // Call method
        env->CallVoidMethod(mJni, scanReadyMid, total);

        env->DeleteLocalRef(dvbsiMgrClazz);
        JNI_GetVM()->DetachCurrentThread();
}

void SIHandler::notifyScanProcessing(int progress, int found)
{
        L_DEBUG(TAG, "Perform [%s].....", __FUNCTION__);

        // Get Env info
        JNIEnv * env;
        int getEnvStat = JNI_GetVM()->GetEnv((void **) &env, JNI_VERSION_1_6);
        if(getEnvStat == JNI_EDETACHED) {
                if(JNI_GetVM()->AttachCurrentThread(&env, NULL) != 0) {
                        L_DEBUG(TAG, "Fail to attach .....");
                        return;
                }
        } else if(getEnvStat == JNI_EVERSION) {
                L_DEBUG(TAG, "ERROR - GetEnv: VERSION IS NOT SUPPORTED.....");
                return;
        }

        // Get DvbSiMgr class info
        jclass dvbsiMgrClazz = env->GetObjectClass(mJni);
        if(dvbsiMgrClazz == NULL) {
                L_DEBUG(TAG, "ERROR - Not Find DvbSiMgr Class");
                return;
        }

        // Get Method ID for onScanProcessing()
        jmethodID scanProcessingMid = env->GetMethodID(dvbsiMgrClazz, "notifyScanProcessing", "(II)V");
        if(scanProcessingMid == NULL) {
                L_DEBUG(TAG, "ERROR - Not Find notifyScanProcessing methodID");
                return;
        }

        // Call method
        env->CallVoidMethod(mJni, scanProcessingMid, progress, found);

        env->DeleteLocalRef(dvbsiMgrClazz);
        JNI_GetVM()->DetachCurrentThread();
}

void SIHandler::notifyScanCompleted(int found)
{
        L_DEBUG(TAG, "Perform [%s].....", __FUNCTION__);

        // Get Env info
        JNIEnv * env;
        int getEnvStat = JNI_GetVM()->GetEnv((void **) &env, JNI_VERSION_1_6);
        if(getEnvStat == JNI_EDETACHED) {
                if(JNI_GetVM()->AttachCurrentThread(&env, NULL) != 0) {
                        L_DEBUG(TAG, "Fail to attach .....");
                        return;
                }
        } else if(getEnvStat == JNI_EVERSION) {
                L_DEBUG(TAG, "ERROR - GetEnv: VERSION IS NOT SUPPORTED.....");
                return;
        }

        // Get DvbSiMgr class info
        jclass dvbsiMgrClazz = env->GetObjectClass(mJni);
        if(dvbsiMgrClazz == NULL) {
                L_DEBUG(TAG, "ERROR - Not Find DvbSiMgr Class");
                return;
        }

        // Get Method ID for onScanCompleted()
        jmethodID scanCompletedMid = env->GetMethodID(dvbsiMgrClazz, "notifyScanCompleted", "(I)V");
        if(scanCompletedMid == NULL) {
                L_DEBUG(TAG, "ERROR - Not Find notifyScanCompleted methodID");
                return;
        }

        // Call method
        env->CallVoidMethod(mJni, scanCompletedMid, found);

        env->DeleteLocalRef(dvbsiMgrClazz);
        JNI_GetVM()->DetachCurrentThread();
}

void SIHandler::notifyScanStop()
{
        L_DEBUG(TAG, "Perform [%s].....", __FUNCTION__);

        // Get Env info
        JNIEnv * env;
        int getEnvStat = JNI_GetVM()->GetEnv((void **) &env, JNI_VERSION_1_6);
        if(getEnvStat == JNI_EDETACHED) {
                if(JNI_GetVM()->AttachCurrentThread(&env, NULL) != 0) {
                        L_DEBUG(TAG, "Fail to attach .....");
                        return;
                }
        } else if(getEnvStat == JNI_EVERSION) {
                L_DEBUG(TAG, "ERROR - GetEnv: VERSION IS NOT SUPPORTED.....");
                return;
        }

        // Get DvbSiMgr class info
        jclass dvbsiMgrClazz = env->GetObjectClass(mJni);
        if(dvbsiMgrClazz == NULL) {
                L_DEBUG(TAG, "ERROR - Not Find DvbSiMgr Class");
                return;
        }

        // Get Method ID for onScanStop()
        jmethodID scanStopMid = env->GetMethodID(dvbsiMgrClazz, "notifyScanStop", "()V");
        if(scanStopMid == NULL) {
                L_DEBUG(TAG, "ERROR - Not Find notifyScanStop methodID");
                return;
        }

        // Call method
        env->CallVoidMethod(mJni, scanStopMid);

        env->DeleteLocalRef(dvbsiMgrClazz);
        JNI_GetVM()->DetachCurrentThread();
}

void SIHandler::notifyTDTDate(uint32_t timeInSec)
{
//        L_DEBUG(TAG, "Perform [%s].....", __FUNCTION__);

        // Get Env info
        JNIEnv * env;
        int getEnvStat = JNI_GetVM()->GetEnv((void **) &env, JNI_VERSION_1_6);
        if(getEnvStat == JNI_EDETACHED) {
                if(JNI_GetVM()->AttachCurrentThread(&env, NULL) != 0) {
                        L_DEBUG(TAG, "Fail to attach .....");
                        return;
                }
        } else if(getEnvStat == JNI_EVERSION) {
                L_DEBUG(TAG, "ERROR - GetEnv: VERSION IS NOT SUPPORTED.....");
                return;
        }

        // Get DvbSiMgr class info
        jclass dvbsiMgrClazz = env->GetObjectClass(mJni);
        if(dvbsiMgrClazz == NULL) {
                L_DEBUG(TAG, "ERROR - Not Find DvbSiMgr Class");
                return;
        }

    // Get Method ID for onTDTDate()
    jmethodID tdtDate = env->GetMethodID(dvbsiMgrClazz, "notifyTDTDate", "(IIIIII)V");
    if(tdtDate == NULL) {
        L_ERROR(TAG, "ERROR - Not Find tdtDate methodID");
        return;
    }

    // Call method
    char temp[50];
    memset(temp, 0, 50);
    time_t atm = static_cast<time_t>(timeInSec);
    struct tm tm_time;
    localtime_r(&atm, &tm_time);

    sprintf(temp, "%04d-%02d-%02dT%02d:%02d:%02d", (int) tm_time.tm_year + 1900, (int) tm_time.tm_mon + 1, (int) tm_time.tm_mday,
            (int) tm_time.tm_hour, (int) tm_time.tm_min, (int) tm_time.tm_sec);

    L_DEBUG(TAG, "time : %s\n", temp);

    env->CallVoidMethod(mJni, tdtDate, (int) tm_time.tm_year + 1900, (int) tm_time.tm_mon + 1, (int) tm_time.tm_mday, (int) tm_time.tm_hour,
            (int) tm_time.tm_min, (int) tm_time.tm_sec);

    env->DeleteLocalRef(dvbsiMgrClazz);
    JNI_GetVM()->DetachCurrentThread();
}

void SIHandler::notifyChannelPMT()
{
    L_DEBUG(TAG, "Perform [%s].....\n", __FUNCTION__);

    if((JNI_GetVM() == NULL) || (mJni == NULL)) {
        L_ERROR(TAG, "ERROR - VM info or DvbSiMgrObj is not existed...\n");
        return;
    }

    // Get Env info
    JNIEnv * env;
    int getEnvStat = JNI_GetVM()->GetEnv((void **) &env, JNI_VERSION_1_6);
    if(getEnvStat == JNI_EDETACHED) {
        if(JNI_GetVM()->AttachCurrentThread(&env, NULL) != 0) {
            L_ERROR(TAG, "Fail to attach .....\n");
            return;
        }
    } else if(getEnvStat == JNI_EVERSION) {
        L_ERROR(TAG, "ERROR - GetEnv: VERSION IS NOT SUPPORTED.....\n");
        return;
    }

    // Get DvbSiMgr class info
    jclass dvbsiMgrClazz = env->GetObjectClass(mJni);
    if(dvbsiMgrClazz == NULL) {
        L_ERROR(TAG, "ERROR - Not Find DvbSiMgr Class\n");
        return;
    }

    // Get Method ID for onChannelPMT()
    jmethodID channelPMTMid = env->GetMethodID(dvbsiMgrClazz, "notifyChannelPMT", "()V");
    if(channelPMTMid == NULL) {
        L_ERROR(TAG, "ERROR - Not Find notifyChannelPMT methodID\n");
        return;
    }

    // Call method
    env->CallVoidMethod(mJni, channelPMTMid);

    env->DeleteLocalRef(dvbsiMgrClazz);
    JNI_GetVM()->DetachCurrentThread();
}

void SIHandler::notifyErrorWepgConn()
{
    L_DEBUG(TAG, "Perform [%s].....\n", __FUNCTION__);

    if((JNI_GetVM() == NULL) || (mJni == NULL)) {
        L_ERROR(TAG, "ERROR - VM info or DvbSiMgrObj is not existed...\n");
        return;
    }

    // Get Env info
    JNIEnv * env;
    int getEnvStat = JNI_GetVM()->GetEnv((void **) &env, JNI_VERSION_1_6);
    if(getEnvStat == JNI_EDETACHED) {
        if(JNI_GetVM()->AttachCurrentThread(&env, NULL) != 0) {
            L_ERROR(TAG, "Fail to attach .....\n");
            return;
        }
    } else if(getEnvStat == JNI_EVERSION) {
        L_ERROR(TAG, "ERROR - GetEnv: VERSION IS NOT SUPPORTED.....\n");
        return;
    }

    // Get DvbSiMgr class info
    jclass dvbsiMgrClazz = env->GetObjectClass(mJni);
    if(dvbsiMgrClazz == NULL) {
        L_ERROR(TAG, "ERROR - Not Find DvbSiMgr Class\n");
        return;
    }

    // Get Method ID for onErrorWepgConn()
    jmethodID errorWepgConn = env->GetMethodID(dvbsiMgrClazz, "notifyErrorWepgConn", "()V");
    if(errorWepgConn == NULL) {
        L_ERROR(TAG, "ERROR - Not Find notifyErrorWepgConn methodID\n");
        return;
    }

    // Call method
    env->CallVoidMethod(mJni, errorWepgConn);

    env->DeleteLocalRef(dvbsiMgrClazz);
    JNI_GetVM()->DetachCurrentThread();
}
void SIHandler::setProvUri(string uri, int tunerId)
{
        string value, data;
        L_DEBUG(TAG, "uri : %s\n", uri.c_str());

        splitKeyAndValue(uri, ":", value, data);

        splitKeyAndValue(data, "/", value, data);
        splitKeyAndValue(data, "/", value, data);

        splitKeyAndValue(data, ":", value, data);
        splitKeyAndValue(data, "?", value, data);

        splitKeyAndValue(data, "=", value, data);

        if(value.compare("pn") == 0) { // project name
                splitKeyAndValue(data, "&", mProvName, data);
        }
        splitKeyAndValue(data, "=", value, data);

        if(value.compare("db") == 0) { // db type
                splitKeyAndValue(data, "&", value, data);
        }
        if(splitKeyAndValue(data, "=", value, data)) {

                if(value.compare("sr") == 0) { // symbolrate
                        splitKeyAndValue(data, "&", value, data);
                        mProvSymbolrate = atoi(value.c_str());
                }

                if(splitKeyAndValue(data, "=", value, data)) {
                        if(value.compare("bw") == 0) { // bandwidth
                                mProvBandwidth = atoi(data.c_str());
                        }
                }
        }

        mProvTunerId = tunerId;
        L_DEBUG(TAG, "uri : pn=%s&sr=%d&bw=%d(tuner%d)\n", mProvName.c_str(), mProvSymbolrate, mProvBandwidth, mProvTunerId);
}

bool SIHandler::splitKeyAndValue(const std::string str, const std::string delimiter, std::string& key, std::string& value)
{
        uint32_t index = str.find(delimiter);
        if(std::string::npos == index) {
                return false;
        }
        key = str.substr(0, index);
        value = str.substr(index + 1);
        return true;
}
