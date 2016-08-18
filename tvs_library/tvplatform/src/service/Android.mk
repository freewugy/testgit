#/*****************************************************************************
# * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
# * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
# *
# * $LastChangedBy: loneowl $
# * $LastChangedDate: 2012-04-02 17:33:14 +0900 (월, 02 4월 2012) $
# * $LastChangedRevision: 2330 $
# * Description:
# * Note:
# *****************************************************************************/

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional

LOCAL_MODULE := libtvs_tvplatform_service

LOCAL_C_INCLUDES :=	\
	$(LOCAL_PATH)/../../include 

LOCAL_SRC_FILES := \
	net/NetControl.cpp

LOCAL_SHARED_LIBRARIES := liblog libbinder libcutils libutils libtvs_tvplatform_proxy

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES:= \
	main_tvplatform.cpp 

LOCAL_C_INCLUDES :=	\
	$(LOCAL_PATH)/../../include \
	$(LOCAL_PATH)/net

LOCAL_SHARED_LIBRARIES := liblog libbinder libcutils libutils libtvs_tvplatform_service

LOCAL_MODULE:= native_tvplatform

include $(BUILD_EXECUTABLE)
