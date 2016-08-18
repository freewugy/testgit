##############################################################################
# Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
# TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
#
# $LastChangedBy: $ cssong
# $LastChangedDate: $ 2015.05.28
# $LastChangedRevision: $ V1.2.0
# Description: Front LED Controler Library
# Note:
#         V1.1.0 
#                  add TVS_NETWORK_STATUS
#         V1.2.0 
#                  add TVSTORM_NETWORK_CONFIGURED
##############################################################################
LOCAL_PATH := $(call my-dir)

ifneq ($(filter SH960C-LN%,$(TARGET_DEVICE)),)
TARGET := SH960C_LN
endif

ifneq ($(filter bcm_platform%,$(TARGET_DEVICE)),)
TARGET := ST950I_LN
endif

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

include $(TOP)/${BCM_VENDOR_STB_ROOT}/refsw/nexus/nxclient/include/nxclient.inc

ifneq ($(findstring NEXUS_HAS_GPIO, $(NEXUS_APP_DEFINES)),)
LOCAL_C_INCLUDES += $(NEXUS_GPIO_PUBLIC_INCLUDES)
endif

LOCAL_C_INCLUDES += $(TOP)/${BCM_VENDOR_STB_ROOT}/bcm_platform/libnexusservice \
                    $(TOP)/${BCM_VENDOR_STB_ROOT}/bcm_platform/libnexusipc \
                    $(TOP)/${BCM_VENDOR_STB_ROOT}/bcm_platform/libnexusir \
                    $(TOP)/${BCM_VENDOR_STB_ROOT}/bcm_platform/pmlibservice \
                    $(TOP)/${BCM_VENDOR_STB_ROOT}/drivers/droid_pm \
                    $(TOP)/${BCM_VENDOR_STB_ROOT}/refsw/BSEAV/linux/driver

LOCAL_C_INCLUDES += $(NXCLIENT_INCLUDES)
LOCAL_C_INCLUDES += $(REFSW_BASE_DIR)/nexus/nxclient/include

LOCAL_CFLAGS:= $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES)) -DANDROID $(MP_CFLAGS)
LOCAL_CFLAGS += -D$(TARGET)

ifeq ($(NEXUS_IPC_CLIENT),join)
LOCAL_CFLAGS += -DNXCLIENT_JOIN
else
LOCAL_CFLAGS += -DNXCLIENT_BINDER
endif

# Nexus multi-process, client-server related CFLAGS
MP_CFLAGS = -DANDROID_CLIENT_SECURITY_MODE=$(ANDROID_CLIENT_SECURITY_MODE)

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
	front_led_ctrl.cpp \
	eeprom.cpp

LOCAL_MODULE := libtvsled
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)

##############################################################################
# Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
# TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
#
# $LastChangedBy: $ cssong
# $LastChangedDate: $ 2015.07.14
# $LastChangedRevision: $ V1.3.2
# Description: Init Front LED Controler
# Note:
#         V1.1.0 
#                   Change Scenarios
#                   remove Network valid status 
#                   add eeprom property of passive standby mode
#                   STB is not provisioned mode
#         V1.1.1
#                   Fix sleep
#         V1.1.2
#                   demo Scenarios
#                   wifi on/off
#         V1.2.0
#                   wifi on/off
#                   wifi wpa_supplicant.conf
#         V1.2.1
#                   fix deleted wifi wpa_supplicant.conf
#         V1.2.2
#                   fix arp to ifc_get_info
#         V1.3.0
#                   add rcu led control
#         V1.3.1
#                   add voice property - Voice search status is tvstorm.bt.rcu.voice.status
#         V1.3.2
#                   fix - property init
##############################################################################
include $(CLEAR_VARS)

ifneq ($(findstring NEXUS_HAS_GPIO, $(NEXUS_APP_DEFINES)),)
LOCAL_C_INCLUDES += $(NEXUS_GPIO_PUBLIC_INCLUDES)
endif

LOCAL_C_INCLUDES += $(TOP)/${BCM_VENDOR_STB_ROOT}/bcm_platform/libnexusservice \
                    $(TOP)/${BCM_VENDOR_STB_ROOT}/bcm_platform/libnexusipc \
                    $(TOP)/${BCM_VENDOR_STB_ROOT}/bcm_platform/libnexusir \
                    $(TOP)/${BCM_VENDOR_STB_ROOT}/bcm_platform/pmlibservice \
                    $(TOP)/${BCM_VENDOR_STB_ROOT}/drivers/droid_pm \
                    $(TOP)/${BCM_VENDOR_STB_ROOT}/refsw/BSEAV/linux/driver

LOCAL_C_INCLUDES += $(NXCLIENT_INCLUDES)

LOCAL_CFLAGS:= $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES)) -DANDROID $(MP_CFLAGS)
LOCAL_CFLAGS += -D$(TARGET)

LOCAL_SHARED_LIBRARIES += \
	libc \
	libutils \
	libcutils \
	libnetutils \
	libtvsled

LOCAL_SRC_FILES := \
	init_tvs_led_ctrl.cpp

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := init_tvs_led_ctrl

include $(BUILD_EXECUTABLE)

##############################################################################
# Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
# TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
#
# $LastChangedBy: $ cssong
# $LastChangedDate: $ 2015.05.13
# $LastChangedRevision: $ V1.1.0
# Description: Front LED Controler test
# Note:
##############################################################################
include $(CLEAR_VARS)

ifneq ($(findstring NEXUS_HAS_GPIO, $(NEXUS_APP_DEFINES)),)
LOCAL_C_INCLUDES += $(NEXUS_GPIO_PUBLIC_INCLUDES)
endif

LOCAL_C_INCLUDES += $(TOP)/${BCM_VENDOR_STB_ROOT}/bcm_platform/libnexusservice \
                    $(TOP)/${BCM_VENDOR_STB_ROOT}/bcm_platform/libnexusipc \
                    $(TOP)/${BCM_VENDOR_STB_ROOT}/bcm_platform/libnexusir \
                    $(TOP)/${BCM_VENDOR_STB_ROOT}/bcm_platform/pmlibservice \
                    $(TOP)/${BCM_VENDOR_STB_ROOT}/drivers/droid_pm \
                    $(TOP)/${BCM_VENDOR_STB_ROOT}/refsw/BSEAV/linux/driver

LOCAL_C_INCLUDES += $(NXCLIENT_INCLUDES)

LOCAL_CFLAGS:= $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES)) -DANDROID $(MP_CFLAGS)
LOCAL_CFLAGS += -D$(TARGET)

LOCAL_SHARED_LIBRARIES += \
	libtvsled

LOCAL_SRC_FILES := \
	tvstorm_led_ctrl_test.cpp

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := tvstorm_led_ctrl_test

include $(BUILD_EXECUTABLE)
