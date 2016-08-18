##############################################################################
# Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
# TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
#
# $LastChangedBy: $ cssong
# $LastChangedDate: $ 2015.05.14
# $LastChangedRevision: $ V1.0.2
# Description: Power button uinput
# Note:
##############################################################################
LOCAL_PATH := $(call my-dir)

ifeq ($(NEXUS_MODE),proxy)
NEXUS_LIB=libnexus
else
ifeq ($(NEXUS_WEBCPU),core1_server)
NEXUS_LIB=libnexus_webcpu
else
NEXUS_LIB=libnexus_client
endif
endif

## whether using createClientContext or NxClient_Join
NEXUS_IPC_CLIENT=binder

# HAL module implemenation, not prelinked and stored in
# hw/<OVERLAY_HARDWARE_MODULE_ID>.<ro.product.board>.so
include $(CLEAR_VARS)

# Nexus multi-process, client-server related CFLAGS
MP_CFLAGS = -DANDROID_CLIENT_SECURITY_MODE=$(ANDROID_CLIENT_SECURITY_MODE)

LOCAL_PRELINK_MODULE := false
LOCAL_SHARED_LIBRARIES :=  \
	$(NEXUS_LIB) \
	liblog \
	libutils \
	libcutils \
	libbinder \
    libnxclient \
	libnexusipcclient

LOCAL_SRC_FILES := \
	tvstorm_power_button.cpp


LOCAL_CFLAGS:= $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES)) -DANDROID $(MP_CFLAGS)
LOCAL_CFLAGS += -DLOGD=ALOGD -DLOGE=ALOGE -DLOGW=ALOGW -DLOGV=ALOGV -DLOGI=ALOGI

ifeq ($(NEXUS_IPC_CLIENT),join)
LOCAL_CFLAGS += -DNXCLIENT_JOIN
else
LOCAL_CFLAGS += -DNXCLIENT_BINDER
endif

LOCAL_C_INCLUDES += $(TOP)/${BCM_VENDOR_STB_ROOT}/bcm_platform/libnexusservice \
                    $(TOP)/${BCM_VENDOR_STB_ROOT}/bcm_platform/libnexusipc \
                    $(TOP)/${BCM_VENDOR_STB_ROOT}/bcm_platform/libnexusir \
                    $(TOP)/${BCM_VENDOR_STB_ROOT}/bcm_platform/pmlibservice \
                    $(TOP)/${BCM_VENDOR_STB_ROOT}/drivers/droid_pm \
                    $(TOP)/${BCM_VENDOR_STB_ROOT}/refsw/BSEAV/linux/driver
 
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../include
LOCAL_C_INCLUDES += $(REFSW_BASE_DIR)/nexus/nxclient/include

LOCAL_MODULE := tvstorm_power_button
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)

