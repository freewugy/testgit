/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: skkim $
 * $LastChangedDate: 2015. 11. 11. $
 * $LastChangedRevision: 80 $
 * Description:
 * Note:
 *****************************************************************************/

#define TVS_LOG_ENABLED
#define LOG_TAG "WaitUserThread"

#include "WaitUserThread.h"
#include "DetectHDDThread.h"
#include "WifiStatusThread.h"

sp<WaitUserThread> pWaitUser;

WaitUserThread::~WaitUserThread()
{
	close(sockfd);
	close(txSockfd);
	close(txSockfd_thread);
}

void WaitUserThread::onFirstRef()
{
    //ALOGD("WaitUserThread::onFirstRef()\n");

	slashFlag = true;

    txClilen = sizeof(txServeraddr);
    txSockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (txSockfd < 0)
    {
        ALOGE("socket error : ");
        return ;
    }

    bzero(&txServeraddr_thread, sizeof(txServeraddr_thread));
    txServeraddr_thread.sin_family = AF_INET;
    txServeraddr_thread.sin_addr.s_addr = inet_addr("127.0.0.1");
    txServeraddr_thread.sin_port = htons(WAIT_USER_THREAD_PORT);

    txClilen_thread = sizeof(txServeraddr_thread);
    txSockfd_thread = socket(AF_INET, SOCK_DGRAM, 0);
    if (txSockfd_thread < 0)
    {
        ALOGE("socket error : ");
        return ;
    }

    bzero(&txServeraddr, sizeof(txServeraddr));
    txServeraddr.sin_family = AF_INET;
    txServeraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    txServeraddr.sin_port = htons(WAIT_USER_PROXY_PORT);

    struct sockaddr_in serveraddr;

    memset(MsgConfig256, 0, 256);

    clilen = sizeof(clientaddr);
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        ALOGE("socket error : ");
        return ;
    }

    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(WAIT_USER_HOST_PORT);

	// prevent bind error
	nSockOpt = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &nSockOpt, sizeof(nSockOpt));

    int state = bind(sockfd, (struct sockaddr *)&serveraddr,
        sizeof(serveraddr));
    if (state == -1)
    {
        ALOGE("bind error : ");
        return ;
    }
}

status_t WaitUserThread::readyToRun()
{
	//ALOGD("WaitUserThread::readyToRun()\n");
	return 0;
}

bool WaitUserThread::threadLoop()
{
	//ALOGD("WaitUserThread::threadLoop()\n");

	memset(MsgConfig256, 0x00, 256);
	int rcvcnt = recvfrom(sockfd, (void *)&MsgConfig256, sizeof(MsgConfig256), 0, (struct sockaddr *)&clientaddr, &clilen);
	//ALOGD("[WaitUserThread::threadLoop] rcvcnt=[%d], MsgConfig256: %d %d %d %d %d ...\n", rcvcnt, MsgConfig256[0], MsgConfig256[1], MsgConfig256[2], MsgConfig256[3], MsgConfig256[4]);
	ALOGD("[WaitUserThread::threadLoop] rcvcnt=[%d], MsgConfig256: %d %d %s", rcvcnt, MsgConfig256[0], MsgConfig256[1], &MsgConfig256[2]);

	char* pos_mnt = strstr((const char *)&MsgConfig256[2], MNT_MEDIA_DIR);
	if (pos_mnt)
	{
		ALOGD("=======> (1): pos_mnt[%s]", pos_mnt);
		char* pos_slash = strchr(pos_mnt+11, '/');
		if (pos_slash)
		{
			uint8_t len = strlen(pos_slash);
			ALOGD("=======> (1): pos_slash[%s], strlen(pos_slash)[%d]", pos_slash, len);
			memmove(&MsgConfig256[2], pos_slash, strlen(pos_slash));
			MsgConfig256[2+len] = '\0';
			ALOGD("=======> (1): &MsgConfig256[2][%s]", &MsgConfig256[2]);
		}
	}
	//One more
	pos_mnt = strstr((const char *)&MsgConfig256[2], MNT_MEDIA_DIR);
	if (pos_mnt)
	{
		ALOGD("=======> (2): pos_mnt[%s]", pos_mnt);
		char* pos_slash = strchr(pos_mnt+11, '/');
		if (pos_slash)
		{
			uint8_t len = strlen(pos_slash);
			ALOGD("=======> (2): pos_slash[%s], strlen(pos_slash)[%d]", pos_slash, len);
			memmove(&MsgConfig256[2], pos_slash, strlen(pos_slash));
			MsgConfig256[2+len] = '\0';
			ALOGD("=======> (2): &MsgConfig256[2][%s]", &MsgConfig256[2]);
		}
	}

	if (rcvcnt >= 2)
	{
		switch(MsgConfig256[1])
		{
			case HDD_GET_HDD_INFO:
				{
					if (pDetectHDD->selectedHDDIndex == 0 || pDetectHDD->selectedHDDIndex == 1)
					{
						pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].GetHDDActualSize(true, false);

						uint8_t sndMsgConfig[64];
						memset(sndMsgConfig, 0, 64);
						sndMsgConfig[0] = 1;
						sndMsgConfig[1] = MsgConfig256[1];
						sndMsgConfig[2] = 1;
						sendTo(sndMsgConfig, 64);
						sendTo((uint8_t*)&(pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo), sizeof(pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo));
					}
					else
					{
						uint8_t sndMsgConfig[64];
						memset(sndMsgConfig, 0, 64);
						sndMsgConfig[0] = 1;
						sndMsgConfig[1] = MsgConfig256[1];
						sndMsgConfig[2] = 0;
						sendTo(sndMsgConfig, 64);
					}
				}
				break;
			case HDD_GET_REAR_FRONT:
				{
					if (pDetectHDD->selectedHDDIndex == 0 || pDetectHDD->selectedHDDIndex == 1)
					{
						uint8_t sndMsgConfig[64];
						memset(sndMsgConfig, 0, 64);
						sndMsgConfig[0] = 1;
						sndMsgConfig[1] = MsgConfig256[1];
						sndMsgConfig[2] = pDetectHDD->selectedHDDIndex;
						sendTo(sndMsgConfig, 64);
					}
					else
					{
						uint8_t sndMsgConfig[64];
						memset(sndMsgConfig, 0, 64);
						sndMsgConfig[0] = 1;
						sndMsgConfig[1] = MsgConfig256[1];
						sndMsgConfig[2] = 2;
						sendTo(sndMsgConfig, 64);
					}
				}
				break;
			case HDD_FORMAT:
				{
					if (pDetectHDD->hddInfoStore[0].SlotConnected &&
						pDetectHDD->hddInfoStore[1].SlotConnected)
					{
						if (pDetectHDD->hddInfoStore[0].HDDInfo.ext4_flag == 1 &&
							pDetectHDD->hddInfoStore[1].HDDInfo.ext4_flag == 1)
						{
							//No change
						}
						else if (pDetectHDD->hddInfoStore[0].HDDInfo.ext4_flag == 1 &&
								 pDetectHDD->hddInfoStore[1].HDDInfo.ext4_flag == 0)
						{
							pDetectHDD->selectedHDDIndex = 1;
						}
						else if (pDetectHDD->hddInfoStore[0].HDDInfo.ext4_flag == 0 &&
								 pDetectHDD->hddInfoStore[1].HDDInfo.ext4_flag == 1)
						{
							pDetectHDD->selectedHDDIndex = 0;
						}
						else
						{
							pDetectHDD->formatSelected = true;
							pDetectHDD->selectedHDDIndex = pDetectHDD->tempFormatSelectedHDDIndex;
						}
					}
					else if ( pDetectHDD->hddInfoStore[0].SlotConnected &&
							 !pDetectHDD->hddInfoStore[1].SlotConnected)
					{
						if (pDetectHDD->hddInfoStore[0].HDDInfo.ext4_flag == 0)
							pDetectHDD->selectedHDDIndex = 0;
					}
					else if (!pDetectHDD->hddInfoStore[0].SlotConnected &&
							  pDetectHDD->hddInfoStore[1].SlotConnected)
					{
						if (pDetectHDD->hddInfoStore[1].HDDInfo.ext4_flag == 0)
							pDetectHDD->selectedHDDIndex = 1;
					}
					else
					{
						//No case
					}

					if (pDetectHDD->selectedHDDIndex == 0 || pDetectHDD->selectedHDDIndex == 1)
					{
						int flag = pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].FormatHDDToExt4();

						uint8_t sndMsgConfig[64];
						memset(sndMsgConfig, 0, 64);
						sndMsgConfig[0] = 1;
						sndMsgConfig[1] = MsgConfig256[1];
						sndMsgConfig[2] = flag;
						sendTo(sndMsgConfig, 64);
					}
					else
					{
						uint8_t sndMsgConfig[64];
						memset(sndMsgConfig, 0, 64);
						sndMsgConfig[0] = 1;
						sndMsgConfig[1] = MsgConfig256[1];
						sndMsgConfig[2] = 0;
						sendTo(sndMsgConfig, 64);
					}
				}
				break;
			case HDD_USED_SIZE:
				{
					if (pDetectHDD->selectedHDDIndex == 0 || pDetectHDD->selectedHDDIndex == 1)
					{
						pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].GetHDDActualSize(true, false);

						uint8_t sndMsgConfig[64];
						memset(sndMsgConfig, 0, 64);
						sndMsgConfig[0] = 1;
						sndMsgConfig[1] = MsgConfig256[1];
						/*
						sndMsgConfig[2] = (pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.hdd_used_B_size & 0xFF00000000000000) >> 56;
						sndMsgConfig[3] = (pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.hdd_used_B_size & 0x00FF000000000000) >> 48;
						sndMsgConfig[4] = (pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.hdd_used_B_size & 0x0000FF0000000000) >> 40;
						sndMsgConfig[5] = (pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.hdd_used_B_size & 0x000000FF00000000) >> 32;
						sndMsgConfig[6] = (pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.hdd_used_B_size & 0x00000000FF000000) >> 24;
						sndMsgConfig[7] = (pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.hdd_used_B_size & 0x0000000000FF0000) >> 16;
						sndMsgConfig[8] = (pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.hdd_used_B_size & 0x000000000000FF00) >> 8;
						sndMsgConfig[9] = (pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.hdd_used_B_size & 0x00000000000000FF);
						*/
						memcpy(&sndMsgConfig[2], &(pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.hdd_used_B_size), 8);
						//ALOGD("hdd_used_B_size(): hdd_used_B_size=[%lld]\n", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.hdd_used_B_size);
						sendTo(sndMsgConfig, 64);
					}
					else
					{
						uint8_t sndMsgConfig[64];
						memset(sndMsgConfig, 0, 64);
						sndMsgConfig[0] = 1;
						sndMsgConfig[1] = MsgConfig256[1];
						sndMsgConfig[2] = 0;
						sendTo(sndMsgConfig, 64);
					}
				}
				break;
			case HDD_FREE_SIZE:
				{
					if (pDetectHDD->selectedHDDIndex == 0 || pDetectHDD->selectedHDDIndex == 1)
					{
						pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].GetHDDActualSize(true, false);

						uint8_t sndMsgConfig[64];
						memset(sndMsgConfig, 0, 64);
						sndMsgConfig[0] = 1;
						sndMsgConfig[1] = MsgConfig256[1];
						/*
						sndMsgConfig[2] = (pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.hdd_free_B_size & 0xFF00000000000000) >> 56;
						sndMsgConfig[3] = (pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.hdd_free_B_size & 0x00FF000000000000) >> 48;
						sndMsgConfig[4] = (pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.hdd_free_B_size & 0x0000FF0000000000) >> 40;
						sndMsgConfig[5] = (pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.hdd_free_B_size & 0x000000FF00000000) >> 32;
						sndMsgConfig[6] = (pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.hdd_free_B_size & 0x00000000FF000000) >> 24;
						sndMsgConfig[7] = (pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.hdd_free_B_size & 0x0000000000FF0000) >> 16;
						sndMsgConfig[8] = (pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.hdd_free_B_size & 0x000000000000FF00) >> 8;
						sndMsgConfig[9] = (pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.hdd_free_B_size & 0x00000000000000FF);
						*/
						memcpy(&sndMsgConfig[2], &(pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.hdd_free_B_size), 8);
						//ALOGD("hdd_free_B_size(): hdd_free_B_size=[%lld]\n", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.hdd_free_B_size);
						sendTo(sndMsgConfig, 64);
					}
					else
					{
						uint8_t sndMsgConfig[64];
						memset(sndMsgConfig, 0, 64);
						sndMsgConfig[0] = 1;
						sndMsgConfig[1] = MsgConfig256[1];
						sndMsgConfig[2] = 0;
						sendTo(sndMsgConfig, 64);
					}
				}
				break;
			case HDD_MOUNT:
				{
					if (pDetectHDD->selectedHDDIndex == 0 || pDetectHDD->selectedHDDIndex == 1)
					{
						if (pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].MountHDD(true))
						{
							uint8_t sndMsgConfig[64];
							memset(sndMsgConfig, 0, 64);
							sndMsgConfig[0] = 1;
							sndMsgConfig[1] = MsgConfig256[1];
							sndMsgConfig[2] = 1;
							sendTo(sndMsgConfig, 64);
						}
						else
						{
							uint8_t sndMsgConfig[64];
							memset(sndMsgConfig, 0, 64);
							sndMsgConfig[0] = 1;
							sndMsgConfig[1] = MsgConfig256[1];
							sndMsgConfig[2] = 0;
							sendTo(sndMsgConfig, 64);
						}
					}
					else
					{
						uint8_t sndMsgConfig[64];
						memset(sndMsgConfig, 0, 64);
						sndMsgConfig[0] = 1;
						sndMsgConfig[1] = MsgConfig256[1];
						sndMsgConfig[2] = 0;
						sendTo(sndMsgConfig, 64);
					}
				}
				break;
			case HDD_UNMOUNT:
				{
					if (pDetectHDD->selectedHDDIndex == 0 || pDetectHDD->selectedHDDIndex == 1)
					{
						if (pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].UnmountHDD())
						{
							uint8_t sndMsgConfig[64];
							memset(sndMsgConfig, 0, 64);
							sndMsgConfig[0] = 1;
							sndMsgConfig[1] = MsgConfig256[1];
							sndMsgConfig[2] = 1;
							sendTo(sndMsgConfig, 64);
						}
						else
						{
							uint8_t sndMsgConfig[64];
							memset(sndMsgConfig, 0, 64);
							sndMsgConfig[0] = 1;
							sndMsgConfig[1] = MsgConfig256[1];
							sndMsgConfig[2] = 0;
							sendTo(sndMsgConfig, 64);
						}
					}
					else
					{
						uint8_t sndMsgConfig[64];
						memset(sndMsgConfig, 0, 64);
						sndMsgConfig[0] = 1;
						sndMsgConfig[1] = MsgConfig256[1];
						sndMsgConfig[2] = 0;
						sendTo(sndMsgConfig, 64);
					}
				}
				break;
			case HDD_FILE_CANREAD:
				{
					if (pDetectHDD->selectedHDDIndex == 0 || pDetectHDD->selectedHDDIndex == 1)
					{
						file_canRead((char*)&MsgConfig256[2]);
					}
					else
					{
						uint8_t sndMsgConfig[64];
						memset(sndMsgConfig, 0, 64);
						sndMsgConfig[0] = 1;
						sndMsgConfig[1] = MsgConfig256[1];
						sndMsgConfig[2] = 0;
						sendTo(sndMsgConfig, 64);
					}
				}
				break;
			case HDD_FILE_CANWRITE:
				{
					if (pDetectHDD->selectedHDDIndex == 0 || pDetectHDD->selectedHDDIndex == 1)
					{
						file_canWrite((char*)&MsgConfig256[2]);
					}
					else
					{
						uint8_t sndMsgConfig[64];
						memset(sndMsgConfig, 0, 64);
						sndMsgConfig[0] = 1;
						sndMsgConfig[1] = MsgConfig256[1];
						sndMsgConfig[2] = 0;
						sendTo(sndMsgConfig, 64);
					}
				}
				break;
			case HDD_FILE_EXISTS:
				{
					if (pDetectHDD->selectedHDDIndex == 0 || pDetectHDD->selectedHDDIndex == 1)
					{
						file_exists((char*)&MsgConfig256[2]);
					}
					else
					{
						uint8_t sndMsgConfig[64];
						memset(sndMsgConfig, 0, 64);
						sndMsgConfig[0] = 1;
						sndMsgConfig[1] = MsgConfig256[1];
						sndMsgConfig[2] = 0;
						sendTo(sndMsgConfig, 64);
					}
				}
				break;
			case HDD_FILE_MKDIR:
				{
					if (pDetectHDD->selectedHDDIndex == 0 || pDetectHDD->selectedHDDIndex == 1)
					{
						file_mkdir((char*)&MsgConfig256[2]);
					}
					else
					{
						uint8_t sndMsgConfig[64];
						memset(sndMsgConfig, 0, 64);
						sndMsgConfig[0] = 1;
						sndMsgConfig[1] = MsgConfig256[1];
						sndMsgConfig[2] = 0;
						sendTo(sndMsgConfig, 64);
					}
				}
				break;
			case HDD_FILE_MKDIRS:
				{
					if (pDetectHDD->selectedHDDIndex == 0 || pDetectHDD->selectedHDDIndex == 1)
					{
						file_mkdirs((char*)&MsgConfig256[2]);
					}
					else
					{
						uint8_t sndMsgConfig[64];
						memset(sndMsgConfig, 0, 64);
						sndMsgConfig[0] = 1;
						sndMsgConfig[1] = MsgConfig256[1];
						sndMsgConfig[2] = 0;
						sendTo(sndMsgConfig, 64);
					}
				}
				break;
			case HDD_FILE_RENAMETO:
				{
					if (pDetectHDD->selectedHDDIndex == 0 || pDetectHDD->selectedHDDIndex == 1)
					{
						ALOGD("===========> MsgConfig256[%s]\n", MsgConfig256);
						char src_filename[100], dest_filename[100];
						memset(src_filename, 0, 100);
						memset(dest_filename, 0, 100);
						memcpy(src_filename, &MsgConfig256[2], 100);
						memcpy(dest_filename, &MsgConfig256[103], 100);
						ALOGD("===========> src_filename[%s], dest_filename[%s]\n", src_filename, dest_filename);
						file_renameTo(src_filename, dest_filename);
					}
					else
					{
						uint8_t sndMsgConfig[64];
						memset(sndMsgConfig, 0, 64);
						sndMsgConfig[0] = 1;
						sndMsgConfig[1] = MsgConfig256[1];
						sndMsgConfig[2] = 0;
						sendTo(sndMsgConfig, 64);
					}
				}
				break;
			case HDD_FILE_NEWFILE:
				{
					if (pDetectHDD->selectedHDDIndex == 0 || pDetectHDD->selectedHDDIndex == 1)
					{
						file_newFile((char*)&MsgConfig256[2]);
					}
					else
					{
						uint8_t sndMsgConfig[64];
						memset(sndMsgConfig, 0, 64);
						sndMsgConfig[0] = 1;
						sndMsgConfig[1] = MsgConfig256[1];
						sndMsgConfig[2] = 0;
						sendTo(sndMsgConfig, 64);
					}
				}
				break;
			case HDD_FILE_NEWPATHFILE:
				{
					if (pDetectHDD->selectedHDDIndex == 0 || pDetectHDD->selectedHDDIndex == 1)
					{
						char pathname[100], filename[100];
						memset(pathname, 0, 100);
						memset(filename, 0, 100);
						memcpy(pathname, &MsgConfig256[2], 100);
						memcpy(filename, &MsgConfig256[103], 100);
						file_newPathFile(pathname, filename);
					}
					else
					{
						uint8_t sndMsgConfig[64];
						memset(sndMsgConfig, 0, 64);
						sndMsgConfig[0] = 1;
						sndMsgConfig[1] = MsgConfig256[1];
						sndMsgConfig[2] = 0;
						sendTo(sndMsgConfig, 64);
					}
				}
				break;
			case HDD_FILE_DELETE:
				{
					if (pDetectHDD->selectedHDDIndex == 0 || pDetectHDD->selectedHDDIndex == 1)
					{
						file_delete((char*)&MsgConfig256[2]);
					}
					else
					{
						uint8_t sndMsgConfig[64];
						memset(sndMsgConfig, 0, 64);
						sndMsgConfig[0] = 1;
						sndMsgConfig[1] = MsgConfig256[1];
						sndMsgConfig[2] = 0;
						sendTo(sndMsgConfig, 64);
					}
				}
				break;
			case HDD_FILE_ISDIRECTORY:
				{
					if (pDetectHDD->selectedHDDIndex == 0 || pDetectHDD->selectedHDDIndex == 1)
					{
						file_isDirectory((char*)&MsgConfig256[2]);
					}
					else
					{
						uint8_t sndMsgConfig[64];
						memset(sndMsgConfig, 0, 64);
						sndMsgConfig[0] = 1;
						sndMsgConfig[1] = MsgConfig256[1];
						sndMsgConfig[2] = 0;
						sendTo(sndMsgConfig, 64);
					}
				}
				break;
			case HDD_FILE_ISFILE:
				{
					if (pDetectHDD->selectedHDDIndex == 0 || pDetectHDD->selectedHDDIndex == 1)
					{
						file_isFile((char*)&MsgConfig256[2]);
					}
					else
					{
						uint8_t sndMsgConfig[64];
						memset(sndMsgConfig, 0, 64);
						sndMsgConfig[0] = 1;
						sndMsgConfig[1] = MsgConfig256[1];
						sndMsgConfig[2] = 0;
						sendTo(sndMsgConfig, 64);
					}
				}
				break;
			case HDD_FILE_LISTFILES:
				{
					if (pDetectHDD->selectedHDDIndex == 0 || pDetectHDD->selectedHDDIndex == 1)
					{
						file_listFiles((char*)&MsgConfig256[2]);
					}
					else
					{
						uint8_t sndMsgConfig[256];
						memset(sndMsgConfig, 0, 256);
						sndMsgConfig[0] = 1;
						sndMsgConfig[1] = MsgConfig256[1];
						sndMsgConfig[2] = 0;
						sendTo(sndMsgConfig, 256);
					}
				}
				break;
			case HDD_FILE_GETPARENT:
				{
					if (pDetectHDD->selectedHDDIndex == 0 || pDetectHDD->selectedHDDIndex == 1)
					{
						file_getParent((char*)&MsgConfig256[2]);
					}
					else
					{
						uint8_t sndMsgConfig[256];
						memset(sndMsgConfig, 0, 256);
						sndMsgConfig[0] = 1;
						sndMsgConfig[1] = MsgConfig256[1];
						sndMsgConfig[2] = 0;
						sendTo(sndMsgConfig, 256);
					}
				}
				break;
			case HDD_FILE_GETNAME:
				{
					if (pDetectHDD->selectedHDDIndex == 0 || pDetectHDD->selectedHDDIndex == 1)
					{
						file_getName((char*)&MsgConfig256[2]);
					}
					else
					{
						uint8_t sndMsgConfig[256];
						memset(sndMsgConfig, 0, 256);
						sndMsgConfig[0] = 1;
						sndMsgConfig[1] = MsgConfig256[1];
						sndMsgConfig[2] = 0;
						sendTo(sndMsgConfig, 256);
					}
				}
				break;
			case HDD_FILE_GETPATH:
				{
					if (pDetectHDD->selectedHDDIndex == 0 || pDetectHDD->selectedHDDIndex == 1)
					{
						file_getPath((char*)&MsgConfig256[2]);
					}
					else
					{
						uint8_t sndMsgConfig[256];
						memset(sndMsgConfig, 0, 256);
						sndMsgConfig[0] = 1;
						sndMsgConfig[1] = MsgConfig256[1];
						sndMsgConfig[2] = 0;
						sendTo(sndMsgConfig, 256);
					}
				}
				break;
			case HDD_FILE_LASTMODIFIED:
				{
					if (pDetectHDD->selectedHDDIndex == 0 || pDetectHDD->selectedHDDIndex == 1)
					{
						file_lastModified((char*)&MsgConfig256[2]);
					}
					else
					{
						uint8_t sndMsgConfig[64];
						memset(sndMsgConfig, 0, 64);
						sndMsgConfig[0] = 1;
						sndMsgConfig[1] = MsgConfig256[1];
						sndMsgConfig[2] = 0;
						sendTo(sndMsgConfig, 64);
					}
				}
				break;
			case HDD_FILE_CHANGE_MODE:
				{
					if (pDetectHDD->selectedHDDIndex == 0 || pDetectHDD->selectedHDDIndex == 1)
					{
						//uint16_t mode = ((MsgConfig256[2] << 8) & 0xFF00) + (MsgConfig256[3] & 0x00FF);
						//file_changeMode(mode, (char*)&MsgConfig256[4]);
						uint8_t sndMsgConfig[64];
						memset(sndMsgConfig, 0, 64);
						sndMsgConfig[0] = 1;
						sndMsgConfig[1] = MsgConfig256[1];
						sndMsgConfig[2] = 1;
						sendTo(sndMsgConfig, 64);
					}
					else
					{
						uint8_t sndMsgConfig[64];
						memset(sndMsgConfig, 0, 64);
						sndMsgConfig[0] = 1;
						sndMsgConfig[1] = MsgConfig256[1];
						sndMsgConfig[2] = 0;
						sendTo(sndMsgConfig, 64);
					}
				}
				break;
			case HDD_FILE_FILE_SIZE:
				{
					if (pDetectHDD->selectedHDDIndex == 0 || pDetectHDD->selectedHDDIndex == 1)
					{
						file_fileSize((char*)&MsgConfig256[2]);
					}
					else
					{
						uint8_t sndMsgConfig[64];
						memset(sndMsgConfig, 0, 64);
						sndMsgConfig[0] = 1;
						sndMsgConfig[1] = MsgConfig256[1];
						sndMsgConfig[2] = 0;
						sendTo(sndMsgConfig, 64);
					}
				}
				break;

			case WifiStatusThread::ETH_RENEW:
				{
					eth_renew((char*)&MsgConfig256[2]);
				}
				break;

			case WifiStatusThread::WIFI_GET_WIFI_STATUS_LED:
				{
					wifi_getWifiStatusLed();
				}
				break;

			case WifiStatusThread::WIFI_SET_WIFI_STATUS_LED:
				{
					//if (pWifiStatus->CurWifiStatus == MsgConfig256[2])
					//{
						wifi_setWifiStatusLed(MsgConfig256[2]);
					//}
					//else
					//{
					//	uint8_t sndMsgConfig[64];
					//	memset(sndMsgConfig, 0, 64);
					//	sndMsgConfig[0] = 1;
					//	sndMsgConfig[1] = MsgConfig256[1];
					//	sndMsgConfig[2] = 0;
					//	sendTo(sndMsgConfig, 64);
					//}
				}
				break;
			case WifiStatusThread::ECM_SET_DOCSIS_STATUS_LED:
				{
					//if (CurDocsisStatus == MsgConfig256[2])
					//{
					pWaitUser->setDocsisStatusLed(MsgConfig256[2]);
					//}
					//else
					//{
					//  uint8_t sndMsgConfig[64];
					//  memset(sndMsgConfig, 0, 64);
					//  sndMsgConfig[0] = 1;
					//  sndMsgConfig[1] = MsgConfig[1];
					//  sndMsgConfig[2] = 0;
					//  sendTo(sndMsgConfig, 64);
					//}
				}
				break;

			case WifiStatusThread::SET_POWER_LED:
				{
					if (MsgConfig256[2] == 0 || MsgConfig256[2] == 1 || MsgConfig256[2] == 2)
					{
						pWaitUser->setPowerLed(MsgConfig256[2]);
					}
					else
					{
						uint8_t sndMsgConfig[64];
						memset(sndMsgConfig, 0, 64);
						sndMsgConfig[0] = 1;
						sndMsgConfig[1] = MsgConfig256[1];
						sndMsgConfig[2] = 0;
						sendTo(sndMsgConfig, 64);
					}
				}
				break;
			default:
				break;
		}
	}
	return true;
}

void WaitUserThread::sendTo(uint8_t *sndMsgConfig, size_t szMsgConfig)
{
	int sndcnt = sendto(txSockfd, (void *)sndMsgConfig, szMsgConfig, 0, (struct sockaddr *)&txServeraddr, txClilen);
	ALOGD("[WaitUserThread::sendTo] sndcnt=[%d]\n", sndcnt);
}

void WaitUserThread::sendToThread(uint8_t *sndMsgConfig, size_t szMsgConfig)
{
	int sndcnt = sendto(txSockfd_thread, (void *)sndMsgConfig, szMsgConfig, 0, (struct sockaddr *)&txServeraddr_thread, txClilen_thread);
	ALOGD("[WaitUserThread::sendToThread] sndcnt=[%d]\n", sndcnt);
}


uint8_t WaitUserThread::file_util_makedirs(char* pathfilename)
{
	char* pos = strrchr(pathfilename, '/');
	if (pos)
	{
		char pathstr[256];
		memset(pathstr, 0, 256);
		memcpy(pathstr, pathfilename, pos-pathfilename);

		return file_util_mkdirs(pathstr);
	}
	else
	{
		return 1;
	}
}

uint8_t WaitUserThread::file_util_mkdirs(char* pathname)
{
	char buf[256];

	memset(buf, 0, 256);
	sprintf(buf, "mkdir -p %s%s", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, file_util_spaceProcessing(pathname));

	int ret = system(buf);
	if (ret == -1)
		return 0;
	else
	{
		if (file_util_exists(pathname))
			return 1;
		else
			return 0;
	}
}

uint8_t WaitUserThread::file_util_exists(char* filename)
{
	char buf[256];

	memset(buf, 0, 256);
	sprintf(buf, "%s%s", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, filename);

	if (access(buf, F_OK) < 0)
		return 0;
	else
		return 1;
}

char* WaitUserThread::file_util_getPath(char* filename)
{
	static char ret_pathname[256];

	char	pathname2[256];
	char*	bef_pos = NULL;
	char*	cur_pos;

	memset(ret_pathname, 0, 256);

	memset(pathname2, 0, 256);
	strcpy(pathname2, filename);
	cur_pos = strchr(pathname2, '/');
	while (cur_pos != NULL)
	{
		bef_pos = cur_pos;
		cur_pos = strchr(cur_pos+1, '/');
	}

	if (bef_pos == pathname2)
		strcpy(ret_pathname, "/");
	else
		memcpy(ret_pathname, pathname2, bef_pos-pathname2);

	return ret_pathname;
}

char* WaitUserThread::file_util_getParent(char* pathname)
{
	static char ret_pathname[256];

	char	pathname2[256];
	char*	bef_pos = NULL;
	char*	cur_pos;

	memset(ret_pathname, 0, 256);

	memset(pathname2, 0, 256);
	strcpy(pathname2, pathname);
	cur_pos = strchr(pathname2, '/');
	while (cur_pos != NULL)
	{
		bef_pos = cur_pos;
		cur_pos = strchr(cur_pos+1, '/');
	}

	if (bef_pos == NULL)
	{
		return NULL;
	}
	else if (bef_pos == pathname2)
	{
		strcpy(ret_pathname, "/");
	}
	else
	{
		bef_pos[0] = '\0';
		strcpy(ret_pathname, pathname2);
	}

	return ret_pathname;
}

char* WaitUserThread::file_util_spaceProcessing(char* filename)
{
	int index = 0;
	static char ret_filename[256];

	char filename2[256];
	char *pos_space, *pos_slash, *pos_slash2;

	memset(ret_filename, 0, 256);

	memset(filename2, 0, 256);
	strcpy(filename2, filename);

	if (strlen(filename) > 1)
	{
		if (filename2[0] != '/')
		{
			char tempFilename2[300];
			memset(tempFilename2, 0, 300);
			sprintf(tempFilename2, "%c%s", '/', filename2);
			memset(filename2, 0, 256);
			strcpy(filename2, tempFilename2);
			slashFlag = false;
			pos_slash = strchr(filename2, '/');
		}
		else
		{
			slashFlag = true;
			pos_slash = strchr(filename2, '/');
		}
		if (pos_slash)
		{
			while (pos_slash != NULL)
			{
				char temp_buf[200];
				memset(temp_buf, 0, 200);
				pos_slash2 = strchr(pos_slash+1, '/');
				if (pos_slash2)
				{
					memcpy(&ret_filename[index], "/\"", 2);
					index += 2;
					int size = pos_slash2-pos_slash-1;
					memcpy(temp_buf, pos_slash+1, size);
					strcpy(&ret_filename[index], temp_buf);
					index += size;
					strcpy(&ret_filename[index++], "\"");
					pos_slash = strchr(pos_slash2, '/');
				}
				else
				{
					memcpy(&ret_filename[index], "/\"", 2);
					index += 2;
					strcpy(temp_buf, pos_slash+1);
					strcpy(&ret_filename[index], temp_buf);
					index += strlen(temp_buf);
					strcpy(&ret_filename[index], "\"");
					pos_slash = NULL;
				}
			}
		}
		else
		{
			strcpy(&ret_filename[index++], "\"");
			strcpy(&ret_filename[index], filename);
			index += strlen(filename);
			strcpy(&ret_filename[index], "\"");
		}
	}
	else
	{
		strcpy(ret_filename, filename);
	}

	if (!slashFlag)
		return &ret_filename[1];
	else
		return ret_filename;

	//int index = 0;
	//static char ret_filename[256];

	//char	*pos_space, *pos_slash = NULL, *pos_slash2;

	//pos_space = strchr(filename, ' ');
	//while (pos_space != NULL)
	//{
	//	char temp_buf[100];
	//	memset(temp_buf, 0, 100);
	//	memcpy(temp_buf, filename, pos_space-filename);
	//	pos_slash = strrchr(temp_buf, '/');
	//	memcpy(&ret_filename[index], "/\"", 2);
	//	index += 2;
	//	pos_slash2 = strchr(pos_space, '/');
	//	if (pos_slash2 == NULL)
	//	{
	//		strcpy(&ret_filename[index], pos_slash+1);
	//		index += (strlen(pos_slash)-1);
	//		strcpy(&ret_filename[index], pos_space);
	//		index += strlen(pos_space);
	//		strcpy(&ret_filename[index], "\"");
	//		pos_space = NULL;
	//	}
	//	else
	//	{
	//		strcpy(&ret_filename[index], pos_slash+1);
	//		index += (strlen(pos_slash)-1);
	//		memcpy(&ret_filename[index], pos_space, pos_slash2-pos_space);
	//		index += pos_slash2-pos_space;
	//		strcpy(&ret_filename[index++], "\"");
	//		pos_space = strchr(pos_slash2+1, ' ');
	//	}
	//}

	//if (pos_slash == NULL)
	//{
	//	strcpy(ret_filename, filename);
	//}

	//return ret_filename;
}

void WaitUserThread::file_canRead(char* filename)
{
	struct stat	statbuf;

	char buf[256];

	memset(buf, 0, 256);
	sprintf(buf, "%s%s", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, filename);

	uint8_t sndMsgConfig[64];
	memset(sndMsgConfig, 0, 64);
	sndMsgConfig[0] = 1;
	sndMsgConfig[1] = HDD_FILE_CANREAD;

	if (access(buf, R_OK) < 0)
		sndMsgConfig[2] = 0;
	else
	{
		if (lstat(buf, &statbuf) < 0)
		{
			ALOGD("%s: lstat error\n", filename);
			sndMsgConfig[2] = 0;
		}
		else
		{
			if (statbuf.st_mode & S_IRUSR)
				sndMsgConfig[2] = 1;
			else
				sndMsgConfig[2] = 0;
		}
	}

	sendTo(sndMsgConfig, 64);
}

void WaitUserThread::file_canWrite(char* filename)
{
	struct stat	statbuf;

	char buf[256];

	memset(buf, 0, 256);
	sprintf(buf, "%s%s", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, filename);

	uint8_t sndMsgConfig[64];
	memset(sndMsgConfig, 0, 64);
	sndMsgConfig[0] = 1;
	sndMsgConfig[1] = HDD_FILE_CANWRITE;

	if (access(buf, W_OK) < 0)
		sndMsgConfig[2] = 0;
	else
	{
		if (lstat(buf, &statbuf) < 0)
		{
			ALOGD("%s: lstat error\n", filename);
			sndMsgConfig[2] = 0;
		}
		else
		{
			if (statbuf.st_mode & S_IWUSR)
				sndMsgConfig[2] = 1;
			else
				sndMsgConfig[2] = 0;
		}
	}

	sendTo(sndMsgConfig, 64);
}

void WaitUserThread::file_exists(char* filename)
{
	char buf[256];

	memset(buf, 0, 256);
	sprintf(buf, "%s%s", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, filename);

	uint8_t sndMsgConfig[64];
	memset(sndMsgConfig, 0, 64);
	sndMsgConfig[0] = 1;
	sndMsgConfig[1] = HDD_FILE_EXISTS;

	if (access(buf, F_OK) < 0)
		sndMsgConfig[2] = 0;
	else
		sndMsgConfig[2] = 1;

	sendTo(sndMsgConfig, 64);
}

void WaitUserThread::file_mkdir(char* pathname)
{
	char buf[256];

	memset(buf, 0, 256);
	sprintf(buf, "mkdir %s%s", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, file_util_spaceProcessing(pathname));

	uint8_t sndMsgConfig[64];
	memset(sndMsgConfig, 0, 64);
	sndMsgConfig[0] = 1;
	sndMsgConfig[1] = HDD_FILE_MKDIR;

	int ret = system(buf);
	if (ret == -1)
		sndMsgConfig[2] = 0;
	else
	{
		if (file_util_exists(pathname))
			sndMsgConfig[2] = 1;
		else
			sndMsgConfig[2] = 0;
	}

	sendTo(sndMsgConfig, 64);
}

void WaitUserThread::file_mkdirs(char* pathname)
{
	char buf[256];

	memset(buf, 0, 256);
	sprintf(buf, "mkdir -p %s%s", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, file_util_spaceProcessing(pathname));

	uint8_t sndMsgConfig[64];
	memset(sndMsgConfig, 0, 64);
	sndMsgConfig[0] = 1;
	sndMsgConfig[1] = HDD_FILE_MKDIRS;

	int ret = system(buf);
	if (ret == -1)
		sndMsgConfig[2] = 0;
	else
	{
		if (file_util_exists(pathname))
			sndMsgConfig[2] = 1;
		else
			sndMsgConfig[2] = 0;
	}

	sendTo(sndMsgConfig, 64);
}

void WaitUserThread::file_renameTo(char* src_filename, char* dest_filename)
{
	char buf[512];
	char temp_buf[256];

	memset(buf, 0, 512);
	memset(temp_buf, 0, 256);
	strcpy(temp_buf, file_util_spaceProcessing(src_filename));
	sprintf(buf, "mv %s%s %s%s", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, temp_buf, pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, file_util_spaceProcessing(dest_filename));

	uint8_t sndMsgConfig[64];
	memset(sndMsgConfig, 0, 64);
	sndMsgConfig[0] = 1;
	sndMsgConfig[1] = HDD_FILE_RENAMETO;

	if (file_util_exists(dest_filename))
		sndMsgConfig[2] = 0;
	else
	{
		int ret = system(buf);
		if (ret == -1)
			sndMsgConfig[2] = 0;
		else
		{
			if (file_util_exists(dest_filename))
				sndMsgConfig[2] = 1;
			else
				sndMsgConfig[2] = 0;
		}
	}

	sendTo(sndMsgConfig, 64);
}

void WaitUserThread::file_newFile(char* filename)
{
	char buf[256];

	memset(buf, 0, 256);
	sprintf(buf, "touch %s%s", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, file_util_spaceProcessing(filename));

	uint8_t sndMsgConfig[64];
	memset(sndMsgConfig, 0, 64);
	sndMsgConfig[0] = 1;
	sndMsgConfig[1] = HDD_FILE_NEWFILE;

	if (!file_util_makedirs(filename))
		sndMsgConfig[2] = 0;
	else
	{
		int ret = system(buf);
		if (ret == -1)
			sndMsgConfig[2] = 0;
		else
		{
			if (file_util_exists(filename))
				sndMsgConfig[2] = 1;
			else
				sndMsgConfig[2] = 0;
		}
	}

	sendTo(sndMsgConfig, 64);
}

void WaitUserThread::file_newPathFile(char* pathname, char* filename)
{
	char buf[256];
	char temp_buf[100];

	memset(buf, 0, 256);
	memset(temp_buf, 0, 100);
	strcpy(temp_buf, file_util_spaceProcessing(pathname));
	sprintf(buf, "touch %s%s/%s", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, temp_buf, file_util_spaceProcessing(filename));

	uint8_t sndMsgConfig[64];
	memset(sndMsgConfig, 0, 64);
	sndMsgConfig[0] = 1;
	sndMsgConfig[1] = HDD_FILE_NEWPATHFILE;

	if (!file_util_mkdirs(pathname))
		sndMsgConfig[2] = 0;
	else
	{
		int ret = system(buf);
		if (ret == -1)
			sndMsgConfig[2] = 0;
		else
		{
			memset(buf, 0, 256);
			sprintf(buf, "%s/%s", pathname, filename);
			if (file_util_exists(buf))
				sndMsgConfig[2] = 1;
			else
				sndMsgConfig[2] = 0;
		}
	}

	sendTo(sndMsgConfig, 64);
}

void WaitUserThread::file_delete(char* pathname)
{
	char buf[256];

	memset(buf, 0, 256);
	sprintf(buf, "rm -rf %s%s", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, file_util_spaceProcessing(pathname));

	uint8_t sndMsgConfig[64];
	memset(sndMsgConfig, 0, 64);
	sndMsgConfig[0] = 1;
	sndMsgConfig[1] = HDD_FILE_DELETE;

	int ret = system(buf);
	if (ret == -1)
		sndMsgConfig[2] = 0;
	else
	{
		if (file_util_exists(pathname))
			sndMsgConfig[2] = 0;
		else
			sndMsgConfig[2] = 1;
	}

	sendTo(sndMsgConfig, 64);
}

void WaitUserThread::file_isDirectory(char* pathname)
{
	struct stat	statbuf;

	char buf[256];

	memset(buf, 0, 256);
	sprintf(buf, "%s%s", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, pathname);

	uint8_t sndMsgConfig[64];
	memset(sndMsgConfig, 0, 64);
	sndMsgConfig[0] = 1;
	sndMsgConfig[1] = HDD_FILE_ISDIRECTORY;

	if (access(buf, F_OK) < 0)
		sndMsgConfig[2] = 0;
	else
	{
		if (lstat(buf, &statbuf) < 0)
		{
			ALOGD("%s: lstat error\n", pathname);
			sndMsgConfig[2] = 0;
		}
		else
		{
			if (S_ISDIR(statbuf.st_mode))
				sndMsgConfig[2] = 1;
			else
				sndMsgConfig[2] = 0;
		}
	}

	sendTo(sndMsgConfig, 64);
}

void WaitUserThread::file_isFile(char* filename)
{
	struct stat	statbuf;

	char buf[256];

	memset(buf, 0, 256);
	sprintf(buf, "%s%s", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, filename);

	uint8_t sndMsgConfig[64];
	memset(sndMsgConfig, 0, 64);
	sndMsgConfig[0] = 1;
	sndMsgConfig[1] = HDD_FILE_ISFILE;

	if (access(buf, F_OK) < 0)
		sndMsgConfig[2] = 0;
	else
	{
		if (lstat(buf, &statbuf) < 0)
		{
			ALOGD("%s: lstat error\n", filename);
			sndMsgConfig[2] = 0;
		}
		else
		{
			if (S_ISREG(statbuf.st_mode))
				sndMsgConfig[2] = 1;
			else
				sndMsgConfig[2] = 0;
		}
	}

	sendTo(sndMsgConfig, 64);
}

void WaitUserThread::file_listFiles(char* pathname)
{
	//FileList filelist;

	//int				i;
	//DIR             *dp;
	//struct dirent   *dirp;

	//char buf[256];

	//memset(buf, 0, 256);
	//sprintf(buf, "%s%s", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, pathname);

	//filelist.file_cnt = 0;
	//for (i = 0; i < 256; i++)
	//{
	//	memset(filelist.filearr[i].filename, 0, 100);
	//}

	//if ((dp = opendir(buf)) != NULL)
	//{
	//	while ((dirp = readdir(dp)) != NULL)
	//	{
	//		if (strcmp(dirp->d_name, ".") == 0 ||
	//			strcmp(dirp->d_name, "..") == 0)
	//			continue;

	//		if (dirp->d_type == DT_REG || dirp->d_type == DT_DIR)
	//		{
	//			strcpy(filelist.filearr[filelist.file_cnt++].filename, dirp->d_name);
	//		}
	//	}

	//	if (closedir(dp) < 0)
	//		ALOGD("can't close directory %s\n", pathname);
	//}

	//uint8_t sndMsgConfig[256];
	//memset(sndMsgConfig, 0, 256);
	//sndMsgConfig[0] = 0;
	//sndMsgConfig[1] = HDD_FILE_LISTFILES;
	//sndMsgConfig[2] = filelist.file_cnt;
	//sendTo(sndMsgConfig, 256);

	//for (int i = 0; i < filelist.file_cnt; i++)
	//{
	//	memset(sndMsgConfig, 0, 256);
	//	sndMsgConfig[0] = i+1;
	//	sndMsgConfig[1] = HDD_FILE_LISTFILES;
	//	strcpy((char*)&sndMsgConfig[2], filelist.filearr[i].filename);
	//	sendTo(sndMsgConfig, 256);
	//}

	char list_buf[100];
	memset(list_buf, 0, 100);
	sprintf(list_buf, "%slist_files.txt", HDD_HOME);
	FILE* fp = fopen(list_buf, "w");
	if (fp)
	{
		char buf[256];
		memset(buf, 0, 256);
		sprintf(buf, "%s%s", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, pathname);
		DIR             *dp;
		struct dirent   *dirp;
		uint16_t file_cnt = 0;
		if ((dp = opendir(buf)) != NULL)
		{
			while ((dirp = readdir(dp)) != NULL)
			{
				if (strcmp(dirp->d_name, ".") == 0 ||
					strcmp(dirp->d_name, "..") == 0)
					continue;

				if (dirp->d_type == DT_REG || dirp->d_type == DT_DIR)
				{
					file_cnt++;
				}
			}
			fseek(fp, 0, SEEK_SET);
			sprintf(buf, "%u", file_cnt);
			fputs(buf, fp);
			fputc('\n', fp);

			rewinddir(dp);
			while ((dirp = readdir(dp)) != NULL)
			{
				if (strcmp(dirp->d_name, ".") == 0 ||
					strcmp(dirp->d_name, "..") == 0)
					continue;

				if (dirp->d_type == DT_REG || dirp->d_type == DT_DIR)
				{
					fputs(dirp->d_name, fp);
					fputc('\n', fp);
					file_cnt++;
				}
			}

			if (closedir(dp) < 0)
				ALOGD("can't close directory %s\n", pathname);
		}
		fclose(fp);

		memset(buf, 0, 256);
		sprintf(buf, "chmod 777 %s", list_buf);
		system(buf);
	}
	uint8_t sndMsgConfig[256];
	memset(sndMsgConfig, 0, 256);
	sndMsgConfig[0] = 0;
	sndMsgConfig[1] = HDD_FILE_LISTFILES;
	sndMsgConfig[2] = 0;
	sendTo(sndMsgConfig, 256);
}

void WaitUserThread::file_getParent(char* filename)
{
	char	pathname2[256];
	char*	bef_pos = NULL;
	char*	cur_pos;

	memset(pathname2, 0, 256);
	strcpy(pathname2, filename);
	cur_pos = strchr(pathname2, '/');
	while (cur_pos != NULL)
	{
		bef_pos = cur_pos;
		cur_pos = strchr(cur_pos+1, '/');
	}

	uint8_t sndMsgConfig[256];
	memset(sndMsgConfig, 0, 256);
	sndMsgConfig[0] = 0;
	sndMsgConfig[1] = HDD_FILE_GETPARENT;

	if (bef_pos == NULL)
	{
		sndMsgConfig[2] = 0;
	}
	else if (bef_pos == pathname2)
	{
		sndMsgConfig[2] = 1;
		sndMsgConfig[3] = '/';
	}
	else
	{
		bef_pos[0] = '\0';
		sndMsgConfig[2] = 1;
		strcpy((char*)&sndMsgConfig[3], pathname2);
	}

	sendTo(sndMsgConfig, 256);
}

void WaitUserThread::file_getName(char* filename)
{
	char*	bef_pos = NULL;
	char*	cur_pos;

	cur_pos = strchr(filename, '/');
	while (cur_pos != NULL)
	{
		bef_pos = cur_pos;
		cur_pos = strchr(cur_pos+1, '/');
	}

	uint8_t sndMsgConfig[256];
	memset(sndMsgConfig, 0, 256);
	sndMsgConfig[0] = 0;
	sndMsgConfig[1] = HDD_FILE_GETNAME;

	if (bef_pos == NULL)
	{
		sndMsgConfig[2] = 0;
	}
	else
	{
		sndMsgConfig[2] = 1;
		strcpy((char*)&sndMsgConfig[3], bef_pos+1);
	}

	sendTo(sndMsgConfig, 256);
}

void WaitUserThread::file_getPath(char* filename)
{
	char	pathname2[256];
	char*	bef_pos = NULL;
	char*	cur_pos;

	memset(pathname2, 0, 256);
	strcpy(pathname2, filename);
	cur_pos = strchr(pathname2, '/');
	while (cur_pos != NULL)
	{
		bef_pos = cur_pos;
		cur_pos = strchr(cur_pos+1, '/');
	}

	uint8_t sndMsgConfig[256];
	memset(sndMsgConfig, 0, 256);
	sndMsgConfig[0] = 0;
	sndMsgConfig[1] = HDD_FILE_GETPATH;

	if (bef_pos == pathname2)
	{
		sndMsgConfig[2] = 1;
		sndMsgConfig[3] = '/';
	}
	else
	{
		sndMsgConfig[2] = 1;
		memcpy(&sndMsgConfig[3], pathname2, bef_pos-pathname2);
	}

	sendTo(sndMsgConfig, 256);
}

void WaitUserThread::file_lastModified(char* filename)
{
	struct stat	statbuf;

	char buf[256];

	memset(buf, 0, 256);
	sprintf(buf, "%s%s", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, filename);

	uint8_t sndMsgConfig[64];
	memset(sndMsgConfig, 0, 64);
	sndMsgConfig[0] = 0;
	sndMsgConfig[1] = HDD_FILE_LASTMODIFIED;

	if (access(buf, F_OK) < 0)
	{
		sndMsgConfig[2] = 0;
	}
	else
	{
		if (lstat(buf, &statbuf) < 0)
		{
			ALOGD("%s: stat error\n", filename);
			sndMsgConfig[2] = 0;
		}
		else
		{
			sndMsgConfig[2] = 1;
			sndMsgConfig[3] = (statbuf.st_mtime & 0xFF000000) >> 24;
			sndMsgConfig[4] = (statbuf.st_mtime & 0x00FF0000) >> 16;
			sndMsgConfig[5] = (statbuf.st_mtime & 0x0000FF00) >>  8;
			sndMsgConfig[6] = (statbuf.st_mtime & 0x000000FF);
		}
	}

	sendTo(sndMsgConfig, 64);
}

void WaitUserThread::file_changeMode(uint16_t change_mode, char* filename)
{
	struct stat	statbuf;
	bool bIsFile = true;
	bool bUpdatedFlag = true;

	struct dirent   *dirp;
	DIR             *dp;

	char buf[256];

	uint8_t sndMsgConfig[64];
	memset(sndMsgConfig, 0, 64);
	sndMsgConfig[0] = 1;
	sndMsgConfig[1] = HDD_FILE_CHANGE_MODE;

	if (filename[strlen(filename)-1] == '*')
	{
		char pathname[256];
		memset(pathname, 0, 256);
		sprintf(pathname, "%s", file_util_getPath(filename));

		char tempBuf[256];
		memset(tempBuf, 0, 256);
		sprintf(tempBuf, "chmod %03d %s%s", change_mode, pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, file_util_spaceProcessing(pathname));
		system(tempBuf);

		char* tempParent;
		tempParent = file_util_getParent(pathname);
		while (tempParent != NULL && strcmp(tempParent, "/") != 0)
		{
			memset(buf, 0, 256);
			sprintf(buf, "chmod %03d %s%s", change_mode, pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, file_util_spaceProcessing(tempParent));
			system(buf);
			memset(buf, 0, 256);
			strcpy(buf, tempParent);
			tempParent = file_util_getParent(buf);
		}

		memset(tempBuf, 0, 256);
		sprintf(tempBuf, "chmod %03d %s%s", change_mode, pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, filename);
		system(tempBuf);

		memset(tempBuf, 0, 256);
		sprintf(tempBuf, "%s%s", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, pathname);
		if ((dp = opendir(tempBuf)) == NULL)
			sndMsgConfig[2] = 0;
		else
		{
			while ((dirp = readdir(dp)) != NULL)
			{
				if (dirp->d_type == DT_DIR &&
				   (strcmp(dirp->d_name, ".") == 0 ||
					strcmp(dirp->d_name, "..") == 0))
					continue;

				memset(buf, 0, 256);
				sprintf(buf, "%s%s/%s", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, pathname, dirp->d_name);
				if (lstat(buf, &statbuf) < 0)
				{
					ALOGD("%s: lstat error\n", filename);
					bUpdatedFlag = false;
					break;
				}
				else
				{
					uint8_t u_mode = change_mode / 100;
					mode_t u_ch_mode = 0;
					if (u_mode & 0x4) u_ch_mode = S_IRUSR;
					if (u_mode & 0x2) u_ch_mode |= S_IWUSR;
					if (u_mode & 0x1) u_ch_mode |= S_IXUSR;
					uint8_t g_mode = (change_mode / 10) % 10;
					mode_t g_ch_mode = 0;
					if (g_mode & 0x4) g_ch_mode = S_IRGRP;
					if (g_mode & 0x2) g_ch_mode |= S_IWGRP;
					if (g_mode & 0x1) g_ch_mode |= S_IXGRP;
					uint8_t o_mode = (change_mode % 100) % 10;
					mode_t o_ch_mode = 0;
					if (o_mode & 0x4) o_ch_mode = S_IROTH;
					if (o_mode & 0x2) o_ch_mode |= S_IWOTH;
					if (o_mode & 0x1) o_ch_mode |= S_IXOTH;

					if (!(statbuf.st_mode & (u_ch_mode | g_ch_mode | o_ch_mode)))
					{
						bUpdatedFlag = false;
						break;
					}
				}
			}
			closedir(dp);

			if (bUpdatedFlag)
				sndMsgConfig[2] = 1;
			else
				sndMsgConfig[2] = 0;
		}
	}
	else
	{
		memset(buf, 0, 256);
		sprintf(buf, "%s%s", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, filename);
		if (access(buf, F_OK) < 0)
		{
			sndMsgConfig[2] = 0;
		}
		else
		{
			memset(buf, 0, 256);
			sprintf(buf, "%s%s", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, filename);
			if (lstat(buf, &statbuf) < 0)
			{
				ALOGD("%s: lstat error\n", filename);
				sndMsgConfig[2] = 0;
			}
			else
			{
				if (S_ISDIR(statbuf.st_mode))
					bIsFile = false;
			}

			memset(buf, 0, 256);
			if (bIsFile)
				sprintf(buf, "%s", file_util_getPath(filename));
			else
				sprintf(buf, "%s", filename);

			char tempBuf[256];
			memset(tempBuf, 0, 256);
			sprintf(tempBuf, "chmod %03d %s%s", change_mode, pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, file_util_spaceProcessing(buf));
			system(tempBuf);

			char* tempParent;
			tempParent = file_util_getParent(buf);
			while (tempParent != NULL && strcmp(tempParent, "/") != 0)
			{
				memset(buf, 0, 256);
				sprintf(buf, "chmod %03d %s%s", change_mode, pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, file_util_spaceProcessing(tempParent));
				system(buf);
				memset(buf, 0, 256);
				strcpy(buf, tempParent);
				tempParent = file_util_getParent(buf);
			}

			if (bIsFile)
			{
				memset(buf, 0, 256);
				sprintf(buf, "chmod %03d %s%s", change_mode, pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, file_util_spaceProcessing(filename));
				system(buf);
			}

			memset(buf, 0, 256);
			sprintf(buf, "%s%s", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, filename);
			if (lstat(buf, &statbuf) < 0)
			{
				ALOGD("%s: lstat error\n", filename);
				sndMsgConfig[2] = 0;
			}
			else
			{
				uint8_t u_mode = change_mode / 100;
				mode_t u_ch_mode = 0;
				if (u_mode & 0x4) u_ch_mode = S_IRUSR;
				if (u_mode & 0x2) u_ch_mode |= S_IWUSR;
				if (u_mode & 0x1) u_ch_mode |= S_IXUSR;
				uint8_t g_mode = (change_mode / 10) % 10;
				mode_t g_ch_mode = 0;
				if (g_mode & 0x4) g_ch_mode = S_IRGRP;
				if (g_mode & 0x2) g_ch_mode |= S_IWGRP;
				if (g_mode & 0x1) g_ch_mode |= S_IXGRP;
				uint8_t o_mode = (change_mode % 100) % 10;
				mode_t o_ch_mode = 0;
				if (o_mode & 0x4) o_ch_mode = S_IROTH;
				if (o_mode & 0x2) o_ch_mode |= S_IWOTH;
				if (o_mode & 0x1) o_ch_mode |= S_IXOTH;

				if (statbuf.st_mode & (u_ch_mode | g_ch_mode | o_ch_mode))
					sndMsgConfig[2] = 1;
				else
					sndMsgConfig[2] = 0;
			}
		}
	}

	sendTo(sndMsgConfig, 64);
}

void WaitUserThread::file_fileSize(char* filename)
{
	struct stat	statbuf;

	char buf[256];

	memset(buf, 0, 256);
	sprintf(buf, "%s%s", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, filename);

	uint8_t sndMsgConfig[64];
	memset(sndMsgConfig, 0, 64);
	sndMsgConfig[0] = 1;
	sndMsgConfig[1] = HDD_FILE_FILE_SIZE;

	if (access(buf, F_OK) < 0)
	{
		sndMsgConfig[2] = 0;
		sndMsgConfig[3] = 0;
		sndMsgConfig[4] = 0;
		sndMsgConfig[5] = 0;
		sndMsgConfig[6] = 0;
	}
	else
	{
		if (lstat(buf, &statbuf) < 0)
		{
			ALOGD("%s: lstat error\n", filename);
			sndMsgConfig[2] = 0;
			sndMsgConfig[3] = 0;
			sndMsgConfig[4] = 0;
			sndMsgConfig[5] = 0;
			sndMsgConfig[6] = 0;
		}
		else
		{
			sndMsgConfig[2] = 1;
			long temp = statbuf.st_size ? (statbuf.st_size/1024)+1 : 0;
			sndMsgConfig[3] = (temp & 0xFF000000) >> 24;
			sndMsgConfig[4] = (temp & 0x00FF0000) >> 16;
			sndMsgConfig[5] = (temp & 0x0000FF00) >>  8;
			sndMsgConfig[6] = (temp & 0x000000FF);
		}
	}

	sendTo(sndMsgConfig, 64);
}


void WaitUserThread::eth_renew(char* ethstr)
{
	uint8_t sndMsgConfig[64];
	memset(sndMsgConfig, 0, 64);
	sndMsgConfig[0] = 1;
	sndMsgConfig[1] = WifiStatusThread::ETH_RENEW;

	char buf[256];

	char dhcp_net_result_prop[PROPERTY_VALUE_MAX] = {'\0'};

	memset(buf, 0, 256);
	sprintf(buf, "dhcp.%s.result", ethstr);
	if (property_get(buf, dhcp_net_result_prop, NULL) &&
		strcmp(dhcp_net_result_prop, "ok") != 0)
	{
		sndMsgConfig[2] = 0;
		sendTo(sndMsgConfig, 64);
	}
	else if (dhcp_net_result_prop[0] == '\0')
	{
		sndMsgConfig[2] = 0;
		sendTo(sndMsgConfig, 64);
	}

	memset(buf, 0, 256);
	sprintf(buf, "ip addr flush dev %s", ethstr);
	int ret = system(buf);
	if (ret == -1)
	{
		sndMsgConfig[2] = 0;
		sendTo(sndMsgConfig, 64);
	}
	else
	{
		if (strcmp(ethstr, "eth0") == 0)
		{
			system("ifconfig eth0 up");
			system("ifconfig eth0:0 172.16.10.200 up");
			system("netcfg eth0 dhcp");
		}
		else
		{
			memset(buf, 0, 256);
			sprintf(buf, "ifconfig %s up", ethstr);
			system(buf);
			memset(buf, 0, 256);
			sprintf(buf, "netcfg %s dhcp", ethstr);
			system(buf);
		}
		sndMsgConfig[2] = 1;
		sendTo(sndMsgConfig, 64);
	}
}

void WaitUserThread::wifi_getWifiStatusLed()
{
	uint8_t sndMsgConfig[64];
	memset(sndMsgConfig, 0, 64);
	sndMsgConfig[0] = 1;
	sndMsgConfig[1] = WifiStatusThread::WIFI_GET_WIFI_STATUS_LED;
	sndMsgConfig[2] = pWifiStatus->WIFI_getWifiStatusLed();
	sendTo(sndMsgConfig, 64);
}

void WaitUserThread::wifi_setWifiStatusLed(uint8_t wifi_status)
{
	uint8_t sndMsgConfig[64];
	memset(sndMsgConfig, 0, 64);
	sndMsgConfig[0] = 1;
	sndMsgConfig[1] = WifiStatusThread::WIFI_SET_WIFI_STATUS_LED;

	if (pWifiStatus->WIFI_setWifiStatusLed(wifi_status))
	{
		sndMsgConfig[2] = 1;
	}
	else
	{
		sndMsgConfig[2] = 0;
	}

	sendTo(sndMsgConfig, 64);
}

void WaitUserThread::setDocsisStatusLed(uint8_t docsis_status)
{
	uint8_t sndMsgConfig[64];
	memset(sndMsgConfig, 0, 64);
	sndMsgConfig[0] = 1;
	sndMsgConfig[1] = WifiStatusThread::ECM_SET_DOCSIS_STATUS_LED;

	if ((docsis_status == 0 && !get_fts_standby()) ||
			(docsis_status == 3 && !get_fts_standby()))
	{
		sndMsgConfig[2] = 1;
	}
	else
	{
		if (pWifiStatus->setDocsisStatusLed(docsis_status))
		{
			sndMsgConfig[2] = 1;
		}
		else
		{
			sndMsgConfig[2] = 0;
		}
	}

	sendTo(sndMsgConfig, 64);
}

void WaitUserThread::setPowerLed(uint8_t power_status)
{
	uint8_t sndMsgConfig[64];
	memset(sndMsgConfig, 0, 64);
	sndMsgConfig[0] = 1;
	sndMsgConfig[1] = WifiStatusThread::SET_POWER_LED;

	if (pWifiStatus->setPowerLed(power_status))
	{
		sndMsgConfig[2] = 1;
	}
	else
	{
		sndMsgConfig[2] = 0;
	}

	sendTo(sndMsgConfig, 64);
}


