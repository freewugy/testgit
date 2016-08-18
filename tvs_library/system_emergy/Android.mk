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
REFSW_PATH :=vendor/broadcom/bcm_platform/brcm_nexus

ifeq ($(NEXUS_MODE),proxy)
NEXUS_LIB=libnexus
else
ifeq ($(NEXUS_WEBCPU),core1_server)
NEXUS_LIB=libnexus_webcpu
else
NEXUS_LIB=libnexus_client
endif
endif

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

#LOCAL_C_INCLUDES := \
#     $(REFSW_PATH)/bin/include \
#     $(REFSW_PATH)/../libnexusservice \
#     $(REFSW_PATH)/../libnexusipc \

LOCAL_C_INCLUDES += \
	vendor/tvstorm/library/eeprom_api \
	$(LOCAL_PATH)

# Nexus multi-process, client-server related CFLAGS
MP_CFLAGS = -DANDROID_CLIENT_SECURITY_MODE=$(ANDROID_CLIENT_SECURITY_MODE)

LOCAL_CFLAGS:= $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES)) -DANDROID $(MP_CFLAGS) -fpermissive
LOCAL_CFLAGS += -DLOGD=ALOGD -DLOGE=ALOGE -DLOGW=ALOGW -DLOGV=ALOGV -DLOGI=ALOGI

LOCAL_SHARED_LIBRARIES += \
 	liblog \
	libtvseeprom

LOCAL_SRC_FILES := \
	system_emergy.cpp

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := system_emergy

include $(BUILD_EXECUTABLE)
