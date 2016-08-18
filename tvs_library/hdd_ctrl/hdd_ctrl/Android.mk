LOCAL_PATH:= $(call my-dir)


ifneq ($(filter SH960C-LN%,$(TARGET_DEVICE)),)
TARGET := SH960C_LN
endif

ifneq ($(filter ST950I-LN%,$(TARGET_DEVICE)),)
TARGET := ST950I_LN
endif

$(warning TARGET $(TARGET))

include $(CLEAR_VARS)

LOCAL_MODULE:= hdd_ctrl
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES:=\
	hdd_ctrl.cpp \
	HDDInfoStore.cpp \
	DetectHDDThread.cpp \
	WaitUserThread.cpp \
	WifiStatusThread.cpp \
	MetaFileReadWriteThread.cpp \
	TestFileReadWriteThread.cpp \
	HDDErrDetectThread.cpp \
	LogHelperThread.cpp \
	flash_ts.cpp


LOCAL_SRC_FILES += logcat.cpp

LOCAL_C_INCLUDES := vendor/tv/frameworks/base/include/utils external/e2fsprogs/lib
LOCAL_C_INCLUDES += system/extras/ext4_utils
#LOCAL_C_INCLUDES += system/vold

LOCAL_CFLAGS := -D$(TARGET) -Wno-unused-parameter

#ifeq ($(TVS_USE_DBG_KERNEL),y)
#LOCAL_CFLAGS += -DUSE_DUMP_KERNEL_OOPS=1
#else
LOCAL_CFLAGS += -DUSE_DUMP_KERNEL_OOPS=0
#endif

LOCAL_SHARED_LIBRARIES := libutils libdl libcutils liblog libext2_blkid libext4_utils

include $(BUILD_EXECUTABLE)

##########################################################################################
#ifeq ($(TVS_USE_DBG_KERNEL),y)
#include $(CLEAR_VARS)

#LOCAL_MODULE:= klogcat
#LOCAL_MODULE_TAGS:= optional

#LOCAL_SRC_FILES:=\
	KernelLog.cpp

#LOCAL_C_INCLUDES :=KernelLog.h

#LOCAL_CFLAGS := -Wno-unused-parameter

#LOCAL_SHARED_LIBRARIES := libutils libdl libcutils liblog

#include $(BUILD_EXECUTABLE)
#endif
