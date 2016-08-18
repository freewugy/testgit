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

#ifndef TDI_SYSTEM_H_
#define TDI_SYSTEM_H_



////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
	@mainpage	TVSTORM Basic porting interface guide

	@section	Introduction
		- 본 가이드는 TVSTORM porting interface에 대한 사용 방법 등을 포함하며  porting glue 구현의 전반적인 가이드 라인에 대하여 다룬다.
		- 본 가이드는 방송의 기본적인 기능과 시스템 자원관리를 포함 한다.
		- 이 가이드의 각 하위 section은 각 기능 모듈의 기능을 설명한다.
		- 본 가이드는 적용대상 하드웨어나 서비스 사업자에 따라 변경될수 있다.
		- 현재 본가이드에 정의 되지 않은 라이브러리 함수는 linux C 표준 라이브러리(C standard library)를 사용한다.
		- 본가이드에서 사용되는 Thread는 유닉스 계열 POSIX 시스템에서, 일반적으로 이용되는 POSIX Threads를 사용한다.
		- 본 가이드는 TVSTORM 내부 개발자나 협력사를 위한 개발 문서이며, 외부 공개를 불허한다.

	@section	모듈명세
		- TDI_System.h : porting glue의 초기화, 종료
		- TDI_Tuner.h : H / W 튜너 및 튜닝 API를 위한  추상화
		- TDI_Dmux.h : H / W 디멀티플렉서 (demux) 및 필터링 API를 위한 추상화
		- TDI_Decoder.h : H / W 디코더와 디코딩 API를 위한 추상화
		- TDI_Gfx.h : H / W 그래픽 장치와 그래픽 API를 위한 추상화
		- TDI_Output.h : H / W 출력 포트 및 구성 API를 위한 추상화
		- TDI_Userevent.h : H / W RCU, 키보드 / 마우스 및 입력 API를 위한 추상화
		- TDI_Primitive_types.h : 상위  S / W 레이어에 대한 일반적인 정의와 타입 .

	@section	Terminology
		- H/W (Hardware)
		- S/W (Software)
		- Interface : 일련의 명령어나 함수, 옵션, 그리고 프로그램 언어에 의해 제공되는 명령어나 데이터를 표현하기 위한 다른 방법들로 구성되는 프로그래밍 인터페이스
		- API Application Programming Interface
		- PI (Porting Interface) : 하드웨어에 독립된 추상화 인터페이스
		- PL (Porting or Portability Layer) : PI와 같은 의미
		- PG (Porting Glue) : PI의 구현체
		- RI (Reference Implementation) : Implementation Example for easy understanding.

	@section	작성정보
		- Version 2.0.4
		- 작성자 : yjkim
		- 작성일 : 2011. 9. 10.

	@section	수정정보
		- 수정자 / 수정일 : 수정내역
		- yjkim / 2011. 9.12. : 메인 페이지 추가
		- yjkim / 2011.10.06. : Dmux API 이름 변경, TDI_Filter_Status 추가
		- yjkim / 2011.10.06. : Set_Timer, Get_Version 추가
		- yjkim / 2011.10.18. : TDI_Decoder_Count 변경,
		- yjkim / 2011.10.18. : TDI_Decoder_Status 추가
		- yjkim / 2011.10.18. : TDI_Tuner_Count 변경
		- yjkim / 2011.10.18. : TDI_Tuner_Status 변경
		- yjkim / 2011.10.18. : TDI_Dmux_Count 변경
		- yjkim / 2011.10.18. : TDI_OUT_FORMAT 추가
		- yjkim / 2011.10.18. : TDI_OUTPORT_STAT 변경
		- yjkim / 2011.10.21. : TDI_Decoder.h에서 TDI_VIDEO_FORMAT 삭제 (TDI_Output.h에서 중복)
		- yjkim / 2011.10.24. : TDI_TUNER_RESULT TDI_Tuner_Reserve	(	THANDLE * 	hdl	 ) 변경
		- yjkim / 2011.10.24. : TDI_DMUX_RESULT TDI_Filter_Reserve(THANDLE *hdl);변경
		- yjkim / 2011.10.24. : TDI_DECODER_RESULT  TDI_Decoder_Reserve(THANDLE *hdl);변경
		- yjkim / 2011.10.24. : 용어 변경 (섹션 필터 -> demultiplexer)
		- yjkim / 2011.10.26. : demux 관련API추가
		- msryu / 2011.11.09. : 영문 문서화 추가.
*/
#else
/**
	@mainpage	TVSTORM Basic porting interface guide

	@section	Introduction
		- This guide is including the usage of TVSTORM porting interface and the all-round guide line to realize porting glue
		- This guide is including the basic function for broadcasting and system resource management.
		- A sub-section of this guide explains functions of an each function module.
		- This guide alternation is depends on operator’s requirement or target hardware
		- Library functions which is not defined in this guide uses the linux C standard; C standard library.
		- Thread which is used in this guide uses the POSIX Thread normally in POSIX system affiliated with UNIX
		- This guide is development paper just for TVSTORM’s inner developer or co-operators, and do not unclose to outside

	@section	Module_list
		- TDI_System.h : Initialization of porting glue, finish
		- TDI_Tuner.h : Abstraction of H / W tuner and tuning API
		- TDI_Dmux.h : Abstraction for H / W demux and filtering API
		- TDI_Decoder.h : Abstraction for H / W decoder and decoding API
		- TDI_Gfx.h : Abstraction for H / W the graphic installation and graphic API
		- TDI_Output.h : Abstraction for H / W output port and composition of API
		- TDI_Userevent.h : Abstraction for H / W RCU, Keyboard / Mouse and input API.
		- TDI_Primitive_types.h : Normal definition and type of upper S / W layer

	@section	Terminology
		- H/W (Hardware)
		- S/W (Software)
		- Interface : Applications or programs running on the operating system may need to interact via streams, and in object oriented programs, objects within an application may need to interact via methods.
		- API Application Programming Interface
		- PI (Porting Interface) : Abstracted interface independent from Hardware
		- PL (Porting or Portability Layer) : Same meaning as PI
		- PG (Porting Glue) : Realized object of PI
		- RI (Reference Implementation) : Implementation Example for easy understanding.

	@section	Information_on_writing
		- Version 2.0.4
		- Writer : yjkim
		- Date   : 2011. 9. 10.

	@section	Rev_History
		- Amender/date : Rev. history
		- yjkim / 2011. 9.12. : Addition of main page
		- yjkim / 2011.10.06. : A change of Name of Dmux API, An addition of  TDI_Filter_Status
		- yjkim / 2011.10.06. : An Addition of Set_Timer, Get_Version
		- yjkim / 2011.10.18. : A change of TDI_Decoder_Count ,
		- yjkim / 2011.10.18. : An Addition of TDI_Decoder_Status
		- yjkim / 2011.10.18. : A change of TDI_Tuner_Count
		- yjkim / 2011.10.18. : A change of TDI_Tuner_Status
		- yjkim / 2011.10.18. : A change of TDI_Dmux_Count
		- yjkim / 2011.10.18. : An addition of TDI_OUT_FORMAT
		- yjkim / 2011.10.18. : A change of TDI_OUTPORT_STAT
		- yjkim / 2011.10.21. : A deletion of TDI_VIDEO_FORMAT fromTDI_Decoder.h (duplication in TDI_Output.h)
		- yjkim / 2011.10.24. : A change of TDI_TUNER_RESULT TDI_Tuner_Reserve ( THANDLE * hdl )
		- yjkim / 2011.10.24. : a change of TDI_DMUX_RESULT TDI_Filter_Reserve(THANDLE *hdl);
		- yjkim / 2011.10.24. : A change of TDI_DECODER_RESULT TDI_Decoder_Reserve(THANDLE *hdl);
		- yjkim / 2011.10.24. : A change of terminology(Section filter -> demultiplexer)
		- yjkim / 2011.10.26. : An addition of API related with demux
		- msryu / 2011.11.09. : add English documentation
*/
#endif // DOXYGEN_LANG_KOREAN



////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @file	TDI_System.h
 * @brief	TDI_System.h 에서 다루고 있는 function 및 data type에 대하여 설명<br>
 */
#else
/**
 * @file	TDI_System.h
 * @brief	Explanation of function and data which are used in TDI_System.h<br>
 */
#endif // DOXYGEN_LANG_KOREAN



////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @brief	System 함수 결과에 대한 열거형<br>
 */
#else
/**
 * @brief	Enumeration of the System function result.
 */
#endif // DOXYGEN_LANG_KOREAN
typedef enum
{
	TDI_SYSTEM_RESULT_OK = 0,
	TDI_SYSTEM_RESULT_FAIL
} TDI_SYSTEM_RESULT;



////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @brief	System infomation 대한 열거형<br>
 */
#else
/**
 * @brief	Enumeration of the System infomation
 */
#endif // DOXYGEN_LANG_KOREAN
typedef enum
{
	TDI_SYSTEM_SN = 0
} TDI_SYSTEM_INFO;



#ifdef __cplusplus
extern "C"{
#endif
////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @brief	타이머 콜백 함수 정의<br>
 */
#else
/**
 * @brief	Definition of the timer call back function
 */
#endif // DOXYGEN_LANG_KOREAN
typedef void (*TDI_Timer_cb)(void* id);



////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @brief	porting glue를 초기화 한다.<br>
 *
 * 시스템 자원 튜너, Dmux, 디코더, 그래픽 등을 사용 가능한 상태로 초기화 한다.<br>
 *
 * @return
 * 	TDI_SYSTEM_RESULT 정의된 열거 값 중 하나가 반환.<br>
 */
#else
/**
 * @brief	Initialization of the porting glue.
 *
 * Enumeration of the system function result
 *
 * @return
 * 	TDI_SYSTEM_RESULT Returning one of the enumerating results
 */
#endif // DOXYGEN_LANG_KOREAN
TDI_SYSTEM_RESULT TDI_Open_System(void);



////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @brief	porting glue 사용을 중지 한다.<br>
 *
 * 시스템 자원  튜너, Dmux, 디코더, 그래픽 등의 사용을 중지 한다.<br>
 * porting glue 초기화할때 사용한 리소스를 반환한다.<br>
 *
 * @return
 * 	TDI_SYSTEM_RESULT 정의된 열거 값 중 하나가 반환.<br>
 */
#else
/**
 * @brief	Stop using the porting glue.
 *
 * Stop using the system resource tuner, Dmux, decoder, and, graphic.<br>
 * When porting glue is initialized, return the used resource
 *
 * @return
 * 	TDI_SYSTEM_RESULT Returning one of the defined enumerating results.
 */
#endif // DOXYGEN_LANG_KOREAN
TDI_SYSTEM_RESULT TDI_Close_System(void);



////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * 	@brief	타이머 설정<br>
 *
 * 설정된 시간후 콜백 함수를 호출한다.<br>
 * 콜백 함수 호출후 사용된 자원을 반환 한다.<br>
 * 타이머는 현재 실행중인 코드에 대해 비동기적으로 실행되므로  thread-safety를 보장해야 한다.<br>
 * 시스템의 성능에 미치는 영향이 크므로 하드웨어에서 제공하는 타이머 사용을  권장한다.<br>
 *
 * @param [IN] id 타이머 아이디 사용자 정의 임의값
 * @param [IN] period 타이머의 주기(1/100 sec)
 * @param [IN] func 콜백합수
 *
 * @return
 * 	TDI_SYSTEM_RESULT 정의된 열거 값 중 하나가 반환.<br>
 */
#else
/**
 * @brief	Setting the timer
 *
 * After set the time, Call the callback function.
 * After calling the callback function, return the used resource .<br>
 * Because timer is realized asynchronously for the code which is under realization currently, thread-safety must be assured.<br>
 * Because the timer has a big impact on system performance, recommend the timer provided by hardware.<br>
 *
 * @param [IN] id Random result defined by the timer ID user’s
 * @param [IN] period Cycle of the timer(1/100 sec)
 * @param [IN] func Callback function
 *
 * @return
 * 	TDI_SYSTEM_RESULT Returning one of the defined enumerating results.
 */
#endif // DOXYGEN_LANG_KOREAN
TDI_SYSTEM_RESULT TDI_Set_Timer(void* id, uint32_t period, TDI_Timer_cb func);

/**
 *  @brief 타이머 삭제
 *
 *  이미 실행 중인 타이머를 취소 한다.
 *  TDI_Set_Timer를 호출 하지 않은 Timer를 취소할 경우 에러를 리턴한다.
 *
 *  @param [IN] id 타이머 아이디 사용자 정의 임의값
 *
 *  @return
 * 	TDI_SYSTEM_RESULT 정의된 열거 값 중 하나가 반환.
 */

TDI_SYSTEM_RESULT TDI_Kill_Timer(void* id);

////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @brief	porting glue의 버전을 반환 한다,
 *
 * @param [OUT] version major
 * @param [OUT] version minor
 * @param [OUT] version micro
 * @param [OUT] version revision
 *
 * @return
 * 	TDI_SYSTEM_RESULT 정의된 열거 값 중 하나가 반환.<br>
 */
#else
/**
 * @brief	Returning the porting glue version
 *
 * @param [OUT] version major
 * @param [OUT] version minor
 * @param [OUT] version micro
 * @param [OUT] version revision
 *
 * @return
 * 	TDI_SYSTEM_RESULT Returning one of the defined enumerating results.
 */
#endif // DOXYGEN_LANG_KOREAN
TDI_SYSTEM_RESULT TDI_Get_Version(uint32_t *major, uint32_t *minor, uint32_t *micro, uint32_t *revision);

//////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @brief	system 정보를  반환 한다,
 *
 * @param [IN] 조회 대상 key
 * @param [OUT] system infomation value
 *
 * @return
 * 	TDI_SYSTEM_RESULT 정의된 열거 값 중 하나가 반환.<br>
 */
#else
/**
 * @brief	Returning the porting glue version
 *
 * @param [OUT] key
 * @param [OUT] system infomation value
 *
 * @return
 * 	TDI_SYSTEM_RESULT Returning one of the defined enumerating results.
 */
#endif // DOXYGEN_LANG_KOREAN
TDI_SYSTEM_RESULT TDI_Get_Sys_Info(TDI_SYSTEM_INFO info, char *value);

////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif /* TDI_SYSTEM_H_ */



