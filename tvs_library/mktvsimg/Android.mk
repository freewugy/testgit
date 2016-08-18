
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := mktvsimg.c compressed_file.c prt_hdr.c gz.c print_ver.c sign_file.c
LOCAL_SRC_FILES += setimg.c common.c extract_part.c
LOCAL_SRC_FILES += inih/ini.c
LOCAL_STATIC_LIBRARIES := libcrypto_static libz
#libmincrypt
#LOCAL_CFLAGS := -Werror

LOCAL_C_INCLUDES += external/openssl/include
LOCAL_C_INCLUDES += vendor/tvstorm/library/mktvsimg/inih
LOCAL_C_INCLUDES += external/zlib/src

$(shell touch $(LOCAL_PATH)/print_ver.c)
LOCAL_CFLAGS += -D_MKTVSIMG -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE=1

ifneq ($(filter SH960C-LN%,$(TARGET_DEVICE)),)
LOCAL_CFLAGS += -DSH960C_LN
else ifneq ($(filter ST950I-LN%,$(TARGET_DEVICE)),)
LOCAL_CFLAGS += -DST950I_LN
endif

LOCAL_MODULE := mktvsimg
LOCAL_MODULE_TAGS := optional
#LOCAL_MODULE_PATH := vendor/tvstorm/prebuilts/build

include $(BUILD_HOST_EXECUTABLE)

$(call dist-for-goals,dist_files,$(LOCAL_BUILT_MODULE))
