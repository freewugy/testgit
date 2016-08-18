# Copyright (C) 2008 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

$(warning nuid_crypt lib)

include $(TOP)/${BCM_VENDOR_STB_ROOT}/refsw/nexus/nxclient/include/nxclient.inc

LOCAL_PRELINK_MODULE := false
LOCAL_SHARED_LIBRARIES := liblog \
                          libcutils \
                          libbinder \
                          libutils \
                          libnexusipcclient \
                          libnexusservice \
                          libnexus \
                          libnxclient

LOCAL_SHARED_LIBRARIES += \
                          libcrypto

LOCAL_STATIC_LIBRARIES := libnxserver

LOCAL_C_INCLUDES += $(TOP)/${BCM_VENDOR_STB_ROOT}/bcm_platform/libnexusservice
LOCAL_C_INCLUDES += $(TOP)/${BCM_VENDOR_STB_ROOT}/bcm_platform/libnexusipc
LOCAL_C_INCLUDES += $(TOP)/${BCM_VENDOR_STB_ROOT}/bcm_platform/libnexusir
LOCAL_C_INCLUDES += $(NXCLIENT_INCLUDES)
LOCAL_C_INCLUDES += $(TOP)/${BCM_VENDOR_STB_ROOT}/refsw/nexus/nxclient/server
LOCAL_C_INCLUDES += $(NEXUS_TOP)/utils
LOCAL_C_INCLUDES += $(TOP)/${BCM_VENDOR_STB_ROOT}/drivers/nx_ashmem
LOCAL_C_INCLUDES += external/openssl/include

LOCAL_CFLAGS += $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES))

ifeq ($(TVS_USE_ENC_SYS),y)
LOCAL_CFLAGS += -DTVS_USE_ENC_SYS
endif

ifneq ($(filter SH960C-LN%,$(TARGET_DEVICE)),)
LOCAL_CFLAGS += -DSH960C_LN
else ifneq ($(filter ST950I-LN%,$(TARGET_DEVICE)),)
LOCAL_CFLAGS += -DST950I_LN
else
$(error It should be defined TARGET_DEVICE to SH960C-LN% or ST950I-LN%)
endif


LOCAL_SRC_FILES := nuid_crypt.cpp \
		   		   nuid.cpp	\
                   endecrypt.cpp

LOCAL_MODULE := libtvs_nuidcrypt
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

################################################################################
include $(CLEAR_VARS)

LOCAL_C_INCLUDES += $(TOP)/${BCM_VENDOR_STB_ROOT}/bcm_platform/libnexusipc
LOCAL_C_INCLUDES += $(TOP)/${BCM_VENDOR_STB_ROOT}/bcm_platform/libnexusservice
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)

# Nexus multi-process, client-server related CFLAGS
MP_CFLAGS = -DANDROID_CLIENT_SECURITY_MODE=$(ANDROID_CLIENT_SECURITY_MODE)

LOCAL_CFLAGS:= $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES)) -DANDROID $(MP_CFLAGS) -fpermissive
LOCAL_CFLAGS += -DLOGD=ALOGD -DLOGE=ALOGE -DLOGW=ALOGW -DLOGV=ALOGV -DLOGI=ALOGI

LOCAL_SHARED_LIBRARIES += \
	libnexus \
	libnexusipcclient \
	libtvs_nuidcrypt

LOCAL_SRC_FILES := \
	tvstorm_nuidcrypt_keygen.cpp

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := tvstorm_nuidcrypt_keygen

include $(BUILD_EXECUTABLE)

