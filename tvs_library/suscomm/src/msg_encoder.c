/*
 * sus_encoder.c
 *
 *  Created on: Jan 28, 2015
 *      Author: jhseo
 */

//////////////////////////////////////////////////////////////////////
// MESSAGE ENCODER
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include "json_format.h"
#include "msg_format.h"
#include "sys_info.h"
#include "utils.h"
#include "sus_comm.h"
#include "operation_mgr.h"

#define	MSG_BUF_SIZE		1024
#define EMPTY_VERSION       "0.0.0"

extern REPORT_INFO_T sReportInfo;

char * makeSusCheckMsg(IMG_FILE_INFO_T* pImgInfo)
{
    char *msgBuf = (char *) malloc(sizeof(char) * MSG_BUF_SIZE);
    if ((msgBuf == NULL)
            || (pImgInfo == NULL)){
        Log("Error! Invalid parameter in makeSusCheckMsg\n");
        return NULL;
    }

    memset(msgBuf, 0x00, sizeof(char) * MSG_BUF_SIZE);

    JSON_OBJECT_T* root = jsonObjectNewObject(NULL);

    JSON_OBJECT_T* cmdNode = jsonObjectNewString(MSG_ELEM_N__CMD, CMD_REQUEST__CHECK);

    JSON_OBJECT_T* dataNode = jsonObjectNewObject(MSG_ELEM_A_DATA);

    if (getModelName() == NULL) {
        free(msgBuf);

        Log("Don't find Model name information \n");
        return NULL;
    }

    char *pFwVersion = NULL;
    if (pImgInfo->pFwVersion != NULL) {
        pFwVersion = pImgInfo->pFwVersion;
    } else {
        pFwVersion = strdup("");
    }

    JSON_OBJECT_T* model = jsonObjectNewString(MSG_ELEM_N__MODEL, getModelName());

    /**
     * Verify Mac address
     */
    if(getMacAddress() == NULL) {
        Log("Don't find Mac address information \n");

        free(msgBuf);

        return NULL;
    }

    JSON_OBJECT_T* mac = jsonObjectNewString(MSG_ELEM_N__MAC, getMacAddress());

    /**
     * Verify serial number
     */
    if(getStbSerialNum() == NULL) {
        Log("Don't find Stb serial number information \n");

        free(msgBuf);

        return NULL;
    }

    JSON_OBJECT_T* serial = jsonObjectNewString(MSG_ELEM_N__SERIAL_NUMBER, getStbSerialNum());

    JSON_OBJECT_T* full = NULL;
    JSON_OBJECT_T* partial = NULL;

    JSON_OBJECT_T* fw;
    if (pImgInfo->pktInfo.upgradeMode == UPGRADE_MODE__RECOVERY) {
        full = jsonObjectNewString(MSG_ELEM_N__FULL_VERSION, "0");
        partial = jsonObjectNewString(MSG_ELEM_N__PARTIAL_VERSION, "0");

        fw = jsonObjectNewString(MSG_ELEM_N__FIRMWARE_VERSION, EMPTY_VERSION);
    } else {
        /**
         * Verify sus version information
         */
        if((getSusFullVer() == NULL) || (getSusPartialVer() == NULL)) {
            Log("Don't find sus version information \n");

            free(msgBuf);

            return NULL;
        }

        full = jsonObjectNewString(MSG_ELEM_N__FULL_VERSION, getSusFullVer());
        partial = jsonObjectNewString(MSG_ELEM_N__PARTIAL_VERSION, getSusPartialVer());

        fw = jsonObjectNewString(MSG_ELEM_N__FIRMWARE_VERSION, pFwVersion);
    }

    JSON_OBJECT_T* groupId = jsonObjectNewString(MSG_ELEM_N_GROUP_ID, getGroupId());

    jsonObjectAddObject(dataNode, model);
    jsonObjectAddObject(dataNode, mac);
    jsonObjectAddObject(dataNode, serial);

    jsonObjectAddObject(dataNode, full);
    jsonObjectAddObject(dataNode, partial);
    jsonObjectAddObject(dataNode, fw);
    jsonObjectAddObject(dataNode, groupId);

    jsonObjectAddObject(root, cmdNode);
    jsonObjectAddObject(root, dataNode);

    jsonEncode(msgBuf, root);

    jsonFreeNode(root);

    return msgBuf;
}

char * makeSusReportMsg(REPORT_TYPE_T reportType)
{
    char *msgBuf = (char *) malloc(sizeof(char) * MSG_BUF_SIZE);
    if (msgBuf == NULL) {
        return NULL;
    }
    memset(msgBuf, 0x00, sizeof(char) * MSG_BUF_SIZE);

    char * status = NULL;

    switch (reportType) {
        case REPORT_TYPE__START:
            status = REPORT_STS__START;
            break;
        case REPORT_TYPE__OK:
            status = REPORT_STS__OK;
            break;
        case REPORT_TYPE__ERR:
            status = REPORT_STS__ERROR;
            break;
    }

    JSON_OBJECT_T* root = jsonObjectNewObject(NULL);
    if(root == NULL) {
        free(msgBuf);
        return NULL;
    }

    JSON_OBJECT_T* cmdNode = jsonObjectNewString(MSG_ELEM_N__CMD, CMD_REQUEST__REPORT);

    JSON_OBJECT_T* dataNode = jsonObjectNewObject(MSG_ELEM_A_DATA);
    JSON_OBJECT_T* model = jsonObjectNewString(MSG_ELEM_N__MODEL, getModelName());
    JSON_OBJECT_T* mac = jsonObjectNewString(MSG_ELEM_N__MAC, getMacAddress());
    JSON_OBJECT_T* serial = jsonObjectNewString(MSG_ELEM_N__SERIAL_NUMBER, getStbSerialNum());

    if (sReportInfo.pFullVer == NULL) {
        Log("Error! Full version is NULL\n");
        free(msgBuf);
        return NULL;
    }
    JSON_OBJECT_T* full = jsonObjectNewString(MSG_ELEM_N__FULL_VERSION, sReportInfo.pFullVer);

    if (sReportInfo.pPartialVer == NULL) {
            Log("Error! Partial version is NULL\n");
            free(msgBuf);
            return NULL;
    }
    JSON_OBJECT_T* partial = jsonObjectNewString(MSG_ELEM_N__PARTIAL_VERSION, sReportInfo.pPartialVer);

    JSON_OBJECT_T* sts = jsonObjectNewString(MSG_ELEM_N__STATUS, status);

    char *fileUrl = (sReportInfo.pFileUrl != NULL)?sReportInfo.pFileUrl:"";
    JSON_OBJECT_T* url = jsonObjectNewString(MSG_ELEM_N_FILE_URL, fileUrl);

    jsonObjectAddObject(dataNode, model);
    jsonObjectAddObject(dataNode, mac);
    jsonObjectAddObject(dataNode, serial);
    jsonObjectAddObject(dataNode, full);
    jsonObjectAddObject(dataNode, partial);
    jsonObjectAddObject(dataNode, sts);
    jsonObjectAddObject(dataNode, url);

    jsonObjectAddObject(root, cmdNode);
    jsonObjectAddObject(root, dataNode);

    jsonEncode(msgBuf, root);

    Log("SUS Report Msg[%s]\n", msgBuf);

    jsonFreeNode(root);

    return msgBuf;
}
