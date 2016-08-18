#/*****************************************************************************
# * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
# * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
# *
# * $LastChangedBy: eschoi $
# * $LastChangedDate: 2012-12-17 22:28:48 +0900 (Mon, 17 Dec 2012) $
# * $LastChangedRevision: 5573 $
# * Description:
# * Note:
# *****************************************************************************/

APP_STL := gnustl_static

APP_CPPFLAGS := \
	-fexceptions \
	-frtti

STLPORT_FORCE_REBUILD := true

APP_ABI := armeabi-v7a
