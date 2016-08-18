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

#ifndef TDI_DMUX_H_
#define TDI_DMUX_H_

#include "TDI.h"

////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @file	TDI_Dmux.h
 * @brief	TDI_Dmux.h 에서 다루고 있는 function 및 data type에 대하여 설명<br>
 *
 * 이 장에서는 filter에서 섹션을얻는 방법에 대해 설명한다.<br>
 * 이 함수들의 성능은 전체 시스템의 성능을 결정 한다.<br>
 * 어떤 경우에도 요청된 섹션을 놓치지 않아야한다.<br>
 * 이 함수들은 모두 thread-safety를 보장 해야 한다. 또한 이 함수들은 다중 프로세스 환경에서도 작동이 보장되어야 한다,.<br>
 */
#else
/**
 * @file	TDI_Dmux.h
 * @brief	Explanation of the function and the data system which are used in TDI_Dmux.h
 *
 * Explaining the method to get the section from filter in this chapter.<br>
 * Explaining The performance of these functions affects to the performance of all system<br>
 * Explaining Do not miss asked section in any event.<br>
 * Explaining These functions have to assure thread-safety. And also perform under multi-process condition,.
 */
#endif // DOXYGEN_LANG_KOREAN



////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @brief	filter mask 최대값<br>
 */
#else
/**
 * @brief	filter mask maximum result
 */
#endif // DOXYGEN_LANG_KOREAN
#define TDI_FILTER_MASK_MAX 7



////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @brief	Demux 함수 반환 값에 대한 열거형<br>
 */
#else
/**
 * @brief	Enumeration for the returning Demux function result.
 */
#endif // DOXYGEN_LANG_KOREAN
typedef enum
{
	TDI_DMUX_OK				= 0,	/**< SUCCESS */
	TDI_DMUX_ERR			= -1,	/**< error */
	TDI_DMUX_ERR_NOMORE		= -2,	/**< cannot Reserve */
	TDI_DMUX_ALREADY_RESERVE = -3, /**< Already Reserve */
	TDI_DMUX_ERR_STATE		= -4	/**< DMux Already start */
} TDI_DMUX_RESULT;



////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @brief	Demux 상태에 대한 열거형<br>
 */
#else
/**
 * @brief	Enumeration for the status of a Demux.
 */
#endif // DOXYGEN_LANG_KOREAN
typedef enum
{
	TDI_DMUX_STATUS_UNKNOWN		= 100,
	TDI_DMUX_STATUS_RELEASED	= 200,
	TDI_DMUX_STATUS_RESERVED	= 300,
	TDI_DMUX_STATUS_STARTED		= 400,
	TDI_DMUX_STATUS_STOPED		= 500
} TDI_DMUX_STATUS;



////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @brief	filter함수 반환 값에 대한 열거형<br>
 */
#else
/**
 * @brief	Enumeration for the filter function returning result
 */
#endif // DOXYGEN_LANG_KOREAN
typedef enum
{
	TDI_FILTER_OK			= 0,	/**< SUCCESS */
	TDI_FILTER_ERR_ARGS		= -1,	/**< Invalid Arguments */
	TDI_FILTER_ERR_PID		= -2,	/**< PID error */
	TDI_FILTER_ERR_MASK		= -3,	/**< Invalid Mask */
	TDI_FILTER_ERR_BUF		= -4,	/**< Buffer error */
	TDI_FILTER_ERR_DEMUX	= -5,	/**< Demux error */
	TDI_FILTER_ERR_SAME		= -6,	/**< Same Filter is already working. */
	TDI_FILTER_ERR_NOMORE	= -7,	/**< Cannot create any more filter. */
	TDI_FILTER_ERR_STATE	= -8 	/**< Already started or stopped */
} TDI_FILTER_RESULT;



////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @brief	filter 상태에 대한 열거형<br>
 */
#else
/**
 * @brief	Enumeration for the status of a filter
 */
#endif // DOXYGEN_LANG_KOREAN
typedef enum
{
	TDI_FILTER_STATUS_UNKNOWN		= 100,
	TDI_FILTER_STATUS_RELEASED		= 200,
	TDI_FILTER_STATUS_RESERVED		= 300,
	TDI_FILTER_STATUS_STARTED		= 400,
	TDI_FILTER_STATUS_STOPED		= 500
} TDI_FILTER_STATUS;


////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @brief	filter 마스크 구조체<br>
 */
#else
/**
 * @brief	filter mask structure
 */
#endif // DOXYGEN_LANG_KOREAN


#ifdef __cplusplus
extern "C"{
#endif




////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @brief	시스템에서 사용 가능한 Demux 갯수<br>
 *
 * @return
 * 	사용 가능한 Demux의 갯수를 반환<br>
 */
#else
/**
 * @brief	Number of available Demux at the system
 *
 * @return
 * 	Returning number of available demux
 */
#endif // DOXYGEN_LANG_KOREAN
uint32_t TDI_Dmux_Get_Available_Count(void);



////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @brief	사용 가능한 Demux 핸들을 반환
 *
 * @param [OUT] hdl Demux 핸들
 * demux가  해당 tuner 에 Reserve 되어 있는 경우 Reserve된 핸들러를 반환 한다.
 * @param [IN] 	tuner 핸들
 *
 * @return
 * 	TDI_DMUX_RESULT 정의된 열거 값 중 하나가 반환<br>
 */
#else
/**
 * @brief	 Returning available Demux handle
 *
 * @param [OUT] hdl Demux handle
 * @param [IN] 	tuner handle
 *
 * @return
 * 	TDI_DMUX_RESULT Returning one of the defined enumerating results
 */
#endif // DOXYGEN_LANG_KOREAN
TDI_DMUX_RESULT TDI_Dmux_Reserve(int *hdl, int tuner_handle);

typedef enum
{
	TDI_DMUX_EVENT_OK,
	TDI_DMUX_EVENT_TIMEOUT,
	TDI_DMUX_EVENT_CORRUPTED
}TDI_DMUX_EVENT;



////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @brief	Demux를 구동 한다.<br>
 *
 * @param [IN] hdl Demux 핸들
 * @param [IN] hdl Demux handle
 *
 * @return
 * 	TDI_DMUX_RESULT 정의된 열거 값 중 하나가 반환<br>
 */
#else
/**
 * @brief	Running Demux
 *
 * @param [IN] hdl Demux handle
 *
 * @return
 * 	TDI_DMUX_RESULT Returning one of the defined enumerating result
 */
#endif // DOXYGEN_LANG_KOREAN
TDI_DMUX_RESULT TDI_Dmux_Start(int  hdl);



////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @brief	Demux를 정지 한다.<br>
 *
 * @param  [IN] hdl Demux 핸들
 *
 * @return
 * 	TDI_DMUX_RESULT 정의된 열거 값 중 하나가 반환<br>
 */
#else
/**
 * @brief	Stop Demux.
 *
 * @param  [IN] hdl Demux handle
 *
 * @return
 * 	TDI_DMUX_RESULT Returning one of the defined enumerating results
 */
#endif // DOXYGEN_LANG_KOREAN
TDI_DMUX_RESULT TDI_Dmux_Stop(int hdl);



////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @brief	Demux의 상태를 조회한다.<br>
 *
 * @param [IN] hdl Demux 핸들
 * @param [OUT] status Demux 상태
 *
 * @return
 * 	TDI_DMUX_RESULT 정의된 열거 값 중 하나가 반환<br>
 */
#else
/**
 * @brief	Checking the status of a Demux.
 *
 * @param [IN] hdl Demux handle
 * @param [OUT] status Demux
 *
 * @return
 * 	TDI_DMUX_RESULT Retuning one of the defined enumerating results
 */
#endif // DOXYGEN_LANG_KOREAN
TDI_DMUX_RESULT TDI_Dmux_Status(int hdl, TDI_DMUX_STATUS *status);



////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @brief	Demux를 Release 한다<br>
 *
 * @param [IN] hdl Demux 핸들
 *
 * @return
 * 	TDI_DMUX_RESULT 정의된 열거 값 중 하나가 반환<br>
 */
#else
/**
 * @brief	Releasing Demux.
 *
 * @param [IN] hdl Demux handle
 *
 * @return
 * 	TDI_DMUX_RESULT Returning one of the defined enumerating results
 */
#endif // DOXYGEN_LANG_KOREAN
TDI_DMUX_RESULT TDI_Dmux_Release(int hdl);



////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @brief	시스템에서 사용 가능한 section filter 갯수<br>
 * @param [IN] hdl Demux handle
 * @return
 * 	현재 사용 가능한 section filter의 최대수를 반환<br>
 */
#else
/**
 * @brief	Number of available section filter at the system
 * @param [IN] hdl Demux handle
 * @return
 * 	Returning maximum result of an available section filter
 */
#endif // DOXYGEN_LANG_KOREAN
int32_t TDI_Filter_Get_Available_Count(int hdl);

#ifdef DOXYGEN_LANG_KOREAN
/**
 * @brief	 demux에 할당가능한  section filter의 최대 갯수를 조회한다 <br>
 * @param [IN] hdl Demux handle
 * @return
 * 	demux에 할당가능한  section filter의 최대 갯수<br>
 */
#else
/**
 * @brief 	Number of Max count section filter at the demux
 * @param [IN] hdl Demux handle
 * @return
 * 	Returning maximum result of filter at the demux
 */
#endif // DOXYGEN_LANG_KOREAN
int32_t TDI_Filter_Get_Max_Count(int hdl);

////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @brief	사용 가능한 section filter 핸들을 반환<br>
 *
 * @param [OUT] hdl section filter 핸들
 * @param [IN] dmux 핸들
 *
 * @return
 * 	TDI_FILTER_RESULT 정의된 열거 값 중 하나가 반환<br>
 */
#else
/**
 * @brief	Returning available section filter handle
 *
 * @param [OUT] hdl section filter  handle
 * @param [IN] dmux handle
 *
 * @return
 * 	TDI_FILTER_RESULT Returning one of the defined enumerating result
 */
#endif // DOXYGEN_LANG_KOREAN
TDI_Error TDI_Filter_Reserve(int *hdl, int dmux_hdl);




////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @brief	filter의 상태를 확인<br>
 *
 * @param [IN] hdl section filter 핸들
 *
 * @return
 * 	TDI_FILTER_RESULT에 정의된 열거 값 중 하나가 반환.<br>
 */
#else
/**
 * @brief	Checking the status of a filter
 *
 * @param [IN] hdl section filter handle
 * @param [IN] status status of section filter
 *
 * @return
 * 	Returning one of the defined enumerating results in TDI_FILTER_RESULT.
 */
#endif // DOXYGEN_LANG_KOREAN
TDI_FILTER_RESULT  TDI_Filter_Status(int hdl, TDI_FILTER_STATUS *status);



////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @brief	사용중인 리소스를 반환하고 section filter를 Release 한다<br>
 *
 * @param [IN] hdl section filter 핸들
 *
 * @return
 * 	TDI_FILTER_RESULT 정의된 열거 값 중 하나가 반환<br>
 */
#else
/**
 * @brief	Returning the resource which is under using and releasing section filter
 *
 * @param [IN] 	hdl section filter handle
 *
 * @return
 * 	TDI_FILTER_RESULT Returning one of the defined enumerating results
 */
#endif // DOXYGEN_LANG_KOREAN
TDI_FILTER_RESULT TDI_Filter_Release(int hdl);


////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif /* TDI_FILTER_H_ */

