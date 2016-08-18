#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "utils.h"

char * sSusAddress = NULL;
int sSusPort = 0;

int MAX_LINE_LEN = 300;
char * SUB_URL = "/SUS/upgrade";
int COMMENT_CHAR = '#';
char INFO_DELIMETER_SPACE = ' ';
char * ID_SUS_SVER = "id=\"sus\"";

/* ***********************************************
 * DEFAULT SERVER INFO
 */

//#define ENV_PC 1

#if defined(ENV_PC)
char * SERVER_LIST_FILE = "/home/jhseo/server-list.conf";
#else
char * SERVER_LIST_FILE = "/data/lkn_home/config/server-list.conf";
#endif


char * DEFAULT_SUS_ADDRESS = "https://sus-tvs.linknet.co.id";
int   DEFAULT_SUS_PORT    = 8181;

enum {
    TRUE = 0,
    FALSE
};

char * getSusAddress() {
    if (sSusAddress == NULL) {
        return DEFAULT_SUS_ADDRESS;
    }

    return sSusAddress;
}

int setSusAddress(char * pAddress) {
    if (pAddress == NULL) {
        return FALSE;
    }

    if (sSusAddress != NULL) {
        free(sSusAddress);
        sSusAddress = NULL;
    }

    sSusAddress = strdup(pAddress);
    return TRUE;
}

int getSusPort() {
    if (sSusPort == 0) {
        return DEFAULT_SUS_PORT;
    }

    return sSusPort;
}

int setSusPort(int port) {
    sSusPort = port;
    return TRUE;
}

char * getSusSubUrl() {
    return SUB_URL;
}

int getSusFullUrl(char ** ppUrl) {
    if (ppUrl == NULL) {
        return FALSE;
    }

    *ppUrl = (char *)malloc(MAX_LINE_LEN);
    if (*ppUrl == NULL) {
        return FALSE;
    }

    memset(*ppUrl, 0x00, sizeof(MAX_LINE_LEN));

    sprintf(*ppUrl, "%s:%d%s",
            getSusAddress(),
            getSusPort(),
            getSusSubUrl());

    Log("FullUrl[%s]\n", *ppUrl);
    return TRUE;
}

int isSusInfo(char * pLine) {
    if (pLine == NULL) {
        return FALSE;
    }

//    char *pTrimedLine = trim(pLine);
//    if (pTrimedLine == NULL) {
//        return FALSE;
//    }
//
//    printf("TrimedLIne[%s]\n", pTrimedLine);
    if (pLine[0] == COMMENT_CHAR) {
        Log(" --> SKIP...COMMENT\n");
        return FALSE;
    }

    if (strstr(pLine, ID_SUS_SVER) == NULL) {
        return FALSE;
    }

    return TRUE;
}

int parsePort(char *pLine) {
    char *pOffPort = strstr(pLine, "port");
     if (pOffPort == NULL) {
         return FALSE;
     }

     char * pOffStartPort = strchr(pOffPort, '"');
     if (pOffStartPort == NULL) {
         return FALSE;
     }

     pOffStartPort++;
     char * pOffEndPort = strchr(pOffStartPort, '"');
     if (pOffEndPort == NULL) {
         return FALSE;
     }

     pOffEndPort--;
     int len = (pOffEndPort - pOffStartPort + 1);

     char * pPort = (char *)malloc(len+1);
     if (pPort == NULL) {
         return FALSE;
     }

     memset(pPort, 0x00, len+1);

     strncpy(pPort, pOffStartPort, len);

     int port = atoi(pPort);
     Log("> Port [%d]\n", port);

     if (pPort != NULL) {
         free(pPort);
     }

     return port;
}

char * parseAddress(char * pLine) {

    char *pOffAddress = strstr(pLine, "address");
    if (pOffAddress == NULL) {
        return NULL;
    }

    char * pOffStartAddress = strchr(pOffAddress, '"');
    if (pOffStartAddress == NULL) {
        return NULL;
    }

    pOffStartAddress++;
    char * pOffEndAddress = strchr(pOffStartAddress, '"');
    if (pOffEndAddress == NULL) {
        return NULL;
    }

    pOffEndAddress--;
    int len = (pOffEndAddress - pOffStartAddress + 1);

    char * pAddress = (char *)malloc(len+1);
    if (pAddress == NULL) {
        return NULL;
    }
    memset(pAddress, 0x00, len+1);

    strncpy(pAddress, pOffStartAddress, len);

    Log("> Address [%s]\n", pAddress);
    return pAddress;
}

int parseSusInfo(char * pLine) {
    if (pLine == NULL) {
        return FALSE;
    }

    char * pSusAddress = parseAddress(pLine);
    if (pSusAddress == NULL) {
        return FALSE;
    }

    int susPort = parsePort(pLine);

    setSusAddress(pSusAddress);
    setSusPort(susPort);

    if (pSusAddress != NULL) {
        free(pSusAddress);
    }

    return TRUE;
}

int loadServerInfo() {
    FILE* fh;
    fh = fopen(SERVER_LIST_FILE, "r");

    Log("Loading server info......\n");

    //check if file exists
    if (fh == NULL) {
        Log("file does not exists %s\n", SERVER_LIST_FILE);
        return 0;
    }


    //read line by line
    char* line = malloc(MAX_LINE_LEN);
    if (line == NULL) {
        fclose(fh);
        Log("Error! memory allocation failure on loadServerInfo\n");
        return 0;
    }

    while (fgets(line, MAX_LINE_LEN, fh) != NULL){

        if (isSusInfo(line) == TRUE) {
            parseSusInfo(line);
        }
    }

    if (line != NULL) {
        fclose(fh);
        free(line);
    }
    return TRUE;
}
