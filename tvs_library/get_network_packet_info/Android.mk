LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES:= get_network_packet_info.c
LOCAL_SHARED_LIBRARIES := liblog
LOCAL_MODULE:= libgetnetworkpacketinfo3
#include $(BUILD_EXECUTABLE)
include $(BUILD_SHARED_LIBRARY)
