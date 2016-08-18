#/*****************************************************************************
# * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
# * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
# *
# * $LastChangedBy: eschoi $
# * $LastChangedDate: 2012-12-17 22:28:48 +0900 (Mon, 17 Dec 2012) $
# * $LastChangedRevision: 5573 $
# * Description:
# * Note:
# *****************************************************************************/

LOCAL_PATH := $(call my-dir)
DUESI := DueSI
TDI := TDI

NEOSI_PATH := $(PWD)/../..
DUESI_PATH := $(NEOSI_PATH)/$(DUESI)
TDI_PATH := $(NEOSI_PATH)/$(TDI)

$(info LOCAL_PATH : $(LOCAL_PATH))
$(info NEOSI_PATH : $(NEOSI_PATH))
$(info DUESI_PATH : $(DUESI_PATH))
$(info TDI_PATH : $(TDI_PATH))

include $(CLEAR_VARS)

STL_PATH := $(ANDROID_NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/$(TOOLCHAIN_VERSION)/include

LOCAL_MODULE := tvs_jnisi

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH) \
	$(LOCAL_PATH)/include \
	$(DUESI_PATH)/include \
	$(DUESI_PATH)/util \

LOCAL_SRC_FILES := \
	DvbSiProxy.cpp \
	JNI_Convertor.cpp \
	JNI_Utils.cpp \
	SIHandler.cpp

LOCAL_SHARED_LIBRARIES := \
	tvs_tdisi \
	tvs_duesi \

LOCAL_LDLIBS := \
	-llog -ldl 

LOCAL_CFLAGS += \
	-D__SAVE_LOG__	\
	
include $(BUILD_SHARED_LIBRARY)
