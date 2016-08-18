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

LOCAL_MODULE := libtvs_tvplatform_proxy

LOCAL_C_INCLUDES :=	\
	$(LOCAL_PATH)/../../include 

LOCAL_SRC_FILES := \
	net/INetControl.cpp

LOCAL_SHARED_LIBRARIES := liblog libbinder libcutils libutils

include $(BUILD_SHARED_LIBRARY)
