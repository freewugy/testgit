/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: eschoi $
 * $LastChangedDate: 2014-06-03 13:58:35 +0900 (화, 03 6월 2014) $
 * $LastChangedRevision: 837 $
 * Description:
 * Note:
 *****************************************************************************/

#ifndef __TUNEPARAMETER_H__
#define __TUNEPARAMETER_H__

namespace TVSTORM {

#define TVS_MAX_LNBS     				4
#define TVS_MAX_DISEQC_SWITCH_INPUTS	6

typedef enum
{
	TVS_TUNER_0,
	TVS_TUNER_1,
	TVS_TUNER_2,
	TVS_TUNER_3,
	MAX_TVS_TUNER
} TVS_TUNER_TYPE;

typedef enum
{
	TVS_DEMOD_TYPE_UNKNOWN,
	TVS_DEMOD_TYPE_NTSC,
	TVS_DEMOD_TYPE_QAM16,
	TVS_DEMOD_TYPE_QAM64,
	TVS_DEMOD_TYPE_QAM256,
	TVS_DEMOD_TYPE_VSB,
	TVS_DEMOD_TYPE_QPSK,
	TVS_DEMOD_TYPE_8PSK
} TVS_DEMOD_TYPE;

/* Transport specification type */
typedef enum
{
	TVS_TUNER_TRANS_DVB,
	TVS_TUNER_TRANS_DVB_S2,
	TVS_TUNER_TRANS_AUTO
} TVS_TRANSPORT_SPEC;

typedef enum
{
	TVS_TUNER_PILOT_OFF,
	TVS_TUNER_PILOT_ON
} TVS_TUNER_SAT_PILOT;

typedef enum
{
	TVS_TUNER_ROLLOFF_02,
	TVS_TUNER_ROLLOFF_025,
	TVS_TUNER_ROLLOFF_035
} TVS_TUNER_SAT_ROLLOFF;

/* FEC ratio */
typedef enum
{
	TVS_TUNER_FEC_RATE_NONE = 0,
	TVS_TUNER_FEC_RATE_1_4,
	TVS_TUNER_FEC_RATE_1_3,
	TVS_TUNER_FEC_RATE_2_5,
	TVS_TUNER_FEC_RATE_1_2,
	TVS_TUNER_FEC_RATE_2_3,
	TVS_TUNER_FEC_RATE_3_4,
	TVS_TUNER_FEC_RATE_5_6,
	TVS_TUNER_FEC_RATE_7_8,
	TVS_TUNER_FEC_RATE_4_5,
	TVS_TUNER_FEC_RATE_3_5,
	TVS_TUNER_FEC_RATE_6_7,
	TVS_TUNER_FEC_RATE_5_11,
	TVS_TUNER_FEC_RATE_8_9,
	TVS_TUNER_FEC_RATE_9_10,
	TVS_TUNER_FEC_RATE_AUTO
} TVS_TUNER_FEC_RATE;

typedef enum
{
	TVS_LNB_SET_LNB_MODE,					/* Set LNB mode */
	TVS_LNB_SET_LNB_POLARIZATION,			/* Manually switch an LNB polarization */
	TVS_LNB_GET_LNB_POLARIZATION,			/* Query an LNB?챪 polarization */
	TVS_LNB_SET_LNB_TONE,					/* Turn an LNB's 22kHz control tone on or off */
	TVS_LNB_GET_LNB_TONE,					/* Query whether an LNB's 22kHz control tone is on or off */
	TVS_LNB_SEND_LNB_TONE_BURST,			/* Send a 22kHz tone burst to the LNB */
	TVS_LNB_SET_DISEQC_SWITCH,				/* Configure a DiSEqC switch */
	TVS_LNB_GET_DISEQC_SWITCH,				/* Query a DiSEqC switch's configuration */
	TVS_LNB_SEND_DISEQC_MESSAGE,			/* Send a DiSEqC message */
	TVS_LNB_RECV_DISEQC_MESSAGE,			/* Command to receive a DiSEqC message */
	TVS_LNB_SEND_FTM_MESSAGE,				/* Send local/network message */
	TVS_LNB_RECV_FTM_MESSAGE,				/* Command to receive local/network mesg */
	TVS_LNB_SEND_ECHOSTAR_LEGACY_BYTE,		/* Send Echostar Legacy Switch Message Byte */
	TVS_LNB_SET_LNB_VOLTAGE,				/* Set LNB Voltage to 13v, 14v, 18v or 19v */
	TVS_LNB_SET_LNB_POWER,					/* Enable LNB POWER*/
	TVS_LNB_GET_LNB_POWER,					/* Query a LNB's POWER*/
	TVS_LNB_SET_LNB_PRESET_DATA,			/* Preset LNB data */
	TVS_LNB_SET_LNB_LLC,					/* Turn an LNB's LLC control on or off */
	TVS_LNB_GET_LNB_LLC,					/* Query whether an LNB's LLC is on or off */
	TVS_LNB_CONFIGURE_LNB_MODE,				/* LNB configure mode  */
	TVS_LNB_SET_DISEQC_PORT,				/* Set DiSEqC Port */
	TVS_LNB_DISEQC_GOTO_POSITION,			/* DiSEqC motor Goto position */
	TVS_LNB_DISEQC_STORE_POSITION,			/* DiSEqC motor Store position*/
	TVS_LNB_DISEQC_RECALCULATE_POSITION,	/* DiSEqC motor Reclculate position*/
	TVS_LNB_DISEQC_REFERENCE_POSITION,		/* DiSEqC motor Reference position*/
	TVS_LNB_DISEQC_SET_LIMITS,				/* DiSEqC motor Set Limits*/
	TVS_LNB_DISEQC_MOVEON_MOVE,				/* DiSEqC motor Move East,West,Up,Down */
	TVS_LNB_DISEQC_MOVE_STOP,				/* DiSEqC motor Move Stop */
	TVS_LNB_DISEQC_MOVE_STEPS,				/* DiSEqC motor Move by steps*/
	TVS_LNB_DISEQC_SET_BAND,				/* DiSEqC motor Set Band */
	TVS_LNB_USALS_GOTO_XX,					/* Usals motor goto */
	TVS_LNB_USALS_GOTO_ZERO,				/* Usals motor goto zero */
	TVS_LNB_SET_DISEQC_SWITCH_EX,			/* Configure a DiSEqC Ex switch */
	TVS_LNB_GET_DISEQC_SWITCH_EX			/* Query a DiSEqC Ex switch's configuration */      
} TVS_TUNER_LNB_CFG_TYPE;

/* Various LNB modes */
typedef enum
{
	TVS_TUNER_LNB_MODE_FTM = 0,
	TVS_TUNER_LNB_MODE_DISEQC
} TVS_TUNER_LNB_MODE;

/* DiSEqC burst type */
typedef enum
{
	TVS_TUNER_BURST_TYPE_UNMODULATED = 0,	/* tone is not modulated */
	TVS_TUNER_BURST_TYPE_MODULATED,			/* tone is modulated */
} TVS_TUNER_DISEQC_BURST_TYPE;

/* DiSEqC reply mode */
typedef enum
{
	TVS_TUNER_DISEQC_QUICK_REPLY = 0,
	TVS_TUNER_DISEQC_INTERROGATION,
	TVS_TUNER_DISEQC_NO_REPLY
} TVS_TUNER_DISEQC_REPLY_MODE;

/* Polarization of the satellite signal */
typedef enum
{
	TVS_TUNER_HORIZONTAL = 0,
	TVS_TUNER_VERTICAL,
	TVS_TUNER_LEFT,
	TVS_TUNER_RIGHT
} TVS_TUNER_LNB_POLARIZATION;

/* DiSEqC message structure */
typedef struct
{
	unsigned char 				uMessage[20];		/* Pointer to message to send */
	unsigned char           	uMessageLength;		/* length in BYTEs of message to send */
	bool						bLastMessage;	/* indicates if it is the last msg */
	TVS_TUNER_DISEQC_BURST_TYPE	BurstType;
	TVS_TUNER_DISEQC_REPLY_MODE	ReplyMode;
	unsigned char 				uReceivedLength;
	bool						bRxError;
} TVS_TUNER_DISEQC_MESSAGE;

/* FTM message structure */
typedef struct
{
	unsigned char 	MsgBuffer[128];
	unsigned char 	uBuffLength;
} TVS_TUNER_FTM_MESSAGE;

typedef enum
{
	/*
	 * For a single-frequency LNB, the frequency shift it applies is constant
	 * and no control signal need be sent.
	 */
	TVS_DEMOD_LNB_SINGLE_FREQUENCY,
	/*
	 * Dual-frequency LNBs require the LNB to be selected, the polarization to
	 * be set and the correct frequency shift to be applied before tuning the
	 * tuner. All of these are done within the standard satellite demod drivers.
	 */
	TVS_DEMOD_LNB_DUAL_FREQUENCY,
	/*
	 * If the LNB is of orbital position type, the orbital position field of the
	 * tuning parameters must be set to a value equal to the value of one of the
	 * orbital positions set in the LNB settings. This is then used to select an
	 * LNB to use by a standard satellite driver.
	 */
	TVS_DEMOD_LNB_ORBITAL_POSITION,
	/*
	 * For a manual LNB, the user will control the LNB directly themselves.
	 * Therefore the automatic LNB control which the standards satellite demod
	 * drivers normally perform is not required.
	 */
	TVS_DEMOD_LNB_MANUAL,
	/*
	 * A frequency-stacking LNB set-up makes a continuous frequency range out of
	 * the two different polarizations. Given a frequency and a polarization to
	 * tune to, demod_lnb.c will calculate the effective frequency to change to,
	 * within the continuous range.
	 */
	TVS_DEMOD_LNB_FREQUENCY_STACK,
	TVS_DEMOD_LNB_TYPE_LAST = TVS_DEMOD_LNB_FREQUENCY_STACK
} TVS_DEMOD_LNB_TYPE;

typedef enum
{
	/* 12 Volts (nominal - may be around 12-14 Volts depending on hardware). */
	TVS_V_12VOLTS = 12,
	/* 18 Volts (nominal - may be around 17-18 Volts depending on hardware). */
	TVS_V_18VOLTS = 18
} TVS_DEMOD_LNB_VOLTAGE;

typedef struct
{
	/* This is the maximum frequency of signal which this LNB will be used to tune to. */
	unsigned long	uFreqMaxKHz;
	/* The frequency subtracted from the broadcast by this LNB, in KHz. */
	unsigned long	uLNB;
	/*
	 * Orbital position of the dish which this LNB is attached to.
	 * This is only relevant if the LNB type is CNXT_DEMOD_LNB_ORBITAL_POSITION.
	 */
	unsigned int	iOrbitalPos;
	/*
	 * TRUE if the orbital LNB is controlled by a voltage modulated at 22KHz.
	 * FALSE if the orbital LNB is controlled by an unmodulated voltage.
	 * Not used if the LNB is not of type CNXT_DEMOD_LNB_ORBITAL_POSITION.
	 */
	bool			bOrbital22khz;
} TVS_DEMOD_LNB_SETTINGS;

typedef struct
{
	/* The type of the demod (see definition of CNXT_DEMOD_LNB_TYPE) */
	TVS_DEMOD_LNB_TYPE		Type;
	/*
	 * The voltage applied to an LNB to instruct it to filter for
	 * horizontal polarization.
	 */
	TVS_DEMOD_LNB_VOLTAGE	Horizontal;
	/*
	 * The voltage applied to an LNB to instruct it to filter for
	 * left polarization.
	 */
	TVS_DEMOD_LNB_VOLTAGE	Left;
	/*
	 * The voltage applied to an LNB to instruct it to filter for
	 * right polarization.
	 */
	TVS_DEMOD_LNB_VOLTAGE	Right;
	/*
	 * The voltage applied to an LNB to instruct it to filter for
	 * vertical polarization.
	 */
	TVS_DEMOD_LNB_VOLTAGE	Vertical;
	/* Size of pLNBSettings array */
	unsigned long			uNumLNBs;
	/*
	 * A dual LNB will have an array of two CNXT_DEMOD_LNB_SETTINGS
	 * attached to a CNXT_DEMOD_LNB_DATA structure and uNumLNBs will be 2.
	 */
	TVS_DEMOD_LNB_SETTINGS	LNBSettings[ TVS_MAX_LNBS ];
} TVS_DEMOD_LNB_DATA;

typedef enum
{
	TVS_LNB_SINGLE_5150,
	TVS_LNB_SINGLE_10600,
	TVS_LNB_SINGLE_10750,
	TVS_LNB_SINGLE_11250_DISH_LEGACY,
	TVS_LNB_DUAL_5150_5750_OCS,
	TVS_LNB_DUAL_11250_14350_DISHPRO,
	TVS_LNB_DUAL_97500_10600_UINVERSAL,
	TVS_LNB_DUAL_97500_10750_UINVERSAL,
	TVS_LNB_DIRECTV_LEGACY,
	TVS_LNB_MANUAL
} TVS_LNB_PRESET_PARAMETER;

typedef struct
{
	bool						bInUse;          /* TRUE if the switch is in use, FALSE otherwise */
	signed short				nOrbitalPosition;/* In 10ths of a degree */
	TVS_TUNER_DISEQC_BURST_TYPE	BurstType;       /* Type of tone burst to send after sending switch control message. */
} TVS_DISEQC_SWITCH_INPUT;

typedef struct
{
	bool					bEnableSignalling;
	unsigned long			uNumSwitchInputs;
	TVS_DISEQC_SWITCH_INPUT	SwitchInput[TVS_MAX_DISEQC_SWITCH_INPUTS];
} TVS_DISEQC_SWITCH_SETTINGS;

// DiSEqc API struct
typedef struct
{
	unsigned char port;
	unsigned char polirity;
	unsigned char tone22khz;
}TVS_TUNER_DISEQC_PORT;

typedef struct
{
	unsigned char direction;
	unsigned char steps;
}TVS_TUNER_DISEQC_MOTOR_STEP;

/* LNB Port Selecting */
typedef enum
{
   TVS_TUNER_DISEQC10_NONE = 0,
   TVS_TUNER_DISEQC10_LNB1,
   TVS_TUNER_DISEQC10_LNB2,
   TVS_TUNER_DISEQC10_LNB3,
   TVS_TUNER_DISEQC10_LNB4,
   TVS_TUNER_TOTAL_DISEQC10_PORT
} TVS_TUNER_DISEQC10_PORT;

typedef enum
{
	TVS_TUNER_DISEQC11_NONE = 0,
	TVS_TUNER_DISEQC11_LNB1,
	TVS_TUNER_DISEQC11_LNB2,
	TVS_TUNER_DISEQC11_LNB3,
	TVS_TUNER_DISEQC11_LNB4,
	TVS_TUNER_DISEQC11_LNB5,
	TVS_TUNER_DISEQC11_LNB6,
	TVS_TUNER_DISEQC11_LNB7,
	TVS_TUNER_DISEQC11_LNB8,
	TVS_TUNER_DISEQC11_LNB9,
	TVS_TUNER_DISEQC11_LNB10,
	TVS_TUNER_DISEQC11_LNB11,
	TVS_TUNER_DISEQC11_LNB12,
	TVS_TUNER_DISEQC11_LNB13,
	TVS_TUNER_DISEQC11_LNB14,
	TVS_TUNER_DISEQC11_LNB15,
	TVS_TUNER_DISEQC11_LNB16,
	TVS_TUNER_DISEQC11_LNBINT1,
	TVS_TUNER_DISEQC11_LNBINT2,
	TVS_TUNER_DISEQC11_LNBINT3,
	TVS_TUNER_DISEQC11_LNBINT4,
	TVS_TUNER_DISEQC11_LNBINT5,
	TVS_TUNER_DISEQC11_LNBINT6,
	TVS_TUNER_DISEQC11_LNBINT7,
	TVS_TUNER_DISEQC11_LNBINT8,
	TVS_TUNER_TOTAL_DISEQC11_PORT
} TVS_TUNER_DISEQC11_PORT;

typedef enum
{
	TVS_MOTOR_NONE = 0,
	TVS_MOTOR_DISEQC12,
	TVS_MOTOR_USALS,
	TVS_MAX_MOTOR_TYPE
}TVS_TUNER_MOTOR_TYPE;

/*
* USALS Longitude, Latitude,
*/
typedef struct
{
	signed short	sat_longit;
	signed short	iLocalLongitude;
	signed short	iLocalLatitude;
} TVS_TUNER_DISEQC_USALS_LONGIT;


typedef struct
{
	/* Disable is 0, A port is 1, B port is 2, C port is 3, D port is 4 */
	unsigned int					uID10numSwitchInputs;
	/* Disable is 0, 1 port is 1, 2 port is 2, 3 port is 3, 4 port is 4, 
	 * 5 port is 5, 6 port is 6, 7 port is 7, 8 port is 8, 
	 * 9 port is 9, 10 port is 10, 11 port is 11, 12 port is 12,
	 * 13 port is 13, 14 port is 14, 15 port is 15, 16 port is 16,
	 * INS1 port is 17, INS2 port is 18, INS3 port is 19, INS4 port is 20,
	 * INS5 port is 21, INS6 port is 22, INS7 port is 23, INS8 port is 24,
	 */
	unsigned int					uID11numSwitchInputs;
	/* Type of tone burst to send after sending switch control message. */
	TVS_TUNER_DISEQC_BURST_TYPE	BurstType;
	/* None is 0 (MOTOR_NONE),
	 * DiSEqC1.2 is 1 (MOTOR_DISEQC12),
	 * USALS is 2 (MOTOR_USALS)
	 */
	unsigned int					uMotor_type;
	unsigned char					uMotor_position;
	TVS_TUNER_DISEQC_USALS_LONGIT	uLongit;
} TVS_DISEQC_SWITCH_EX_SETTINGS;

typedef enum
{
	TVS_AUDIO_FORMAT_UNKNOWN,
	TVS_AUDIO_FORMAT_MPEG = 0x04, /* MPEG-1 Layers I and II */
	// TVS_AUDIO_FORMAT_MP3, /* MPEG-1 Layer III */
	TVS_AUDIO_FORMAT_AC3 = 0x81, /* Dolby Digital, i.e. AC-3 */
	TVS_AUDIO_FORMAT_AAC = 0x0f,
	// TVS_AUDIO_FORMAT_AAC_PLUS,
	// TVS_AUDIO_FORMAT_DD_PLUS,
	TVS_AUDIO_FORMAT_DTS = 0x8a,
	// TVS_AUDIO_FORMAT_AVS,
	// TVS_AUDIO_FORMAT_MLP,
	// TVS_AUDIO_FORMAT_WMA,
	// TVS_AUDIO_FORMAT_REALAUDIO,
	// TVS_AUDIO_FORMAT_RAW,
} TVS_AUDIO_FORMAT;

typedef enum
{
	TVS_VIDEO_FORMAT_UNKNOWN,
	// TVS_VIDEO_FORMAT_MPEG1, /* MPEG-1 Video (ISO/IEC 11172-2) */
	TVS_VIDEO_FORMAT_MPEG2 = 0x02, /* MPEG-2 Video (ISO/IEC 13818-2) */
	// TVS_VIDEO_FORMAT_H263, /* H.263 Video. The value of the enum is not based on PSI standards. */
	TVS_VIDEO_FORMAT_H264 = 0x1b, /* H.264 (ITU-T) or ISO/IEC 14496-10/MPEG-4 AVC */
	TVS_VIDEO_FORMAT_MPEG4PART2 = 0x10,
	// TVS_VIDEO_FORMAT_VC1,
	// TVS_VIDEO_FORMAT_DIVX,
	// TVS_VIDEO_FORMAT_AVS,
	// TVS_VIDEO_FORMAT_VIP, /* VC-1 Advanced Profile */
	// TVS_VIDEO_FORMAT_JPEG,
	// TVS_VIDEO_FORMAT_GIF,
	// TVS_VIDEO_FORMAT_PNG
} TVS_VIDEO_FORMAT;

typedef enum
{
	TVS_SOURCE_TYPE_TUNER  = 0,
	TVS_SOURCE_TYPE_IPTV,
} TVS_SOURCE_TYPE;

typedef struct
{
	TVS_SOURCE_TYPE 				SourceType;
	TVS_DEMOD_TYPE					DemodType;
	unsigned int 					FreqKHz;
	unsigned int 					SymbolRate;
	/* satellite parameters */
	TVS_TRANSPORT_SPEC				TransSpec;
	// TVS_TUNER_SAT_PILOT			Pilot;
	TVS_TUNER_SAT_ROLLOFF			Rolloff;
	TVS_TUNER_FEC_RATE 				Fec;
	bool							bPilot;
	bool							bSpectrum;
	bool							bScrambling;

	TVS_TUNER_LNB_POLARIZATION		Polarization;
	bool							bLLCEnabled;
	bool							bTone22khz;
	TVS_TUNER_LNB_MODE				Lnbmode;
    TVS_LNB_PRESET_PARAMETER		Lnb_preset;
    TVS_DEMOD_LNB_DATA				Lnb_data;
    TVS_DISEQC_SWITCH_SETTINGS		SwitchData;
    TVS_DISEQC_SWITCH_EX_SETTINGS	SwitchExData;
    TVS_AUDIO_FORMAT				AudioFormat;
    TVS_VIDEO_FORMAT				VideoFormat;
    unsigned int					AudioPid;
    unsigned int 					VideoPid;
    unsigned int 					PcrPid;
    bool							bIsPip;

} TUNER_SOURCE;

} /* namespace TVSTORM */
#endif /* __TUNEPARAMETER_H__ */
