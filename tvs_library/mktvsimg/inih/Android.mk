
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := ini.c
#LOCAL_STATIC_LIBRARIES := 
LOCAL_CFLAGS := -Werror

LOCAL_MODULE := libini

include $(BUILD_HOST_STATIC_LIBRARY)

#$(call dist-for-goals,dist_files,$(LOCAL_BUILT_MODULE))
