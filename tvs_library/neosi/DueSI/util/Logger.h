/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: eschoi $
 * $LastChangedDate: 2014-06-03 13:58:35 +0900 (화, 03 6월 2014) $
 * $LastChangedRevision: 837 $
 * Description:
 * Note:
 *****************************************************************************/

#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <stddef.h>


#ifdef ANDROID
#include <android/log.h>
#endif

using namespace std;

#ifndef __APP_NAME__
#define __APP_NAME__ "NativeSI"
#endif

void TSL_Printf(const char *Format, ...);

#ifdef ANDROID
#define L_VERB(tag, fmt, ...)       __android_log_print(ANDROID_LOG_VERBOSE, \
				__APP_NAME__, "%s:%s(%d):"fmt, tag, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define L_DEBUG(tag, fmt, ...)      __android_log_print(ANDROID_LOG_DEBUG, \
				__APP_NAME__, "%s:%s(%d):"fmt, tag, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define L_INFO(tag, fmt, ...)       __android_log_print(ANDROID_LOG_INFO, \
				__APP_NAME__, "%s:%s(%d):"fmt, tag, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define L_WARN(tag, fmt, ...)       __android_log_print(ANDROID_LOG_WARN, \
				__APP_NAME__, "%s:%s(%d):"fmt, tag, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define L_ERROR(tag, fmt, ...)      __android_log_print(ANDROID_LOG_ERROR, \
				__APP_NAME__, "%s:%s(%d):"fmt, tag, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define L_TRACE(tag, fmt, ...)      __android_log_print(ANDROID_LOG_DEBUG, \
				__APP_NAME__, "%s:%s(%d):"fmt, tag, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define L_VERB(tag, fmt, args...)       printf("[VBS] %s:%s(%d):"fmt, tag, __FUNCTION__, __LINE__, ##args)
#define L_DEBUG(tag, fmt, args...)      printf("[DBG] %s:%s(%d):"fmt, tag, __FUNCTION__, __LINE__, ##args)
#define L_INFO(tag, fmt, args...)       printf("[INF] %s:%s(%d):"fmt, tag, __FUNCTION__, __LINE__, ##args)
#define L_WARN(tag, fmt, args...)       printf("[WRN] %s:%s(%d):"fmt, tag, __FUNCTION__, __LINE__, ##args)
#define L_ERROR(tag, fmt, args...)      printf("[ERR] %s:%s(%d):"fmt, tag, __FUNCTION__, __LINE__, ##args)
#define L_TRACE(tag, fmt, args...)      printf("[TRC] %s:%s(%d):"fmt, tag, __FUNCTION__, __LINE__, ##args)

#endif /* ANDROID */

#ifdef DEBUGLOG_ENABLE
#define DL_VERB(tag, fmt, args...)      L_VERB(tag, fmt, ##args);\
        TSL_Printf("[VBS] %s:%s(%d):"fmt, tag, __FUNCTION__, __LINE__, ##args)
#define DL_DEBUG(tag, fmt, args...)     L_DEBUG(tag, fmt, ##args);\
        TSL_Printf("[DBG] %s:%s(%d):"fmt, tag, __FUNCTION__, __LINE__, ##args)
#define DL_INFO(tag, fmt, args...)      L_INFO(tag, fmt, ##args);\
        TSL_Printf("[INF] %s:%s(%d):"fmt, tag, __FUNCTION__, __LINE__, ##args)
#define DL_WARN(tag, fmt, args...)      L_WARN(tag, fmt, ##args);\
        TSL_Printf("[WRN] %s:%s(%d):"fmt, tag, __FUNCTION__, __LINE__, ##args)
#define DL_ERROR(tag, fmt, args...)     L_ERROR(tag, fmt, ##args);\
        TSL_Printf("[ERR] %s:%s(%d):"fmt, tag, __FUNCTION__, __LINE__, ##args)
#define DL_TRACE(tag, fmt, args...)     L_TRACE(tag, fmt, ##args);\
        TSL_Printf("[TRC] %s:%s(%d):"fmt, tag, __FUNCTION__, __LINE__, ##args)
#else
#define DL_VERB(tag, fmt, args...)      L_VERB(tag, fmt, ##args)
#define DL_DEBUG(tag, fmt, args...)     L_DEBUG(tag, fmt, ##args)
#define DL_INFO(tag, fmt, args...)      L_INFO(tag, fmt, ##args)
#define DL_WARN(tag, fmt, args...)      L_WARN(tag, fmt, ##args)
#define DL_ERROR(tag, fmt, args...)     L_ERROR(tag, fmt, ##args)
#define DL_TRACE(tag, fmt, args...)     L_TRACE(tag, fmt, ##args)

#endif /* DEBUGLOG_ENABLE */

#endif /* LOGGER_H_ */
