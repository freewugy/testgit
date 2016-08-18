##############################################################################
# Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
# TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
#
# $LastChangedBy: $ syrics
# $LastChangedDate: $ 2015.10.18
# $LastChangedRevision: $ V0.1
# Description: tvs cmhost service
# Note:
##############################################################################

ifneq ($(filter SH960C-LN%,$(TARGET_DEVICE)),)

LOCAL_PATH:= $(call my-dir)

ifeq ($(NEXUS_MODE),proxy)
NEXUS_LIB=libnexus
else
ifeq ($(NEXUS_WEBCPU),core1_server)
NEXUS_LIB=libnexus_webcpu
else
NEXUS_LIB=libnexus_client
endif
endif

# HAL module implemenation, not prelinked and stored in
# hw/<OVERLAY_HARDWARE_MODULE_ID>.<ro.product.board>.so
include $(CLEAR_VARS)

include $(TOP)/${BCM_VENDOR_STB_ROOT}/refsw/nexus/modules/frontend/build/frontend.inc
LOCAL_PRELINK_MODULE := false

# Nexus multi-process, client-server related CFLAGS
MP_CFLAGS = -DANDROID_CLIENT_SECURITY_MODE=$(ANDROID_CLIENT_SECURITY_MODE)

LOCAL_CFLAGS:= $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES)) -DANDROID $(MP_CFLAGS)
LOCAL_CFLAGS += -DLOGD=ALOGD -DLOGE=ALOGE -DLOGW=ALOGW -DLOGV=ALOGV -DLOGI=ALOGI

include $(TOP)/${BCM_VENDOR_STB_ROOT}/refsw/nexus/nxclient/include/nxclient.inc

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/include \
	$(REFSW_PATH)/bin/include \
	$(TOP)/${BCM_VENDOR_STB_ROOT}/bcm_platform/libnexusservice \
	$(TOP)/${BCM_VENDOR_STB_ROOT}/bcm_platform/libnexusipc \
	$(TOP)/${BCM_VENDOR_STB_ROOT}/refsw/nexus/modules/frontend/common/include \
	$(TOP)/${BCM_VENDOR_STB_ROOT}/refsw/nexus/modules/frontend/docsis/common/include \
	$(TOP)/${BCM_VENDOR_STB_ROOT}/refsw/nexus/modules/frontend/docsis/include \

LOCAL_SRC_FILES := \
	src/ICMHostService.cpp \
	src/CMHostService.cpp \
	src/CMHostServiceClient.cpp \

LOCAL_SHARED_LIBRARIES := \
	$(NEXUS_LIB) \
	libnetutils \
	libcutils \
	libutils \
	libbinder \
	libnexusipcclient

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE := libtvs_cmhost
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
########################################################################################
include $(CLEAR_VARS)

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/include

LOCAL_SRC_FILES := \
	src/CMHostNative.cpp

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libutils \
	libbinder \
	libtvs_cmhost

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE := libtvs_cmhostjni
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
########################################################################################
include $(CLEAR_VARS)

LOCAL_C_INCLUDES := \
	bionic \
	bionic/libstdc++/include \
	$(LOCAL_PATH)/include

LOCAL_SRC_FILES := \
    src/main_CMHostService.cpp

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libutils \
	libbinder \
	libtvs_cmhost

LOCAL_MODULE := tvs_cmhostservice
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
########################################################################################
include $(CLEAR_VARS)

LOCAL_C_INCLUDES := \
	bionic \
	bionic/libstdc++/include \
	external/stlport/stlport \
	$(LOCAL_PATH)/include \
	$(LOCAL_PATH)/test/include

LOCAL_SRC_FILES := \
	test/src/CMHostTest.cpp \
	test/src/CMHostTestMain.cpp

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libutils \
	libstlport \
	libbinder \
	libdl \
	libtvs_cmhost \

LOCAL_MODULE := tvs_cmhost_test
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
########################################################################################

endif
