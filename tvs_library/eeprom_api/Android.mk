##############################################################################
# Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
# TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
#
# $LastChangedBy: $ cssong
# $LastChangedDate: $ 2015.04.01
# $LastChangedRevision: $ V1.1.1
# Description: eeprom controler
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

$(warning NEXUS_LIB $(NEXUS_LIB))

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)
LOCAL_SHARED_LIBRARIES :=  \
	$(NEXUS_LIB) \
	liblog \
	libutils \
	libcutils \
	libbinder \
    libnxclient \
	libnexusipcclient

LOCAL_SRC_FILES := \
	tvstorm_eeprom.cpp

LOCAL_CFLAGS:= $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES)) -DANDROID $(MP_CFLAGS) -fpermissive
LOCAL_CFLAGS += -DLOGD=ALOGD -DLOGE=ALOGE -DLOGW=ALOGW -DLOGV=ALOGV -DLOGI=ALOGI

ifeq ($(NEXUS_IPC_CLIENT),join)
LOCAL_CFLAGS += -DNXCLIENT_JOIN
else
LOCAL_CFLAGS += -DNXCLIENT_BINDER
endif

LOCAL_C_INCLUDES += $(TOP)/${BCM_VENDOR_STB_ROOT}/bcm_platform/libnexusipc
LOCAL_C_INCLUDES += $(TOP)/${BCM_VENDOR_STB_ROOT}/bcm_platform/libnexusservice

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../include
LOCAL_C_INCLUDES += $(REFSW_BASE_DIR)/nexus/nxclient/include

LOCAL_MODULE := libtvseeprom
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)


##############################################################################
# Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
# TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
#
# $LastChangedBy: $ cssong
# $LastChangedDate: $ 2015.03.27
# $LastChangedRevision: $ V1.0.0
# Description: eeprom controler test
# Note:
##############################################################################
include $(CLEAR_VARS)

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)

# Nexus multi-process, client-server related CFLAGS
MP_CFLAGS = -DANDROID_CLIENT_SECURITY_MODE=$(ANDROID_CLIENT_SECURITY_MODE)

LOCAL_CFLAGS:= $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES)) -DANDROID $(MP_CFLAGS) -fpermissive
LOCAL_CFLAGS += -DLOGD=ALOGD -DLOGE=ALOGE -DLOGW=ALOGW -DLOGV=ALOGV -DLOGI=ALOGI

LOCAL_SHARED_LIBRARIES += \
	libtvseeprom

LOCAL_SRC_FILES := \
	tvstorm_eeprom_test.cpp

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := tvstorm_eeprom_test

include $(BUILD_EXECUTABLE)
