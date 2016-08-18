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

#ifndef TDI_TUNER_H_
#define TDI_TUNER_H_

#include "TDI.h"

////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @file	TDI_Tuner.h
 * @brief	TDI_Tuner.h 에서 다루고 있는 function 및 data type에 대하여 설명 <br>
 *
 * 튜너에서 정보를 얻는 방법을 설명<br>
 * 다중 튜너를 지원해야 하며 각 디바이스는 핸들값으로 구분 한다.<br>
 * 튜너는 H/W , S/W 튜너를 지원해야 한다  <br>
 * 이 함수들은 모두 thread-safety를 보장 해야 한다. 또한 이 함수들은 다중 프로세스 환경에서도 작동이 보장되어야 한다,.<br>
 */
#else
/**
 * @file	TDI_Tuner.h
 * @brief	Explanation of the function and the data type which are used in TDI_Tuner.h
 *
 * Explanation of the method to get information from a tuner<br>
 * Multiple tuner have to be supported, each device is assorted by handling result<br>
 * A tuner have to support H/W, S/W tuners.<br>
 * These all functions have to assure thread-safety. And also perform under muti-process status.
 */
#endif // DOXYGEN_LANG_KOREAN



////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @brief	튜너 함수 반환 값에 대한 열거형<br>
 */
#else
/**
* @brief	Enumeration for the tuner function returning result
 */
#endif // DOXYGEN_LANG_KOREAN
typedef enum
{
	TDI_TUNER_RV_OK				= 0,
	TDI_TUNER_RV_ERR_TUNE		= -1,	/**< Tune failed */
	TDI_TUNER_RV_ERR_RESERVE	= -2,	/**< Reserve error (not or already) */
	TDI_TUNER_RV_ERR_INVALID	= -3,	/**< Invalid arguments */
	TDI_TUNER_RV_ERR_TIMEOUT	= -4,	/**< Connection timeout error */
	TDI_TUNER_RV_ERR_NOSIGNAL	= -5,	/**< No signal error */
	TDI_TUNER_RV_ERR_NOTFOUND	= -6	/**< File not found */
} TDI_TUNER_RESULT;



////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @brief	튜너 타입에 대한 열거형<br>
 */
#else
/**
 * @brief	Enumeration for a tuner type
 */
#endif // DOXYGEN_LANG_KOREAN
typedef enum
{
	TDI_TUNER_TYPE_SATELLITE	= 0x00000001,	/**< Satellite Tuner */
	TDI_TUNER_TYPE_CABLE		= 0x00000010,	/**< Cable Tuner */
	TDI_TUNER_TYPE_ANALOGUE		= 0x00000100,	/**< Analogue Tuner */
	TDI_TUNER_TYPE_IP_MC		= 0x00001000,	/**< IP Multicast Tuner */
	TDI_TUNER_TYPE_IP_UC		= 0x00010000,	/**< IP Unicast Tuner */
	TDI_TUNER_TYPE_FILE			= 0x00100000 	/**< FILE Tuner */
} TDI_TUNER_TYPE;



////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @brief	튜너 상태에 대한 열거형<br>
 */
#else
/**
 * @brief	Enumeration for the status of a tuner.
 */
#endif // DOXYGEN_LANG_KOREAN
typedef enum
{
	TDI_TUNER_STATUS_UNRESERVED	= 100,
	TDI_TUNER_STATUS_RESERVED	= 200,
	TDI_TUNER_STATUS_TUNING		= 300,
	TDI_TUNER_STATUS_TUNED		= 400
} TDI_TUNER_STATUS;



////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @brief	rf 튜닝에 대한 구조체<br>
 */
#else
/**
 * @brief	rf A structure of a tuning
 */
#endif // DOXYGEN_LANG_KOREAN
typedef struct
{
	uint32_t frequencyHz;
	uint32_t symbolrate;
} TDI_TUNE_RF;



////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @brief	ip 튜닝에 대한 구조체<br>
 */
#else
/**
 * @brief	ip a structure of a tuning
 */
#endif // DOXYGEN_LANG_KOREAN
typedef struct
{
	char ip[256];
	uint32_t port;
	char url[512];
} TDI_TUNE_IP;



////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @brief	file 튜닝에 대한 구조체<br>
 */
#else
/**
 * @brief	file a structure of a tuning
 */
#endif // DOXYGEN_LANG_KOREAN
typedef struct
{
	char name[256];
} TDI_TUNE_FILE;



////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @brief	튜닝 파라메타 구조체<br>
 */
#else
/**
 * @brief	Tuning parameter structure
 */
#endif // DOXYGEN_LANG_KOREAN
typedef struct
{
	TDI_TUNER_TYPE type;
	TDI_TunerProtocol protocol;

	TDI_TUNE_RF rf;
	TDI_TUNE_IP ip;
	TDI_TUNE_FILE file;

} TDI_TUNE_PARAM;



////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @brief	튜너상태를 기술하는 구조체<br>
 */
#else
/**
 * @brief	Structure which describes a status of a tuner
 */
#endif // DOXYGEN_LANG_KOREAN
typedef struct
{
	int						hdl;	/**< Tuner ID */
	TDI_TUNER_TYPE		type;	/**< Supported Types */
	TDI_TUNE_PARAM		param;
	TDI_TUNER_STATUS	status;	/**< Tuner status */
	int16_t 						Signal_noise_ratio;		/**< sSignal-to-noise ratio -1 is not connected cable */
	int16_t 						signal_strength;		/**< signal strength, if TDI_TUNER_STATUS is not TDI_TUNER_STATUS_TUNED then -1 */
} TDI_TUNER_INFO;

/*
	for TVS- 100 hardware Signal_noise_ratio
	- VSB mode
	  LGDT_SIGNAL_NO          :    ~ 0
	  LGDT_SIGNAL_WEAK        :  0 ~ 10
	  LGDT_SIGNAL_MODERATE    : 10 ~ 16
	  LGDT_SIGNAL_STRONG      : 16 ~ 24
	  LGDT_SIGNAL_VERY_STRONG : 24 ~

	- 256QAM mode
	  LGDT_SIGNAL_NO          :    ~ 0
	  LGDT_SIGNAL_WEAK        :  0 ~ 28
	  LGDT_SIGNAL_MODERATE    : 28 ~ 30
	  LGDT_SIGNAL_STRONG      : 31 ~ 32
	  LGDT_SIGNAL_VERY_STRONG : 33 ~
 */


#ifdef __cplusplus
extern "C"{
#endif

////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @brief	사용 가능한 튜너를 조회<br>
 *
 * @param [OUT] count 튜너의 갯수
 * @param [OUT] info 튜너의 종류
 *
 * @return
 * 	TDI_TUNER_RESULT에 정의된 열거 값 중 하나가 반환.<br>
 */
#else
/**
 * @brief	Finding an available tuner
 *
 * @param [OUT] count Number of a tuner
 * @param [OUT] info Kind of a tuner
 *
 * @return
 * 	Returning one of the defined enumerating results in TDI_TUNER_RESULT.
 */
#endif // DOXYGEN_LANG_KOREAN
TDI_TUNER_RESULT  TDI_Tuner_Get_Capability(int32_t *count, TDI_TUNER_STATUS **status);



////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @brief	튜너를 예약 합니다.<br>
 *
 * @param [OUT] hdl 튜너 핸들
 *
 * @return
 * 	TDI_TUNER_RESULT에 정의된 열거 값 중 하나가 반환.<br>
 */
#else
/**
 * @brief	Reserve a tuner.
 *
 * @param [OUT] hdl Tuner handle
 *
 * @return
 * 	Returning one of the defined enumerating result in TDI_TUNER_RESULT.
 */
#endif // DOXYGEN_LANG_KOREAN
TDI_TUNER_RESULT  TDI_Tuner_Reserve(int *hdl);





////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @brief	튜너의 상태를 확인<br>
 *
 * @param [IN] hdl 튜너 핸들
 * @param [IN] status 튜너의 상태를 얻는 구조체
 *
 * @return
 * 	TDI_TUNER_RESULT에 정의된 열거 값 중 하나가 반환.<br>
 */
#else
/**
 * @brief	Check the status of a tuner
 *
 * @param [IN] hdl Tuner handle
 * @param [IN] status A structure which gets the status of a tuner
 *
 * @return
 * 	Returning one of the defined enumerating results in TDI_TUNER_RESULT
 */
#endif // DOXYGEN_LANG_KOREAN
TDI_TUNER_RESULT  TDI_Tuner_Status(int hdl, TDI_TUNER_INFO *info);


////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @brief	리소스를 헤제 하고 TDI_TUNER_STATUS_UNRESERVED 상태로 전이<br>
 *
 * @param [IN] hdl 튜너 핸들
 *
 * @return
 * 	TDI_TUNER_RESULT에 정의된 열거 값 중 하나가 반환.<br>
 */
#else
/**
 * @brief	Release a resource and convert to TDI_TUNER_STATUS_UNRESERVED
 *
 * @param [IN] hdl Tuner handle
 *
 * @return
 * 	Returning one the defined enumerating results in TDI_TUNER_RESULT
 */
#endif // DOXYGEN_LANG_KOREAN
TDI_TUNER_RESULT  TDI_Tuner_Release(int hdl);



////////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
}
#endif
void init_tuner(void);
void term_tuner(void);

#endif /* TDI_TUNER_H_ */



