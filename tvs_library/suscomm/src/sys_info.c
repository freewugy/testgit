/*
 * sys_info.c
 *
 *  Created on: Jan 28, 2015
 *      Author: jhseo
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "sys_info.h"
#include "property.h"
#include "utils.h"

enum {
    TRUE = 0,
    FALSE = 1
};

//#define ENV_PC 1

#if defined(ENV_PC)
char * SUS_VERSION_FILE = "/home/jhseo/susVersion.dat";
char * FW_VERSION_FILE = "/home/jhseo/version.txt";
char * SERIAL_NUMBER_FILE = "/home/jhseo/serial.txt";
char * SUS_CLIENT_CONFIG_FILE = "/home/jhseo/susclient.cfg";
#else
char * SUS_VERSION_FILE = "/data/lkn_home/config/susVersion.dat";
char * FW_VERSION_FILE = "/system/usr/lkn/version.txt";
char * SERIAL_NUMBER_FILE = "/factory_settings/serial";
char * SUS_CLIENT_CONFIG_FILE = "/data/lkn_home/config/susclient.cfg";
#endif

char * EMPTY_STRING = "";
char * PROP_FULL_VERSION = "FULL";
char * PROP_PARTIAL_VERSION = "PARTIAL";
char * PROP_GROUP_ID = "GROUP_ID";
char * CONST_DEFAULT_GROUP_ID = "default";
char NEW_LINE_CHARACTER = '\n';
char * GET_MAC_ADDRESS_CMD = "busybox ifconfig | grep eth0 | busybox awk '{ print $5 }'";
char * GET_MODEL_NAME_CMD = "getprop ro.product.model";
char * INITIAL_VERSION = "0";

int isValidFwVersion(char * pLine) {
    if (pLine == NULL) {
        return TRUE;
    }

    int len = strlen(pLine);
    int idx = 0;

    if (pLine[idx] == NEW_LINE_CHARACTER) {
        return FALSE;
    }

    for (idx = 0; idx < len;idx++) {
        if (!isdigit(pLine[idx])) {
            if ((pLine[idx] != '.') && (pLine[idx] != NEW_LINE_CHARACTER)){
                return FALSE;
            }
        }
    }

    return TRUE;
}

char * getStbSerialNum()
{
    char * serialNum = EMPTY_STRING;

    FILE* hSerialFile = fopen(SERIAL_NUMBER_FILE, "r");
    if (hSerialFile == NULL) {
        Log("serial number file is not exist\n");
        return serialNum;
    }

    char line[300];
    memset(line, 0x00, sizeof(char)*300);


    while (fgets(line, 256, hSerialFile) != NULL){
        if (line[strlen(line)-1] == NEW_LINE_CHARACTER) {
            line[strlen(line)-1] = 0;
        }

        serialNum = strdup(line);
        break;
    }

    if (hSerialFile != NULL) {
        fclose(hSerialFile);
    }
    return serialNum;
}

char * convertMacAddress(char * pOfficialMacAddr) {
    if (pOfficialMacAddr == NULL) {
        return EMPTY_STRING;
    }

    char* DELIMITER = ":";
    const int MAX_BUF_LEN = 128;
    char tmpBuf[MAX_BUF_LEN];
    memset(tmpBuf, 0x00, MAX_BUF_LEN);

    char * token = NULL;
    token = strtok(pOfficialMacAddr, DELIMITER);
    strcat(tmpBuf, token);

    while((token = strtok(NULL, DELIMITER)) != NULL) {
        strcat(tmpBuf, token);
    }

    if (tmpBuf[strlen(tmpBuf)-1] == NEW_LINE_CHARACTER) {
        tmpBuf[strlen(tmpBuf)-1] = '\0';
    }

    toLowerString(tmpBuf, strlen(tmpBuf));
    Log("MAC address[%s]\n", tmpBuf);
    return strdup(tmpBuf);
}

#if defined(ENV_PC)
char * getMacAddress()
{
    const int MAX_INFO_LEN = 256;
    char macInfo[MAX_INFO_LEN];
    memset(macInfo, 0x00, sizeof(char)*MAX_INFO_LEN);

//    sprintf(macInfo, "11:22:33:44:55:66");
//    sprintf(macInfo, "00:91:00:00:00:00");
//    sprintf(macInfo, "98:93:cc:3e:7b:08");
    sprintf(macInfo, "3c:bd:d8:09:86:a8");
    char * convertedMacAddress = convertMacAddress(macInfo);

    return convertedMacAddress;
}

#else
char * getMacAddress()
{
    const int MAX_INFO_LEN = 256;

    FILE *fp;
    char macInfo[MAX_INFO_LEN];

    memset(macInfo, 0x00, sizeof(char)*MAX_INFO_LEN);

    fp = popen(GET_MAC_ADDRESS_CMD, "r");
    if (fp == NULL) {
        Log("Error! Get Mac address..\n");
        return EMPTY_STRING;
    }

    char * convertedMacAddress = NULL;

    while (fgets(macInfo, MAX_INFO_LEN, fp) != NULL) {
        Log("MAC address: [%s]\n", macInfo);
        convertedMacAddress = convertMacAddress(macInfo);
        break;
    }


    pclose(fp);
    return convertedMacAddress;
}
#endif


#if defined(ENV_PC)
char * getModelName()
{
//    return "BCM7XXX_TEST_SETTOP";
//    return "SH940C-LN";
    return "ST950I-LN";
}
#else
char * getModelName()
{
    const int MAX_INFO_LEN = 256;

    FILE *fp;
    char modelInfo[MAX_INFO_LEN];

    memset(modelInfo, 0x00, sizeof(char)*MAX_INFO_LEN);

    fp = popen(GET_MODEL_NAME_CMD, "r");
    if (fp == NULL) {
        Log("Error! Get model name..\n");
        return EMPTY_STRING;
    }

    fgets(modelInfo, MAX_INFO_LEN, fp);

    if (modelInfo[strlen(modelInfo)-1] == NEW_LINE_CHARACTER) {
        modelInfo[strlen(modelInfo)-1] = '\0';
    }
    Log("Model Name: [%s]\n", modelInfo);

    pclose(fp);
    return strdup(modelInfo);
}
#endif

char * getSusFullVer()
{
    char * fullVer = NULL;
    int result = getProperty(SUS_VERSION_FILE, PROP_FULL_VERSION, &fullVer);
    if (result == FALSE) {
        return INITIAL_VERSION;
    }

    return fullVer;
}

char * getSusPartialVer()
{
    char * partialVer = NULL;
    int result = getProperty(SUS_VERSION_FILE, PROP_PARTIAL_VERSION, &partialVer);
    if (result == FALSE) {
        return INITIAL_VERSION;
    }

    return partialVer;
}

char * getGroupId() {
    char * pGroupId = NULL;
    int result = getProperty(SUS_CLIENT_CONFIG_FILE, PROP_GROUP_ID, &pGroupId);
    if ((result == FALSE)
            || (pGroupId == NULL)){
        return CONST_DEFAULT_GROUP_ID;
    }

    return pGroupId;
}

int saveGroupId(char * pGroupId) {
    if (pGroupId == NULL) {
        return FALSE;
    }

    setProperty(SUS_CLIENT_CONFIG_FILE, PROP_GROUP_ID, pGroupId);

    Log("Save Group Id[%s]\n", pGroupId);
    return TRUE;
}
