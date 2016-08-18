/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: loneowl $
 * $LastChangedDate: 2014-04-15 13:01:53 +0900 (화, 15 4월 2014) $
 * $LastChangedRevision: 613 $
 * Description:
 * Note:
 *****************************************************************************/

#include "MPEG_Descriptor.h"


#include "BitStream.h"
#include "Logger.h"

static const char* TAG = "MPEG_Descriptor";

////////////////////////////////////////////////////////////////////////////////
MPEG_Descriptor::MPEG_Descriptor()
{
}



MPEG_Descriptor::~MPEG_Descriptor()
{
}



BaseDescriptor* MPEG_Descriptor::createInstance(BitStream* bs)
{
	uint8_t descriptor_tag;
	if (false == bs->readBits(8, &descriptor_tag))
	{
		return NULL;
	}

	BaseDescriptor* descriptor = NULL;
	switch (descriptor_tag)
	{
	    case MPEG_VideoStream_DESCRIPTOR:       	    descriptor = new MPEG_VideoStream_Descriptor();	    break;
	    case MPEG_AudioStream_DESCRIPTOR:       	    descriptor = new MPEG_AudioStream_Descriptor();     break;
	    case MPEG_Hierarchy_DESCRIPTOR:         	    descriptor = new MPEG_Hierarchy_Descriptor();       break;
	    case MPEG_Registration_DESCRIPTOR:      	    descriptor = new MPEG_Registration_Descriptor();    break;
	    case MPEG_DataStreamAlignment_DESCRIPTOR:	    descriptor = new MPEG_DataStreamAlignment_Descriptor();        break;
	    case MPEG_TargetBackgroundGrid_DESCRIPTOR:	    descriptor = new MPEG_TargetBackgroundGrid_Descriptor();        break;
	    case MPEG_VideoWindow_DESCRIPTOR:       	    descriptor = new MPEG_VideoWindow_Descriptor();         break;
	    case MPEG_CA_DESCRIPTOR:                	    descriptor = new MPEG_CA_Descriptor();                  break;

	    case MPEG_ISO639_Lang_DESCRIPTOR:       	    descriptor = new MPEG_ISO639_Lang_Descriptor();        break;
	    case MPEG_SystemClock_DESCRIPTOR:       	    descriptor = new MPEG_SystemClock_Descriptor();        break;
	    case MPEG_MultiplexBufUtil_DESCRIPTOR:  	    descriptor = new MPEG_MultiplexBufUtil_Descriptor();        break;
	    case MPEG_Copyright_DESCRIPTOR:         	    descriptor = new MPEG_Copyright_Descriptor();        break;
	    case MPEG_MaxBitrate_DESCRIPTOR:        	    descriptor = new MPEG_MaxBitrate_Descriptor();        break;
	    case MPEG_PrivateDataIndicator_DESCRIPTOR:	    descriptor = new MPEG_PrivateDataIndicator_Descriptor();        break;
	    case MPEG_SmoothingBuf_DESCRIPTOR:      	    descriptor = new MPEG_SmoothingBuf_Descriptor();        break;
	    case MPEG_STD_DESCRIPTOR:               	    descriptor = new MPEG_STD_Descriptor();        break;
	    case MPEG_IBP_DESCRIPTOR:               	    descriptor = new MPEG_IBP_Descriptor();        break;
	    /* 13818-6 , TR 102 006 */
	    case MPEG_Carousel_Identifier_DESCRIPTOR:	    descriptor = new MPEG_Carousel_Identifier_Descriptor();     break;
	    case MPEG_Association_Tag_DESCRIPTOR:           descriptor = new MPEG_Association_Tag_Descriptor();         break;
	    case MPEG_Deferred_Association_Tags_DESCRIPTOR: descriptor = new MPEG_Deferred_Association_Tags_Descriptor();        break;

	    /* 13818-6  Stream descriptors */
	    case MPEG_NPT_Reference_DESCRIPTOR:             descriptor = new MPEG_NPT_Reference_Descriptor();        break;
	    case MPEG_NPT_Endpoint_DESCRIPTOR:              descriptor = new MPEG_NPT_Endpoint_Descriptor();        break;
	    case MPEG_Stream_Mode_DESCRIPTOR:               descriptor = new MPEG_Stream_Mode_Descriptor();        break;
	    case MPEG_Stream_Event_DESCRIPTOR:              descriptor = new MPEG_Stream_Event_Descriptor();        break;

	    /* MPEG4 */
	    case MPEG_MPEG4_Video_DESCRIPTOR:               descriptor = new MPEG_MPEG4_Video_Descriptor();        break;
	    case MPEG_MPEG4_Audio_DESCRIPTOR:               descriptor = new MPEG_MPEG4_Audio_Descriptor();        break;
	    case MPEG_IOD_DESCRIPTOR:                       descriptor = new MPEG_IOD_Descriptor();        break;
	    case MPEG_SL_DESCRIPTOR:                        descriptor = new MPEG_SL_Descriptor();        break;
	    case MPEG_FMC_DESCRIPTOR:                       descriptor = new MPEG_FMC_Descriptor();        break;
	    case MPEG_External_ES_ID_DESCRIPTOR:            descriptor = new MPEG_External_ES_ID_Descriptor();        break;
	    case MPEG_MuxCode_DESCRIPTOR:                   descriptor = new MPEG_MuxCode_Descriptor();        break;
	    case MPEG_FMXBufferSize_DESCRIPTOR:             descriptor = new MPEG_FMXBufferSize_Descriptor();        break;
	    case MPEG_MultiplexBuffer_DESCRIPTOR:           descriptor = new MPEG_MultiplexBuffer_Descriptor();        break;
	    case MPEG_ContentLabeling_DESCRIPTOR:           descriptor = new MPEG_ContentLabeling_Descriptor();        break;

	    /* TV ANYTIME, TS 102 323 */
	    case MPEG_TVA_Metadata_Pointer_DESCRIPTOR:      descriptor = new MPEG_TVA_Metadata_Pointer_Descriptor();        break;
	    case MPEG_TVA_Metadata_DESCRIPTOR:              descriptor = new MPEG_TVA_Metadata_Descriptor();        break;
	    case MPEG_TVA_Metadata_STD_DESCRIPTOR:          descriptor = new MPEG_TVA_Metadata_STD_Descriptor();        break;

	    /* H.222.0 AMD3 */
	    case MPEG_AVC_Video_DESCRIPTOR:                 descriptor = new MPEG_AVC_Video_Descriptor();        break;
	    case MPEG_IPMP_DESCRIPTOR:                      descriptor = new MPEG_IPMP_Descriptor();        break;
	    case MPEG_AVC_Timing_And_HRD_DESCRIPTOR:        descriptor = new MPEG_AVC_Timing_And_HRD_Descriptor();        break;

	    /* H.222.0 AMD4 */
	    case MPEG_MPEG2_AAC_Audio_DESCRIPTOR:           descriptor = new MPEG_MPEG2_AAC_Audio_Descriptor();        break;
	    case MPEG_FlexMuxTiming_DESCRIPTOR:             descriptor = new MPEG_FlexMuxTiming_Descriptor();        break;

	default:
		L_INFO(TAG, "unsupported descriptor_tag[0x%2x]\n", descriptor_tag);
		descriptor = new BaseDescriptor();
		break;
	} // END switch ()

	return descriptor;
}

////////////////////////////////////////////////////////////////////////////////



