/*
 * json_format.h
 *
 *  Created on: Jan 29, 2015
 *      Author: jhseo
 */

#ifndef JSON_FORMAT_H_
#define JSON_FORMAT_H_

#ifdef __cplusplus
extern "C" {
#endif

#define		MAX_SUB_NODE_NUM		256
#define		LINE_BUF_SIZE			256

typedef enum {
    JSON_RET_SUCCESS = 0, JSON_RET_FAIL
} JSON_RETURN_T;

typedef enum {
    JSON_OBJ_STRING = 1, JSON_OBJ_OBJECT = 2, JSON_OBJ_ARRAY = 3
} JSON_OBJECT_TYPE_T;

typedef struct JSON_OBJECT {
    JSON_OBJECT_TYPE_T nodeType;
    char * pNodeName;
    char * pNodeValue;
    int subNodesNum;
    struct JSON_OBJECT* subNodes[MAX_SUB_NODE_NUM];
} JSON_OBJECT_T;

#define	SYM_START_PAREN						"{"
#define	SYM_END_PAREN						"}"
#define SYM_START_SQUARE					"["
#define	SYM_END_SQUARE						"]"
#define SYM_SEPERATE						","

JSON_OBJECT_T* jsonObjectNewNode(int type, char * pName, char * pValue);
JSON_OBJECT_T* jsonObjectNewArray(char * pName);
JSON_OBJECT_T* jsonObjectNewObject(char * pName);
JSON_OBJECT_T* jsonObjectNewString(char * pName, char * pValue);
int jsonObjectAddObject(JSON_OBJECT_T * parentNode, JSON_OBJECT_T * node);
int jsonObjectAddArray(JSON_OBJECT_T * parentNode, JSON_OBJECT_T * array);
int jsonEncoding(char *pBuf, JSON_OBJECT_T* pRootNode);
int jsonEncode(char * pBuf, JSON_OBJECT_T* pRootNode);
int jsonFreeNode(JSON_OBJECT_T* pNode);


#ifdef __cplusplus
}
#endif

#endif /* JSON_FORMAT_H_ */
