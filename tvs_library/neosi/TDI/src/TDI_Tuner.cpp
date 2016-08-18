/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: ksh78 $
 * $LastChangedDate: 2014-07-07 15:08:00 +0900 (월, 07 7월 2014) $
 * $LastChangedRevision: 914 $
 * Description:
 * Note:
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <malloc.h>
#include "unistd.h"
#include <time.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <getopt.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/ipc.h>
#ifndef ANDROID
#include <sys/sem.h>
#endif
#include <errno.h>
#include <sys/time.h>
#include <queue>

#include "TDI.h"
#include "TDI_Tuner.h"
#include "TDI_TSParser.h"
#include "Logger.h"

#define REJOINTIMEOUT_SEC       60

char TsFilePath[100];
pthread_t ID_FileFeeding_TASK;
pthread_t ID_MCFeeding_TASK;
pthread_t ID_TUNERCB_TASK;

void TSH_Base_MF_CreatFileFeeding();
void TSH_Base_MF_DeleteFileFeeding();
void TSH_Base_MF_CreatMCFeeding();

void SocketPortOpen(char* IP, int PORT);
void SocketPortClose(void);

TDI_TunerCallback pcallbackfn = NULL;
int gCurrentFreqKHz = 0;
int gCurrentProgNum = 2;

TDI_TUNER_RESULT TDI_Tuner_Get_Capability(int32_t *count, TDI_TUNER_STATUS **status)
{
    fprintf(stdout, "TDI_Tuner_Get_Capability called\n");
    *count = 1;
    **status = TDI_TUNER_STATUS_UNRESERVED;
    return (TDI_TUNER_RESULT) 0;
}

TDI_TUNER_RESULT TDI_Tuner_Reserve(int *hdl)
{
    fprintf(stdout, "TDI_Tuner_Reserve called[%d]\n", __LINE__);
    *hdl = 1;
    return (TDI_TUNER_RESULT) 0;
}

TDI_TUNER_RESULT TDI_Tuner_Tune_original(int hdl, TDI_TUNE_PARAM *param)
{
    fprintf(stdout, "TDI_Tuner_Tune called , handle[%d] , tuner type[%d]\n", hdl, param->type);
    TSH_Base_MF_DeleteFileFeeding();
    SocketPortClose();
    usleep(100000);

    if(param->type == TDI_TUNER_TYPE_FILE) {
        sprintf((char*) TsFilePath, "%s", param->file.name);
        TSH_Base_MF_CreatFileFeeding();
    } else if(param->type == TDI_TUNER_TYPE_IP_MC) {
        //param->ip.ip;
        //param->ip.port;
        SocketPortOpen(param->ip.ip, param->ip.port);
        TSH_Base_MF_CreatMCFeeding();
    }
    return (TDI_TUNER_RESULT) 0;
}

TDI_TUNER_RESULT TDI_Tuner_Status(int hdl, TDI_TUNER_INFO *info)
{
    fprintf(stdout, "TDI_Tuner_Status called , handle[%d] , tuner type[%d], tuner status[%d]\n", hdl, info->type, info->status);
    return (TDI_TUNER_RESULT) 0;
}

TDI_Error TDI_Tuner_SetCallback(IN int tunerId, IN TDI_TunerCallback cbFunc)
{
    pcallbackfn = cbFunc;
    return (TDI_Error) 0;
}

TDI_TUNER_RESULT TDI_Tuner_Release(int hdl)
{
    fprintf(stdout, "TDI_Tuner_SetCallback called , handle[%d]\n", hdl);
    TSH_Base_MF_DeleteFileFeeding();
    SocketPortClose();
    return (TDI_TUNER_RESULT) 0;
}

FILE *feedfp = NULL;
bool bfeedingon = false;

static int prevTunerState = 0;

static long FileLength(FILE *fp)
{
    long temp, len;

    temp = ftell(fp);
    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    fseek(fp, temp, SEEK_SET);
    return len;
}
extern int siUSleep(int usec);
void* tTSFileFeeding(void *param)
{
    unsigned char pBuf[188];
    long data_count = 0;
    long data_offset = 0;
    long file_length = 0;
    int sleepcount = 0;

    feedfp = fopen((char*) TsFilePath, "rb");
    if(feedfp != NULL) {
        file_length = FileLength(feedfp);

        while (bfeedingon) {
            sleepcount++;
            data_count += 188;

            if(data_count < file_length) {

                fread(pBuf, 188, 1, feedfp);
                //if(data_count == 188)
                {
                    if(pBuf[0] != 0x47) {
                        printf("ts file feedings pBuf=0x%X\n", pBuf[0]);
                        for (data_offset = 0; data_offset < 188; data_offset++) {
                            if(pBuf[data_offset] == 0x47) {
                                break;
                            }
                        }
                        if(data_offset < 188 && pBuf[data_offset] == 0x47) {
                            fseek(feedfp, data_count + data_offset - 188, SEEK_SET);
                            data_count = data_count + data_offset - 188;
                            continue;
                        }
                    }
                }
                SetTSBufferEx(pBuf);

            } else {
                printf("ts file feedings Repeated file_length=%ld\n", file_length);
                fseek(feedfp, 0, SEEK_SET);
                data_count = 0;
            }
            if(sleepcount % 100 == 0) {
                sleepcount = 0;
                siUSleep(5000);
            }
        }

        fclose(feedfp);
        feedfp = NULL;
    }

    return NULL;
}

void TSH_Base_MF_CreatFileFeeding()
{
    L_INFO("\n");
    if(bfeedingon)
        return;

    bfeedingon = true;
    pthread_create(&ID_FileFeeding_TASK, NULL, tTSFileFeeding, NULL);
}
void TSH_Base_MF_DeleteFileFeeding()
{
    L_INFO("\n");
    bfeedingon = false;
}

#define ADDR_MAX_LEN 32
#define PACKET_SIZE							188*6
int gSocket = -1;
time_t ts_receiverTimeT;
char gip_str[50];
int gport_num = 0;

void convertclock2Time_t(time_t* intime)
{
    struct timespec tp;

    clock_gettime(CLOCK_MONOTONIC, &tp);
    *intime = tp.tv_sec;
}

void* tTsFeeding(void* arg)
{
    time_t currentTimeT;
    int len;
    unsigned char ts_buf[188];

    convertclock2Time_t(&ts_receiverTimeT);

    while (1) {
        len = recv(gSocket, ts_buf, 188, MSG_DONTWAIT);

        if(len < 0) {
            usleep(6000);
            convertclock2Time_t(&currentTimeT);
            if(gport_num > 0 && currentTimeT - ts_receiverTimeT > REJOINTIMEOUT_SEC) {
                printf("multicast join retry\n");
                SocketPortClose();
                SocketPortOpen(gip_str, gport_num);
            }
        } else if(len == 0) {
            usleep(6000);
            convertclock2Time_t(&currentTimeT);
            if(gport_num > 0 && currentTimeT - ts_receiverTimeT > REJOINTIMEOUT_SEC) {
                printf("multicast join retry\n");
                SocketPortClose();
                SocketPortOpen(gip_str, gport_num);
            }
        } else {
            convertclock2Time_t(&ts_receiverTimeT);
            int start_ptr = 0;
            while (len >= 188) {
                SetTSBufferEx(ts_buf + start_ptr);
                len -= 188;
                start_ptr += 188;
            }
        }
    }

    return NULL;
}

void SocketPortOpen(char* IP, int PORT)
{
    L_INFO("\n");
    struct ip_mreq mreq;
    struct sockaddr_in addr;

    unsigned char ip_addr[ADDR_MAX_LEN + 1];
    unsigned short port;
    struct timeval tv;
    socklen_t buf_size, buf_len;

    int tmp;

    sprintf((char*) ip_addr, "%s", IP);
    port = PORT;

    sprintf((char*) gip_str, "%s", IP);
    gport_num = PORT;

    convertclock2Time_t(&ts_receiverTimeT);

    if((gSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("fail to open socket\n");
        return;
    }

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    tmp = 1;
    setsockopt(gSocket, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(int));

    if(bind(gSocket, (struct sockaddr *) &addr, sizeof(struct sockaddr_in)) < 0) {
        fprintf(stderr, "fail to bind socket\n");
        close(gSocket);
        gSocket = -1;
        return;
    }

    mreq.imr_multiaddr.s_addr = inet_addr((const char *) &(ip_addr[0]));
    if((ntohl(mreq.imr_multiaddr.s_addr) >> 28) == 0xe) {
        mreq.imr_interface.s_addr = htonl(INADDR_ANY);
        if(setsockopt(gSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
            fprintf(stderr, "setsockopt:IP_ADD_MEMBERSHIP\n");
            close(gSocket);
            gSocket = -1;
            return;
        }
    } else {
        fprintf(stderr, "the requested ip does not include class D\n");
    }

    tv.tv_sec = 0;
    tv.tv_usec = 200000;
    setsockopt(gSocket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    //buf_size = 127*1024;	 // max size......- -;
    buf_size = 2 * 1024 * 1024; // max size......- -;
    setsockopt(gSocket, SOL_SOCKET, SO_RCVBUF, &buf_size, sizeof(buf_size));

    buf_len = sizeof(buf_size);
    if(getsockopt(gSocket, SOL_SOCKET, SO_RCVBUF, &buf_size, &buf_len) != -1)
        printf("[DVB] RECV BUFSize : %d\n", buf_size);
}

void SocketPortClose(void)
{
    L_INFO("\n");
    if(gSocket != -1)
        close(gSocket);
    gSocket = -1;
}

void TSH_Base_MF_CreatMCFeeding()
{
    static bool bMCthreadwork = false;
    if(bMCthreadwork)
        return;

    bMCthreadwork = true;
    pthread_create(&ID_MCFeeding_TASK, NULL, tTsFeeding, NULL);
}

#ifdef ANDROID
pthread_mutex_t TunerCallbacLockID = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
#else    
pthread_mutex_t TunerCallbacLockID = PTHREAD_MUTEX_INITIALIZER;
#endif

std::queue<int> TunerQ;

TDI_Error TDI_Tuner_SetSettings(int tunerId, TDI_TunerSettings *pSettings)
{
    int hdl = 0;
    TDI_TUNE_PARAM param;

    printf("%s() pSettings->type : %d\n", __FUNCTION__, pSettings->type);

    if(pSettings->type == TDI_TunerType_Ip) {
        param.type = TDI_TUNER_TYPE_IP_MC;
        param.protocol = TDI_TunerProtocol_Udp;

        sprintf(param.ip.ip, "%s", pSettings->parameter.ip.ip);
        param.ip.port = pSettings->parameter.ip.port;

        TDI_Tuner_Tune_original(hdl, &param);
    } else if(pSettings->type == TDI_TunerType_File) {
        param.type = TDI_TUNER_TYPE_FILE;
        sprintf(param.file.name, "%s", pSettings->parameter.file.name);
        TDI_Tuner_Tune_original(hdl, &param);
    } else {
        L_INFO("[TDI_Tuner_SetSettings]\n");
        //if(pSettings->type == )
        {
            printf("pSettings->protocol : %d\n", pSettings->protocol);
            printf("pSettings->parameter.rf.frequencyHz : %d\n", pSettings->parameter.rf.frequencyHz);
            printf("pSettings->parameter.rf.symbolrate : %d\n", pSettings->parameter.rf.symbolrate);
        }

    }

    if((pSettings->type == TDI_TunerType_Ip) || (pSettings->type == TDI_TunerType_File)) {
        int tunerState = 1;
        pthread_mutex_lock(&TunerCallbacLockID);
        TunerQ.push(tunerState);
        pthread_mutex_unlock(&TunerCallbacLockID);
    } else {
        int tunerState = -1;
        pthread_mutex_lock(&TunerCallbacLockID);
        TunerQ.push(tunerState);
        pthread_mutex_unlock(&TunerCallbacLockID);
    }

    return (TDI_Error) 0;
}

TDI_Error TDI_Tuner_Stop(int tunerId)
{
    TDI_Error err = TDI_SUCCESS;
    printf("%s()\n", __FUNCTION__);

    int tunerState = 0;
    pthread_mutex_lock(&TunerCallbacLockID);
    TunerQ.push(tunerState);
    pthread_mutex_unlock(&TunerCallbacLockID);
    TSH_Base_MF_DeleteFileFeeding();
    SocketPortClose();

    return err;
}

void TDI_Tuner_setCurrentChannel(int chnum)
{
    gCurrentProgNum = chnum;
}

void* tTunerCallbackHandle(void *param)
{
    while (1) {
        pthread_mutex_lock(&TunerCallbacLockID);
        if(TunerQ.empty()) {
            pthread_mutex_unlock(&TunerCallbacLockID);
            siUSleep(100000);
        } else {
            int tunerState = TunerQ.front();
            TunerQ.pop();
            pthread_mutex_unlock(&TunerCallbacLockID);

            if(tunerState != prevTunerState) {
                prevTunerState = tunerState;
                if(pcallbackfn) {
                    if(tunerState == 0) {
                        L_INFO("TunerCallback : TDI_TUNER_EVENT_REQ_TUNE\n");
                        pcallbackfn(0, TDI_TUNER_EVENT_REQ_TUNE, -1, 0);
                    } else if(tunerState == -1) {
                        L_INFO("TunerCallback : TDI_TUNER_EVENT_FAIL_TUNE\n");
                        pcallbackfn(0, TDI_TUNER_EVENT_FAIL_TUNE, -1, gCurrentFreqKHz);
                    } else if(tunerState == 1) {
                        L_INFO("TunerCallback : TDI_TUNER_EVENT_SUCCESS_TUNE\n");
                        pcallbackfn(0, TDI_TUNER_EVENT_SUCCESS_TUNE, gCurrentProgNum, gCurrentFreqKHz);
                    }
                }
            }
        }
    }

    return NULL;
}

TDI_Error TDI_Tuner_Open(int *pTunerId)
{
    L_INFO("\n");
    static int tunerinit = 0;
    if(tunerinit == 0) {
        tunerinit = 1;
        pthread_create(&ID_TUNERCB_TASK, NULL, tTunerCallbackHandle, NULL);
    }
#ifdef USE_ANDROID	

#else
    return (TDI_Error) 0;
#endif	
}

TDI_Error TDI_Tuner_Close(int tunerId)
{
#ifdef USE_ANDROID	
#else	
    return (TDI_Error) 0;
#endif	
}
