
/***************************************************************************************
* NAME: TVS_DVBSI_Interface.h
*---------------------------------------------------------------------------------------
* Copyright 2007 TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
* TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
*---------------------------------------------------------------------------------------
* CREATED_BY: TVSTORM
* CREATION_DATE: 2007/11/23
* $Author: skkim73 $
* $Revision: 3289 $
* $Date: 2010-10-14 12:16:17 +0900 (Thu, 14 Oct 2010) $
*---------------------------------------------------------------------------------------
* PURPOSE:
* - Define DVB SI Interface
****************************************************************************************/

#ifndef __TVS_DVBSI_INTERFACE_H__
#define __TVS_DVBSI_INTERFACE_H__
/////////////////////////////////////////////////////////////////////////////////////////


/*_________ I N C L U D E S ___________________________________________________________*/

#include "TVI_types.h"


/*_________ D E F I N I T I O N _______________________________________________________*/



//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------------- ES types
// Elementary stream types
// ISO_IEC_13818-1 > Table 2-29. Stream type assignments
// + MPEG4 stream types added

#define ESStreamType_MPEG1Video                         0x01
#define ESStreamType_MPEG2Video                         0x02
#define ESStreamType_MPEG1Audio                         0x03
#define ESStreamType_MPEG2Audio                         0x04
#define ESStreamType_MPEG2PrivateSection                0x05
#define ESStreamType_DVBAC3Audio                        0x06 // MPEG2PESPrivateData
#define ESStreamType_MHEG                               0x07
#define ESStreamType_AnnexADSMCC                        0x08
#define ESStreamType_H222_1                             0x09
#define ESStreamType_DSMCCTypeA                         0x0A
#define ESStreamType_DSMCCTypeB                         0x0B
#define ESStreamType_DSMCCTypeC                         0x0C
#define ESStreamType_DSMCCTypeD                         0x0D
#define ESStreamType_MPEG2Auxiliary                     0x0E
#define ESStreamType_AACAudio                           0x0F
#define ESStreamType_MPEG4Video                         0x10
#define ESStreamType_H264Video                          0x1B
#define ESStreamType_DC2Video                           0x80
#define ESStreamType_AC3Audio                           0x81

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------------- Result Code
// Common result codes

typedef enum _TDSResult
{
	TDS_OK = 0, 		/* No error occured. */
	TDS_FAILURE, 		/* A general or unknown error occured. */
	TDS_INVARG, 		/* An invalid argument has been specified. */
	TDS_ACCESSDENIED, 	/* Access to the resource is denied. */
	TDS_NORESOURCE,		/* There's no available resource */
	TDS_INFO_EMPTY,		/* There's no available data */
	TDS_INFO_NOT_READY	/* There's no available data because SI gathering is not finished yet */
} TDSResult;

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------- Monitoring Result Code
typedef enum _TDSMonitorResult {
	TDS_DVBSI_MONITOR_OK = 0,
	TDS_DVBSI_MONITOR_ERROR_THREAD_STALL,
	TDS_DVBSI_MONITOR_ERROR_DB_CORRUPTION
} TDSMonitorResult;

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------------- Event Types
// Event types

typedef enum _TDSEventType {
	TDS_DVBSI_ACQUIRE_STARTED,		//DVB-SI parsing start
	TDS_DVBSI_ACQUIRE_STOPED,		//DVB-SI parsing stop
	TDS_DVBSI_CH_INFO_COMPLETED,	//DVB-SI psrsing and gathering of CH info is completed
	TDS_DVBSI_CH_INFO_UPDATED,		//DVB-SI updated(Channel info is changed)
	TDS_DVBSI_EVENT_PF_INFO_COMPLETED,	//DVB-SI present & following Event info is completed
	TDS_DVBSI_EVENT_PF_INFO_UPDATED,	//DVB-SI present & following Event info is updated
	TDS_DVBSI_EVENT_INFO_COMPLETED,	//DVB-SI parsing and gathering of Event info is completed
	TDS_DVBSI_EVENT_INFO_UPDATED,	//DVB-SI updated(Event info is changed)
	TDS_DVBSI_CUR_SERVICE_EVENT_INFO_COMPLETED,	//Getting event info of current service channel is completed
	TDS_DVBSI_TIME_INFO_UPDATED,
	TDS_DVBSI_CUR_SERVICE_EVENT_CHANGED, //current service event is changed by time.
	TDS_DVBSI_LOCAL_AREACODE_CHANGED, //local area code is changed by external application

	TDS_DVBSI_EVENT_LAST
} TDSEventType ;




//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------- Time

typedef struct _TDSTime {
	TVIUint16	year;					// year
	TVIUint8	month;					// 1~12
	TVIUint8	day;					// 1~31
	TVIUint8	hour;					// 0~23
	TVIUint8	min;					// 0~59
	TVIUint8	sec;					// 0~59
	TVIUint8	week_day;				// 0: SUN, 1: MON, 2: TUE ....
} TDSTime;


//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------- Product Info
// Product list descriptor info

typedef struct _TDSProductInfo {
	TVIUint8	product_type;
	TVIUint8*	id_product_ptr;
	TVIUint8	id_product_len;
	TVIUint8*	nm_product_ptr;
	TVIUint8	nm_product_len;
	TVIUint8* 	desc_product_ptr;
	TVIUint8	desc_product_len;
	TVIUint32	amt_price;
	TVIUint32	amt_dc_incr;
	TVIUint8 	FG_term;
	TVIUint8	FG_value;

	TDSTime		dt_product_start;
	TDSTime		dt_product_end;
} TDSProductInfo;


//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ ES Info
// Elementary stream info

typedef struct _TDSESInfo {
	TVIUint8	stream_type;		// Stream Type;
	TVIUint16	pid;				// PID
} TDSESInfo;


//--------------------------------------------------------------------------------------
//------------------------------------------------------------------- EMM Multicast Info
// EMM multicast info

typedef struct _TDSEMMInfo {
	TVIUint16	CA_system_ID;		// CAS system ID;
	TVIUint32	EMM_multicast_ip;	// EMM multicast TS IP address
	TVIUint16	EMM_multicast_port;	// EMM multicast TS port number
} TDSEMMInfo;


//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ CA Info
// CA info

typedef struct _TDSCAInfo {
	TVIUint16	CA_system_ID;		// CAS system ID;
	TVIUint16	CA_PID;				// ECM PID
} TDSCAInfo;


//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- ID Product Info
// ID product info

typedef struct _TDSIDProduct {
	TVIUint8*	id_product_ptr;
	TVIUint8	id_product_len;
} TDSIDProduct;


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Channel DB
// Channel DB

typedef struct _TDSChannelDB {
	TVIUint32	service_id;				// a service ID
	TVIUint8	service_type;			// a service type 0x00:RFU, 0x01:dtv, 0x02:digital radio, 0x80: audio channel service
	TVIUint8*	service_name_ptr;		// a pointer of service name text as DVB SI Spec. and null character is not included.
	TVIUint32	service_name_length;	// a service name length
	TVIUint16	channel_number;			// a channel numner
	TVIUint32	IP_address;   			// a Multicast IP address
	TVIUint16	TS_port_number;			// a Multicast IP port number

	TVIUint8	id_product_list_count;
	TDSIDProduct* id_product_list_ptr;

	TVIUint8	genre_code;				// a Genre code
	TVIUint8	category_code;			// a Catagory code
	TVIUint8	local_area_code;		// a Local Area Code
	TVIUint8	running_status; 		// a Running Status
	TVIUint8	rating;					// 0: not rated,  rating>0: over rating age ( if rating=3,then over 3 age )
	TVIUint8	pay_channel;			// 0: no pay channel, 1: pay channel
	TVIUint32	pay_ch_sample_sec;		// a display sampling time of pay channel

	TVIUint8	resolution;				// 0: SD   1: HD
	TVIUint16	pcr_pid;				// PCR PID
	TVIUint8	element_stream_count;	// a number of element stream
	TDSESInfo*	pES;					// a start pointer of elementary stream type array

	TVIUint8	CA_count;				// a number of CA information count
	TDSCAInfo*	pCA_Info;				// a start pointer of CA info. array

	TVIUint8	channel_image_text_length;
	TVIUint8*	channel_image_text_ptr;

} TDSChannelDB;



//--------------------------------------------------------------------------------------
//------------------------------------------------------------------ Program Linked Info
// Program linked descriptor info

typedef struct _TDSProgramLinkedInfo {
	TVIUint8	linked_service_flag;
	TVIUint8*	linked_service_text_ptr;
	TVIUint8	linked_service_text_len;
	TVIUint8	button_type;
	TVIUint32	c_menu;
	TVIUint8*	button_image_filename_ptr;
	TVIUint8	button_image_filename_len;
	TDSTime		display_start_time;
	TDSTime		display_end_time;

	TVIUint8	vas_itemid_len;
	TVIUint8*	vas_itemid_ptr;
	TVIUint8	vas_path_len;
	TVIUint8*	vas_path_ptr;
	TVIUint8	vas_serviceid_len;
	TVIUint8*	vas_serviceid_ptr;
} TDSProgramLinkedInfo;


//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------------- Event DB
// Event DB

typedef struct _TDSEventDB {
	TVIUint32	service_id;				// a service ID
	TVIUint16	event_id;				// a event ID

	TVIUint8*	event_name_ptr;			// event name or audio title name in audio channel (refer service_type)
	TVIUint32	event_name_length;

	TVIUint8*	event_description_ptr;	// event description or singer name in audio channel (refer service_type)
	TVIUint32	event_description_length;

	TVIUint8*	image_URL_ptr;			// URL address
	TVIUint32	image_URL_length;

	TVIUint8*	director_name_ptr;		// director name
	TVIUint32	director_name_length;

	TVIUint8*	actors_name_ptr;		// actor names
	TVIUint32	actors_name_length;

	TVIUint8*	price_text_ptr;			// price
	TVIUint32	price_text_length;

	TVIUint8	dolby;					// 0: no info, 'N': not dolby, 'D': dolby support
	TVIUint8	audio;				 	// 0: no info, 'M': mono, 'S': stereo, 'A': AC-3
	TVIUint8	resolution;				// 0: no info, 'S': SD, 'H': HD

	TVIUint8	free_CA_mode;
	TVIUint8	running_status;

	TVIUint8	rating;					// 0: not rated,  rating>0: over rating age ( if rating=3,then over 3 age )
	TVIUint8	content_nibble_level_1;
	TVIUint8	content_nibble_level_2;
	TVIUint8	user_nibble_1;
	TVIUint8	user_nibble_2;

	TDSTime		start_time;
	TDSTime		end_time;
	TVIUint32	duration;				//seconds

	TVIUint32	program_linked_info_cnt;
	TDSProgramLinkedInfo*  program_linked_array_ptr;

	TVIUint8	caption;				//	caption : Y/N
	TVIUint8	dvs;					//	explanation: Y/N

} TDSEventDB;

typedef struct _TDSBulkEventDB {
	TVIUint16	channel_number;
	TVIUint16	event_count;
	TDSEventDB*	pEventDB;
} TDSBulkEventDB;

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------------- Local_area_code_info
// Local_area_code_info
#define common_area_code 0x00
typedef struct _TDSLocal_area_code_info
{
	TVIUint8	m_local_area_code_1;	// 1st local area code
	TVIUint8	m_local_area_code_2;	// 2nd local area code
	TVIUint8	m_local_area_code_3;	// 3nd local area code
} TDSLocal_area_code_info;

enum
{
	DVBSI_USE_INTERNAL_SI_RECEIVER	= (1 << 0),
	DVBSI_SAVE_SI_DATA				= (1 << 1),
	DVBSI_USE_FILE_SOURCE			= (1 << 2),
};

typedef struct
{
	TVIUint32 flags;
	TVIUint32 ip;		//	si multicast ip.
	TVIUint32 port;		//	si multicast port.
	const char * si_save_file;
	const char * si_source_file;
}
TDSDvbSiInitParam;

/**
 * dvbsi library status container.
 */
typedef struct _TDSDvbsiStatus
{
    const char * version;

	TVIBool 	active;

	TDSDvbSiInitParam init_param;

	TDSLocal_area_code_info area_code;

	TVIUint32 	current_channel_number;
	TVIUint32 	current_service_id;
	TVIBool 	channel_change_by_zapping_flag;

	TVIBool 	channel_completed;
	TDSTime 	channel_complete_time;
	TDSTime 	channel_update_time;
	TVIUint32	channel_count;

	TVIBool 	event_completed;
	TDSTime 	event_complete_time;
	TDSTime 	event_update_time;

	TDSTime		last_event_call_times[TDS_DVBSI_EVENT_LAST];
	TDSTime		last_feed_time;

} TDSDvbSiStatus;

/*_________ D E C L A R A T I O N _____________________________________________________*/



#ifdef __cplusplus
extern "C" {
#endif
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
//
//	!!! NOTE ~!!!
//	FOLLOWING TDS INTERFACEs ARE FOR GET DVB-SI DATA and SERVE CH&EPG INFO.
//	IT's implemented by TVSTORM.
//	PURPOSE : to GET DVB-SI DATA and SERVE A CUSTOMER CHANNEL & EPG INFORMATION.
//	--------------------------------------------------------------------------------------
//	A CUSTOMER CAN GET THE CH&EPG INFO BY USING BELOWs APIs IN DVB-SI TABLEs that streaming
//	through OOB-IP.
//
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------- Macro
// macro for an interface declaration


#define TDS_DECLARE_INTERFACE( ifaceName )	typedef struct _##ifaceName ifaceName;

#define TDS_DEFINE_INTERFACE( ifaceName, ... )	\
	struct _##ifaceName									\
	{													\
		void 	*priv;	/* private data */				\
		__VA_ARGS__		/* any field/function member */	\
	};

TDS_DECLARE_INTERFACE( DVBSIControl )


//--------------------------------------------------------------------------------------
//------------------------------------------------------------------ Event callback Func
// TDS Event callback function


typedef void (*TDSEventCallback) (
	DVBSIControl*		pDvbSiControl,
	TDSEventType		eventType,
	TVIUint32*			pParam,
	void* 				userParam
);



/***************************************************************************************
 *
 * TDS(TVSTORM DVB-SI) functions
 *
 ***************************************************************************************/


/**
 * This function return a version of DVB SI API.
 * ex) return value 0x0312 means version 3.12 as BCD codes
 */
const char * TDS_GetDvbSiApiVersion(void);


/**
 * returns dvbsi library status.
 *
 *	@param TDSDvbSiStatus * status_buf
 *			pointer to receive dvbsi library status.
 *
 * 	@return return TDS_FAILURE, if status_buf is not NULL.
 */
TDSResult TDS_GetDvbSiStatus(TDSDvbSiStatus * status_buf);


/**
 *	dump status to given file pointer using read/write.
 *	use STDOUT_FILENO/STDERR_FILENO
 */
void TDS_DumpDvbsiStatus(int fd, const TDSDvbSiStatus * status);


/**
 * This function creates and initializes an TDSFace object, and store the address
 * of the created DVBSIControl object in the 'ppDvbSiControl' parameter.
 * This function shall be called only once. Thus, if this function is called more
 * than once, its behavior is undefined.
 *
 * @param	ppDvbSiControl
 * @return 	TDS_OK, 	if no error.
 * 			TDS_INVARG, if ppFace is NULL.
 * 			TDS_FAILURE, or other error code, if failed for other reason.
 */
TDSResult TDS_DvbSiInitialize( DVBSIControl** ppDvbSiControl, TVIUint32 IP_address, TVIUint16 port_number);

/**
 * This function create DVBSIControl control with initialization parameter.
 *
 * @param TDS_DvbSiInitParam * pInitParam	specify parameters, how to initialize dvbsi control
 */
TDSResult TDS_DvbSiInitializeEx( DVBSIControl** ppDvbSiControl, TDSDvbSiInitParam * pInitParam );

/**
 * This function cleans up all the resources allocated to the DVBSIControl object pointed by pDvbSiControl.
 *
 * @param	pDvbSiControl
 * @return 	TDS_OK, 	if no error.
 * 			TDS_INVARG, if ppFace is NULL.
 * 			TDS_FAILURE, or other error code, if failed for other reason.
 */
TDSResult TDS_DvbSiCleanUp( DVBSIControl* pDvbSiControl );



/***************************************************************************************
 *
 * DVBSIControl
 *
 ***************************************************************************************/



TDS_DEFINE_INTERFACE( 	DVBSIControl,

	/**
	 * This function registers EventCallback function which is to be called
	 * whenever DVB SI event is acqured.
	 * The currently registered callback function, if any, will be overrided by the given callback function.
	 */

	TDSResult (*setEventCallback) (
		DVBSIControl* 		thiz,
		TDSEventCallback	callback,
		void*				userParam
	);


	/**
	 * This function make to start process for DVB SI.
	 */

	TDSResult (*siProcessStart) (
		DVBSIControl* 		thiz
	);


	/**
	 * This function make to stop process for DVB SI.
	 */


	TDSResult (*siProcessStop) (
		DVBSIControl* 		thiz
	);


	/**
	 * This function send some ts data-let to DVB SI processing part.
	 */

	TDSResult (*feedingData) (
		DVBSIControl* 		thiz,
		TVIUint8*			pData,
		TVIUint32			size
	);


	/**
	 * This function gets EMM multicast informations which acquired from DVB SI.
	 * Allocated memory of *ppEMMInfo will be cleared after function call
	 */

	TDSResult (*getEMMInfoList) (
		DVBSIControl* 		thiz,
		TDSEMMInfo**		ppEMMInfo,		// the pointer of start address for EMM info. array is field up
		TVIUint32*			pCount			// total channel number is field up
	);

	/**
	 * This function gets product list informations which acquired from DVB SI.
	 * Allocated memory of *ppProductInfo will be cleared after function call
	 */

	TDSResult (*getProductList) (
		DVBSIControl* 		thiz,
		TDSProductInfo**	ppProductInfo,	// the pointer of start address for product info. array is field up
		TVIUint32*			pCount			// total channel number is field up
	);


	/**
	 * This function gets Channel DB which acquired from DVB SI.
	 * Allocated memory of *ppChDB will be cleared after function call
	 * and channel db must be coppied in API caller !!!
	 */

	TDSResult (*getChDB) (
		DVBSIControl* 		thiz,
		TDSChannelDB**		ppChDB,		// the pointer of start address for channel DB is field up
		TVIUint32*			pCount		// total channel number is field up
	);


	/**
	 * This function sets current service channel (newly tuned channel)
	 * for aquiring DVB SI event information about current service (channel).
	 * If you would not set current service, then It takes more time to take DVB SI events.
	 */

	TDSResult (*setCurrentService) (
		DVBSIControl* 		thiz,
		TVIUint32			channel_number
	);


	/**
	 * This function gets event informations for specific service (channel).
	 * Allocated memory of *ppEventDB will be cleared after function call
	 * and event db must be coppied in API caller !!!
	 */

	TDSResult (*getEventDB) (
		DVBSIControl* 		thiz,
		TVIUint32			channel_number,	// channel number of service (channel) for getting event DB.
		TDSEventDB**		ppEventDB,		// the pointer of start address for event DB is field up
		TVIUint32*			pCount			// total event number is field up
	);

	/**
	 * This function gets current(On-Air) event informations for specific service (channel).
	 * Allocated memory of *ppEventDB will be cleared after function call
	 * and event db must be coppied in API caller !!!
	 */

	TDSResult (*getCurrentEventDB) (
		DVBSIControl* 		thiz,
		TVIUint32			channel_number,	// channel number of service (channel) for getting event DB.
		TDSEventDB**		ppEventDB		// the pointer of start address for event DB is field up
	);

	/**
	 * This function gets n-th event informations for specific service (channel).
	 * Allocated memory of *ppEventDB will be cleared after function call
	 * and event db must be coppied in API caller !!!
	 */

	TDSResult (*getMiniEpgEventDB) (
		DVBSIControl* 		thiz,
		TVIUint32			channel_number,		// channel number of service (channel) for getting event DB.
		TVIInt8				get_event_mode,		// 0: first program, -1: prev program, 1:next program, other: not available
		TDSEventDB**		ppEventDB,			// the pointer of start address for event DB is field up
		TVIUint8*			pPrevProgramPresent,// 0: this program is first program, 1: next program is present
		TVIUint8*			pNextProgramPresent	// 0: this program is last program, 1: next program is present
	);

	/**
	 * This function gets current Time. 	// get DVB TDT time
	 * if return value is TDS_OK, it is valid.
	 */

	TDSResult (*getCurrentTime) (
		TDSTime*			pTime			// current time is field up
	);

	/**
	 * This function set local area code.
	 * if return value is TDS_OK, it is success.
	 */

	TDSResult (*setLocalAreaCode) (
		TDSLocal_area_code_info* pLocal
	);

	/**
	*This function set Time.
	 * if return value is TDS_OK, it is success.
	 */

	TDSResult (*setDVBSICurrentTime)(
		TVIUint32 CurrentTime
	);

	/**
	*This function get Time.
	 * if return value is TDS_OK, it is success.
	 */

	TDSResult (*getDVBSICurrentTime)(
		TVIUint32 *pCurrentTime
	);

	/**
	*This function get partial eit info. 
	 * if return value is TDS_OK, it is success.
	 */
    TDSResult (*getDVBSIPartialEventDB)(
		DVBSIControl* 		thiz,
        TVIUint32 channel, 
        TVIUint16 startIdx, 
        TVIUint16 requestCnt, 
        TDSEventDB** ppEventDB, 
        TVIUint16* returnCnt, 
        TVIUint16* totalCnt
    );

	/**
	*This function get DVBSI running status.
	 * if return value is TDS_OK, it is success.
	 */
    TDSResult (*getDVBSIRunningStatus)(
    	TDSMonitorResult * pStatus
    );

	/**
	*This function get event DB for several channel.
	 * if return value is TDS_OK, it is success.
	 */
    TDSResult (*getBulkEventDB)(
    		DVBSIControl* 		thiz,
    		TVIUint8*			pChannelList,		// channel number list
    		TDSBulkEventDB**	ppBulkEventDB
    );

	/**
	*This function get event DB for several channel.
	 * if return value is TDS_OK, it is success.
	 */
    TDSResult (*getBulkEventDBByTime)(
    		DVBSIControl* 		thiz,
    		TVIUint8*			pChannelList,		// channel number list
    		TDSBulkEventDB**	ppBulkEventDB,
    		TVIUint32			startTime,
    		TVIUint32			endTime
    );
)


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
//
//	!!! NOTE ~!!!
//	FOLLOWING SECTION FILTER RELATED FUNCTIONs ARE BASED ON H/W SECTION FILTERing.
//	IT's NOT implemented by TVSTORM.
//	PURPOSE : to get DVB-SI section through the HW section filter.
//	--------------------------------------------------------------------------------------
//	TVSTORM DVB-SI MODULE will using it to get DVB-SI sections in order to make CHANNEL &
//	EPG DB.
//
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////


/***************************************************************************************
 *
 * TDS Section filter defines
 *
 ***************************************************************************************/
typedef TVIUint32 TDSSectionFilterID;
#define TDS_NULL_SFID  ((TDSSectionFilterID) 0xFFffFFff)

#define TDS_MAX_FILTER_MASK_LEN		12
typedef struct _TDSSectionFilterParam
{
	TVIUint16	pid;
	TVIUint8 	mask[TDS_MAX_FILTER_MASK_LEN];		// mask
	TVIUint8 	mode[TDS_MAX_FILTER_MASK_LEN];		// mode - positive, negative
	TVIUint8 	comp[TDS_MAX_FILTER_MASK_LEN];		// bits for comparison
} TDSSectionFilterParam;

/*
 * Called whenever section filter gets filtering result.
 */
typedef void (*TDSSectionFilterCallback) (
	TDSResult 			result, 	// result code of the filtering.
	TVIUint8			buf[],		// buffer to contain section data. It must contain the WHOLE data of just ONE section.
	TVIUint32 			len, 		// the number of section data bytes in the buf. It shall not exeed 4K bytes according to the standards.
	TDSSectionFilterID 	filterID,	// the ID of the filter which made this result.
	void* 				userparam	// user parameter passed when startFiltering(..) was called.
	);

/***************************************************************************************
 *
 * TDS Section filter functions
 *
 ***************************************************************************************/
/*
   * Reserve a section filter and stores the section filter ID in the location
   * pointed by 'pSectionFilterID'.
   *
   * @return  TDS_OK,			  if no error.
   *		  TDS_INVARG,		  if 'pSectionFilterID' is invalid.
   *		  TDS_NORESOURCE, if there's no more available filter.
   *		  TDS_FAILURE, or other error code, if failed for other reason.
   */
TDSResult TDS_SF_reserve(TDSSectionFilterID* pSectionFilterID);

/*
  * Release the filter associated with the sectionFilterID.
  *
  * @return  TDS_OK,				 if no error.
  * 		 TDS_INVARG,			 if 'sectionFilterID' is invalid.
  * 		 TDS_ACCESSDENIED,	 if the given TVISectionFilterControl is not the owner of the the filter.
  * 		 TDS_FAILURE, or other error code, if failed for other reason.
  */
TDSResult TDS_SF_release(TDSSectionFilterID SectionfilterID);


/*
   * Start section filtering with the given filtering parameters.
   *
   * @return  TDS_OK,				  if no error.
   *		  TDS_INVARG,			  if 'sectionFilterID' is invalid.
   *		  TDS_ACCESSDENIED,   if the given TVISectionFilterControl is not the owner of the the filter.
   *		  TDS_FAILURE, or other error code, if failed for other reason.
   */
TDSResult TDS_SF_startFiltering(TDSSectionFilterID sectionFilterID,
								TDSSectionFilterParam* pParam,
                           		TDSSectionFilterCallback callbackFn,
                           		void*    userParam);

/*
  * Stop filtering of the section filter referred by sectionFilterID.
  * If the sectionFilter specified by sectionFilterID has not started, TVI_FAILURE shall be returned.
  *
  * @return  TDS_OK,				 if no error.
  * 		 TDS_INVARG,			 if 'sectionFilterID' is invalid.
  * 		 TDS_ACCESSDENIED,		 if the given TVISectionFilterControl is not the owner of the the filter.
  * 		 TDS_FAILURE, or other error code, if failed for other reason.
  */
TDSResult TDS_SF_stopFiltering(TDSSectionFilterID SectionFilterID);

/*
  * update Section filter Parameters of the section filter referred by sectionFilterID.
  *
  * @return  TDS_OK,				 if no error.
  * 		 TDS_INVARG,			 if 'sectionFilterID' is invalid.
  * 		 TDS_ACCESSDENIED,		 if the given TVISectionFilterControl is not the owner of the the filter.
  * 		 TDS_FAILURE, or other error code, if failed for other reason.
  */
TDSResult TDS_SF_updateFiltering(TDSSectionFilterID SectionFilterID,TDSSectionFilterParam* pParam);


extern const char * DOUBLE_LINE;
extern const char * SINGLE_LINE;

/**
 *	time formatter
 *
 *	@param char * buffer 			buffer to formated text
 *	@param const TDSTime * time 	time value to format.
 *	@return pointer of buffer
 */
char * TDS_sprintTime(char * buffer, const TDSTime * time);

/**
 * return day of week string
 */
const char * TDS_getDayOfWeekString(int week_day);

/////////////////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////
#endif /* __TVS_DVBSI_INTERFACE_H__ */


