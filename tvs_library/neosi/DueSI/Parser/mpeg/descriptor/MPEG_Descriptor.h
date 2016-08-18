/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: loneowl $
 * $LastChangedDate: 2014-04-10 15:55:50 +0900 (목, 10 4월 2014) $
 * $LastChangedRevision: 580 $
 * Description:
 * Note:
 *****************************************************************************/

#ifndef MPEG_DESCRIPTOR_H_
#define MPEG_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "BaseDescriptor.h"
#include "MPEG_VideoStream_Descriptor.h"
#include "MPEG_AudioStream_Descriptor.h"
#include "MPEG_Hierarchy_Descriptor.h"
#include "MPEG_DataStreamAlignment_Descriptor.h"
#include "MPEG_Registration_Descriptor.h"
#include "MPEG_TargetBackgroundGrid_Descriptor.h"
#include "MPEG_VideoWindow_Descriptor.h"
#include "MPEG_CA_Descriptor.h"

#include "MPEG_ISO639_Lang_Descriptor.h"
#include "MPEG_SystemClock_Descriptor.h"
#include "MPEG_MultiplexBufUtil_Descriptor.h"
#include "MPEG_Copyright_Descriptor.h"
#include "MPEG_MaxBitrate_Descriptor.h"
#include "MPEG_PrivateDataIndicator_Descriptor.h"
#include "MPEG_SmoothingBuf_Descriptor.h"
#include "MPEG_STD_Descriptor.h"
#include "MPEG_IBP_Descriptor.h"

#include "MPEG_Carousel_Identifier_Descriptor.h"
#include "MPEG_Association_Tag_Descriptor.h"
#include "MPEG_Deferred_Association_Tags_Descriptor.h"

#include "MPEG_NPT_Reference_Descriptor.h"
#include "MPEG_NPT_Endpoint_Descriptor.h"
#include "MPEG_Stream_Mode_Descriptor.h"
#include "MPEG_Stream_Event_Descriptor.h"

#include "MPEG_MPEG4_Video_Descriptor.h"
#include "MPEG_MPEG4_Audio_Descriptor.h"
#include "MPEG_IOD_Descriptor.h"
#include "MPEG_SL_Descriptor.h"
#include "MPEG_FMC_Descriptor.h"
#include "MPEG_External_ES_ID_Descriptor.h"
#include "MPEG_MuxCode_Descriptor.h"
#include "MPEG_FMXBufferSize_Descriptor.h"
#include "MPEG_MultiplexBuffer_Descriptor.h"
#include "MPEG_ContentLabeling_Descriptor.h"

#include "MPEG_TVA_Metadata_Pointer_Descriptor.h"
#include "MPEG_TVA_Metadata_Descriptor.h"
#include "MPEG_TVA_Metadata_STD_Descriptor.h"

#include "MPEG_AVC_Video_Descriptor.h"
#include "MPEG_IPMP_Descriptor.h"
#include "MPEG_AVC_Timing_And_HRD_Descriptor.h"

#include "MPEG_MPEG2_AAC_Audio_Descriptor.h"
#include "MPEG_FlexMuxTiming_Descriptor.h"


enum MPEG_Descriptor_TAG {
    MPEG_VideoStream_DESCRIPTOR = 0x02,
    MPEG_AudioStream_DESCRIPTOR = 0x03,
    MPEG_Hierarchy_DESCRIPTOR = 0x04,
    MPEG_Registration_DESCRIPTOR = 0x05,
    MPEG_DataStreamAlignment_DESCRIPTOR = 0x06,
    MPEG_TargetBackgroundGrid_DESCRIPTOR = 0x07,
    MPEG_VideoWindow_DESCRIPTOR = 0x08,
    MPEG_CA_DESCRIPTOR = 0x09,

    MPEG_ISO639_Lang_DESCRIPTOR = 0x0A,
    MPEG_SystemClock_DESCRIPTOR = 0x0B,
    MPEG_MultiplexBufUtil_DESCRIPTOR = 0x0C,
    MPEG_Copyright_DESCRIPTOR = 0x0D,
    MPEG_MaxBitrate_DESCRIPTOR = 0x0E,
    MPEG_PrivateDataIndicator_DESCRIPTOR = 0x0F,
    MPEG_SmoothingBuf_DESCRIPTOR = 0x10,
    MPEG_STD_DESCRIPTOR = 0x11,
    MPEG_IBP_DESCRIPTOR = 0x12,
    /* 13818-6 , TR 102 006 */
    MPEG_Carousel_Identifier_DESCRIPTOR = 0x13,
    MPEG_Association_Tag_DESCRIPTOR = 0x14,
    MPEG_Deferred_Association_Tags_DESCRIPTOR = 0x15,

    /* 13818-6  Stream descriptors */
    MPEG_NPT_Reference_DESCRIPTOR = 0x17,
    MPEG_NPT_Endpoint_DESCRIPTOR = 0x18,
    MPEG_Stream_Mode_DESCRIPTOR = 0x19,
    MPEG_Stream_Event_DESCRIPTOR = 0x1A,

    /* MPEG4 */
    MPEG_MPEG4_Video_DESCRIPTOR = 0x1B,
    MPEG_MPEG4_Audio_DESCRIPTOR = 0x1C,
    MPEG_IOD_DESCRIPTOR = 0x1D,
    MPEG_SL_DESCRIPTOR = 0x1E,
    MPEG_FMC_DESCRIPTOR = 0x1F,
    MPEG_External_ES_ID_DESCRIPTOR = 0x20,
    MPEG_MuxCode_DESCRIPTOR = 0x21,
    MPEG_FMXBufferSize_DESCRIPTOR = 0x22,
    MPEG_MultiplexBuffer_DESCRIPTOR = 0x23,
    MPEG_ContentLabeling_DESCRIPTOR = 0x24,

    /* TV ANYTIME, TS 102 323 */
    MPEG_TVA_Metadata_Pointer_DESCRIPTOR = 0x25,
    MPEG_TVA_Metadata_DESCRIPTOR = 0x26,
    MPEG_TVA_Metadata_STD_DESCRIPTOR = 0x27,

    /* H.222.0 AMD3 */
    MPEG_AVC_Video_DESCRIPTOR = 0x28,
    MPEG_IPMP_DESCRIPTOR = 0x29,
    MPEG_AVC_Timing_And_HRD_DESCRIPTOR = 0x2A,

    /* H.222.0 AMD4 */
    MPEG_MPEG2_AAC_Audio_DESCRIPTOR = 0x2B,
    MPEG_FlexMuxTiming_DESCRIPTOR = 0x2C,

};

////////////////////////////////////////////////////////////////////////////////
class MPEG_Descriptor {
public:
    MPEG_Descriptor();
    virtual ~MPEG_Descriptor();
    static BaseDescriptor* createInstance(BitStream* bs);
};

////////////////////////////////////////////////////////////////////////////////

#endif /* MPEG_DESCRIPTOR_H_ */

