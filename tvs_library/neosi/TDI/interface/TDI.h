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

#ifndef TDI_H_
#define TDI_H_

/**
 * @ingroup	base
 */
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief
 */
typedef unsigned char BYTE;

////////////////////////////////////////////////////////////////////////////////
typedef int HANDLE;

/**
 * @brief	입력 인자값 (parameters for input)
 */
#ifndef IN
#define IN
#endif

/**
 * @brief	출력 인자값 (parameters for output)
 */
#ifndef OUT
#define OUT
#endif

/**
 * @brief	입,출력 인자값	(parameters that in & out)
 */
#ifndef INOUT
#define INOUT
#endif

/**
 * @brief
 */
typedef enum _TDI_Error
{
	TDI_ERROR = -1,  /**< Error */                                                                  //!< TDI_ERROR
	TDI_SUCCESS = 0, /**< success (always zero) */                                                  //!< TDI_SUCCESS
	TDI_NOT_INITIALIZED, /**< parameter not initialized */                                          //!< TDI_NOT_INITIALIZED
	TDI_INVALID_PARAMETER, /**< parameter is invalid */                                             //!< TDI_INVALID_PARAMETER
	TDI_OUT_OF_SYSTEM_MEMORY, /**< out of system memory (aka OS memory) */                          //!< TDI_OUT_OF_SYSTEM_MEMORY
	TDI_OUT_OF_DEVICE_MEMORY, /**< out of device memory (aka heap memory) */                        //!< TDI_OUT_OF_DEVICE_MEMORY
	TDI_TIMEOUT, /**< reached timeout limit */                                                      //!< TDI_TIMEOUT
	TDI_OS_ERROR, /**< generic OS error */                                                          //!< TDI_OS_ERROR
	TDI_LEAKED_RESOURCE, /**< resource being freed has attached resources that haven't been freed *///!< TDI_LEAKED_RESOURCE
	TDI_NOT_SUPPORTED, /**< requested feature is not supported */                                   //!< TDI_NOT_SUPPORTED
	TDI_UNKNOWN, /**< unknown */                                                                    //!< TDI_UNKNOWN
	TDI_NOT_AVAILABLE, /**< no resource available */                                                //!< TDI_NOT_AVAILABLE
} TDI_Error;

#ifdef __cplusplus
extern "C"
{
#endif

TDI_Error TDI_System_Open(IN bool bUseDirectFB);

TDI_Error TDI_System_Close(void);

typedef enum _TDI_TunerId
{
	TDI_TUNER_1 = 0,						/**< */            //!< TDI_TUNER_1
	TDI_TUNER_2 = 1,						/**< */            //!< TDI_TUNER_2
	TDI_TUNER_USER0 = 2,					/**< */         //!< TDI_TUNER_USER0
	TDI_TUNER_USER1 = 3,					/**< */         //!< TDI_TUNER_USER1
	TDI_TUNER_IP = TDI_TUNER_USER0,			/**< *///!< TDI_TUNER_IP
	TDI_TUNER_COUNT							/**< */            //!< TDI_TUNER_COUNT
} TDI_TunerId;

typedef enum _TDI_TunerType
{
	TDI_TunerType_Analog = 0x00000001,		/**< Analogue Tuner */
	TDI_TunerType_Terrestrial = 0x00000010, /**< Cable Tuner */
	TDI_TunerType_Cable = 0x00000100,		/**< Cable Tuner */
	TDI_TunerType_Satellite = 0x00001000,	/**< Satellite Tuner */
	TDI_TunerType_Ip = 0x00010000,			/**< IP Multicast Tuner */
	TDI_TunerType_File = 0x00100000			/**< FILE Tuner */
} TDI_TunerType;

typedef enum _TDI_TunerProtocol
{
	TDI_TunerProtocol_Unknown,
	TDI_TunerProtocol_File,
	TDI_TunerProtocol_VSB,
	TDI_TunerProtocol_8VSB,
	TDI_TunerProtocol_16QAM,
	TDI_TunerProtocol_32QAM,
	TDI_TunerProtocol_64QAM,
	TDI_TunerProtocol_128QAM,
	TDI_TunerProtocol_256QAM,
	TDI_TunerProtocol_QPSK,
	TDI_TUnerProrocol_8PSK,
	TDI_TunerProtocol_16PSK,
	TDI_TunerProtocol_Udp,
	TDI_TunerProtocol_Rtp,
	TDI_TunerProtocol_Rtsp,
	TDI_TunerProtocol_Http,
	TDI_TunerProtocol_Net,
	TDI_TunerProtocol_Feed,
	TDI_TunerProtocol_DVBT,
	TDI_TunerProtocol_DVBT2,
} TDI_TunerProtocol;
typedef enum _TDI_TunerState
{
	TDI_TUNER_UNKNOWN,
	TDI_TUNER_CLOSED = TDI_TUNER_UNKNOWN,
	TDI_TUNER_OPENED,
	TDI_TUNER_TUNE_FAILED,
	TDI_TUNER_TUNING,
	TDI_TUNER_LOCKED,
	TDI_TUNER_UNLOCKED,
} TDI_TunerState;

typedef enum _TDI_TunerEvent
{
	TDI_TUNER_EVENT_REQ_TUNE,
	TDI_TUNER_EVENT_SUCCESS_TUNE,
	TDI_TUNER_EVENT_FAIL_TUNE,
	TDI_TUNER_EVENT_LOW_POWER,
	TDI_TUNER_EVENT_LOW_SIGNAL,
	TDI_TUNER_EVENT_NO_SIGNAL,
} TDI_TunerEvent;

typedef struct _TDI_TunerRfParameter
{
	uint32_t frequencyHz;
	uint32_t symbolrate;
	uint32_t bandwidth;
} TDI_TunerRfParameter;

typedef struct _TDI_TunerIpParameter
{
	char ip[256];
	uint16_t port;
} TDI_TunerIpParameter;

#define TDI_MAX_LNBS     				4
#define TDI_MAX_DISEQC_SWITCH_INPUTS	6

typedef enum
{
	TDI_TUNER_TRANS_DVB,
	TDI_TUNER_TRANS_DVB_S2,
	TDI_TUNER_TRANS_AUTO
} TDI_TRANSPORT_SPEC;
typedef enum
{
	TDI_TUNER_ROLLOFF_02,
	TDI_TUNER_ROLLOFF_025,
	TDI_TUNER_ROLLOFF_035
} TDI_TUNER_SAT_ROLLOFF;
typedef enum
{
	TDI_TUNER_FEC_RATE_NONE = 0,
	TDI_TUNER_FEC_RATE_1_4,
	TDI_TUNER_FEC_RATE_1_3,
	TDI_TUNER_FEC_RATE_2_5,
	TDI_TUNER_FEC_RATE_1_2,
	TDI_TUNER_FEC_RATE_2_3,
	TDI_TUNER_FEC_RATE_3_4,
	TDI_TUNER_FEC_RATE_5_6,
	TDI_TUNER_FEC_RATE_7_8,
	TDI_TUNER_FEC_RATE_4_5,
	TDI_TUNER_FEC_RATE_3_5,
	TDI_TUNER_FEC_RATE_6_7,
	TDI_TUNER_FEC_RATE_5_11,
	TDI_TUNER_FEC_RATE_8_9,
	TDI_TUNER_FEC_RATE_9_10,
	TDI_TUNER_FEC_RATE_AUTO
} TDI_TUNER_FEC_RATE;
typedef enum
{
	TDI_TUNER_HORIZONTAL = 0,
	TDI_TUNER_VERTICAL,
	TDI_TUNER_LEFT,
	TDI_TUNER_RIGHT
} TDI_TUNER_LNB_POLARIZATION;
typedef enum
{
	TDI_TUNER_LNB_MODE_FTM = 0,
	TDI_TUNER_LNB_MODE_DISEQC
} TDI_TUNER_LNB_MODE;
typedef enum
{
	TDI_LNB_SINGLE_5150,
	TDI_LNB_SINGLE_10600,
	TDI_LNB_SINGLE_10750,
	TDI_LNB_SINGLE_11250_DISH_LEGACY,
	TDI_LNB_DUAL_5150_5750_OCS,
	TDI_LNB_DUAL_11250_14350_DISHPRO,
	TDI_LNB_DUAL_97500_10600_UINVERSAL,
	TDI_LNB_DUAL_97500_10750_UINVERSAL,
	TDI_LNB_DIRECTV_LEGACY,
	TDI_LNB_MANUAL
} TDI_LNB_PRESET_PARAMETER;
typedef enum
{
	/*
	 * For a single-frequency LNB, the frequency shift it applies is constant
	 * and no control signal need be sent.
	 */
	TDI_DEMOD_LNB_SINGLE_FREQUENCY,
	/*
	 * Dual-frequency LNBs require the LNB to be selected, the polarization to
	 * be set and the correct frequency shift to be applied before tuning the
	 * tuner. All of these are done within the standard satellite demod drivers.
	 */
	TDI_DEMOD_LNB_DUAL_FREQUENCY,
	/*
	 * If the LNB is of orbital position type, the orbital position field of the
	 * tuning parameters must be set to a value equal to the value of one of the
	 * orbital positions set in the LNB settings. This is then used to select an
	 * LNB to use by a standard satellite driver.
	 */
	TDI_DEMOD_LNB_ORBITAL_POSITION,
	/*
	 * For a manual LNB, the user will control the LNB directly themselves.
	 * Therefore the automatic LNB control which the standards satellite demod
	 * drivers normally perform is not required.
	 */
	TDI_DEMOD_LNB_MANUAL,
	/*
	 * A frequency-stacking LNB set-up makes a continuous frequency range out of
	 * the two different polarizations. Given a frequency and a polarization to
	 * tune to, demod_lnb.c will calculate the effective frequency to change to,
	 * within the continuous range.
	 */
	TDI_DEMOD_LNB_FREQUENCY_STACK,
	TDI_DEMOD_LNB_TYPE_LAST = TDI_DEMOD_LNB_FREQUENCY_STACK
} TDI_DEMOD_LNB_TYPE;
typedef enum
{
	/* 12 Volts (nominal - may be around 12-14 Volts depending on hardware). */
	TDI_V_12VOLTS = 12,
	/* 18 Volts (nominal - may be around 17-18 Volts depending on hardware). */
	TDI_V_18VOLTS = 18
} TDI_DEMOD_LNB_VOLTAGE;
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
} TDI_DEMOD_LNB_SETTINGS;
typedef struct
{
	/* The type of the demod (see definition of CNXT_DEMOD_LNB_TYPE) */
	TDI_DEMOD_LNB_TYPE		Type;
	/*
	 * The voltage applied to an LNB to instruct it to filter for
	 * horizontal polarization.
	 */
	TDI_DEMOD_LNB_VOLTAGE	Horizontal;
	/*
	 * The voltage applied to an LNB to instruct it to filter for
	 * left polarization.
	 */
	TDI_DEMOD_LNB_VOLTAGE	Left;
	/*
	 * The voltage applied to an LNB to instruct it to filter for
	 * right polarization.
	 */
	TDI_DEMOD_LNB_VOLTAGE	Right;
	/*
	 * The voltage applied to an LNB to instruct it to filter for
	 * vertical polarization.
	 */
	TDI_DEMOD_LNB_VOLTAGE	Vertical;
	/* Size of pLNBSettings array */
	unsigned long			uNumLNBs;
	/*
	 * A dual LNB will have an array of two CNXT_DEMOD_LNB_SETTINGS
	 * attached to a CNXT_DEMOD_LNB_DATA structure and uNumLNBs will be 2.
	 */
	TDI_DEMOD_LNB_SETTINGS	LNBSettings[ TDI_MAX_LNBS ];
} TDI_DEMOD_LNB_DATA;
typedef enum
{
	TDI_TUNER_BURST_TYPE_UNMODULATED = 0,	/* tone is not modulated */
	TDI_TUNER_BURST_TYPE_MODULATED,			/* tone is modulated */
} TDI_TUNER_DISEQC_BURST_TYPE;
typedef struct
{
	bool						bInUse;          /* TRUE if the switch is in use, FALSE otherwise */
	signed short				nOrbitalPosition;/* In 10ths of a degree */
	TDI_TUNER_DISEQC_BURST_TYPE	BurstType;       /* Type of tone burst to send after sending switch control message. */
} TDI_DISEQC_SWITCH_INPUT;
typedef struct
{
	bool					bEnableSignalling;
	unsigned long			uNumSwitchInputs;
	TDI_DISEQC_SWITCH_INPUT	SwitchInput[TDI_MAX_DISEQC_SWITCH_INPUTS];
} TDI_DISEQC_SWITCH_SETTINGS;
typedef struct
{
	signed short	sat_longit;
	signed short	iLocalLongitude;
	signed short	iLocalLatitude;
} TDI_TUNER_DISEQC_USALS_LONGIT;
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
	TDI_TUNER_DISEQC_BURST_TYPE	BurstType;
	/* None is 0 (MOTOR_NONE),
	 * DiSEqC1.2 is 1 (MOTOR_DISEQC12),
	 * USALS is 2 (MOTOR_USALS)
	 */
	unsigned int					uMotor_type;
	unsigned char					uMotor_position;
	TDI_TUNER_DISEQC_USALS_LONGIT	uLongit;
} TDI_DISEQC_SWITCH_EX_SETTINGS;
typedef struct _TDI_TunerSatParameter
{
	unsigned int 					FreqKHz;
	unsigned int 					SymbolRate;
	/* satellite parameters */
	TDI_TRANSPORT_SPEC				TransSpec;
	TDI_TUNER_SAT_ROLLOFF			Rolloff;
	TDI_TUNER_FEC_RATE 				Fec;
	bool							bPilot;
	bool							bSpectrum;
	bool							bScrambling;

	TDI_TUNER_LNB_POLARIZATION		Polarization;
	bool							bLLCEnabled;
	bool							bTone22khz;
	TDI_TUNER_LNB_MODE				Lnbmode;
  TDI_LNB_PRESET_PARAMETER		Lnb_preset;
  TDI_DEMOD_LNB_DATA				Lnb_data;
  TDI_DISEQC_SWITCH_SETTINGS		SwitchData;
  TDI_DISEQC_SWITCH_EX_SETTINGS	SwitchExData;
}TDI_TunerSatParameter;

typedef struct _TDI_TunerFileParameter
{
	char name[256];
}TDI_TunerFileParameter;

typedef union _TDI_TunerParameter
{
	TDI_TunerRfParameter rf;
	TDI_TunerIpParameter ip;
	TDI_TunerSatParameter sat;
	TDI_TunerFileParameter file;
} TDI_TunerParameter;
typedef struct _TDI_TunerSettings
{
	int tunerId; /**< Tuner ID */
	TDI_TunerType type; /**< Supported Types */
	TDI_TunerProtocol protocol;
	TDI_TunerParameter parameter;
} TDI_TunerSettings;

typedef void (*TDI_TunerCallback)(int tunerId, int eventId,int chNum, int freq);


TDI_Error TDI_Tuner_Open(int *pTunerId);

TDI_Error TDI_Tuner_Close(int tunerId);

TDI_Error TDI_Tuner_SetCallback(IN int tunerId, IN TDI_TunerCallback cbFunc);

TDI_Error TDI_Tuner_UnsetCallback(IN int tunerId);

TDI_Error TDI_Tuner_SetSettings(int tunerId,
        TDI_TunerSettings *pSettings);
TDI_Error TDI_Tuner_Stop(int tunerId);        

#define TDI_Tuner_Tune	TDI_Tuner_SetSettings

TDI_Error TDI_Tuner_GetSettings(IN int tunerId, OUT TDI_TunerSettings *pSettings);

TDI_TunerState TDI_Tuner_GetState(IN int tunerId);

TDI_TunerType TDI_Tuner_GetType(IN int tunerId);
TDI_TunerProtocol TDI_Tuner_GetProtocol(IN int tunerId);

int TDI_Tuner_GetSNR(int tunerId);
int TDI_Tuner_GetPower(int tunerId);


typedef enum _TDI_DemuxState
{
	TDI_DEMUX_UNKNOWN,                   //!< TDI_DEMUX_UNKNOWN
	TDI_DEMUX_CLOSED = TDI_DEMUX_UNKNOWN,//!< TDI_DEMUX_CLOSED
	TDI_DEMUX_OPENED,                    //!< TDI_DEMUX_OPENED
	TDI_DEMUX_STOPPED,                   //!< TDI_DEMUX_STOPPED
	TDI_DEMUX_STARTED,                   //!< TDI_DEMUX_STARTED
} TDI_DemuxState;

typedef enum _TDI_DemuxEvent
{
	TDI_DEMUX_EVENT_OK, /* Demux is OK */
	TDI_DEMUX_EVENT_TIMEOUT, /* Demux Got Timeout occured */
	TDI_DEMUX_EVENT_CORRUPTED, /* Demux data is corrupted */
} TDI_DemuxEvent;


#define TDI_FILTER_MASK_LENGTH	7

typedef enum _TDI_FilterState
{
	TDI_FILTER_UNKNOWN,
	TDI_FILTER_CLOSED = TDI_FILTER_UNKNOWN,
	TDI_FILTER_OPENED,
	TDI_FILTER_STOPPED,
	TDI_FILTER_STARTED,
} TDI_FilterState;

typedef enum _TDI_FilterEvent
{
	TDI_FILTER_EVENT_OK, /**< Section is available */
	TDI_FILTER_EVENT_DATAREADY = TDI_FILTER_EVENT_OK, /**< Section is available */
	TDI_FILTER_EVENT_TIMEOUT, /**< Section is time-out. */
	TDI_FILTER_EVENT_CORRUPTED /**< Section is corrupted. */
} TDI_FilterEvent;

/**
 * @brief   필터 타입을 정의하는 열거형.
 */
typedef enum _TDI_FilterType
{
	TDI_FilterType_Section, /* Filter for PSI */
	TDI_FilterType_PesPacket, /* Filter for raw Packet Data (eg. Teletext, EPG) */
} TDI_FilterType;

typedef struct
{
	uint8_t mask[TDI_FILTER_MASK_LENGTH];	/**<  mask */
	//uint8_t mode[TDI_FILTER_MASK_LENGTH];	/**<  mode - positive, negative */
	uint8_t comp[TDI_FILTER_MASK_LENGTH];	/**<  bits for comparison */
} TDI_FILTER_MASK;


typedef void (*TDI_FilterCallback)(int pid, BYTE *buffer, void* pReceiver);

TDI_Error TDI_Demux_Get_Available_ID(int* demuxId);

TDI_Error TDI_Demux_Open(IN int demuxId, IN int tunerId);

TDI_Error TDI_Demux_Close(IN int demuxId);

TDI_Error TDI_Demux_Start(int demuxId);

TDI_Error TDI_Demux_Stop(int demuxId);

TDI_DemuxState TDI_Demux_GetState(int demuxId);

int TDI_Filter_Get_Available_Count(int demuxId);

int TDI_Filter_Get_Max_Count(int demuxId);
TDI_Error TDI_Filter_Open(int demuxId, int* filterId);
TDI_Error TDI_Filter_Start(int filterId, uint16_t pid,
		TDI_FILTER_MASK *pMask, TDI_FilterCallback cbFunc, void* pReceiver);
TDI_Error TDI_Filter_Stop(int filterId);

TDI_FilterState TDI_Filter_GetState(int filterId);
TDI_Error TDI_Filter_Close(int filterId);


#ifdef __cplusplus
}
#endif



#endif /* TDI_H_ */
