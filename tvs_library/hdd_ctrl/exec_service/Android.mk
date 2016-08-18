LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES:= exec_service.cpp 
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libtvsled

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../front_led_control

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= exec_service
include $(BUILD_EXECUTABLE)
