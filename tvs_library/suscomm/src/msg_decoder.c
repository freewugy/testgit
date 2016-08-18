/*
 * sus_decoder.c
 *
 *  Created on: Jan 28, 2015
 *      Author: jhseo
 */

#include <stdio.h>
#include "jsmn.h"
#include "msg_format.h"
#include "json_format.h"
#include "utils.h"

#define	MAX_TOKEN_NUM		256

static int jsoneq(const char *json, jsmntok_t *tok, const char *s)
{
    if((tok->type == JSMN_STRING) && ((int) strlen(s) == tok->end - tok->start) && (strncmp(json + tok->start, s, tok->end - tok->start) == 0)) {
        return 0;
    }
    return -1;
}

int parse_string_value(char * pMsg, jsmntok_t valueToken, char ** dest)
{
    if((pMsg == NULL) && (dest == NULL)) {
        return JSON_RET_FAIL;
    }

    int valueLen = valueToken.end - valueToken.start;
    char* pValue = pMsg + valueToken.start;

    *dest = (char *) malloc(sizeof(char) * (valueLen + 1));
    memset(*dest, 0x00, sizeof(char) * (valueLen + 1));
    strncpy(*dest, pValue, valueLen);

    Log("Parsed string[%s]\n", *dest);
    return JSON_RET_SUCCESS;
}

int parseCheckRespMsg(char * pCheckRespMsg, UPGRADE_INFO_T** ppUpgradeInfo)
{

    if(pCheckRespMsg == NULL) {
        *ppUpgradeInfo = NULL;
        return JSON_RET_FAIL;
    }

    int msgLen = strlen(pCheckRespMsg);

    *ppUpgradeInfo = (UPGRADE_INFO_T *) malloc(sizeof(UPGRADE_INFO_T));
    if (*ppUpgradeInfo == NULL) {
        return JSON_RET_FAIL;
    }
    memset(*ppUpgradeInfo, 0x00, sizeof(UPGRADE_INFO_T));

    jsmn_parser parser;
    jsmntok_t tokens[MAX_TOKEN_NUM];

    jsmn_init(&parser);
    int res = jsmn_parse(&parser, pCheckRespMsg, msgLen, tokens, MAX_TOKEN_NUM);
    if(res < 0) {
        Log("Fail to parse json\n");

        *ppUpgradeInfo = NULL;
        return JSON_RET_FAIL;
    }

    if(res < 1 || tokens[0].type != JSMN_OBJECT) {
        Log("root element is not object\n");

        *ppUpgradeInfo = NULL;
        return JSON_RET_FAIL;
    }

    int i = 0;
    int fwInfoIdx = 0;
    int isParsed = 0;

    for (i = 1; i < MAX_TOKEN_NUM; i++, isParsed = 0) {
        if(jsoneq(pCheckRespMsg, &tokens[i], MSG_ELEM_N_FILE_MD5) == 0) {
            parse_string_value(pCheckRespMsg,
                                tokens[i + 1],
                                &((*ppUpgradeInfo)->fwInfoList[fwInfoIdx].pMd5));
        } else if(jsoneq(pCheckRespMsg, &tokens[i], MSG_ELEM_N_FILE_NAME) == 0) {
            parse_string_value(pCheckRespMsg,
                                tokens[i + 1],
                                &((*ppUpgradeInfo)->fwInfoList[fwInfoIdx].pFileName));
        } else if(jsoneq(pCheckRespMsg, &tokens[i], MSG_ELEM_N_FILE_PATH) == 0) {
            parse_string_value(pCheckRespMsg,
                                tokens[i + 1],
                                &((*ppUpgradeInfo)->fwInfoList[fwInfoIdx].pFilePath));
        } else if(jsoneq(pCheckRespMsg, &tokens[i], MSG_ELEM_N_FILE_SIZE) == 0) {
            parse_string_value(pCheckRespMsg,
                                tokens[i + 1],
                                &((*ppUpgradeInfo)->fwInfoList[fwInfoIdx].pFileSize));
        } else if(jsoneq(pCheckRespMsg, &tokens[i], MSG_ELEM_N_FILE_URL) == 0) {
            parse_string_value(pCheckRespMsg,
                                tokens[i + 1],
                                &((*ppUpgradeInfo)->fwInfoList[fwInfoIdx].pFileUrl));

            ++((*ppUpgradeInfo)->fwInfoNum);

        } else if(jsoneq(pCheckRespMsg, &tokens[i], MSG_ELEM_N__FULL_VERSION) == 0) {
            parse_string_value(pCheckRespMsg,
                                tokens[i + 1],
                                &((*ppUpgradeInfo)->fwInfoList[fwInfoIdx].pFullVer));
        } else if(jsoneq(pCheckRespMsg, &tokens[i], MSG_ELEM_N__PARTIAL_VERSION) == 0) {
            parse_string_value(pCheckRespMsg,
                                tokens[i + 1],
                                &((*ppUpgradeInfo)->fwInfoList[fwInfoIdx].pPartialVer));
        } else if(jsoneq(pCheckRespMsg, &tokens[i], MSG_ELEM_N_MODE) == 0) {
            parse_string_value(pCheckRespMsg,
                                tokens[i + 1],
                                &((*ppUpgradeInfo)->fwInfoList[fwInfoIdx].pMode));
        } else if(jsoneq(pCheckRespMsg, &tokens[i], MSG_ELEM_N_SW_ID) == 0) {
            parse_string_value(pCheckRespMsg,
                                tokens[i + 1],
                                &((*ppUpgradeInfo)->fwInfoList[fwInfoIdx].pSwId));
        } else if(jsoneq(pCheckRespMsg, &tokens[i], MSG_ELEM_N_SW_TYPE) == 0) {
            parse_string_value(pCheckRespMsg,
                                tokens[i + 1],
                                &((*ppUpgradeInfo)->fwInfoList[fwInfoIdx].pSwType));
        } else if(jsoneq(pCheckRespMsg, &tokens[i], MSG_ELEM_N_VERSION) == 0) {
            parse_string_value(pCheckRespMsg,
                                tokens[i + 1],
                                &((*ppUpgradeInfo)->fwInfoList[fwInfoIdx].pFwVersion));
        } else if(jsoneq(pCheckRespMsg, &tokens[i], MSG_ELEM_N_MESSAGE) == 0) {
            parse_string_value(pCheckRespMsg,
                                tokens[i + 1],
                                &((*ppUpgradeInfo)->pMesg));
        } else if(jsoneq(pCheckRespMsg, &tokens[i], MSG_ELEM_N_CODE) == 0) {
            parse_string_value(pCheckRespMsg,
                                tokens[i + 1],
                                &((*ppUpgradeInfo)->pCode));
        } else if(jsoneq(pCheckRespMsg, &tokens[i], MSG_ELEM_N_GROUP_ID) == 0) {
            parse_string_value(pCheckRespMsg,
                                tokens[i + 1],
                                &((*ppUpgradeInfo)->pGroupId));
        }

    }

    return JSON_RET_SUCCESS;
}
