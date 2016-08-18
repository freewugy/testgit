/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 * StreamTypeUtil.cpp
 *
 * LastChangedBy: owl
 * LastChangedDate: 2014. 11. 17.
 * Description:
 * Note:
 *****************************************************************************/
#include "StreamTypeUtil.h"

StreamTypeUtil* StreamTypeUtil::instance = NULL;

StreamTypeUtil::~StreamTypeUtil()
{
    // TODO Auto-generated destructor stub
}

bool StreamTypeUtil::isVideoStream(int streamType)
{
    switch (streamType) {
        /* Video */
        case ESStreamType_MPEG1Video:
        case ESStreamType_MPEG2Video:
        case ESStreamType_MPEG4Video:
        case ESStreamType_H264Video:
        case ESStreamType_HVECVideo:
        return true;
        default:
        return false;
    }
}

bool StreamTypeUtil::isAudioStream(int streamType)
{
    switch (streamType) {
        /* Audio */
        case ESStreamType_MPEG1Audio:
        case ESStreamType_MPEG2Audio:
        case ESStreamType_AACAudio:
        case ESStreamType_AC3Audio:
        case ESStreamType_MPEG2PESPrivateData: // 0x06 ESStreamType_DVBAC3Audio
        return true;
        default:
        return false;
    }
}
