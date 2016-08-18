LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES:= hdd_ctrl_jni_test.c
LOCAL_SHARED_LIBRARIES := libdl

LOCAL_MODULE:= hdd_ctrl_jni_test
include $(BUILD_EXECUTABLE)
