/*
 * property.c
 *
 *  Created on: Feb 16, 2015
 *      Author: jhseo
 */

#include <stdio.h>
#include <string.h>
#include "utils.h"

const int MAX_LINE_LENGTH =    300;
const char PROP_DELIMITER = '=';

enum {
    TRUE = 0,
    FALSE = 1
};

int getValue(char * pLine, char * pKey, char ** ppValue) {
    if ((pLine == NULL) || (pKey == NULL)) {
        return FALSE;
    }

    char * pOffsetDelimiter = strchr(pLine, PROP_DELIMITER);
    if (pOffsetDelimiter == NULL) {
        return FALSE;
    }

    int valueLen = strlen(pOffsetDelimiter+1);
    *ppValue = (char *)malloc(valueLen+1);
    if (*ppValue == NULL) {
        return FALSE;
    }
    memset(*ppValue, 0x00, valueLen+1);
    strcpy(*ppValue, pOffsetDelimiter+1);

    if ((*ppValue)[valueLen-1] == '\n') {
        (*ppValue)[valueLen-1] = 0;
    }

    return TRUE;
}

int isTheProperty(char * pLine, char * pKey) {
    if ((pLine == NULL) || (pKey == NULL)) {
        return FALSE;
    }

    char * pOffsetDelimiter = strchr(pLine, PROP_DELIMITER);
    if (pOffsetDelimiter == NULL) {
        return FALSE;
    }

    char keyBuf[256];
    memset(keyBuf, 0x00, 256);

    strncpy(keyBuf, pLine, (pOffsetDelimiter - pLine));

    if (strlen(keyBuf) == strlen(pKey)) {
        if (strcmp(keyBuf, pKey) == 0) {
            return TRUE;
        }
    }
    return FALSE;
}

int getProperty(char* pFile, char * pKey, char **ppValue) {

    int result = FALSE;

    if ((pFile == NULL) || (pKey == NULL) || (ppValue == NULL)) {
        return result;
    }


    FILE* pFh = fopen(pFile, "r");
    if (pFh == NULL) {
        return result;
    }

    char line[MAX_LINE_LENGTH];
    memset(line, 0x00, sizeof(char)* MAX_LINE_LENGTH);

    while (fgets(line, MAX_LINE_LENGTH, pFh) != NULL){
        if (isTheProperty(line, pKey) == TRUE) {
            getValue(line, pKey, ppValue);

            result = TRUE;
            break;
        }
    }

    if (pFh != NULL) {
        fclose(pFh);
    }

    return result;
}

int setProperty(char* pFile, char* pKey, char *pValue) {

    int result = FALSE;
    int bFound = FALSE;

    if ((pFile == NULL) || (pKey == NULL) || (pValue == NULL)) {
        Log("Error! Invalid parameter in setProperty()\n");
        return result;
    }

    FILE* pFh = fopen(pFile, "r");
    if (pFh == NULL) {
        Log("Error! Not exist property file[%s]\n", pFile);
        return result;
    }

#ifdef ENV_PC
    const char * TEMP_PROP_FILE = "/home/jhseo/.property.tmp";
#else
    const char * TEMP_PROP_FILE = "/data/lkn_home/run/susc/.property.tmp";
#endif

    FILE* pDestFh = fopen(TEMP_PROP_FILE, "w+");
    if (pDestFh == NULL) {
        Log("Error! Can't open temp property file[%s]\n", TEMP_PROP_FILE);
        return result;
    }

    char line[MAX_LINE_LENGTH];
    memset(line, 0x00, sizeof(char)* MAX_LINE_LENGTH);

    while (fgets(line, MAX_LINE_LENGTH, pFh) != NULL){
        if (isTheProperty(line, pKey) == TRUE) {
            Log("Found the property\n");
        } else {
            fputs(line, pDestFh);
        }
    }

    {
        memset(line, 0x00, sizeof(char)* MAX_LINE_LENGTH);
        sprintf(line, "%s=%s", pKey, pValue);

        fputs(line, pDestFh);

        Log("Insert [%s=%s] property in property file\n", pKey, pValue);
    }

    if (pFh != NULL) {
        fclose(pFh);
    }

    if (pDestFh != NULL) {
	fflush(pDestFh);
        fclose(pDestFh);
    }

    if (rename(TEMP_PROP_FILE, pFile) == 0) {
        Log("Configuration file is saved successfully\n");
        result = TRUE;
    } else {
        Log("Configuration file is not saved because of rename error\n");
        result = FALSE;
    }

    return result;
}
