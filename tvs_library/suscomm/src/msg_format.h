/*
 * msg_format.h
 *
 *  Created on: Jan 28, 2015
 *      Author: jhseo
 */

#ifndef MSG_FORMAT_H_
#define MSG_FORMAT_H_

#include <stdio.h>
#include <string.h>

#define		MAX_FW_INFO_NUM					50

#define		CMD_REQUEST__CHECK				"CHECK"
#define		CMD_REQUEST__REPORT				"REPORT"

///////////////////////////////////////////////////////
// ELEMENT FOR CHECK REQUEST MESSAGE
///////////////////////////////////////////////////////
#define 	MSG_ELEM_N__CMD					"cmd"
#define		MSG_ELEM_A_DATA					"data"
#define		MSG_ELEM_N__MODEL				"model"
#define		MSG_ELEM_N__MAC					"mac"
#define		MSG_ELEM_N__SERIAL_NUMBER		"sn"
#define		MSG_ELEM_N__FULL_VERSION		"full"
#define		MSG_ELEM_N__PARTIAL_VERSION		"partial"
#define		MSG_ELEM_N__FIRMWARE_VERSION	"fw"

///////////////////////////////////////////////////////
// ELEMENT FOR CHECK RESPONSE MESSAGE
///////////////////////////////////////////////////////
#define 	MSG_ELEM_N_CODE					"code"
#define		MSG_ELEM_N_MESSAGE				"mesg"
#define		MSG_ELEM_N__DATA				"data"
#define		MSG_ELEM_A_UPGRADE_INFO			"upgrade"
#define		MSG_ELEM_N_FILE_MD5				"file_md5"
#define		MSG_ELEM_N_FILE_NAME			"file_name"
#define		MSG_ELEM_N_FILE_PATH			"file_path"
#define		MSG_ELEM_N_FILE_SIZE			"file_size"
#define		MSG_ELEM_N_FILE_URL				"file_url"
#define     MSG_ELEM_N_GROUP_ID             "group_id"
#define		MSG_ELEM_N__FULL_VERSION		"full"
#define		MSG_ELEM_N__PARTIAL_VERSION		"partial"
#define		MSG_ELEM_N_MODE					"mode"
#define		MSG_ELEM_N_SW_GUIDE				"sw_guide"
#define		MSG_ELEM_N_SW_ID				"sw_id"
#define		MSG_ELEM_N_SW_TYPE				"sw_type"
#define		MSG_ELEM_N_VERSION				"version"
#define		MSG_ELEM_N__STATUS				"status"

#define		REPORT_STS__START				"START";
#define		REPORT_STS__OK					"OK";
#define		REPORT_STS__ERROR				"ERROR";

typedef struct {
    char * pMd5;
    char * pFileName;
    char * pFilePath;
    char * pFileSize;
    char * pFileUrl;
    char * pFullVer;
    char * pPartialVer;
    char * pMode;
    char * pSwGuide;
    char * pSwId;
    char * pSwType;
    char * pFwVersion;
} FW_INFO_T;

typedef struct {
    char * pFullVer;
    char * pPartialVer;
    char * pFileUrl;
    int status;
} REPORT_INFO_T;

typedef struct {
    char * pCode;
    char * pMesg;
    char * pGroupId;
    FW_INFO_T fwInfoList[MAX_FW_INFO_NUM];
    int fwInfoNum;
} UPGRADE_INFO_T;

typedef enum {
    REPORT_TYPE__START, REPORT_TYPE__OK, REPORT_TYPE__ERR
} REPORT_TYPE_T;
#endif /* MSG_FORMAT_H_ */
