# Copyright (C) 2014 The Android Open Source Project
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
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../include 
LOCAL_SRC_FILES := \
		ftps_client.c \
		https_client.c \
		jsmn.c \
		json_encode.c \
		msg_decoder.c \
		msg_encoder.c \
		operation_mgr.c \
		sus_comm.c \
		sys_info.c \
		property.c \
		server_info.c \
		utils.c		


LOCAL_LDLIBS := -L$(LOCAL_PATH)/../libs -lcurl -lc
LOCAL_MODULE    := libsuscomm

LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

########################################################################

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := NagraDownloader.c

LOCAL_C_INCLUDES := $(LOCAL_PATH)

LOCAL_LDLIBS := -L$(LOCAL_PATH) -lsuscomm -lcrypto -lc

LOCAL_MODULE    := nagra_downloader_test

include $(BUILD_EXECUTABLE)
