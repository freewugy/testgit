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

#ifndef __TERROR_H__
#define __TERROR_H__


/*	____________________________________________
 *		COMMON RESULT CODEs
 *	--------------------------------------------	*/
typedef enum 
{
	TOK = 0x0L, 		/* No error occured. */

	/*	________________________________
	 * 		TST global error codes
	 *	--------------------------------	*/
	
	TERR_FAILURE, 			/* 0x01 - A general or unknown error occured. */
	TERR_INVARG, 			/* 0x02 - An invalid argument has been specified. */
	TERR_INVHANDLE,			/* 0x03 - used wrong handle*/
	TERR_INVPARAM,			/* 0x04 - invalid API input arqument use*/
	TERR_NOTSUPPORTED, 		/* 0x05 - The requested operation or an argument is (currently) not supported. */
	TERR_NOTIMPLEMENTED, 	/* 0x06 - The requested operation is not implemented, yet. */
	TERR_NOMEMORY,			/* 0x07 - There's not enough memory. */
	TERR_FILENOTFOUND, 		/* 0x08 - The specified file has not been found. */
	TERR_IO, 				/* 0x09 - A general I/O error occured. */
	TERR_WRONGFORMAT, 		/* 0x0A - The given media does not have the expected format. */
	TERR_ACCESSDENIED, 		/* 0x0B - Access to the resource is denied. */
	TERR_NORESOURCE,			/* 0x0C - There's no available resource */
	TERR_BUSY, 				/* 0x0D - The resource or device is busy. */
	TERR_TIMEOUT, 			/* 0x0E - The operation timed out. */
	TERR_INTERRUPTED, 		/* 0x0F - The operation has been interrupted. */
	TERR_OBSOLETE,			// obsolete operation
	/*	TST global common error values to be instered here -------------------*/
	TERR_SAME_SI_VER,

	/* RESOURCE related error values inserted here ----------------------------*/
	TERR_RES_GENERAL = (0x01<<5),	//(32:0x20) start from 32 (0x20)
	TERR_RES_NOTACCEPT,				//(33:0x21) there is no application ID in the filter list
	TERR_RES_NORESOURCE,			//(34:0x22) there is no available resource
	TERR_RES_NORESOURCE_SCHEDULE,	//(35:0x23) there is no available resource for schedule
	TERR_RES_REQSAMERESOURCE,		//(36:0x24) application request same resource with same usage (got it before)
	TERR_RES_CONFLICT,				//(37:0x25) conflict occurs
	TERR_RES_INVREQUEST,			//(38:0x26) invalid request 
	
	/* Cable Modem error value */
	TERR_RES_CM_OFFLINE,
	
	/**/
	TERR_LAST				/* End of the TST error */

} TRESULT;

/*	____________________________________________
 *		MACROs for TST Error handling 
 *	--------------------------------------------	*/
#define TSUCCESS(e) 	((e) == TOK)	
#define TERROR(e)	((e) != TOK)

/*	____________________________________________
 *		MACROs for CHECK NULL Pointer
 *	--------------------------------------------	*/
#define TCHK_VALID_POINTER( p ) (p==NULL)?TERR_INVARG:TOK

#define kshd()	printf("[KSH] %s %d\n",__FUNCTION__,__LINE__)

#endif	/*__TERROR_H__*/
