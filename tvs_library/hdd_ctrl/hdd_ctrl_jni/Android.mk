LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES:= hdd_ctrl_jni.c
LOCAL_SHARED_LIBRARIES := liblog
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= libhddctrljni
#include $(BUILD_EXECUTABLE)
include $(BUILD_SHARED_LIBRARY)
