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

#ifndef LOGGER_TDI_H_
#define LOGGER_TDI_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <stddef.h>

#include "stdarg.h"


#ifdef ANDROID
#include <android/log.h>
#endif

#ifndef __APP_NAME__
#define __APP_NAME__ "NativeSI"
#endif

#ifdef DEBUGLOG_ENABLE

#ifdef ANDROID
	#define L_VERB(fmt, ...) \
		__android_log_print(ANDROID_LOG_VERBOSE, \
				__APP_NAME__, "%s(%d):"fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
	#define L_DEBUG(fmt, ...) \
		__android_log_print(ANDROID_LOG_DEBUG, \
				__APP_NAME__, "%s(%d):"fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
	#define L_INFO(fmt, ...) \
		__android_log_print(ANDROID_LOG_INFO, \
				__APP_NAME__, "%s(%d):"fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
	#define L_WARN(fmt, ...) \
		__android_log_print(ANDROID_LOG_WARN, \
				__APP_NAME__, "%s(%d):"fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
	#define L_ERROR(fmt, ...) \
		__android_log_print(ANDROID_LOG_ERROR, \
				__APP_NAME__, "%s(%d):"fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
	#define L_TRACE(fmt, ...) \
		__android_log_print(ANDROID_LOG_DEBUG, \
				__APP_NAME__, "%s(%d):"fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#else
    #ifdef RELEASE
        #define L_INFO(fmt, args...) 		printf("INFO: %s(%d) "fmt, __FUNCTION__, __LINE__, ##args)
        #define L_DEBUG(fmt, args...) 		printf("DEBUG: %s(%d)"fmt, __FUNCTION__, __LINE__, ##args)
        #define L_ERROR(fmt, args...) 		printf("ERROR: %s(%d)"fmt, __FUNCTION__, __LINE__, ##args)
        #define	L_WARN(fmt, args...) 		printf("WARN: %s(%d)"fmt, __FUNCTION__, __LINE__, ##args)

    #else
        #define L_INFO(fmt, args...) 		printf("INFO: %s::%s(%d) "fmt, __FILE__, __FUNCTION__, __LINE__, ##args)
        #define L_DEBUG(fmt, args...) 		printf("DEBUG: %s::%s(%d) "fmt, __FILE__, __FUNCTION__, __LINE__, ##args)
        #define L_ERROR(fmt, args...) 		printf("ERROR: %s::%s(%d) "fmt, __FILE__, __FUNCTION__, __LINE__, ##args)
        #define	L_WARN(fmt, args...) 		printf("WARN: %s::%s(%d)"fmt, __FILE__, __FUNCTION__, __LINE__, ##args)
    #endif
#endif

#else
	#define L_INFO(fmt, args...) 		
	#define L_DEBUG(fmt, args...) 		
	#define L_ERROR(fmt, args...) 		
	#define	L_WARN(fmt, args...) 		
#endif

        #define L_TEST(fmt, ...) \
                __android_log_print(ANDROID_LOG_DEBUG, \
                                __APP_NAME__, "%s(%d):"fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#endif /* LOGGER_H_ */
