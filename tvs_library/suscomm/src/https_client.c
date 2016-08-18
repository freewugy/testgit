/*
 * https_client.c
 *
 *  Created on: Jan 28, 2015
 *      Author: jhseo
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "curl/curl.h"
#include "utils.h"

const char * SYM_QUERY_PARAMETER	= "?q=";

const long HTTP_SOCKET_TIMEOUT_SEC = 15;
const int RECV_BUFFER_SIZE = 4096;

struct DownloadStruct {
    char *memory;
    size_t downloadSize;
};

char * psFullUrl;
char RECV_BUFFER[4096];

void initRecvBuffer() {
    memset(RECV_BUFFER, 0x0, RECV_BUFFER_SIZE);
}

int appendParameter(char ** ppFullUrl, char * pUrl, char * pParameter)
{
    if((pUrl == NULL) || (pParameter == NULL)) {
        return -1;
    }

    int urlLen = strlen(pUrl);
    int totalMesgLen = urlLen + strlen(SYM_QUERY_PARAMETER) + strlen(pParameter) + 1;

    *ppFullUrl = (char *) malloc(sizeof(char) * totalMesgLen);
    if(*ppFullUrl != NULL) {
        memset(*ppFullUrl, 0x00, totalMesgLen);
    }

    sprintf(*ppFullUrl, "%s%s%s", pUrl, SYM_QUERY_PARAMETER, pParameter);

    Log("Request URL[%s]\n", *ppFullUrl);
    return 0;
}

size_t recvSusResp(void *contents, size_t size, size_t nmemb, void *pUserData)
{
    size_t realsize = size * nmemb;
    struct DownloadStruct *mem = (struct DownloadStruct *) pUserData;

//    mem->memory = realloc(mem->memory, mem->downloadSize + realsize + 1);
#if 1
    int msgLen = size * nmemb;

    mem->memory = malloc(msgLen);
    memset(mem->memory, 0x00, msgLen);

    memcpy(mem->memory, contents, realsize);
    mem->downloadSize = realsize;
#else
    mem->memory = malloc(realsize+1); 
    if(mem->memory == NULL) {
        /* out of memory! */
        Log("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    memcpy(mem->memory, contents, realsize);
    mem->downloadSize += realsize;
    mem->memory[realsize] = 0;
#endif
    return realsize;
}

void cleanupMemory()
{
    if(psFullUrl != NULL) {
        free(psFullUrl);

        psFullUrl = NULL;
    }
}

int requestSusMsg(char * pMsg, char ** ppRespMsg)
{
    CURL *curl;
    CURLcode res = -1;

    struct DownloadStruct chunk;

    chunk.memory = NULL; /* will be grown as needed by the realloc above */
    chunk.downloadSize = 0; /* no data at this point */

    curl = curl_easy_init();
    if(curl) {
        char * psFullUrl = NULL;

        char * pUrl = NULL;

        getSusFullUrl(&pUrl);

        appendParameter(&psFullUrl, pUrl, pMsg);

        curl_easy_setopt(curl, CURLOPT_URL, psFullUrl);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, recvSusResp);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void * )&chunk);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, HTTP_SOCKET_TIMEOUT_SEC);
        curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 1L);
        curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, HTTP_SOCKET_TIMEOUT_SEC);

        res = curl_easy_perform(curl);

        /* Check for errors */
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            Log("############### SUCCESS #############!!!\n");

            *ppRespMsg = chunk.memory;
        }

        if(psFullUrl != NULL) {
            free(psFullUrl);
        }

        /* always cleanup */
        curl_easy_cleanup(curl);

//	    cleanupMemory();
    }

    return res;
}
