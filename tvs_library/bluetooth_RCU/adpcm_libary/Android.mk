##############################################################################
# Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
# TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
#
# $LastChangedBy: $ cssong
# $LastChangedDate: $ 2015.05.8
# $LastChangedRevision: $ V1.0.0
# Description: CSR adpcm to pcm Converter
# Note:
##############################################################################
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := libadpcm

LOCAL_SRC_FILES := csr_adpcm.c
include $(BUILD_STATIC_LIBRARY)