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
#include <list>
#include <time.h>

#include "JNIConvertor.h"

#include "Logger.h"
//#define __DEBUG_JNI__
static const char* TAG = "JNI_Convertor";

static jclass gCaInfoCls = 0;
static jclass gEsInfoCls = 0;
static jclass gChInfoCls = 0;
static jclass gPrgmLinkInfoCls = 0;
static jclass gPrgmInfoCls = 0;

int convertSIResultValue(TDSResult result)
{
        switch (result) {
                case TDS_OK:
                return CONST_JAVA_SI_RESULT_OK;
                case TDS_ACCESSDENIED:
                case TDS_FAILURE:
                return CONST_JAVA_SI_RESULT_ERR_GEN_FAILURE;
                case TDS_INVARG:
                return CONST_JAVA_SI_RESULT_ERR_INVALID_PARAMETERS;
                case TDS_NORESOURCE:
                return CONST_JAVA_SI_RESULT_ERR_NO_RESOURCE;
                case TDS_INFO_EMPTY:
                return CONST_JAVA_SI_RESULT_ERR_INFO_IS_NOT_EXIST;
                case TDS_INFO_NOT_READY:
                return CONST_JAVA_SI_RESULT_ERR_INFO_ON_SCANNING;
                default:
                return CONST_JAVA_SI_RESULT_ERR_GEN_FAILURE;
        }
}

jstring convertSeconds2Date(JNIEnv* env, uint32_t seconds)
{
        char temp[50];
        memset(temp, 0, 50);
        time_t atm = static_cast<time_t>(seconds);
        struct tm tm_time;
        localtime_r(&atm, &tm_time);
        sprintf(temp, "%04d-%02d-%02dT%02d:%02d:%02d", (int) tm_time.tm_year + 1900, (int) tm_time.tm_mon + 1, (int) tm_time.tm_mday,
                        (int) tm_time.tm_hour, (int) tm_time.tm_min, (int) tm_time.tm_sec);

#ifdef __DEBUG_JNI__
        NLOG_D(TAG, "Convert Native-to-Java... COMPLETE - Time[%s]", temp);
#endif

        return env->NewStringUTF(temp);
}

void setStringToJavaParam(JNIEnv* env, jobject jObj, jfieldID jfId, string param)
{
        if(param.c_str() != NULL && param.length() > 0) {
                jstring strValue;

                strValue = env->NewStringUTF(param.c_str());
                env->SetObjectField(jObj, jfId, strValue);
                env->DeleteLocalRef(strValue);
        }
}

//////////////////////////////////////////////////////////////////////////////////////////////
///////                        CHANNEL INFO CONVERSION                              //////////
//////////////////////////////////////////////////////////////////////////////////////////////
bool convertJNIProductIdInfo(JNIEnv* env, jclass jChInfoCls, jobject jChInfoObj, list<string> listProductId)
{
        if(env == NULL) {
                L_DEBUG(TAG, "ERROR - %s has INVALID parameter...", __FUNCTION__);
                return false;
        }

        if(listProductId.size() <= 0) {
#ifdef __DEBUG_JNI__
                NLOG_D(TAG, "ERROR - %s Production ID Data is Empty...", __FUNCTION__);
#endif
                return false;
        }

        jstring strValue;
        jmethodID mid = env->GetMethodID(jChInfoCls, "addProductId", "(Ljava/lang/String;)V");

        for (list<string>::iterator itr = listProductId.begin(); itr != listProductId.end(); itr++) {
                string id_product = *itr;
                if(id_product.length() > 0) {

                        strValue = env->NewStringUTF(id_product.c_str());
                        env->CallVoidMethod(jChInfoObj, mid, strValue);
                        env->DeleteLocalRef(strValue);
#ifdef __DEBUG_JNI__
                        NLOG_D(TAG, "Convert Native-to-Java... COMPLETE - id_product[%s]", id_product.c_str());
#endif
                }
        }
        return true;
}

bool convertJNIESInfo(JNIEnv* env, jclass jChInfoCls, jobject jChInfoObj, list<elementary_stream> listESInfo)
{
        if(env == NULL) {
                L_DEBUG(TAG, "ERROR - %s has INVALID parameter...", __FUNCTION__);
                return false;
        }

        if(listESInfo.size() <= 0) {
#ifdef __DEBUG_JNI__
                NLOG_D(TAG, "ERROR - %s Elementary Stream Info Data is Empty...", __FUNCTION__);
#endif
                return false;
        }

        jclass mEsInfoCls;
        jobject mEsInfoObj;
        jmethodID mid, addESInfoMid;

        if(gEsInfoCls == 0) {
                mEsInfoCls = env->FindClass("com/tvstorm/tv/si/channel/ESInfo");
                gEsInfoCls = (jclass) env->NewGlobalRef(mEsInfoCls);
        }

        addESInfoMid = env->GetMethodID(jChInfoCls, "addESInfo", "(Lcom/tvstorm/tv/si/channel/ESInfo;)V");

        for (list<elementary_stream>::iterator itr = listESInfo.begin(); itr != listESInfo.end(); itr++) {
                elementary_stream esInfo = *itr;

                mid = env->GetMethodID(gEsInfoCls, "<init>", "(III)V");
                mEsInfoObj = env->NewObject(gEsInfoCls, mid, (int) esInfo.stream_type, (int) esInfo.stream_pid, (int) esInfo.language_code);

                env->CallVoidMethod(jChInfoObj, addESInfoMid, mEsInfoObj);
                env->DeleteLocalRef(mEsInfoObj);
        }
        return true;
}

bool convertJNICAInfo(JNIEnv* env, jclass jChInfoCls, jobject jChInfoObj, list<CAInfo> listCAInfo)
{
        if(env == NULL) {
                L_DEBUG(TAG, "ERROR - %s has INVALID parameter...", __FUNCTION__);
                return false;
        }

        if(listCAInfo.size() <= 0) {
#ifdef __DEBUG_JNI__
                NLOG_D(TAG, "ERROR - %s CA Info Data is Empty...", __FUNCTION__);
#endif
                return false;
        }

        jclass mCaInfoCls;
        jobject mCaInfoObj;
        jmethodID mid, addCAInfoMid;

        if(gCaInfoCls == 0) {
                mCaInfoCls = env->FindClass("com/tvstorm/tv/si/channel/CAInfo");
                gCaInfoCls = (jclass) env->NewGlobalRef(mCaInfoCls);
        }

        addCAInfoMid = env->GetMethodID(jChInfoCls, "addCAInfo", "(Lcom/tvstorm/tv/si/channel/CAInfo;)V");

        for (list<CAInfo>::iterator itr = listCAInfo.begin(); itr != listCAInfo.end(); itr++) {
                CAInfo pInfo = *itr;

                mid = env->GetMethodID(gCaInfoCls, "<init>", "(II)V");
                mCaInfoObj = env->NewObject(gCaInfoCls, mid, (int) pInfo.CA_system_ID, (int) pInfo.CA_PID);

                env->CallVoidMethod(jChInfoObj, addCAInfoMid, mCaInfoObj);
                env->DeleteLocalRef(mCaInfoObj);
        }
        return true;
}

bool convertJNIChannelInfo(JNIEnv* env, jclass jRetChListCls, jobject jRetChListObj, list<ServiceInfo*> listChannelInfo, uint32_t provider,
                uint32_t symbolrate, uint32_t bandwidth, uint32_t tunerId)
{
        if((env == NULL) || (jRetChListCls == NULL) || (jRetChListObj == NULL)) {
                L_DEBUG(TAG, "ERROR - %s has INVALID parameter...", __FUNCTION__);
                return true;
        }

        if(listChannelInfo.size() == 0) {
#ifdef __DEBUG_JNI__
                NLOG_D(TAG, "ERROR - %s ChannelInfo Data is Empty...", __FUNCTION__);
#endif
                return true;
        }

        jclass mChInfoCls;
        jobject mChInfoObj;
        jmethodID mid, addChannelInfoMid;

        jfieldID fidChUid;
        jfieldID fidChId, fidChType, fidChName, fidChNum, fidGenre, fidCategory, fidRunningSts;
        jfieldID fidFreeCaMode, fidRating, fidAreaCode, fidPayChannel, fidSampleTime, fidResolution;
        jfieldID fidChImageUri, fidChUri, fidPcrPid;

        jstring strValue;
        char tmpChars[200];
        char tmpCa[20];
        char tmpEs[20];
        char tmpFe[20];
        int tmpPp;

        if(gChInfoCls == 0) {
                mChInfoCls = env->FindClass("com/tvstorm/tv/si/channel/ChannelInfo");
                gChInfoCls = (jclass) env->NewGlobalRef(mChInfoCls);
        }
        fidChUid = env->GetFieldID(gChInfoCls, "mChannelUid", "Ljava/lang/String;");
        fidChId = env->GetFieldID(gChInfoCls, "mChannelId", "I");
        fidChType = env->GetFieldID(gChInfoCls, "mChannelType", "I");
        fidChName = env->GetFieldID(gChInfoCls, "mChannelName", "Ljava/lang/String;");
        fidChNum = env->GetFieldID(gChInfoCls, "mChannelNumber", "I");
        fidGenre = env->GetFieldID(gChInfoCls, "mGenre", "I");
        fidCategory = env->GetFieldID(gChInfoCls, "mCategory", "I");
        fidRunningSts = env->GetFieldID(gChInfoCls, "mRunningStatus", "I");
        fidFreeCaMode = env->GetFieldID(gChInfoCls, "mFreeCaMode", "I");
        fidRating = env->GetFieldID(gChInfoCls, "mRating", "I");
        fidAreaCode = env->GetFieldID(gChInfoCls, "mLocalAreaCode", "I");
        fidPayChannel = env->GetFieldID(gChInfoCls, "mPayChannel", "I");
        fidSampleTime = env->GetFieldID(gChInfoCls, "mSampleTime", "I");
        fidResolution = env->GetFieldID(gChInfoCls, "mResolution", "I");
        fidChImageUri = env->GetFieldID(gChInfoCls, "mChannelImageUri", "Ljava/lang/String;");
        fidChUri = env->GetFieldID(gChInfoCls, "mChannelUri", "Ljava/lang/String;");
        fidPcrPid = env->GetFieldID(gChInfoCls, "mPcrPid", "I");

        addChannelInfoMid = env->GetMethodID(jRetChListCls, "addChannelInfo", "(Lcom/tvstorm/tv/si/channel/ChannelInfo;)V");

        L_DEBUG(TAG, "ChannelInfo size is %u", listChannelInfo.size());
        for (list<ServiceInfo*>::iterator itr = listChannelInfo.begin(); itr != listChannelInfo.end(); itr++) {
                ServiceInfo* channelInfo = (ServiceInfo *) (*itr);
                mid = env->GetMethodID(gChInfoCls, "<init>", "()V");
                mChInfoObj = env->NewObject(gChInfoCls, mid);

                // mChannelUid
                setStringToJavaParam(env, mChInfoObj, fidChUid, channelInfo->service_uid);

                // mChannelId
                env->SetIntField(mChInfoObj, fidChId, channelInfo->service_id);

                // mChannelType
                env->SetIntField(mChInfoObj, fidChType, channelInfo->service_type);

                // mChannelName
                setStringToJavaParam(env, mChInfoObj, fidChName, channelInfo->service_name);

                // mChannelNumber
                env->SetIntField(mChInfoObj, fidChNum, channelInfo->channel_number);

                // mGenre
                env->SetIntField(mChInfoObj, fidGenre, channelInfo->genre_code);

                // mCategory
                env->SetIntField(mChInfoObj, fidCategory, channelInfo->category_code);

                // mRunningStatus
                env->SetIntField(mChInfoObj, fidRunningSts, channelInfo->running_status);

                // mFreeCaMode
                env->SetIntField(mChInfoObj, fidFreeCaMode, channelInfo->free_CA_mode);

                // mRating
                env->SetIntField(mChInfoObj, fidRating, channelInfo->channel_rating);

                // mLocalAreaCode
                env->SetIntField(mChInfoObj, fidAreaCode, channelInfo->local_area_code);

                // mPayChannel
                env->SetIntField(mChInfoObj, fidPayChannel, channelInfo->pay_channel);

                // mSampleTime
                env->SetIntField(mChInfoObj, fidSampleTime, channelInfo->pay_ch_sample_sec);

                // mResolution
                env->SetIntField(mChInfoObj, fidResolution, channelInfo->resolution);

                // mChannelImageUri
                setStringToJavaParam(env, mChInfoObj, fidChImageUri, channelInfo->channel_image_url);

                // mChannelUri

                string strUri = channelInfo->getUri();

                memset(tmpFe, 0, 20);
				if (channelInfo->symbol_rate <= 0) {
						sprintf(tmpFe, "&sr=%d&bw=%d", symbolrate, bandwidth);
				} else {
						sprintf(tmpFe, "&sr=%d&bw=%d", channelInfo->symbol_rate, bandwidth);
				}
				strUri += tmpFe;
                setStringToJavaParam(env, mChInfoObj, fidChUri, strUri);

//                L_DEBUG(TAG, "Convert Native-to-Java... COMPLETE - ChannelUri[%s]", strUri.c_str());

                // mPcrPid
                //env->SetIntField(mChInfoObj, fidPcrPid, channelInfo->pcr_pid);
                env->SetIntField(mChInfoObj, fidPcrPid, tmpPp);

                // Convert Product info to Java object
                convertJNIProductIdInfo(env, gChInfoCls, mChInfoObj, channelInfo->list_product_id);

                // Convert ES info to Java object
                convertJNIESInfo(env, gChInfoCls, mChInfoObj, channelInfo->list_elementary_stream);

                // Convert CA info to Java object
                convertJNICAInfo(env, gChInfoCls, mChInfoObj, channelInfo->listCAInfo);

                env->CallVoidMethod(jRetChListObj, addChannelInfoMid, mChInfoObj);
                env->DeleteLocalRef(mChInfoObj);
        }

        return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////
///////                        PROGRAM INFO CONVERSION                              //////////
//////////////////////////////////////////////////////////////////////////////////////////////
bool convertJNIProgramLinkInfo(JNIEnv* env, jclass prgmInfoClass, jobject jProgramInfoObject, list<ProgramLinkedInfo> listLinkInfo)
{
        if(env == NULL) {
                L_DEBUG(TAG, "ERROR - %s has INVALID parameter...", __FUNCTION__);
                return false;
        }

        if(listLinkInfo.size() <= 0) {
#ifdef __DEBUG_JNI__
                NLOG_D(TAG, "%s : listLinkInfo Data is Empty...", __FUNCTION__);
#endif
                return false;
        }

        jclass mPrgmLinkInfoCls;
        jobject jProgramLinkInfoObj;
        jmethodID mid, addProgramLinkInfoMid;

        jfieldID fidLinkSvcType, fidLinkSvcText, fidBtnType, fidBtnUri;
        jfieldID fidCMenu, fidVasItemId, fidVasPath, fidVasSvcId;
        jfieldID fidDisplayStart, fidDisplayEnd;

        jstring strValue;

        if(gPrgmLinkInfoCls == 0) {
                mPrgmLinkInfoCls = env->FindClass("com/tvstorm/tv/si/program/ProgramLinkInfo");
                gPrgmLinkInfoCls = (jclass) env->NewGlobalRef(mPrgmLinkInfoCls);
        }
        fidLinkSvcType = env->GetFieldID(gPrgmLinkInfoCls, "mLinkedServiceType", "I");
        fidLinkSvcText = env->GetFieldID(gPrgmLinkInfoCls, "mLinkedServiceText", "Ljava/lang/String;");
        fidBtnType = env->GetFieldID(gPrgmLinkInfoCls, "mButtonType", "I");
        fidBtnUri = env->GetFieldID(gPrgmLinkInfoCls, "mButtonUri", "Ljava/lang/String;");
        fidDisplayStart = env->GetFieldID(gPrgmLinkInfoCls, "mDisplayStartTime", "Ljava/lang/String;");
        fidDisplayEnd = env->GetFieldID(gPrgmLinkInfoCls, "mDisplayEndTime", "Ljava/lang/String;");
        fidCMenu = env->GetFieldID(gPrgmLinkInfoCls, "mCMenu", "I");
        fidVasItemId = env->GetFieldID(gPrgmLinkInfoCls, "mVasItemId", "Ljava/lang/String;");
        fidVasPath = env->GetFieldID(gPrgmLinkInfoCls, "mVasPath", "Ljava/lang/String;");
        fidVasSvcId = env->GetFieldID(gPrgmLinkInfoCls, "mVasServiceId", "Ljava/lang/String;");

        addProgramLinkInfoMid = env->GetMethodID(prgmInfoClass, "addProgramLinkInfo", "(Lcom/tvstorm/tv/si/program/ProgramLinkInfo;)V");
        if(addProgramLinkInfoMid == NULL) {
                L_DEBUG(TAG, "ERROR - convertJPrograms do not find addProgramLinkInfo method info...");
                return false;
        }

        for (list<ProgramLinkedInfo>::iterator itr = listLinkInfo.begin(); itr != listLinkInfo.end(); itr++) {
                mid = env->GetMethodID(gPrgmLinkInfoCls, "<init>", "()V");
                jProgramLinkInfoObj = env->NewObject(gPrgmLinkInfoCls, mid);
                if(jProgramLinkInfoObj == NULL) {
                        L_DEBUG(TAG, "Fail to crerate ProgramLinkInfo object...");
                        continue;
                }

                ProgramLinkedInfo programLinkedInfo = *itr;

                // mLinkedServiceType
                env->SetIntField(jProgramLinkInfoObj, fidLinkSvcType, programLinkedInfo.linked_service_flag);

                // mLinkedServiceText
                setStringToJavaParam(env, jProgramLinkInfoObj, fidLinkSvcText, programLinkedInfo.linked_service_text);

                // mButtonType
                env->SetIntField(jProgramLinkInfoObj, fidBtnType, programLinkedInfo.button_type);

                // mButtonUri
                setStringToJavaParam(env, jProgramLinkInfoObj, fidBtnUri, programLinkedInfo.button_image_filename);

                // mDisplayStartTime
                strValue = convertSeconds2Date(env, programLinkedInfo.display_start_time);
                env->SetObjectField(jProgramLinkInfoObj, fidDisplayStart, strValue);
                env->DeleteLocalRef(strValue);

                // mDisplayEndTime
                strValue = convertSeconds2Date(env, programLinkedInfo.display_end_time);
                env->SetObjectField(jProgramLinkInfoObj, fidDisplayEnd, strValue);
                env->DeleteLocalRef(strValue);

                // mCMenu
                env->SetIntField(jProgramLinkInfoObj, fidCMenu, programLinkedInfo.c_menu);

                // mVasItemId
                setStringToJavaParam(env, jProgramLinkInfoObj, fidVasItemId, programLinkedInfo.vas_itemid);

                // mVasPath
                setStringToJavaParam(env, jProgramLinkInfoObj, fidVasPath, programLinkedInfo.vas_path);

                // VasServiceId
                setStringToJavaParam(env, jProgramLinkInfoObj, fidVasSvcId, programLinkedInfo.vas_serviceid);

                env->CallVoidMethod(jProgramInfoObject, addProgramLinkInfoMid, jProgramLinkInfoObj);
                env->DeleteLocalRef(jProgramLinkInfoObj);
        }
        return true;
}

bool convertJNIPrograms(JNIEnv* env, jclass jPrgmsCls, jobject jPrgmsObj, list<EventInfo*> listEventInfo, jstring channelUid)
{
        if((env == NULL) || (jPrgmsCls == NULL) || (jPrgmsObj == NULL)) {
                L_DEBUG(TAG, "ERROR - %s has INVALID parameter...env[0x%p]/proObject[0x%p]", __FUNCTION__, env, jPrgmsObj);
                return false;
        }

        if(listEventInfo.size() <= 0) {
#ifdef __DEBUG_JNI__
                NLOG_D(TAG, "ERROR - %s ProgramInfo Data is Empty ...", __FUNCTION__);
#endif
                return false;
        }

        jfieldID fidProgramId, fidChannelId, fidProgramName, fidProgramShortDesc, fidProgramDesc, fidRunningSts, fidFreeCaMode;
        jfieldID fidRating, fidNibble1, fidNibble2, fidUserNibble1, fidUserNibble2, fidProgramImgUri;
        jfieldID fidAudioType, fidPrice, fidDirector, fidActors, fidResolution, fidDolby, fidCaption, fidDvs;
        jfieldID fidStartTime, fidEndTime, fidDuration, fidLinkedServiceList;
        jfieldID fidChannelUid;

        jclass mPrgmInfoCls;
        jobject jProgramInfoObj;
        jmethodID mid, addProgramInfoMid;

        jstring strValue;

        if(gPrgmInfoCls == 0) {
                mPrgmInfoCls = env->FindClass("com/tvstorm/tv/si/program/ProgramInfo");
                gPrgmInfoCls = (jclass) env->NewGlobalRef(mPrgmInfoCls);
        }
        fidChannelUid = env->GetFieldID(gPrgmInfoCls, "mChannelUid", "Ljava/lang/String;");
        fidChannelId = env->GetFieldID(gPrgmInfoCls, "mChannelId", "I");
        fidProgramId = env->GetFieldID(gPrgmInfoCls, "mProgramId", "I");
        fidProgramName = env->GetFieldID(gPrgmInfoCls, "mProgramName", "Ljava/lang/String;");
        fidProgramShortDesc = env->GetFieldID(gPrgmInfoCls, "mProgramShortDescription", "Ljava/lang/String;");
        fidProgramDesc = env->GetFieldID(gPrgmInfoCls, "mProgramDescription", "Ljava/lang/String;");
        fidRunningSts = env->GetFieldID(gPrgmInfoCls, "mRunningStatus", "I");
        fidFreeCaMode = env->GetFieldID(gPrgmInfoCls, "mFreeCAMode", "I");
        fidRating = env->GetFieldID(gPrgmInfoCls, "mRating", "I");
        fidNibble1 = env->GetFieldID(gPrgmInfoCls, "mContentNibble1", "I");
        fidNibble2 = env->GetFieldID(gPrgmInfoCls, "mContentNibble2", "I");
        fidUserNibble1 = env->GetFieldID(gPrgmInfoCls, "mContentUserNibble1", "I");
        fidUserNibble2 = env->GetFieldID(gPrgmInfoCls, "mContentUserNibble2", "I");
        fidProgramImgUri = env->GetFieldID(gPrgmInfoCls, "mProgramImageUri", "Ljava/lang/String;");
        fidAudioType = env->GetFieldID(gPrgmInfoCls, "mAudioType", "I");
        fidPrice = env->GetFieldID(gPrgmInfoCls, "mPrice", "Ljava/lang/String;");
        fidDirector = env->GetFieldID(gPrgmInfoCls, "mDirector", "Ljava/lang/String;");
        fidActors = env->GetFieldID(gPrgmInfoCls, "mActors", "Ljava/lang/String;");
        fidResolution = env->GetFieldID(gPrgmInfoCls, "mResolution", "I");
        fidDolby = env->GetFieldID(gPrgmInfoCls, "mDolbyAudio", "I");
        fidCaption = env->GetFieldID(gPrgmInfoCls, "mCaption", "I");
        fidDvs = env->GetFieldID(gPrgmInfoCls, "mDvs", "I");
        fidStartTime = env->GetFieldID(gPrgmInfoCls, "mStartTime", "Ljava/lang/String;");
        fidEndTime = env->GetFieldID(gPrgmInfoCls, "mEndTime", "Ljava/lang/String;");
        fidDuration = env->GetFieldID(gPrgmInfoCls, "mDuration", "I");

        addProgramInfoMid = env->GetMethodID(jPrgmsCls, "addProgramInfo", "(Lcom/tvstorm/tv/si/program/ProgramInfo;)V");
        if(addProgramInfoMid == NULL) {
                L_DEBUG(TAG, "ERROR - convertJPrograms do not find addProgramInfo method info...");
                return false;
        }

        for (list<EventInfo*>::iterator itr = listEventInfo.begin(); itr != listEventInfo.end(); itr++) {
                EventInfo* eventInfo = (EventInfo *) (*itr);

                mid = env->GetMethodID(gPrgmInfoCls, "<init>", "()V");
                jProgramInfoObj = env->NewObject(gPrgmInfoCls, mid);
                if(jProgramInfoObj == NULL) {
                        continue;
                }

                // mChannelUid
                setStringToJavaParam(env, jProgramInfoObj, fidChannelUid, eventInfo->service_uid);

                // mChannelId
                env->SetIntField(jProgramInfoObj, fidChannelId, eventInfo->service_id);

                // mProgramId
                env->SetIntField(jProgramInfoObj, fidProgramId, eventInfo->event_id);

                // mProgramName
                setStringToJavaParam(env, jProgramInfoObj, fidProgramName, eventInfo->event_name);

                // mProgramShortDescription
                setStringToJavaParam(env, jProgramInfoObj, fidProgramShortDesc, eventInfo->event_short_description);

                // mProgramDescription
                setStringToJavaParam(env, jProgramInfoObj, fidProgramDesc, eventInfo->event_description);

                // mRunningStatus
                env->SetIntField(jProgramInfoObj, fidRunningSts, eventInfo->running_status);

                // mFreeCAMode
                env->SetIntField(jProgramInfoObj, fidFreeCaMode, eventInfo->free_CA_mode);

                // mRating
                env->SetIntField(jProgramInfoObj, fidRating, eventInfo->rating);

                // mContentNibble1
                env->SetIntField(jProgramInfoObj, fidNibble1, eventInfo->content_nibble_level_1);

                // mContentNibble2
                env->SetIntField(jProgramInfoObj, fidNibble2, eventInfo->content_nibble_level_2);

                // mContentUserNibble1
                // eschoi_20140821 | used to Season for WEPG v2, 0 ~ 999
                env->SetIntField(jProgramInfoObj, fidUserNibble1, eventInfo->user_nibble_1);

                // mContentUserNibble2
                // eschoi_20140821 | used to EpisodeNo for WEPG v2
                env->SetIntField(jProgramInfoObj, fidUserNibble2, eventInfo->user_nibble_2);

                // mProgramImageUri
                setStringToJavaParam(env, jProgramInfoObj, fidProgramImgUri, eventInfo->image_url);

                // mAudioType
                env->SetIntField(jProgramInfoObj, fidAudioType, eventInfo->audio);

                // mPrice
                setStringToJavaParam(env, jProgramInfoObj, fidPrice, eventInfo->price_text);

                // mDirector
                setStringToJavaParam(env, jProgramInfoObj, fidDirector, eventInfo->director_name);

                // mActors
                setStringToJavaParam(env, jProgramInfoObj, fidActors, eventInfo->actors_name);

                // mResolution
                env->SetIntField(jProgramInfoObj, fidResolution, eventInfo->resolution);

                // mDolbyAudio
                env->SetIntField(jProgramInfoObj, fidDolby, eventInfo->dolby);

                // mCaption
                env->SetIntField(jProgramInfoObj, fidCaption, eventInfo->caption);

                // mDvs
                env->SetIntField(jProgramInfoObj, fidDvs, eventInfo->dvs);

                // mStartTime
                strValue = convertSeconds2Date(env, eventInfo->start_time);

                env->SetObjectField(jProgramInfoObj, fidStartTime, strValue);
                env->DeleteLocalRef(strValue);

                // mEndTime
                strValue = convertSeconds2Date(env, eventInfo->end_time);

                env->SetObjectField(jProgramInfoObj, fidEndTime, strValue);
                env->DeleteLocalRef(strValue);

                // mDuration
                env->SetIntField(jProgramInfoObj, fidDuration, eventInfo->duration);

                // Convert Prorgram link info to Java object
                convertJNIProgramLinkInfo(env, gPrgmInfoCls, jProgramInfoObj, eventInfo->listProgramLinkedInfo);

                env->CallVoidMethod(jPrgmsObj, addProgramInfoMid, jProgramInfoObj);
                env->DeleteLocalRef(jProgramInfoObj);
        }
        return true;
}

bool convertJRetProgramList(JNIEnv* env, jclass jRetPrgmListCls, jobject jRetPrgmListObj, jobject jPrgmsObj)
{
        if((env == NULL) || (jRetPrgmListCls == NULL) || (jRetPrgmListObj == NULL) || (jPrgmsObj == NULL)) {
                L_DEBUG(TAG, "ERROR - convertJRetProgramList has invalid parameters, evn[0x%p]/retProgramClass[0x%p]/retProgramObj[0x%p]/programsObj[0x%p]", env, jRetPrgmListCls, jRetPrgmListObj, jPrgmsObj);
                return false;
        }

        jmethodID mid = env->GetMethodID(jRetPrgmListCls, "addPrograms", "(Lcom/tvstorm/tv/si/program/Programs;)V");
        if(mid == NULL) {
                L_DEBUG(TAG, "ERROR - convertJRetProgramList do not find addPrograms method info...");
                return false;
        }

        env->CallVoidMethod(jRetPrgmListObj, mid, jPrgmsObj);
        return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////
///////                        PRODUCT INFO CONVERSION                              //////////
//////////////////////////////////////////////////////////////////////////////////////////////
bool convertJProductList(JNIEnv* env, jclass jPrdtInfoCls, jobject jPrdtInfoObj, TDSProductInfo* pProductInfo)
{
        if((env == NULL) || (jPrdtInfoCls == NULL) || (jPrdtInfoObj == NULL) || (pProductInfo == NULL)) {
                L_DEBUG(TAG, "ERROR - convertJProductList has invalid parameters");
                return false;
        }

        jfieldID fidProductType, fidProductId, fidProductName, fidProductDesc;
        jfieldID fidProductPrice, fidProductDcPrice, fidFgTerm, fidFgValue;
        jfieldID fidDtProductStart, fidDtProductEnd;

        jstring strValue;
        char tmpChars[200];

        fidProductId = env->GetFieldID(jPrdtInfoCls, "mProductId", "[B");
        fidProductType = env->GetFieldID(jPrdtInfoCls, "mProductType", "I");
        fidProductName = env->GetFieldID(jPrdtInfoCls, "mProductName", "[B");
        fidProductDesc = env->GetFieldID(jPrdtInfoCls, "mProductDescription", "[B");
        fidDtProductStart = env->GetFieldID(jPrdtInfoCls, "mDtProductStart", "Ljava/lang/String;");
        fidDtProductEnd = env->GetFieldID(jPrdtInfoCls, "mDtProductEnd", "Ljava/lang/String;");
        fidProductPrice = env->GetFieldID(jPrdtInfoCls, "mProductPrice", "I");
        fidProductDcPrice = env->GetFieldID(jPrdtInfoCls, "mProductDcPrice", "I");
        fidFgTerm = env->GetFieldID(jPrdtInfoCls, "mFgTerm", "I");
        fidFgValue = env->GetFieldID(jPrdtInfoCls, "mFgValue", "I");

        // mProductId
        if(pProductInfo->id_product_len > 0) {

                jbyteArray javaBytes;
                javaBytes = env->NewByteArray(pProductInfo->id_product_len);
                env->SetByteArrayRegion(javaBytes, 0, pProductInfo->id_product_len, (jbyte *) pProductInfo->id_product_ptr);

                env->SetObjectField(jPrdtInfoObj, fidProductId, javaBytes);
                env->DeleteLocalRef(javaBytes);
                //NLOG_D(TAG, "Convert Native-to-Java... COMPLETE - ProductId, Leng[%d]", pProductInfo->id_product_len);
        }

        // ProductType
        env->SetIntField(jPrdtInfoObj, fidProductType, pProductInfo->product_type);

        // mProductName
        if(pProductInfo->nm_product_len > 0) {

                jbyteArray javaBytes;
                javaBytes = env->NewByteArray(pProductInfo->nm_product_len);
                env->SetByteArrayRegion(javaBytes, 0, pProductInfo->nm_product_len, (jbyte *) pProductInfo->nm_product_ptr);

                env->SetObjectField(jPrdtInfoObj, fidProductName, javaBytes);
                env->DeleteLocalRef(javaBytes);
                //NLOG_D(TAG, "Convert Native-to-Java... COMPLETE - ProductName, Leng[%d]", pProductInfo->nm_product_len);
        }

        // mProductDescription
        if(pProductInfo->desc_product_len > 0) {

                jbyteArray javaBytes;
                javaBytes = env->NewByteArray(pProductInfo->desc_product_len);
                env->SetByteArrayRegion(javaBytes, 0, pProductInfo->desc_product_len, (jbyte *) pProductInfo->desc_product_ptr);

                env->SetObjectField(jPrdtInfoObj, fidProductDesc, javaBytes);
                env->DeleteLocalRef(javaBytes);
                //NLOG_D(TAG, "Convert Native-to-Java... COMPLETE - ProductDescription, Leng[%d]", pProductInfo->desc_product_len);
        }

        // mDtProductStart
        TDSTime start_time = pProductInfo->dt_product_start;
        sprintf(tmpChars, "%04d-%02d-%02dT%02d:%02d:%02d", start_time.year, start_time.month, start_time.day, start_time.hour, start_time.min,
                        start_time.sec);
        strValue = env->NewStringUTF(tmpChars);

        env->SetObjectField(jPrdtInfoObj, fidDtProductStart, strValue);
        env->DeleteLocalRef(strValue);
        //NLOG_D(TAG, "Convert Native-to-Java... COMPLETE - DtProductStart[%s]", tmpChars);

        // mDtProductEnd
        TDSTime end_time = pProductInfo->dt_product_end;
        sprintf(tmpChars, "%04d-%02d-%02dT%02d:%02d:%02d", end_time.year, end_time.month, end_time.day, end_time.hour, end_time.min, end_time.sec);
        strValue = env->NewStringUTF(tmpChars);

        env->SetObjectField(jPrdtInfoObj, fidDtProductEnd, strValue);
        env->DeleteLocalRef(strValue);
        //NLOG_D(TAG, "Convert Native-to-Java... COMPLETE - DtProductEnd[%s]", tmpChars);

        // mProductPrice
        env->SetIntField(jPrdtInfoObj, fidProductPrice, pProductInfo->amt_price);

        // mProductDcPrice
        env->SetIntField(jPrdtInfoObj, fidProductDcPrice, pProductInfo->amt_dc_incr);

        // mFgTerm
        env->SetIntField(jPrdtInfoObj, fidFgTerm, pProductInfo->FG_term);

        // mFgValue
        env->SetIntField(jPrdtInfoObj, fidFgValue, pProductInfo->FG_value);
        return true;
}

bool convertJRetProductList(JNIEnv* env, jclass jRetPrdtListCls, jobject jRetPrdtListObj, jobject jPrdtInfoObj)
{
        if((env == NULL) || (jRetPrdtListCls == NULL) || (jRetPrdtListObj == NULL) || (jPrdtInfoObj == NULL)) {
                return false;
        }

        jmethodID mid = env->GetMethodID(jRetPrdtListCls, "addProductInfo", "(Lcom/tvstorm/tv/si/common/ProductInfo;)V");
        if(mid == NULL) {
                L_DEBUG(TAG, "FAIL to find method id for addProductInfo() in convertJRetProductList()");
                return false;
        }

        env->CallVoidMethod(jRetPrdtListObj, mid, jPrdtInfoObj);
        return true;
}

void setResultOfRetObjectList(JNIEnv* env, jclass jRetObjectListCls, jobject jRetObjectListObj, TDSResult result)
{
        jfieldID fid;

        fid = env->GetFieldID(jRetObjectListCls, "mResult", "I");
        env->SetIntField(jRetObjectListObj, fid, convertSIResultValue(result));
}

