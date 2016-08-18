/*
 ============================================================================
 Name        : NagraDownloader.c
 Author      : Jhseo
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "sus_comm.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define	UPGRADE_SUCCESS 		0
#define	UPGRADE_FAIL 			1

int RECV_CNT = 0;

void downloadListener(char* pBuf, int unitSize, long current, long dest, void* pPrivData) {
	printf("!!!>>> Received Buf[%c], unitSize[%d], Current [%ld]bytes in total [%ld]bytes, priv[0x%X]\n",
	        pBuf[0], unitSize, current, dest, pPrivData);

#if 0
	if (++RECV_CNT > 50) {
	    reportUpgradeResult(UPGRADE_ERR__GENERAL);
	    RECV_CNT = 0;
	
	    return;
	}
	printf("Reported [%d]th...\n", RECV_CNT);
#endif
}

void cleanup_downloading(char * pImgBuf) {
	if (pImgBuf != NULL) {
		free(pImgBuf);
		printf("freed img buffer..\n");
	}
}

int upgrade_fw() {
    /**
     * IMPLEMENTS UPGRADE LOGIC
     */
	return UPGRADE_SUCCESS;
}

int main(void) {

	char* pImageBuffer = NULL;
	int imageFileSize = 0;

	printf("########## 1. Request download @@@@ #########\n");
	DownloadListener listener = downloadListener;

	// Example #1: download in memory
//	int download_result = requestFwDownloadInMem(&pImageBuffer, &imageFileSize, listener);

	int testPriv;
	// Example #2 : download in a given file
//    int download_result = requestFwDownloadInFile("/home/jhseo/test.txt", &imageFileSize, listener);
	int download_result = requestFwDownloadInPkt(&testPriv, 4096, listener, "3.0.31", UPGRADE_MODE__NORMAL);

	printf(">> download result[%ld]/ size [%d] bytes\n", download_result, imageFileSize);

	if (download_result != SUSC_RES__SUCCESS) {
	    printf("Download is failed. error_code[%d]\n", download_result);
	    return download_result;
	}

    printf("########## 2. Upgrade firmware #########\n");
	int upgrade_result = upgrade_fw();

    printf("########## 3. Report Upgrade Result ####\n");
	if (upgrade_result == UPGRADE_SUCCESS) {
		reportUpgradeResult(UPGRADE_ERR__NO_ERR);
	} else {
		reportUpgradeResult(UPGRADE_ERR__GENERAL);
	}

    printf("########## 4. Clean up temporary memory #\n");
	cleanup_downloading(pImageBuffer);

	return 0;
}

