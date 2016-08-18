/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: skkim $
 * $LastChangedDate: 2015. 1. 28. $
 * $LastChangedRevision: 80 $
 * Description:
 * Note:
 *****************************************************************************/

#ifndef INCLUDE_TVSTORM_MMFCLIENT_TVSMMFPLAYERTYPES_H_
#define INCLUDE_TVSTORM_MMFCLIENT_TVSMMFPLAYERTYPES_H_

#include <stddef.h>		//	for size_t
#include <inttypes.h>	//	for intXX_t, ...

#ifdef __LP64__ // if 64 bit environment
#ifdef IS64BIT
#undef IS64BIT
#endif
#define IS64BIT true
#endif

namespace tvstorm
{

/******************************************************************************
 * Player error & info code.
 ******************************************************************************/

enum PlayError
{
	//	general
	ERROR_OK,
	ERROR_UNINITIALIZED,
	ERROR_INVALID_URI,
	ERROR_NO_SIGNAL,
	ERROR_RESOURCE_PREEMPTED,

	//	for Android & Linux RPC framework.
	ERROR_RPC_DISCONNECTED,

	//	hardware
	ERROR_HW_BEGIN = 0x01000000,

	//	CAS
	ERROR_CAS_BEGIN = 0x02000000,

	//RTSP & DRM. these were used in VOD DataSource.
	/*
	ERROR_RTSP_100 = 100, //Setup's gethostbyname failure (socket error)
	ERROR_RTSP_101 = 101, //server connection error (socket error)
	ERROR_RTSP_102 = 102, //vod content does not found
	ERROR_RTSP_103 = 103, //vod server's Setup failure
	ERROR_RTSP_105 = 105, //network error (socket error)
	ERROR_RTSP_201 = 201, //wrong rtsp message
	ERROR_RTSP_202 = 202, //keep alive msg error
	ERROR_RTSP_204 = 204, //server or network error like socket closed
	ERROR_RTSP_205 = 205, //incorrect scale when trick call
	ERROR_RTSP_206 = 206, //incorrect time when trick call
	ERROR_RTSP_207 = 207, //Play call error
	ERROR_RTSP_208 = 208, //Pause call error
	ERROR_RTSP_209 = 209, //trick call error
	ERROR_RTSP_300 = 300, //unknown error
	ERROR_RTSP_401 = 401, //DRM RO got fail
	ERROR_RTSP_30101 = 30101, //sync byte error
	ERROR_RTSP_30201 = 30201, //duplicated thread error
	ERROR_RTSP_30201 = 30300, //url not working unknown
	ERROR_RTSP_30201 = 30301, //url not working on wan
	ERROR_RTSP_30201 = 30302, //url not working on lan
	*/
};


enum PlayInfoCode
{
	//	legacy - app interface: onInfo(10000/*TVSPlayer extension*/, infoCode)
#ifdef TVSTORM_SOURCE_SKB_IPTV_ENABLED
	PLAY_INFO_UNKNOWN,              //	0
	PLAY_INFO_TRICK_BOF,            //	1
	PLAY_INFO_TRICK_SUCCESS,        //	2
	PLAY_INFO_TRICK_FAILURE,        //	3
	PLAY_INFO_SEEK_COMPLETED,       //	4
	PLAY_INFO_CURRENT_POSITION,     //	5
	PLAY_INFO_EOS_FEEDED,           //	6
	PLAY_INFO_TRACK_UPDATED,           //	7
#else
	PLAY_INFO_UNKNOWN = 20000,
	PLAY_INFO_TRICK_BOF = 20001,
	PLAY_INFO_TRICK_SUCCESS = 20002,
	PLAY_INFO_TRICK_FAILURE = 20003,
	PLAY_INFO_SEEK_COMPLETED = 20004,
	PLAY_INFO_CURRENT_POSITION = 20005,
	PLAY_INFO_EOS_FEEDED = 20006,
	PLAY_INFO_TRACK_UPDATED = 20007,
#endif

	PLAY_INFO_PLAY_GOOD = 1000,     //	Good
	PLAY_INFO_PLAY_BAD_RECEIVING,   //	bad - cannot receive.
	PLAY_INFO_PLAY_BAD_DECODING,    //	bad - something wrong while decoding.
	PLAY_INFO_PLAY_BUFFER_FULL,     //  bad - skb live only.

	//	internal use only for between TVSMediaServiceImpl and TVSGtvPlayer
	PLAY_INFO_ON_RESOURCE_ALLOCATED = 2000,	//	extra = video plane id.

	TDI_ERROR_BASE = 3000,

	//	new numbering - app interface: onInfo(infoCode, extra)

	PLAY_INFO_SERVER_IP = 10001,	//	10001
	PLAY_INFO_SERVER_PORT,			//	10002

	PLAY_INFO_STOPPED		= 25000,		// 25000
	PLAY_INFO_STARTED,					// 25001
	PLAY_INFO_START_OF_STREAM,			// 25002
	PLAY_INFO_END_OF_STREAM,				// 25003
	PLAY_INFO_FIRST_FRAME_DISPLAYED,	// 25004

	PLAY_INFO_TUNER_LOCKED = 26000,                   // used for MMF Client(SI Service)
	PLAY_INFO_TUNER_LOCK_FAILED = 26001,              //  used for MMF Client(SI Service)
	PLAY_INFO_TUNER_SIGNAL_LOST = 26002,              //  used for MMF Client(SI Service)
	PLAY_INFO_TUNER_SIGNAL_ACQUIRE = 26003,              //  used for MMF Client(SI Service)
	PLAY_INFO_TUNER_WEAK_SIGNAL = 26004,              //  used for MMF Client(SI Service)
	PLAY_INFO_TUNER_VALUE_START = 26005,            	//  used for MMF Client
	PLAY_INFO_TUNER_VALUE_IS_VALID,            	//  used for MMF Client
	PLAY_INFO_TUNER_VALUE_STRENGTH,            	//  used for MMF Client
	PLAY_INFO_TUNER_VALUE_QUALITY,             	//  used for MMF Client
	PLAY_INFO_TUNER_VALUE_POWERLEVEL,        	//  used for MMF Client
	PLAY_INFO_TUNER_VALUE_BER,              	//  used for MMF Client
	PLAY_INFO_TUNER_VALUE_PACKETERR,       	//  used for MMF Client
	PLAY_INFO_TUNER_VALUE_AGC,              	//  used for MMF Client
	PLAY_INFO_TUNER_VALUE_SNR,              	//  used for MMF Client
	PLAY_INFO_TUNER_VALUE_MER,              	//  used for MMF Client
	PLAY_INFO_TUNER_VALUE_END,
	/*
	 * RECORD INFO
	 */
	MEDIA_INFO_RECORD_STOPPED = 27000,
	MEDIA_INFO_RECORD_STARTED = 27001,
	MEDIA_INFO_RECORD_FAILED = 27002,                  // Need stop recorder
	MEDIA_INFO_RECORD_STORAGE_SPACE_LEFT = 27003,      // '10' means capacity is 10% left
	MEDIA_INFO_RECORD_NO_STREAM = 27004,               // Need stop recorder
	MEDIA_INFO_RECORD_LOW_STORAGE_LEFT = 27005,      // means under 15GByte
	MEDIA_INFO_RECORD_STORAGE_FULL = 27006,            //means under 100MByte
	MEDIA_INFO_RECORD_FIRST_IO_SUCCEED = 27007,

	/*
	 * MediaGateway
	 */
	MEDIA_INFO_MGW_PAIR_POPUP_SHOW = 28000,
	MEDIA_INFO_MGW_PAIR_POPUP_CLOSE = 28001,

	/*
	 * CAS
	 */
	MEDIA_INFO_CAS = 29000,

	/*
	 * TVSVOD
	 */
	TVSVOD_SERVER_DISCONNECTED = 30000,

	/*
	 * Internal Event
	 */
	MEDIA_INFO_VIDEO_CODEC_UPDATED = -5000,
	MEDIA_INFO_VIDEO_PID_UPDATED = -5001,
	MEDIA_INFO_AUDIO_CODEC_UPDATED = -5002,
	MEDIA_INFO_AUDIO_PID_UPDATED = -5003,
	MEDIA_INFO_PCR_PID_UPDATED = -5004,
	MEDIA_INFO_PSI_UPDATE_COMPLETED = -5005,

	MEDIA_INFO_NEED_EOS_EVENT = -5006,
	MEDIA_INFO_NEED_BOF_EVENT = -5007,

};

}

#endif /* INCLUDE_TVSTORM_MMFCLIENT_TVSMMFPLAYERTYPES_H_ */
