#/*****************************************************************************
# * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
# * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
# *
# * $LastChangedBy: skkim $
# * $LastChangedDate: 2011-09-14 16:51:52 +0900 (수, 14 9월 2011) $
# * $LastChangedRevision: 164 $
# * Description:
# * Note:
# *****************************************************************************/

CURRENT_DIR_PATH:= $(call my-dir)
# Include makefiles here.
include $(CURRENT_DIR_PATH)/proxy/Android.mk
include $(CURRENT_DIR_PATH)/service/Android.mk
include $(CURRENT_DIR_PATH)/client/Android.mk

