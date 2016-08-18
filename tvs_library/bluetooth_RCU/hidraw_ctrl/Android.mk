##############################################################################
# Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
# TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
#
# $LastChangedBy: $ cssong
# $LastChangedDate: $ 2015.06.30
# $LastChangedRevision: $ V1.2.5
# Description: Hidraw controler
# Note: 
#         V1.1.0 
#                   add RCU Un-paring request protocol
#         V1.2.0 
#                   add RCU RCU Battery level Request protocol
#         V1.2.1 
#                   logcat
#                   time to modify RCU Version and RCU battery level coming
#         V1.2.2 
#                   fix - Version Information call after unparing
#         V1.2.3
#                   add property tvstorm.bt.rcu.status and verion and battery request time
#         V1.2.4
#                   remove property tvstorm.bt.rcu.status
#         V1.2.5
#                   fix - Voice search Exceptions - back, home, power key
#          V1.2.6
#                   add voice property - Voice search status is tvstorm.bt.rcu.voice.status
##############################################################################
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= hidraw_ctrl.c
	
LOCAL_LDLIBS := -L$(LOCAL_PATH) -ladpcm

LOCAL_SHARED_LIBRARIES  := libutils  libcutils liblog

LOCAL_MODULE := hidraw_ctrl
include $(BUILD_EXECUTABLE)
