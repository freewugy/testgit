// hdd_ctrl_jni.c 

#include "hdd_ctrl_jni.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <dirent.h>
#include <time.h>

#undef LOG_TAG
#define LOG_TAG	"HDD_CTRL_JNI"
#include "cutils/log.h"

//#undef ALOGD
//#define ALOGD(...)

// timeout value extended for big size file deletion (over 40GB)
#define DEFAULT_TIMEOUT_SEC		(10)

#define LINKNET_CABLE_PROJECT	0	//LinkNet-CABLE Project
#define LINKNET_IPTV_PROJECT	1	//LinkNet-IPTV  Project

#if LINKNET_CABLE_PROJECT == 1
//===== <LinkNet-CABLE: Marvell BG2-CT, Android 4.2.2 JellyBean + marvell patch> ======================================
#define WAIT_USER_HOST_PORT		0x5562
#define WAIT_USER_PROXY_PORT	0x5563
#define WAIT_USER_THREAD_PORT	0x5564
//===============================================================================================================
#endif

#if LINKNET_IPTV_PROJECT == 1
//===== <LinkNet-IPTV: BCM7252, Android 5.0.0 Lollipop r2.0.1 + bcm patch> ======================================
#define WAIT_USER_HOST_PORT		0x5572
#define WAIT_USER_PROXY_PORT	0x5573
#define WAIT_USER_THREAD_PORT	0x5574
//===============================================================================================================
#endif

void	(*g_ptr_hdd_add_detect)(HDDInfoStruct* pHDDInfo, uint8_t rear_front_index) = NULL;
void	(*g_ptr_hdd_remove_detect)(uint8_t rear_front_index) = NULL;
void	(*g_ptr_hdd_update_detect)(HDDInfoStruct* pHDDInfo, uint8_t rear_front_index) = NULL;
void	(*g_ptr_hdd_hdderr_detect)(uint8_t rear_front_index) = NULL;
void	(*g_ptr_hdd_not_supported)(uint8_t rear_front_index) = NULL;
void	(*g_ptr_hdd_media_disk)(uint8_t rear_front_index) = NULL;

void	(*g_ptr_wifi_status)(int status) = NULL;

int txSockfd;
int txClilen;
struct sockaddr_in txServeraddr;

int sockfd;
struct sockaddr_in serveraddr;
int nSockOpt;

int sockfd_thread;
struct sockaddr_in serveraddr_thread;
int nSockOpt_thread;

uint8_t MsgConfig64[64];
uint8_t MsgConfig256[256];
HDDInfoStruct HDDInfo;
uint8_t attached_flag = 0;

void set_hdd_add_detect_callback(void (*ptr_hdd_add_detect)(HDDInfoStruct* pHDDInfo, uint8_t rear_front_index))
{
	g_ptr_hdd_add_detect = ptr_hdd_add_detect;
	//ALOGD("========> g_ptr_hdd_add_detect [0x%x], ptr_hdd_add_detect [0x%x]\n", (int)g_ptr_hdd_add_detect, (int)ptr_hdd_add_detect);
}

void set_hdd_remove_detect_callback(void (*ptr_hdd_remove_detect)(uint8_t rear_front_index))
{
	g_ptr_hdd_remove_detect = ptr_hdd_remove_detect;
	//ALOGD("========> g_ptr_hdd_remove_detect [0x%x], ptr_hdd_remove_detect [0x%x]\n", (int)g_ptr_hdd_remove_detect, (int)ptr_hdd_remove_detect);
}

void set_hdd_update_detect_callback(void (*ptr_hdd_update_detect)(HDDInfoStruct* pHDDInfo, uint8_t rear_front_index))
{
	g_ptr_hdd_update_detect = ptr_hdd_update_detect;
	//ALOGD("========> g_ptr_hdd_update_detect [0x%x], ptr_hdd_update_detect [0x%x]\n", (int)g_ptr_hdd_update_detect, (int)ptr_hdd_update_detect);
}

void set_hdd_hdderr_detect_callback(void (*ptr_hdd_hdderr_detect)(uint8_t rear_front_index))
{
	g_ptr_hdd_hdderr_detect = ptr_hdd_hdderr_detect;
	//ALOGD("========> g_ptr_hdd_hdderr_detect [0x%x], ptr_hdd_hdderr_detect [0x%x]\n", (int)g_ptr_hdd_hdderr_detect, (int)ptr_hdd_hdderr_detect);
}

void set_hdd_not_supported_callback(void (*ptr_hdd_not_supported)(uint8_t rear_front_index))
{
	g_ptr_hdd_not_supported = ptr_hdd_not_supported;
	//ALOGD("========> g_ptr_hdd_not_supported [0x%x], ptr_hdd_not_supported [0x%x]\n", (int)g_ptr_hdd_not_supported, (int)ptr_hdd_not_supported);
}

void set_hdd_media_disk_callback(void (*ptr_hdd_media_disk)(uint8_t rear_front_index))
{
	g_ptr_hdd_media_disk = ptr_hdd_media_disk;
	//ALOGD("========> g_ptr_hdd_media_disk [0x%x], ptr_hdd_media_disk [0x%x]\n", (int)g_ptr_hdd_media_disk, (int)ptr_hdd_media_disk);
}

void set_wifi_status_callback(void (*ptr_wifi_status)(int status))
{
	g_ptr_wifi_status = ptr_wifi_status;
}

pthread_t	hdd_detect_thread[1];

void *thread_hdd_detect_main(void *);

int create_hdd_detect_thread()
{
	memset(&HDDInfo, 0, sizeof(HDDInfo));

    sockfd_thread = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd_thread < 0)
    {
        ALOGE("socket error(%d)", __LINE__);
        return -1;
    }

    bzero(&serveraddr_thread, sizeof(serveraddr_thread));
    serveraddr_thread.sin_family = AF_INET;
    serveraddr_thread.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr_thread.sin_port = htons(WAIT_USER_THREAD_PORT);

	// prevent bind error
	nSockOpt_thread = 1;
	setsockopt(sockfd_thread, SOL_SOCKET, SO_REUSEADDR, &nSockOpt_thread, sizeof(nSockOpt_thread));

    int state = bind(sockfd_thread, (struct sockaddr *)&serveraddr_thread, 
        sizeof(serveraddr_thread));
    if (state == -1)
    {
        ALOGE("bind error(%d)", __LINE__);
        return -1;
    }

	pthread_create(&hdd_detect_thread[0], NULL, &thread_hdd_detect_main, (void *)0);

	return 0;
}

void *thread_hdd_detect_main(void *arg)
{
	while (1)
	{
   		int rcvcnt = recvfrom(sockfd_thread, (void *)MsgConfig64, sizeof(MsgConfig64), 0, NULL, NULL); 
   		ALOGD("recvfrom --> rcvcnt=[%d], MsgConfig64[0]=[%d], [1]=[%d], [2]=[%d], [3]=[%d]\n", rcvcnt, MsgConfig64[0], MsgConfig64[1], MsgConfig64[2], MsgConfig64[3]);
		if (MsgConfig64[2] == 1)
		{
			if (MsgConfig64[1] == HDD_ADD_DETECT)
			{
				memset(&HDDInfo, 0, sizeof(HDDInfo));
   				int rcvcnt = recvfromTimeout(sockfd_thread, (void *)&HDDInfo, sizeof(HDDInfo), 0, NULL, NULL);
				if (rcvcnt >= sizeof(HDDInfo))
				{
					attached_flag = 1;
					if (g_ptr_hdd_add_detect != NULL)
						g_ptr_hdd_add_detect(&HDDInfo, MsgConfig64[3]);
				}
				else
					memset(&HDDInfo, 0, sizeof(HDDInfo));
			}
			else if (MsgConfig64[1] == HDD_REMOVE_DETECT)
			{
				memset(&HDDInfo, 0, sizeof(HDDInfo));
				attached_flag = 0;
				if (g_ptr_hdd_remove_detect != NULL)
					g_ptr_hdd_remove_detect(MsgConfig64[3]);
			}
			else if (MsgConfig64[1] == HDD_UPDATE_DETECT)
			{
				memset(&HDDInfo, 0, sizeof(HDDInfo));
   				int rcvcnt = recvfromTimeout(sockfd_thread, (void *)&HDDInfo, sizeof(HDDInfo), 0, NULL, NULL); 

				if (rcvcnt >= sizeof(HDDInfo))
				{
					attached_flag = 1;
					if (g_ptr_hdd_update_detect != NULL)
						g_ptr_hdd_update_detect(&HDDInfo, MsgConfig64[3]);
				}
			}
			else if (MsgConfig64[1] == HDD_HDDERR_DETECT)
			{
				//memset(&HDDInfo, 0, sizeof(HDDInfo));
				attached_flag = 1;
				if (g_ptr_hdd_hdderr_detect != NULL)
					g_ptr_hdd_hdderr_detect(MsgConfig64[3]);
			}
			else if (MsgConfig64[1] == HDD_NOT_SUPPORTED)
			{
				//memset(&HDDInfo, 0, sizeof(HDDInfo));
				attached_flag = 0;
				if (g_ptr_hdd_not_supported != NULL)
					g_ptr_hdd_not_supported(MsgConfig64[3]);
			}
			else if (MsgConfig64[1] == HDD_MEDIA_DISK)
			{
				//memset(&HDDInfo, 0, sizeof(HDDInfo));
				attached_flag = 0;
				if (g_ptr_hdd_media_disk != NULL)
					g_ptr_hdd_media_disk(MsgConfig64[3]);
			}
			else if (MsgConfig64[1] == WIFI_WIFI_STATUS)
			{
				if (g_ptr_wifi_status != NULL)
					g_ptr_wifi_status(MsgConfig64[3]);
			}
		}
	}

	return NULL;
}

uint8_t hdd_jni_open()
{
    memset(MsgConfig64, 0, 64);
    memset(MsgConfig256, 0, 256);

    txClilen = sizeof(txServeraddr);
    txSockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (txSockfd < 0)
    {
        ALOGE("socket error(%d)", __LINE__);
        return -1;
    }

    bzero(&txServeraddr, sizeof(txServeraddr));
    txServeraddr.sin_family = AF_INET;
    txServeraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    txServeraddr.sin_port = htons(WAIT_USER_HOST_PORT);
	
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        ALOGE("socket error(%d)", __LINE__);
        return -1;
    }

    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(WAIT_USER_PROXY_PORT);

	// prevent bind error
	nSockOpt = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &nSockOpt, sizeof(nSockOpt));

    int state = bind(sockfd, (struct sockaddr *)&serveraddr, 
        sizeof(serveraddr));
    if (state == -1)
    {
        ALOGE("bind error(%d)", __LINE__);
        return -1;
    }

	if(create_hdd_detect_thread()) return -1;

	return 1;
}

void hdd_jni_close()
{
    close(sockfd);
    close(txSockfd);
    close(sockfd_thread);
}

int recvfromTimeout(int sockfd, void *buf, size_t len, int flags,
                 struct sockaddr *src_addr, socklen_t *addrlen)
{
	int result = 0;
	
	// Setup timeval variable
	struct timeval timeout;
	timeout.tv_sec = DEFAULT_TIMEOUT_SEC;
	timeout.tv_usec = 0;
	// Setup fd_set structure
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(sockfd, &fds);
	// Return value:
	// -1: error occured
	// 0: timed out
	// >0: data ready to be read
	result = select(sockfd+1, &fds, 0, 0, &timeout);
	
	if (result > 0)
		result = recvfrom(sockfd, buf, len, flags, src_addr, addrlen);
	else if (result == 0) 
	{
		ALOGD("recvfromTimeout() : timeout!!") ;
	}
	else
	{
		ALOGD("recvfromTimeout() : select error %d!!", result) ; 
	}

	return result;
}

HDDInfoStruct* hdd_get_hdd_info()
{
	static HDDInfoStruct tempHDDInfo;
	memset(&tempHDDInfo, 0, sizeof(tempHDDInfo));

	memset(MsgConfig64, 0, 64);
	memset(MsgConfig256, 0, 256);

	MsgConfig256[0] = 1;
	MsgConfig256[1] = HDD_GET_HDD_INFO;
	MsgConfig256[2] = 1;

	int sndcnt = sendto(txSockfd, (void *)MsgConfig256, sizeof(MsgConfig256), 0, (struct sockaddr *)&txServeraddr, txClilen);
	ALOGD("hdd_get_hdd_info(): sendto   --> sndcnt=[%d], MsgConfig256[0]=[%d], [1]=[%d], [2]=[%d]\n", sndcnt, MsgConfig256[0], MsgConfig256[1], MsgConfig256[2]);

	int rcvcnt = recvfromTimeout(sockfd, (void *)MsgConfig64, sizeof(MsgConfig64), 0, NULL, NULL); 
	ALOGD("hdd_get_hdd_info(): recvfrom --> rcvcnt=[%d], MsgConfig64[0]= [%d], [1]=[%d], [2]=[%d]\n", rcvcnt, MsgConfig64[0], MsgConfig64[1], MsgConfig64[2]);
	
	if (rcvcnt > 0  && MsgConfig64[2])
	{
		rcvcnt = recvfromTimeout(sockfd, (void *)&tempHDDInfo, sizeof(tempHDDInfo), 0, NULL, NULL); 

		if (rcvcnt != sizeof(tempHDDInfo))
			memset(&tempHDDInfo, 0, sizeof(tempHDDInfo));
	}

	return (HDDInfoStruct*)&tempHDDInfo;
}

uint8_t hdd_get_rear_front()
{
	memset(MsgConfig64, 0, 64);
	memset(MsgConfig256, 0, 256);

	MsgConfig256[0] = 1;
	MsgConfig256[1] = HDD_GET_REAR_FRONT;
	MsgConfig256[2] = 1;

	int sndcnt = sendto(txSockfd, (void *)MsgConfig256, sizeof(MsgConfig256), 0, (struct sockaddr *)&txServeraddr, txClilen);
	ALOGD("hdd_get_rear_front(): sendto   --> sndcnt=[%d], MsgConfig256[0]=[%d], [1]=[%d], [2]=[%d]\n", sndcnt, MsgConfig256[0], MsgConfig256[1], MsgConfig256[2]);

	int rcvcnt = recvfromTimeout(sockfd, (void *)MsgConfig64, sizeof(MsgConfig64), 0, NULL, NULL); 
	ALOGD("hdd_get_rear_front(): recvfrom --> rcvcnt=[%d], MsgConfig64[0]=[%d], [1]=[%d], [2]=[%d]\n", rcvcnt, MsgConfig64[0], MsgConfig64[1], MsgConfig64[2]);
	
	if (rcvcnt > 0)
		return MsgConfig64[2];
	
	return 0; 
}

uint8_t hdd_format()
{
	memset(MsgConfig64, 0, 64);
	memset(MsgConfig256, 0, 256);

	MsgConfig256[0] = 1;
	MsgConfig256[1] = HDD_FORMAT;
	MsgConfig256[2] = 1;

	int sndcnt = sendto(txSockfd, (void *)MsgConfig256, sizeof(MsgConfig256), 0, (struct sockaddr *)&txServeraddr, txClilen);
	ALOGD("hdd_format(): sendto   --> sndcnt=[%d], MsgConfig256[0]=[%d], [1]=[%d], [2]=[%d]\n", sndcnt, MsgConfig256[0], MsgConfig256[1], MsgConfig256[2]);

	int rcvcnt = recvfromTimeout(sockfd, (void *)MsgConfig64, sizeof(MsgConfig64), 0, NULL, NULL); 
	ALOGD("hdd_format(): recvfrom --> rcvcnt=[%d], MsgConfig64[0]= [%d], [1]=[%d], [2]=[%d]\n", rcvcnt, MsgConfig64[0], MsgConfig64[1], MsgConfig64[2]);

	if (rcvcnt > 0)
		return MsgConfig64[2];

	return 0;
}

long long hdd_used_size()
{
	memset(MsgConfig64, 0, 64);
	memset(MsgConfig256, 0, 256);

	MsgConfig256[0] = 1;
	MsgConfig256[1] = HDD_USED_SIZE;
	MsgConfig256[2] = 1;

	int sndcnt = sendto(txSockfd, (void *)MsgConfig256, sizeof(MsgConfig256), 0, (struct sockaddr *)&txServeraddr, txClilen);
	ALOGD("hdd_used_size(): sendto   --> sndcnt=[%d], MsgConfig256[0]=[%d], [1]=[%d], [2]=[%d]\n", sndcnt, MsgConfig256[0], MsgConfig256[1], MsgConfig256[2]);

	int rcvcnt = recvfromTimeout(sockfd, (void *)MsgConfig64, sizeof(MsgConfig64), 0, NULL, NULL); 
	ALOGD("hdd_used_size(): recvfrom --> rcvcnt=[%d], MsgConfig64[0]= [%d], [1]=[%d], [2]=[%d]\n", rcvcnt, MsgConfig64[0], MsgConfig64[1], MsgConfig64[2]);

	/*
	long long a, b, c, d, e, f, g, h, ret;
	a = MsgConfig64[2]; a = (a << 56) & 0xFF00000000000000;
	b = MsgConfig64[3]; b = (b << 48) & 0x00FF000000000000;
	c = MsgConfig64[4]; c = (c << 40) & 0x0000FF0000000000;
	d = MsgConfig64[5]; d = (d << 32) & 0x000000FF00000000;
	e = MsgConfig64[6]; e = (e << 24) & 0x00000000FF000000;
	f = MsgConfig64[7]; f = (f << 16) & 0x0000000000FF0000;
	g = MsgConfig64[8]; g = (g <<  8) & 0x000000000000FF00;
	h = MsgConfig64[9]; h =  h        & 0x00000000000000FF;
	ret = a + b + c + d + e + f + g + h;
	*/
	long long ret;

	if (rcvcnt > 0)
		memcpy(&ret, &MsgConfig64[2], 8);
	else
		memset(&ret, 0, 8);

	//ALOGD("hdd_used_size(): ret=[%lld]\n", ret);
	return ret;
}

long long hdd_free_size()
{
	memset(MsgConfig64, 0, 64);
	memset(MsgConfig256, 0, 256);

	MsgConfig256[0] = 1;
	MsgConfig256[1] = HDD_FREE_SIZE;
	MsgConfig256[2] = 1;

	int sndcnt = sendto(txSockfd, (void *)MsgConfig256, sizeof(MsgConfig256), 0, (struct sockaddr *)&txServeraddr, txClilen);
	ALOGD("hdd_free_size(): sendto   --> sndcnt=[%d], MsgConfig256[0]=[%d], [1]=[%d], [2]=[%d]\n", sndcnt, MsgConfig256[0], MsgConfig256[1], MsgConfig256[2]);

	int rcvcnt = recvfromTimeout(sockfd, (void *)MsgConfig64, sizeof(MsgConfig64), 0, NULL, NULL); 
	ALOGD("hdd_free_size(): recvfrom --> rcvcnt=[%d], MsgConfig64[0]= [%d], [1]=[%d], [2]=[%d]\n", rcvcnt, MsgConfig64[0], MsgConfig64[1], MsgConfig64[2]);

	/*
	long long a, b, c, d, e, f, g, h, ret;
	a = MsgConfig64[2]; a = (a << 56) & 0xFF00000000000000;
	b = MsgConfig64[3]; b = (b << 48) & 0x00FF000000000000;
	c = MsgConfig64[4]; c = (c << 40) & 0x0000FF0000000000;
	d = MsgConfig64[5]; d = (d << 32) & 0x000000FF00000000;
	e = MsgConfig64[6]; e = (e << 24) & 0x00000000FF000000;
	f = MsgConfig64[7]; f = (f << 16) & 0x0000000000FF0000;
	g = MsgConfig64[8]; g = (g <<  8) & 0x000000000000FF00;
	h = MsgConfig64[9]; h =  h        & 0x00000000000000FF;
	ret = a + b + c + d + e + f + g + h;
	*/
	long long ret;

	if (rcvcnt > 0)
		memcpy(&ret, &MsgConfig64[2], 8);
	else
		memset(&ret, 0, 8);

	//ALOGD("hdd_free_size(): ret=[%lld]\n", ret);
	return ret;
}

uint8_t hdd_mount()
{
	memset(MsgConfig64, 0, 64);
	memset(MsgConfig256, 0, 256);

	MsgConfig256[0] = 1;
	MsgConfig256[1] = HDD_MOUNT;
	MsgConfig256[2] = 1;

	int sndcnt = sendto(txSockfd, (void *)MsgConfig256, sizeof(MsgConfig256), 0, (struct sockaddr *)&txServeraddr, txClilen);
	ALOGD("hdd_mount(): sendto   --> sndcnt=[%d], MsgConfig256[0]=[%d], [1]=[%d], [2]=[%d]\n", sndcnt, MsgConfig256[0], MsgConfig256[1], MsgConfig256[2]);

	int rcvcnt = recvfromTimeout(sockfd, (void *)MsgConfig64, sizeof(MsgConfig64), 0, NULL, NULL); 
	ALOGD("hdd_mount(): recvfrom --> rcvcnt=[%d], MsgConfig64[0]= [%d], [1]=[%d], [2]=[%d]\n", rcvcnt, MsgConfig64[0], MsgConfig64[1], MsgConfig64[2]);

	if (rcvcnt > 0) 
		return MsgConfig64[2];
		
	return 0;
}

uint8_t hdd_unmount()
{
	memset(MsgConfig64, 0, 64);
	memset(MsgConfig256, 0, 256);

	MsgConfig256[0] = 1;
	MsgConfig256[1] = HDD_UNMOUNT;
	MsgConfig256[2] = 1;

	int sndcnt = sendto(txSockfd, (void *)MsgConfig256, sizeof(MsgConfig256), 0, (struct sockaddr *)&txServeraddr, txClilen);
	ALOGD("hdd_unmount(): sendto   --> sndcnt=[%d], MsgConfig256[0]=[%d], [1]=[%d], [2]=[%d]\n", sndcnt, MsgConfig256[0], MsgConfig256[1], MsgConfig256[2]);

	int rcvcnt = recvfromTimeout(sockfd, (void *)MsgConfig64, sizeof(MsgConfig64), 0, NULL, NULL); 
	ALOGD("hdd_unmount(): recvfrom --> rcvcnt=[%d], MsgConfig64[0]= [%d], [1]=[%d], [2]=[%d]\n", rcvcnt, MsgConfig64[0], MsgConfig64[1], MsgConfig64[2]);

	if (rcvcnt > 0)
		return MsgConfig64[2];
	
	return 0;
}

uint8_t file_canRead(char* filename)
{
	memset(MsgConfig64, 0, 64);
	memset(MsgConfig256, 0, 256);

	MsgConfig256[0] = 1;
	MsgConfig256[1] = HDD_FILE_CANREAD;
	strcpy((char*)&MsgConfig256[2], filename);

	int sndcnt = sendto(txSockfd, (void *)MsgConfig256, sizeof(MsgConfig256), 0, (struct sockaddr *)&txServeraddr, txClilen);
	ALOGD("file_canRead: sendto   --> sndcnt=[%d], MsgConfig256[0]=[%d], [1]=[%d], [2]=[%d], filename[%s]\n", sndcnt, MsgConfig256[0], MsgConfig256[1], MsgConfig256[2], filename);

	int rcvcnt = recvfromTimeout(sockfd, (void *)MsgConfig64, sizeof(MsgConfig64), 0, NULL, NULL); 
	ALOGD("file_canRead: recvfrom --> rcvcnt=[%d], MsgConfig64[0]= [%d], [1]=[%d], [2]=[%d]\n", rcvcnt, MsgConfig64[0], MsgConfig64[1], MsgConfig64[2]);

	if (rcvcnt > 0)
		return MsgConfig64[2];
	
	return 0;
}

uint8_t file_canWrite(char* filename)
{
	memset(MsgConfig64, 0, 64);
	memset(MsgConfig256, 0, 256);

	MsgConfig256[0] = 1;
	MsgConfig256[1] = HDD_FILE_CANWRITE;
	strcpy((char*)&MsgConfig256[2], filename);

	int sndcnt = sendto(txSockfd, (void *)MsgConfig256, sizeof(MsgConfig256), 0, (struct sockaddr *)&txServeraddr, txClilen);
	ALOGD("file_canWrite: sendto   --> sndcnt=[%d], MsgConfig256[0]=[%d], [1]=[%d], [2]=[%d], filename[%s]\n", sndcnt, MsgConfig256[0], MsgConfig256[1], MsgConfig256[2], filename);

	int rcvcnt = recvfromTimeout(sockfd, (void *)MsgConfig64, sizeof(MsgConfig64), 0, NULL, NULL); 
	ALOGD("file_canWrite: recvfrom --> rcvcnt=[%d], MsgConfig64[0]= [%d], [1]=[%d], [2]=[%d]\n", rcvcnt, MsgConfig64[0], MsgConfig64[1], MsgConfig64[2]);

	if (rcvcnt > 0)
		return MsgConfig64[2];
	
	return 0;
}

uint8_t file_exists(char* filename)
{
	memset(MsgConfig64, 0, 64);
	memset(MsgConfig256, 0, 256);

	MsgConfig256[0] = 1;
	MsgConfig256[1] = HDD_FILE_EXISTS;
	strcpy((char*)&MsgConfig256[2], filename);

	int sndcnt = sendto(txSockfd, (void *)MsgConfig256, sizeof(MsgConfig256), 0, (struct sockaddr *)&txServeraddr, txClilen);
	ALOGD("file_exists: sendto   --> sndcnt=[%d], MsgConfig256[0]=[%d], [1]=[%d], [2]=[%d], filename[%s]\n", sndcnt, MsgConfig256[0], MsgConfig256[1], MsgConfig256[2], filename);

	int rcvcnt = recvfromTimeout(sockfd, (void *)MsgConfig64, sizeof(MsgConfig64), 0, NULL, NULL); 
	ALOGD("file_exists: recvfrom --> rcvcnt=[%d], MsgConfig64[0]= [%d], [1]=[%d], [2]=[%d]\n", rcvcnt, MsgConfig64[0], MsgConfig64[1], MsgConfig64[2]);

	if (rcvcnt > 0)
		return MsgConfig64[2];
	
	return 0;
}

uint8_t file_mkdir(char* pathname)
{
	memset(MsgConfig64, 0, 64);
	memset(MsgConfig256, 0, 256);

	MsgConfig256[0] = 1;
	MsgConfig256[1] = HDD_FILE_MKDIR;
	strcpy((char*)&MsgConfig256[2], pathname);

	int sndcnt = sendto(txSockfd, (void *)MsgConfig256, sizeof(MsgConfig256), 0, (struct sockaddr *)&txServeraddr, txClilen);
	ALOGD("file_mkdir: sendto   --> sndcnt=[%d], MsgConfig256[0]=[%d], [1]=[%d], [2]=[%d], pathname[%s]\n", sndcnt, MsgConfig256[0], MsgConfig256[1], MsgConfig256[2], pathname);

	int rcvcnt = recvfromTimeout(sockfd, (void *)MsgConfig64, sizeof(MsgConfig64), 0, NULL, NULL); 
	ALOGD("file_mkdir: recvfrom --> rcvcnt=[%d], MsgConfig64[0]=[%d], [1]=[%d], [2]=[%d]\n", rcvcnt, MsgConfig64[0], MsgConfig64[1], MsgConfig64[2]);

	if (rcvcnt > 0)
		return MsgConfig64[2];
	
	return 0;
}

uint8_t file_mkdirs(char* pathname)
{
	memset(MsgConfig64, 0, 64);
	memset(MsgConfig256, 0, 256);

	MsgConfig256[0] = 1;
	MsgConfig256[1] = HDD_FILE_MKDIRS;
	strcpy((char*)&MsgConfig256[2], pathname);

	int sndcnt = sendto(txSockfd, (void *)MsgConfig256, sizeof(MsgConfig256), 0, (struct sockaddr *)&txServeraddr, txClilen);
	ALOGD("file_mkdirs: sendto   --> sndcnt=[%d], MsgConfig256[0]=[%d], [1]=[%d], [2]=[%d], pathname[%s]\n", sndcnt, MsgConfig256[0], MsgConfig256[1], MsgConfig256[2], pathname);

	int rcvcnt = recvfromTimeout(sockfd, (void *)MsgConfig64, sizeof(MsgConfig64), 0, NULL, NULL); 
	ALOGD("file_mkdirs: recvfrom --> rcvcnt=[%d], MsgConfig64[0]=[%d], [1]=[%d], [2]=[%d]\n", rcvcnt, MsgConfig64[0], MsgConfig64[1], MsgConfig64[2]);

	if (rcvcnt > 0)
		return MsgConfig64[2];
	
	return 0;
}

uint8_t file_renameTo(char* src_filename, char* dest_filename)
{
	memset(MsgConfig64, 0, 64);
	memset(MsgConfig256, 0, 256);

	MsgConfig256[0] = 1;
	MsgConfig256[1] = HDD_FILE_RENAMETO;
	memcpy(&MsgConfig256[2], src_filename, 100);
	memcpy(&MsgConfig256[103], dest_filename, 100);

	int sndcnt = sendto(txSockfd, (void *)MsgConfig256, sizeof(MsgConfig256), 0, (struct sockaddr *)&txServeraddr, txClilen);
	ALOGD("file_renameTo: sendto   --> sndcnt=[%d], MsgConfig256[0]=[%d], [1]=[%d], [2]=[%d], src_filename[%s], dest_filename[%s]\n", sndcnt, MsgConfig256[0], MsgConfig256[1], MsgConfig256[2], src_filename, dest_filename);

	int rcvcnt = recvfromTimeout(sockfd, (void *)MsgConfig64, sizeof(MsgConfig64), 0, NULL, NULL); 
	ALOGD("file_renameTo: recvfrom --> rcvcnt=[%d], MsgConfig64[0]=[%d], [1]=[%d], [2]=[%d]\n", rcvcnt, MsgConfig64[0], MsgConfig64[1], MsgConfig64[2]);

	if (rcvcnt > 0)
		return MsgConfig64[2];
	
	return 0;
}

uint8_t file_newFile(char* pathname)
{
	memset(MsgConfig64, 0, 64);
	memset(MsgConfig256, 0, 256);

	MsgConfig256[0] = 1;
	MsgConfig256[1] = HDD_FILE_NEWFILE;
	strcpy((char*)&MsgConfig256[2], pathname);

	int sndcnt = sendto(txSockfd, (void *)MsgConfig256, sizeof(MsgConfig256), 0, (struct sockaddr *)&txServeraddr, txClilen);
	ALOGD("file_newFile: sendto   --> sndcnt=[%d], MsgConfig256[0]=[%d], [1]=[%d], [2]=[%d], pathname[%s]\n", sndcnt, MsgConfig256[0], MsgConfig256[1], MsgConfig256[2], pathname);

	int rcvcnt = recvfromTimeout(sockfd, (void *)MsgConfig64, sizeof(MsgConfig64), 0, NULL, NULL); 
	ALOGD("file_newFile: recvfrom --> rcvcnt=[%d], MsgConfig64[0]=[%d], [1]=[%d], [2]=[%d]\n", rcvcnt, MsgConfig64[0], MsgConfig64[1], MsgConfig64[2]);

	if (rcvcnt > 0)
		return MsgConfig64[2];

	return 0;
}

uint8_t file_newPathFile(char* pathname, char* filename)
{
	memset(MsgConfig64, 0, 64);
	memset(MsgConfig256, 0, 256);

	MsgConfig256[0] = 1;
	MsgConfig256[1] = HDD_FILE_NEWPATHFILE;
	memcpy(&MsgConfig256[2], pathname, 100);
	memcpy(&MsgConfig256[103], filename, 100);

	int sndcnt = sendto(txSockfd, (void *)MsgConfig256, sizeof(MsgConfig256), 0, (struct sockaddr *)&txServeraddr, txClilen);
	ALOGD("file_newPathFile: sendto   --> sndcnt=[%d], MsgConfig256[0]=[%d], [1]=[%d], [2]=[%d], pathname[%s], filename[%s]\n", sndcnt, MsgConfig256[0], MsgConfig256[1], MsgConfig256[2], pathname, filename);

	int rcvcnt = recvfromTimeout(sockfd, (void *)MsgConfig64, sizeof(MsgConfig64), 0, NULL, NULL); 
	ALOGD("file_newPathFile: recvfrom --> rcvcnt=[%d], MsgConfig64[0]=[%d], [1]=[%d], [2]=[%d]\n", rcvcnt, MsgConfig64[0], MsgConfig64[1], MsgConfig64[2]);

	if (rcvcnt > 0)
		return MsgConfig64[2];
	
	return 0;
}

uint8_t file_delete(char* pathname)
{
	memset(MsgConfig64, 0, 64);
	memset(MsgConfig256, 0, 256);

	MsgConfig256[0] = 1;
	MsgConfig256[1] = HDD_FILE_DELETE;
	strcpy((char*)&MsgConfig256[2], pathname);

	int sndcnt = sendto(txSockfd, (void *)MsgConfig256, sizeof(MsgConfig256), 0, (struct sockaddr *)&txServeraddr, txClilen);
	ALOGD("file_delete: sendto   --> sndcnt=[%d], MsgConfig256[0]=[%d], [1]=[%d], [2]=[%d], pathname[%s]\n", sndcnt, MsgConfig256[0], MsgConfig256[1], MsgConfig256[2], pathname);

	int rcvcnt = recvfromTimeout(sockfd, (void *)MsgConfig64, sizeof(MsgConfig64), 0, NULL, NULL); 
	ALOGD("file_delete: recvfrom --> rcvcnt=[%d], MsgConfig64[0]=[%d], [1]=[%d], [2]=[%d]\n", rcvcnt, MsgConfig64[0], MsgConfig64[1], MsgConfig64[2]);

	if (rcvcnt > 0)
		return MsgConfig64[2];
	
	return 0;
}

uint8_t file_isDirectory(char* pathname)
{
	memset(MsgConfig64, 0, 64);
	memset(MsgConfig256, 0, 256);

	MsgConfig256[0] = 1;
	MsgConfig256[1] = HDD_FILE_ISDIRECTORY;
	strcpy((char*)&MsgConfig256[2], pathname);

	int sndcnt = sendto(txSockfd, (void *)MsgConfig256, sizeof(MsgConfig256), 0, (struct sockaddr *)&txServeraddr, txClilen);
	ALOGD("file_isDirectory: sendto   --> sndcnt=[%d], MsgConfig256[0]=[%d], [1]=[%d], [2]=[%d], pathname[%s]\n", sndcnt, MsgConfig256[0], MsgConfig256[1], MsgConfig256[2], pathname);

	int rcvcnt = recvfromTimeout(sockfd, (void *)MsgConfig64, sizeof(MsgConfig64), 0, NULL, NULL); 
	ALOGD("file_isDirectory: recvfrom --> rcvcnt=[%d], MsgConfig64[0]= [%d], [1]=[%d], [2]=[%d]\n", rcvcnt, MsgConfig64[0], MsgConfig64[1], MsgConfig64[2]);
	
	if (rcvcnt > 0)
		return MsgConfig64[2];
	
	return 0;
}

uint8_t file_isFile(char* filename)
{
	memset(MsgConfig64, 0, 64);
	memset(MsgConfig256, 0, 256);

	MsgConfig256[0] = 1;
	MsgConfig256[1] = HDD_FILE_ISFILE;
	strcpy((char*)&MsgConfig256[2], filename);

	int sndcnt = sendto(txSockfd, (void *)MsgConfig256, sizeof(MsgConfig256), 0, (struct sockaddr *)&txServeraddr, txClilen);
	ALOGD("file_isFile: sendto   --> sndcnt=[%d], MsgConfig256[0]=[%d], [1]=[%d], [2]=[%d], filename[%s]\n", sndcnt, MsgConfig256[0], MsgConfig256[1], MsgConfig256[2], filename);

	int rcvcnt = recvfromTimeout(sockfd, (void *)MsgConfig64, sizeof(MsgConfig64), 0, NULL, NULL); 
	ALOGD("file_isFile: recvfrom --> rcvcnt=[%d], MsgConfig64[0]= [%d], [1]=[%d], [2]=[%d]\n", rcvcnt, MsgConfig64[0], MsgConfig64[1], MsgConfig64[2]);

	if (rcvcnt > 0)
		return MsgConfig64[2];
	
	return 0;
}

FileList* file_listFiles(char* pathname)
{
	int i;
	static FileList filelist;

	filelist.file_cnt = 0;
	for (i = 0; i < 256; i++)
	{
		memset(filelist.filearr[i].filename, 0, 100);
	}

	memset(MsgConfig256, 0, 256);

	MsgConfig256[0] = 1;
	MsgConfig256[1] = HDD_FILE_LISTFILES;
	strcpy((char*)&MsgConfig256[2], pathname);

	int sndcnt = sendto(txSockfd, (void *)MsgConfig256, sizeof(MsgConfig256), 0, (struct sockaddr *)&txServeraddr, txClilen);
	ALOGD("file_listFiles: sendto   --> sndcnt=[%d], MsgConfig256[0]=[%d], [1]=[%d], [2]=[%d], pathname[%s]\n", sndcnt, MsgConfig256[0], MsgConfig256[1], MsgConfig256[2], pathname);

	int rcvcnt = recvfromTimeout(sockfd, (void *)MsgConfig256, sizeof(MsgConfig256), 0, NULL, NULL); 
	ALOGD("file_listFiles: recvfrom --> rcvcnt=[%d], MsgConfig256[0]=[%d], [1]=[%d], [2]=[%d]\n", rcvcnt, MsgConfig256[0], MsgConfig256[1], MsgConfig256[2]);

	if (rcvcnt > 0)
	{
		if (MsgConfig256[0] == 0 && MsgConfig256[1] == HDD_FILE_LISTFILES)
			filelist.file_cnt = MsgConfig256[2];	

		for (i = 0; i < filelist.file_cnt; i++)
		{
			rcvcnt = recvfromTimeout(sockfd, (void *)MsgConfig256, sizeof(MsgConfig256), 0, NULL, NULL); 
			ALOGD("file_listFiles: recvfrom --> rcvcnt=[%d], MsgConfig256[0]=[%d], [1]=[%d], [2]=[%d]\n", rcvcnt, MsgConfig256[0], MsgConfig256[1], MsgConfig256[2]);
			
			if (rcvcnt > 0)
			{
				if (MsgConfig256[0] > 0 && MsgConfig256[1] == HDD_FILE_LISTFILES)
					strcpy(filelist.filearr[MsgConfig256[0]-1].filename, (char*)&MsgConfig256[2]);
			}
			else
			{
				filelist.file_cnt = i; 
				break;
			}
		}
	}

	return (FileList*)&filelist;
}

char* file_getParent(char* pathname)
{
	static char parentpathname[256];

	memset(parentpathname, 0, 256);

	memset(MsgConfig256, 0, 256);

	MsgConfig256[0] = 1;
	MsgConfig256[1] = HDD_FILE_GETPARENT;
	strcpy((char*)&MsgConfig256[2], pathname);

	int sndcnt = sendto(txSockfd, (void *)MsgConfig256, sizeof(MsgConfig256), 0, (struct sockaddr *)&txServeraddr, txClilen);
	ALOGD("file_getParent: sendto   --> sndcnt=[%d], MsgConfig256[0]=[%d], [1]=[%d], [2]=[%d], pathname[%s]\n", sndcnt, MsgConfig256[0], MsgConfig256[1], MsgConfig256[2], pathname);

	int rcvcnt = recvfromTimeout(sockfd, (void *)MsgConfig256, sizeof(MsgConfig256), 0, NULL, NULL); 
	ALOGD("file_getParent: recvfrom --> rcvcnt=[%d], MsgConfig256[0]=[%d], [1]=[%d], [2]=[%d]\n", rcvcnt, MsgConfig256[0], MsgConfig256[1], MsgConfig256[2]);

	if (rcvcnt > 0)
	{
		if (MsgConfig256[1] == HDD_FILE_GETPARENT && MsgConfig256[2] == 1)
			strcpy(parentpathname, (char*)&MsgConfig256[3]);
	}

	return parentpathname;
}

char* file_getName(char* filename)
{
	static char retfilename[256];

	memset(retfilename, 0, 256);

	memset(MsgConfig256, 0, 256);

	MsgConfig256[0] = 1;
	MsgConfig256[1] = HDD_FILE_GETNAME;
	strcpy((char*)&MsgConfig256[2], filename);

	int sndcnt = sendto(txSockfd, (void *)MsgConfig256, sizeof(MsgConfig256), 0, (struct sockaddr *)&txServeraddr, txClilen);
	ALOGD("file_getName: sendto   --> sndcnt=[%d], MsgConfig256[0]=[%d], [1]=[%d], [2]=[%d], filename[%s]\n", sndcnt, MsgConfig256[0], MsgConfig256[1], MsgConfig256[2], filename);

	int rcvcnt = recvfromTimeout(sockfd, (void *)MsgConfig256, sizeof(MsgConfig256), 0, NULL, NULL); 
	ALOGD("file_getName: recvfrom --> rcvcnt=[%d], MsgConfig256[0]=[%d], [1]=[%d], [2]=[%d]\n", rcvcnt, MsgConfig256[0], MsgConfig256[1], MsgConfig256[2]);

	if (rcvcnt > 0)
	{
		if (MsgConfig256[1] == HDD_FILE_GETNAME && MsgConfig256[2] == 1)
			strcpy(retfilename, (char*)&MsgConfig256[3]);
	}

	return retfilename;
}

char* file_getPath(char* filename)
{
	static char retpathname[256];

	memset(retpathname, 0, 256);

	memset(MsgConfig256, 0, 256);

	MsgConfig256[0] = 1;
	MsgConfig256[1] = HDD_FILE_GETPATH;
	strcpy((char*)&MsgConfig256[2], filename);

	int sndcnt = sendto(txSockfd, (void *)MsgConfig256, sizeof(MsgConfig256), 0, (struct sockaddr *)&txServeraddr, txClilen);
	ALOGD("file_getPath: sendto   --> sndcnt=[%d], MsgConfig256[0]=[%d], [1]=[%d], [2]=[%d], filename[%s]\n", sndcnt, MsgConfig256[0], MsgConfig256[1], MsgConfig256[2], filename);

	int rcvcnt = recvfromTimeout(sockfd, (void *)MsgConfig256, sizeof(MsgConfig256), 0, NULL, NULL); 
	ALOGD("file_getPath: recvfrom --> rcvcnt=[%d], MsgConfig256[0]=[%d], [1]=[%d], [2]=[%d]\n", rcvcnt, MsgConfig256[0], MsgConfig256[1], MsgConfig256[2]);

	if (rcvcnt > 0)
	{
		if (MsgConfig256[1] == HDD_FILE_GETPATH && MsgConfig256[2] == 1)
			strcpy(retpathname, (char*)&MsgConfig256[3]);
	}

	return retpathname;
}

long file_lastModified(char* filename)
{
	memset(MsgConfig64, 0, 64);
	memset(MsgConfig256, 0, 256);

	MsgConfig256[0] = 1;
	MsgConfig256[1] = HDD_FILE_LASTMODIFIED;
	strcpy((char*)&MsgConfig256[2], filename);

	int sndcnt = sendto(txSockfd, (void *)MsgConfig256, sizeof(MsgConfig256), 0, (struct sockaddr *)&txServeraddr, txClilen);
	ALOGD("file_lastModified: sendto   --> sndcnt=[%d], MsgConfig256[0]=[%d], [1]=[%d], [2]=[%d], filename[%s]\n", sndcnt, MsgConfig256[0], MsgConfig256[1], MsgConfig256[2], filename);

	int rcvcnt = recvfromTimeout(sockfd, (void *)MsgConfig64, sizeof(MsgConfig64), 0, NULL, NULL); 
	ALOGD("file_lastModified: recvfrom --> rcvcnt=[%d], MsgConfig64[0]=[%d], [1]=[%d], [2]=[%d]\n", rcvcnt, MsgConfig64[0], MsgConfig64[1], MsgConfig64[2]);

	if (rcvcnt > 0)
	{
		if (MsgConfig64[1] == HDD_FILE_LASTMODIFIED && MsgConfig64[2] == 1)
		{
			long a, b, c, d;
			a = (MsgConfig64[3] << 24) & 0xFF000000;
			b = (MsgConfig64[4] << 16) & 0x00FF0000;
			c = (MsgConfig64[5] <<  8) & 0x0000FF00;
			d = (MsgConfig64[6])	   & 0x000000FF;
			return (a + b + c + d);
		}
	}

	return 0L;
}

uint8_t file_changeMode(uint16_t change_mode, char* filename)
{
	memset(MsgConfig64, 0, 64);
	memset(MsgConfig256, 0, 256);

	MsgConfig256[0] = 1;
	MsgConfig256[1] = HDD_FILE_CHANGE_MODE;
	MsgConfig256[2] = (change_mode & 0xFF00) >> 8;
	MsgConfig256[3] = (change_mode & 0x00FF);
	strcpy((char*)&MsgConfig256[4], filename);

	int sndcnt = sendto(txSockfd, (void *)MsgConfig256, sizeof(MsgConfig256), 0, (struct sockaddr *)&txServeraddr, txClilen);
	ALOGD("file_changeMode: sendto   --> sndcnt=[%d], MsgConfig256[0]=[%d], [1]=[%d], [2]=[%d], change_mode[%d], filename[%s]\n", sndcnt, MsgConfig256[0], MsgConfig256[1], MsgConfig256[2], change_mode, filename);

	int rcvcnt = recvfromTimeout(sockfd, (void *)MsgConfig64, sizeof(MsgConfig64), 0, NULL, NULL); 
	ALOGD("file_changeMode: recvfrom --> rcvcnt=[%d], MsgConfig64[0]=[%d], [1]=[%d], [2]=[%d]\n", rcvcnt, MsgConfig64[0], MsgConfig64[1], MsgConfig64[2]);
	
	if (rcvcnt > 0)
		return MsgConfig64[2];
	
	return 0;
}

long file_fileSize(char* filename)
{
	long ret_st_size = -1L;

	memset(MsgConfig64, 0, 64);
	memset(MsgConfig256, 0, 256);

	MsgConfig256[0] = 1;
	MsgConfig256[1] = HDD_FILE_FILE_SIZE;
	strcpy((char*)&MsgConfig256[2], filename);

	int sndcnt = sendto(txSockfd, (void *)MsgConfig256, sizeof(MsgConfig256), 0, (struct sockaddr *)&txServeraddr, txClilen);
	ALOGD("file_fileSize: sendto   --> sndcnt=[%d], MsgConfig256[0]=[%d], [1]=[%d], [2]=[%d], filename[%s]\n", sndcnt, MsgConfig256[0], MsgConfig256[1], MsgConfig256[2], filename);

	int rcvcnt = recvfromTimeout(sockfd, (void *)MsgConfig64, sizeof(MsgConfig64), 0, NULL, NULL); 
	ALOGD("file_fileSize: recvfrom --> rcvcnt=[%d], MsgConfig64[0]=[%d], [1]=[%d], [2]=[%d]\n", rcvcnt, MsgConfig64[0], MsgConfig64[1], MsgConfig64[2]);

	if (rcvcnt > 0)
	{
		if (MsgConfig64[1] == HDD_FILE_FILE_SIZE && MsgConfig64[2] != 0)
		{
			long a, b, c, d;
			a = (MsgConfig64[3] << 24) & 0xFF000000;
			b = (MsgConfig64[4] << 16) & 0x00FF0000;
			c = (MsgConfig64[5] <<  8) & 0x0000FF00;
			d = (MsgConfig64[6]      ) & 0x000000FF;
			ret_st_size = a + b + c + d;
		}
	}

	ALOGD("file_fileSize: ret_st_size[%ld]", ret_st_size);

	return ret_st_size;
}


uint8_t eth_renew(char* ethstr)
{
	memset(MsgConfig64, 0, 64);
	memset(MsgConfig256, 0, 256);

	MsgConfig256[0] = 1;
	MsgConfig256[1] = ETH_RENEW;
	strcpy((char*)&MsgConfig256[2], ethstr);

	int sndcnt = sendto(txSockfd, (void *)MsgConfig256, sizeof(MsgConfig256), 0, (struct sockaddr *)&txServeraddr, txClilen);
	ALOGD("file_fileSize: sendto   --> sndcnt=[%d], MsgConfig256[0]=[%d], [1]=[%d], [2]=[%d], ethstr[%s]\n", sndcnt, MsgConfig256[0], MsgConfig256[1], MsgConfig256[2], ethstr);

	int rcvcnt = recvfromTimeout(sockfd, (void *)MsgConfig64, sizeof(MsgConfig64), 0, NULL, NULL); 
	ALOGD("file_fileSize: recvfrom --> rcvcnt=[%d], MsgConfig64[0]=[%d], [1]=[%d], [2]=[%d]\n", rcvcnt, MsgConfig64[0], MsgConfig64[1], MsgConfig64[2]);

	if (rcvcnt > 0)
		return MsgConfig64[2];
	
	return 0;
}


uint8_t wifi_getWifiStatusLed()
{
	memset(MsgConfig64, 0, 64);
	memset(MsgConfig256, 0, 256);

	MsgConfig256[0] = 1;
	MsgConfig256[1] = WIFI_GET_WIFI_STATUS_LED;

	int sndcnt = sendto(txSockfd, (void *)MsgConfig256, sizeof(MsgConfig256), 0, (struct sockaddr *)&txServeraddr, txClilen);
	ALOGD("wifi_setWifiStatusLed: sendto   --> sndcnt=[%d], MsgConfig256[0]=[%d], [1]=[%d], [2]=[%d]\n", sndcnt, MsgConfig256[0], MsgConfig256[1], MsgConfig256[2]);

	int rcvcnt = recvfromTimeout(sockfd, (void *)MsgConfig64, sizeof(MsgConfig64), 0, NULL, NULL); 
	ALOGD("wifi_setWifiStatusLed: recvfrom --> rcvcnt=[%d], MsgConfig64[0]=[%d], [1]=[%d], [2]=[%d]\n", rcvcnt, MsgConfig64[0], MsgConfig64[1], MsgConfig64[2]);

	if (rcvcnt > 0)
		return MsgConfig64[2];
	
	return 0;
}

uint8_t wifi_setWifiStatusLed(uint8_t wifi_status)
{
	memset(MsgConfig64, 0, 64);
	memset(MsgConfig256, 0, 256);

	MsgConfig256[0] = 1;
	MsgConfig256[1] = WIFI_SET_WIFI_STATUS_LED;
	MsgConfig256[2] = wifi_status;

	int sndcnt = sendto(txSockfd, (void *)MsgConfig256, sizeof(MsgConfig256), 0, (struct sockaddr *)&txServeraddr, txClilen);
	ALOGD("wifi_setWifiStatusLed: sendto   --> sndcnt=[%d], MsgConfig256[0]=[%d], [1]=[%d], [2]=[%d], wifi_status[%d]\n", sndcnt, MsgConfig256[0], MsgConfig256[1], MsgConfig256[2], wifi_status);

	int rcvcnt = recvfromTimeout(sockfd, (void *)MsgConfig64, sizeof(MsgConfig64), 0, NULL, NULL); 
	ALOGD("wifi_setWifiStatusLed: recvfrom --> rcvcnt=[%d], MsgConfig64[0]=[%d], [1]=[%d], [2]=[%d]\n", rcvcnt, MsgConfig64[0], MsgConfig64[1], MsgConfig64[2]);

	if (rcvcnt > 0)
		return MsgConfig64[2];
	
	return 0;
}


uint8_t setDocsisStatusLed(uint8_t docsis_status)
{
	memset(MsgConfig64, 0, 64);
	memset(MsgConfig256, 0, 256);

	MsgConfig256[0] = 1;
	MsgConfig256[1] = ECM_SET_DOCSIS_STATUS_LED;
	MsgConfig256[2] = docsis_status;

	int sndcnt = sendto(txSockfd, (void *)MsgConfig256, sizeof(MsgConfig256), 0, (struct sockaddr *)&txServeraddr, txClilen);
	ALOGD("sendto   --> sndcnt=[%d], MsgConfig256[0]=[%d], [1]=[%d], [2]=[%d], docsis_status[%d]\n", sndcnt, MsgConfig256[0], MsgConfig256[1], MsgConfig256[2], docsis_status);

	int rcvcnt = recvfromTimeout(sockfd, (void *)MsgConfig64, sizeof(MsgConfig64), 0, NULL, NULL);
	ALOGD("recvfrom --> rcvcnt=[%d], MsgConfig64[0]=[%d], [1]=[%d], [2]=[%d]\n", rcvcnt, MsgConfig64[0], MsgConfig64[1], MsgConfig64[2]);

	if (rcvcnt > 0)
		return MsgConfig64[2];
	
	return 0;
}


uint8_t setPowerLed(uint8_t power_status)
{
	memset(MsgConfig64, 0, 64);
	memset(MsgConfig256, 0, 256);

	MsgConfig256[0] = 1;
	MsgConfig256[1] = SET_POWER_LED;
	MsgConfig256[2] = power_status;

	int sndcnt = sendto(txSockfd, (void *)MsgConfig256, sizeof(MsgConfig256), 0, (struct sockaddr *)&txServeraddr, txClilen);
	printf("setPowerLed: sendto   --> sndcnt=[%d], MsgConfig256[0]=[%d], [1]=[%d], [2]=[%d], power_status[%d]\n", sndcnt, MsgConfig256[0], MsgConfig256[1], MsgConfig256[2], power_status);

	int rcvcnt = recvfromTimeout(sockfd, (void *)MsgConfig64, sizeof(MsgConfig64), 0, NULL, NULL);
	printf("setPowerLed: recvfrom --> rcvcnt=[%d], MsgConfig64[0]=[%d], [1]=[%d], [2]=[%d]\n", rcvcnt, MsgConfig64[0], MsgConfig64[1], MsgConfig64[2]);

	if (rcvcnt > 0)
		return MsgConfig64[2];
	
	return 0;
}
