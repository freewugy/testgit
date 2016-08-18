LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES:= get_network_packet_info_test.c
LOCAL_SHARED_LIBRARIES := libdl

LOCAL_MODULE:= get_network_packet_info_test3
include $(BUILD_EXECUTABLE)
