/*
 * json_encode.c
 *
 *  Created on: Jan 30, 2015
 *      Author: jhseo
 */

#include <stdio.h>
#include <stdlib.h>

#include "sys_info.h"
#include "msg_format.h"
#include "json_format.h"
#include "utils.h"

#define MSG_BUFF_SIZE	1024
#define LINE_BUFF_SIZE	256

void appendObjectElement(char *pBuf, char *pElmName)
{
    char lineBuf[LINE_BUFF_SIZE];
    memset(lineBuf, 0x00, sizeof(char) * LINE_BUFF_SIZE);

    sprintf(lineBuf, "\"%s\":", pElmName);
    strcat(pBuf, lineBuf);
}

void appendArrayElement(char *pBuf, char *pElmName)
{
    char lineBuf[LINE_BUFF_SIZE];
    memset(lineBuf, 0x00, sizeof(char) * LINE_BUFF_SIZE);

    sprintf(lineBuf, "\"%s\":", pElmName);
    strcat(pBuf, lineBuf);
}

void appendString(char *pBuf, char *pElmName, char *pValue)
{
    char lineBuf[LINE_BUFF_SIZE];
    memset(lineBuf, 0x00, sizeof(char) * LINE_BUFF_SIZE);

    sprintf(lineBuf, "\"%s\":\"%s\"", pElmName, pValue);
    strcat(pBuf, lineBuf);
}

void appendSymbol(char *pBuf, char * pSymbol)
{
    char lineBuf[LINE_BUFF_SIZE];
    memset(lineBuf, 0x00, sizeof(char) * LINE_BUFF_SIZE);

    sprintf(lineBuf, "%s", pSymbol);
    strcat(pBuf, lineBuf);
}

JSON_OBJECT_T* jsonObjectNewNode(int type, char * pName, char * pValue)
{
    JSON_OBJECT_T* node = (JSON_OBJECT_T *) malloc(sizeof(JSON_OBJECT_T));

    if(node == NULL) {
        return NULL;
    }

    memset(node, 0x00, sizeof(JSON_OBJECT_T));

    node->nodeType = type;
    if((pName != NULL)) {
        node->pNodeName = strdup(pName);
    } else {
        node->pNodeName = NULL;
    }

    if((pValue != NULL)) {
        node->pNodeValue = strdup(pValue);
    } else {
        node->pNodeValue = NULL;
    }

    node->subNodesNum = 0;
    memset(node->subNodes, 0x00, sizeof(struct JSON_OBJECT*) * MAX_SUB_NODE_NUM);

    return node;
}

JSON_OBJECT_T* jsonObjectNewArray(char * pName)
{
    return jsonObjectNewNode(JSON_OBJ_ARRAY, pName, NULL);
}

JSON_OBJECT_T* jsonObjectNewObject(char * pName)
{
    return jsonObjectNewNode(JSON_OBJ_OBJECT, pName, NULL);
}

JSON_OBJECT_T* jsonObjectNewString(char * pName, char * pValue)
{
    return jsonObjectNewNode(JSON_OBJ_STRING, pName, pValue);
}

int jsonObjectAddObject(JSON_OBJECT_T * parentNode, JSON_OBJECT_T * node)
{
    if((parentNode == NULL) || (node == NULL)) {
        Log("CAUTION! Invalid parameter in %s\n", __FUNCTION__);

        return JSON_RET_FAIL;
    }

    int indexToAdd = parentNode->subNodesNum;
    if(indexToAdd >= MAX_SUB_NODE_NUM) {
        Log("CAUTION! Overflow sub nodes");

        return JSON_RET_FAIL;
    }

    parentNode->subNodes[indexToAdd] = node;
    parentNode->subNodesNum++;

    return JSON_RET_SUCCESS;
}

int jsonObjectAddArray(JSON_OBJECT_T * parentNode, JSON_OBJECT_T * array)
{
    return jsonObjectAddObject(parentNode, array);
}

int jsonEncoding(char *pBuf, JSON_OBJECT_T* pRootNode)
{
    if((pBuf == NULL) || (pRootNode == NULL)) {
        Log("CAUTION! input parameter is invalid..%s\n", __FUNCTION__);
        return JSON_RET_FAIL;
    }

    switch (pRootNode->nodeType) {
        case JSON_OBJ_ARRAY:
        appendSymbol(pBuf, SYM_START_SQUARE);
            break;
        case JSON_OBJ_OBJECT:
        appendSymbol(pBuf, SYM_START_PAREN);
            break;
	case JSON_OBJ_STRING:
	    break;
    }

    int subNodeSize = pRootNode->subNodesNum;
    int idx;
    for (idx = 0; idx < subNodeSize; idx++) {
        JSON_OBJECT_T * subNode = pRootNode->subNodes[idx];
        if(subNode == NULL) {
            continue;
        }

        switch (subNode->nodeType) {
            case JSON_OBJ_ARRAY:
            appendArrayElement(pBuf, subNode->pNodeName);

            jsonEncoding(pBuf, subNode);
                break;
            case JSON_OBJ_STRING:
            appendString(pBuf, subNode->pNodeName, subNode->pNodeValue);
                break;
            case JSON_OBJ_OBJECT:
            if(subNode->pNodeName != NULL) {
                appendObjectElement(pBuf, subNode->pNodeName);
            } else {
                appendSymbol(pBuf, SYM_START_PAREN);
            }

            jsonEncoding(pBuf, subNode);
                break;
            default:
                break;
        }

        if(idx < (subNodeSize - 1)) {
            appendSymbol(pBuf, SYM_SEPERATE);
        }
    }

    switch (pRootNode->nodeType) {
        case JSON_OBJ_ARRAY:
    	    appendSymbol(pBuf, SYM_END_SQUARE);
            break;
        case JSON_OBJ_OBJECT:
       	    appendSymbol(pBuf, SYM_END_PAREN);
            break;
	case JSON_OBJ_STRING:
	    break;
    }
    return JSON_RET_SUCCESS;
}

int jsonEncode(char * pBuf, JSON_OBJECT_T* pRootNode)
{
    if((pBuf == NULL) || (pRootNode == NULL)) {
        return JSON_RET_FAIL;
    }

    return jsonEncoding(pBuf, pRootNode);
}

int jsonFreeNode(JSON_OBJECT_T* pNode)
{
    if(pNode == NULL) {
        return JSON_RET_FAIL;
    }

    if(pNode->pNodeName != NULL) {
        free(pNode->pNodeName);
    }

    if(pNode->pNodeValue != NULL) {
        free(pNode->pNodeValue);
    }

    int subNodeNum = pNode->subNodesNum;
    int idx = 0;
    for (idx = 0; idx < subNodeNum; idx++) {
        JSON_OBJECT_T* pSubNode = pNode->subNodes[idx];
        if(pSubNode != NULL) {
            jsonFreeNode(pSubNode);
        }
    }

    free(pNode);

    return JSON_RET_SUCCESS;
}
