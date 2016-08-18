/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 * StreamTypeUtil.h
 *
 * LastChangedBy: owl
 * LastChangedDate: 2014. 11. 17.
 * Description:
 * Note:
 *****************************************************************************/
#ifndef STREAMTYPEUTIL_H_
#define STREAMTYPEUTIL_H_

#include <stdint.h>
#include <stdbool.h>

#include <string>
#include <iostream>

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------------- ES types
// Elementary stream types
// ISO_IEC_13818-1 > Table 2-29. Stream type assignments
// + MPEG4 stream types added

#define ESStreamType_MPEG1Video                 0x01
#define ESStreamType_MPEG2Video                 0x02
#define ESStreamType_MPEG1Audio                 0x03
#define ESStreamType_MPEG2Audio                 0x04
#define ESStreamType_MPEG2PrivateSection        0x05
#define ESStreamType_MPEG2PESPrivateData        0x06
#define ESStreamType_MHEG                       0x07
#define ESStreamType_AnnexADSMCC                0x08
#define ESStreamType_H222_1                     0x09
#define ESStreamType_DSMCCTypeA                 0x0A
#define ESStreamType_DSMCCTypeB                 0x0B
#define ESStreamType_DSMCCTypeC                 0x0C
#define ESStreamType_DSMCCTypeD                 0x0D
#define ESStreamType_MPEG2Auxiliary             0x0E
#define ESStreamType_AACAudio                   0x0F
#define ESStreamType_MPEG4Video                 0x10
#define ESStreamType_H264Video                  0x1B
#define ESStreamType_HVECVideo                  0x24

#define ESStreamType_DC2Video                   0x80
#define ESStreamType_AC3Audio                   0x81

/*
 *
 */
class StreamTypeUtil {
public:
    virtual ~StreamTypeUtil();

    /**
     * @brief instance를 생성하여서 반환
     */
    static StreamTypeUtil& getInstance() {
        if (instance == NULL) {
            if (instance == NULL) {
                instance = new StreamTypeUtil;
            }
        }

        return *instance;
    }

    static StreamTypeUtil* instance;

    bool isVideoStream(int streamType);
    bool isAudioStream(int streamType);
private:
    /**
     * @brief 생성자
     */
    StreamTypeUtil() {
    };                   // Constructor? (the {} brackets) are needed here.
    // Dont forget to declare these two. You want to make sure they
    // are unaccessable otherwise you may accidently get copies of
    // your singleton appearing.
    StreamTypeUtil(StreamTypeUtil const&);            // Don't Implement
    void operator=(StreamTypeUtil const&);         // Don't implement

};

#endif /* STREAMTYPEUTIL_H_ */
