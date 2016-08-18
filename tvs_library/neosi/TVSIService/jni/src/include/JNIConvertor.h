
#ifndef __JNICONVERTOR_H__
#define __JNICONVERTOR_H__

#include <jni.h>
#include <map>
#include <list>
#include <string>

#include "TVS_DVBSI_Interface.h"
#include "ServiceInfo.h"
#include "EventInfo.h"

/***********************************************************************
 *                                                                     *
 *                      CONSTANT DEFINITION                            *
 *                                                                     *
 ***********************************************************************/
//////////////////////////////////////////////////////////////////////
// Video Resolution
//////////////////////////////////////////////////////////////////////
const char CONST_STR_VIDEO_RESOLUTION_HD = 'H';
const char CONST_STR_VIDEO_RESOLUTION_SD = 'S';

const int CONST_INT_VIDEO_RESOLUTION_HD = 1;
const int CONST_INT_VIDEO_RESOLUTION_SD = 0;

//////////////////////////////////////////////////////////////////////
// Audio Type
//////////////////////////////////////////////////////////////////////
const char CONST_STR_AUDIO_TYPE_NONE = 0;
const char CONST_STR_AUDIO_TYPE_MONO = 'M';
const char CONST_STR_AUDIO_TYPE_STREREO = 'S';
const char CONST_STR_AUDIO_TYPE_AC3 = 'A';

const int CONST_INT_AUDIO_TYPE_NONE = 0;
const int CONST_INT_AUDIO_TYPE_MONO = 1;
const int CONST_INT_AUDIO_TYPE_STREREO = 2;
const int CONST_INT_AUDIO_TYPE_AC3 = 3;

//////////////////////////////////////////////////////////////////////
// DOLBY Type
//////////////////////////////////////////////////////////////////////
const char CONST_STR_DOLBY_NOT_SUPPORT = 'N';
const char CONST_STR_DOLBY_SUPPORT = 'D';

const int CONST_INT_DOLBY_NOT_SUPPORT = 0;
const int CONST_INT_DOLBY_SUPPORT = 1;

//////////////////////////////////////////////////////////////////////
// CAPTION Type
//////////////////////////////////////////////////////////////////////
const char CONST_STR_CAPTION_NO = 'N';
const char CONST_STR_CAPTION_YES = 'Y';

const int CONST_INT_CAPTION_NO = 0;
const int CONST_INT_CAPTION_YES = 1;

//////////////////////////////////////////////////////////////////////
// DVS Type
//////////////////////////////////////////////////////////////////////
const char CONST_STR_DVS_NO = 'N';
const char CONST_STR_DVS_YES = 'Y';

const int CONST_INT_DVS_NO = 0;
const int CONST_INT_DVS_YES = 1;

//////////////////////////////////////////////////////////////////////
// JAVA RESULT CODE
//////////////////////////////////////////////////////////////////////
const int CONST_JAVA_SI_RESULT_OK = 0;
const int CONST_JAVA_SI_RESULT_ERR_GEN_FAILURE = 1;
const int CONST_JAVA_SI_RESULT_ERR_INVALID_PARAMETERS = 2;
const int CONST_JAVA_SI_RESULT_ERR_NO_RESOURCE = 3;
const int CONST_JAVA_SI_RESULT_ERR_INFO_IS_NOT_EXIST = 4;
const int CONST_JAVA_SI_RESULT_ERR_INFO_ON_SCANNING = 5;

//////////////////////////////////////////////////////////////////////
// EPG PROVIDER
//////////////////////////////////////////////////////////////////////
const int CONST_JAVA_SI_EPG_PROVIDER_SKB = 0;
const int CONST_JAVA_SI_EPG_PROVIDER_LKN = 1;

/***********************************************************************
 *                                                                     *
 *                      FUCNTION DEFINITION                            *
 *                                                                     *
 ***********************************************************************/
void setStringToJavaParam(JNIEnv* env,
                                jobject jObj,
                                jfieldID jfId,
                                string param);

bool convertJNIChannelInfo(JNIEnv* env,
                                jclass jRetChListCls,
                                jobject jRetChListObj,
                                list<ServiceInfo*> listChannelInfo,
                                uint32_t provider,
                                uint32_t symbolrate,
                                uint32_t bandwidth,
                                uint32_t tunerId);

bool convertJNIPrograms(JNIEnv* env,
                                jclass jPrgmsCls,
                                jobject jPrgmsObj,
                                list<EventInfo*> listEventInfo,
                                jstring channelUid);

bool convertJRetProgramList(JNIEnv* env,
                                jclass jRetPrgmListCls,
                                jobject jRetPrgmListObj,
                                jobject jPrgmsObj);

bool convertJProductList(JNIEnv* env,
                                jclass jPrdtInfoCls,
                                jobject jPrdtInfoObj,
                                TDSProductInfo* pProductInfo);

bool convertJRetProductList(JNIEnv* env,
                                jclass jRetPrdtListCls,
                                jobject jRetPrdtListObj,
                                jobject jPrdtInfoObj);

void setResultOfRetObjectList(JNIEnv* env,
                                jclass jRetObjectListCls,
                                jobject jRetObjectListObj,
                                TDSResult result);

#endif
