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
#define LOG_TAG "DetectHDDThread"

#include "DetectHDDThread.h"
#include "WaitUserThread.h"
#include "LogHelperThread.h"

sp<DetectHDDThread> pDetectHDD;

bool ignore_change_msgiov_flag = false;

pthread_t hdd_detect_thread[1];
void* thread_hdd_detect_main(void*);

void* thread_hdd_detect_main(void* arg)
{
	thread_running_flag = true;

	if (!pDetectHDD->hddInfoStore[0].add_event_sent_flag || !pDetectHDD->hddInfoStore[1].add_event_sent_flag)
	{
		ALOGD("=================> pDetectHDD->hddInfoStore[0].add_event_sent_flag(%d), pDetectHDD->hddInfoStore[1].add_event_sent_flag(%d)", pDetectHDD->hddInfoStore[0].add_event_sent_flag, pDetectHDD->hddInfoStore[1].add_event_sent_flag);
		ALOGD("=================> pDetectHDD->selectedHDDIndex [%d]", pDetectHDD->selectedHDDIndex);

		if (pDetectHDD->hddInfoStore[0].SlotConnected &&
			pDetectHDD->hddInfoStore[1].SlotConnected)
		{
			int waiting_cnt = 0;
			while (waiting_cnt < 10)
			{
				if (pDetectHDD->hddInfoStore[0].HDDInfo.fs_type[0] != '\0' &&
					pDetectHDD->hddInfoStore[1].HDDInfo.fs_type[0] != '\0')
				{
					break;
				}
				waiting_cnt++;
				if (waiting_cnt >= 10) break;
				usleep(100000);	// 100ms
			}
		}

		pDetectHDD->setHDDQualified();

		if (pDetectHDD->hddInfoStore[0].hdd_qualified &&
			pDetectHDD->hddInfoStore[1].hdd_qualified)
		{
#if DEV_FTS_USE == 1
			char* key_val = pDetectHDD->getKeyValue("hdd.serial.number");
#else
			char* key_val = pDetectHDD->getSerialNumber(); //"hdd.serial.number"
#endif
			ALOGD("====================================> key_val [%s]", key_val);
			for (int i = 0; i < USB_SLOT_COUNT; i++)
			{
				if (key_val != NULL && strcmp(pDetectHDD->hddInfoStore[i].HDDInfo.serial, key_val) == 0)
				{
					if (pDetectHDD->hddInfoStore[i].HDDInfo.ext4_flag == 1)
						pDetectHDD->selectedHDDIndex = i;
					else
					{
#if DEV_FTS_USE == 1
						pDetectHDD->setKeyValue("hdd.serial.number", "");
#else
						pDetectHDD->setSerialNumber(""); //"hdd.serial.number"
#endif
					}
				}
			}
			if (pDetectHDD->selectedHDDIndex < 0)
			{
				if (pDetectHDD->hddInfoStore[pDetectHDD->firstDetectedHDDIndex].HDDInfo.ext4_flag == 1)
					pDetectHDD->selectedHDDIndex = pDetectHDD->firstDetectedHDDIndex;
			}
			else
			{
				if (pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.ext4_flag == 1)
				{
#if DEV_FTS_USE == 1
					pDetectHDD->setKeyValue("hdd.serial.number", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.serial);
#else
					pDetectHDD->setSerialNumber(pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.serial); //"hdd.serial.number"
#endif
				}
			}
		}
		else if (pDetectHDD->hddInfoStore[0].hdd_qualified)
		{
			if (pDetectHDD->hddInfoStore[0].HDDInfo.ext4_flag == 1)
			{
				pDetectHDD->selectedHDDIndex = 0;
#if DEV_FTS_USE == 1
				pDetectHDD->setKeyValue("hdd.serial.number", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.serial);
#else
				pDetectHDD->setSerialNumber(pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.serial); //"hdd.serial.number"
#endif
			}
		}
		else if (pDetectHDD->hddInfoStore[1].hdd_qualified)
		{
			if (pDetectHDD->hddInfoStore[1].HDDInfo.ext4_flag == 1)
			{
				pDetectHDD->selectedHDDIndex = 1;
#if DEV_FTS_USE == 1
				pDetectHDD->setKeyValue("hdd.serial.number", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.serial);
#else
				pDetectHDD->setSerialNumber(pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.serial); //"hdd.serial.number"
#endif
			}
		}

		ALOGD("==========> thread_hdd_detect_main: pDetectHDD->selectedHDDIndex[%d]", pDetectHDD->selectedHDDIndex);
		if (pDetectHDD->selectedHDDIndex < 0)
		{
			if (pDetectHDD->hddInfoStore[0].hdd_qualified && pDetectHDD->hddInfoStore[1].hdd_qualified)
			{
				if (!pDetectHDD->hddInfoStore[0].add_event_sent_flag &&
					!pDetectHDD->hddInfoStore[1].add_event_sent_flag)
				{
					if (pDetectHDD->hddInfoStore[0].HDDInfo.ext4_flag == 1 && pDetectHDD->hddInfoStore[1].HDDInfo.ext4_flag == 1)
					{
						pDetectHDD->selectedHDDIndex = pDetectHDD->firstDetectedHDDIndex;
#if DEV_FTS_USE == 1
						pDetectHDD->setKeyValue("hdd.serial.number", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.serial);
#else
						pDetectHDD->setSerialNumber(pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.serial); //"hdd.serial.number"
#endif
						pDetectHDD->sendAddEvent(pDetectHDD->selectedHDDIndex);
					}
					else if (pDetectHDD->hddInfoStore[0].HDDInfo.ext4_flag == 1 && pDetectHDD->hddInfoStore[1].HDDInfo.ext4_flag == 0)
					{
						pDetectHDD->selectedHDDIndex = 0;
#if DEV_FTS_USE == 1
						pDetectHDD->setKeyValue("hdd.serial.number", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.serial);
#else
						pDetectHDD->setSerialNumber(pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.serial); //"hdd.serial.number"
#endif
						pDetectHDD->sendAddEvent(pDetectHDD->selectedHDDIndex);
					}
					else if (pDetectHDD->hddInfoStore[0].HDDInfo.ext4_flag == 0 && pDetectHDD->hddInfoStore[1].HDDInfo.ext4_flag == 1)
					{
						pDetectHDD->selectedHDDIndex = 1;
#if DEV_FTS_USE == 1
						pDetectHDD->setKeyValue("hdd.serial.number", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.serial);
#else
						pDetectHDD->setSerialNumber(pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.serial); //"hdd.serial.number"
#endif
						pDetectHDD->sendAddEvent(pDetectHDD->selectedHDDIndex);
					}
					else if (pDetectHDD->hddInfoStore[0].HDDInfo.ext4_flag == 0 && pDetectHDD->hddInfoStore[1].HDDInfo.ext4_flag == 0)
					{
						//pDetectHDD->sendAddEvent(pDetectHDD->firstDetectedHDDIndex);
						const uint8_t COUNT = 10;
						ALOGD("=========================> pDetectHDD->two_non_ext4_detect_booting_flag [%s]", pDetectHDD->two_non_ext4_detect_booting_flag ? "true" : "false");
						if (pDetectHDD->two_non_ext4_detect_booting_flag)
						{
							pDetectHDD->two_non_ext4_detect_booting_flag = false;
							sleep(20);
						}

						pDetectHDD->sendAddEvent(0);

						uint8_t count = 0;
						pDetectHDD->formatSelected = false;
						pDetectHDD->tempFormatSelectedHDDIndex = 0;
						while (count < COUNT)
						{
							if (pDetectHDD->formatSelected)
								break;
							count++;
							if (count >= COUNT) break;
							sleep(1);
						}
						if (!pDetectHDD->formatSelected && count == COUNT)
						{
							//pDetectHDD->sendRemoveEvent(0);
							pDetectHDD->hddInfoStore[0].SlotConnected = false;
							//sleep(5);
							pDetectHDD->sendAddEvent(1);
							pDetectHDD->tempFormatSelectedHDDIndex = 1;
						}
						pDetectHDD->formatSelected = false;
					}
				}
				else if (!pDetectHDD->hddInfoStore[0].add_event_sent_flag &&
						  pDetectHDD->hddInfoStore[1].add_event_sent_flag)
				{
					if (pDetectHDD->hddInfoStore[0].HDDInfo.ext4_flag == 1 && pDetectHDD->hddInfoStore[1].HDDInfo.ext4_flag == 1)
					{
						//No work
					}
					else if (pDetectHDD->hddInfoStore[0].HDDInfo.ext4_flag == 1 && pDetectHDD->hddInfoStore[1].HDDInfo.ext4_flag == 0)
					{
						//pDetectHDD->sendRemoveEvent(1);
						pDetectHDD->hddInfoStore[1].SlotConnected = false;
						//sleep(3);
						pDetectHDD->selectedHDDIndex = 0;
#if DEV_FTS_USE == 1
						pDetectHDD->setKeyValue("hdd.serial.number", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.serial);
#else
						pDetectHDD->setSerialNumber(pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.serial); //"hdd.serial.number"
#endif
						pDetectHDD->sendAddEvent(pDetectHDD->selectedHDDIndex);
					}
					else if (pDetectHDD->hddInfoStore[0].HDDInfo.ext4_flag == 0 && pDetectHDD->hddInfoStore[1].HDDInfo.ext4_flag == 1)
					{
						//No work
					}
					else if (pDetectHDD->hddInfoStore[0].HDDInfo.ext4_flag == 0 && pDetectHDD->hddInfoStore[1].HDDInfo.ext4_flag == 0)
					{
						//pDetectHDD->sendRemoveEvent(1);
						pDetectHDD->hddInfoStore[1].SlotConnected = false;
						//sleep(5);
						pDetectHDD->selectedHDDIndex = 0;
						pDetectHDD->sendAddEvent(pDetectHDD->selectedHDDIndex);
						pDetectHDD->selectedHDDIndex = -1;
					}
				}
				else if ( pDetectHDD->hddInfoStore[0].add_event_sent_flag &&
						 !pDetectHDD->hddInfoStore[1].add_event_sent_flag)
				{
					if (pDetectHDD->hddInfoStore[0].HDDInfo.ext4_flag == 1 && pDetectHDD->hddInfoStore[1].HDDInfo.ext4_flag == 1)
					{
						//No work
					}
					else if (pDetectHDD->hddInfoStore[0].HDDInfo.ext4_flag == 1 && pDetectHDD->hddInfoStore[1].HDDInfo.ext4_flag == 0)
					{
						//No work
					}
					else if (pDetectHDD->hddInfoStore[0].HDDInfo.ext4_flag == 0 && pDetectHDD->hddInfoStore[1].HDDInfo.ext4_flag == 1)
					{
						//pDetectHDD->sendRemoveEvent(0);
						pDetectHDD->hddInfoStore[0].SlotConnected = false;
						//sleep(3);
						pDetectHDD->selectedHDDIndex = 1;
#if DEV_FTS_USE == 1
						pDetectHDD->setKeyValue("hdd.serial.number", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.serial);
#else
						pDetectHDD->setSerialNumber(pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.serial); //"hdd.serial.number"
#endif
						pDetectHDD->sendAddEvent(pDetectHDD->selectedHDDIndex);
					}
					else if (pDetectHDD->hddInfoStore[0].HDDInfo.ext4_flag == 0 && pDetectHDD->hddInfoStore[1].HDDInfo.ext4_flag == 0)
					{
						//pDetectHDD->sendRemoveEvent(0);
						pDetectHDD->hddInfoStore[0].SlotConnected = false;
						//sleep(5);
						pDetectHDD->selectedHDDIndex = 1;
						pDetectHDD->sendAddEvent(pDetectHDD->selectedHDDIndex);
						pDetectHDD->selectedHDDIndex = -1;
					}
				}
				else
				{
					//No case
				}
			}
			else if (pDetectHDD->hddInfoStore[0].hdd_qualified)
			{
				if (!pDetectHDD->hddInfoStore[0].add_event_sent_flag)
				{
					pDetectHDD->sendAddEvent(0);
				}
			}
			else if (pDetectHDD->hddInfoStore[1].hdd_qualified)
			{
				if (!pDetectHDD->hddInfoStore[1].add_event_sent_flag)
				{
					pDetectHDD->sendAddEvent(1);
				}
			}
			else
			{
				pDetectHDD->hddInfoStore[0].ClearHDDInfoStore();
				pDetectHDD->hddInfoStore[1].ClearHDDInfoStore();
			}
		}
		else
		{
			if (!pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].add_event_sent_flag)
			{
				int index = -1;
				if (pDetectHDD->selectedHDDIndex == 0)
					index = 1;
				else if (pDetectHDD->selectedHDDIndex == 1)
					index = 0;
				if (index >= 0)
				{
					pDetectHDD->hddInfoStore[index].SlotConnected = false;
					if (pDetectHDD->hddInfoStore[index].add_event_sent_flag && pDetectHDD->hddInfoStore[index].HDDInfo.ext4_flag == 0)
					{
						//pDetectHDD->sendRemoveEvent(index);
						//sleep(3);
					}
				}
				pDetectHDD->sendAddEvent(pDetectHDD->selectedHDDIndex);
			}
			else
			{
				if (pDetectHDD->selectedHDDIndex == 0)
					pDetectHDD->hddInfoStore[1].ClearHDDInfoStore();
				else if (pDetectHDD->selectedHDDIndex == 1)
					pDetectHDD->hddInfoStore[0].ClearHDDInfoStore();
			}
		}
		pDetectHDD->firstDetectedHDDIndex = 0;	// Initialization
	}
	else
	{
		pDetectHDD->firstDetectedHDDIndex = 0;	// Initialization
		if (pDetectHDD->selectedHDDIndex == 0)
		{
			pDetectHDD->hddInfoStore[1].ClearHDDInfoStore();
		}
		else if (pDetectHDD->selectedHDDIndex == 1)
		{
			pDetectHDD->hddInfoStore[0].ClearHDDInfoStore();
		}
	}

	pDetectHDD->two_non_ext4_detect_booting_flag = false;
	thread_running_flag = false;

	return NULL;
}

DetectHDDThread::~DetectHDDThread()
{
	close(ns);
#if SEARCH_HDD_ON_BOOTING == 1
	for (int i=0; i < HDD_EVENT_ON_BOOTING_MAX; i++)
	{
		if (SearchHDDOnBooting[i] != NULL)
		{
			free(SearchHDDOnBooting[i]);
			SearchHDDOnBooting[i] = NULL;
		}
	}
#endif
}

void DetectHDDThread::onFirstRef()
{
    //ALOGD("DetectHDDThread::onFirstRef()");
#if SEARCH_HDD_ON_BOOTING == 1
	for (int i=0; i < HDD_EVENT_ON_BOOTING_MAX; i++)
	{
		SearchHDDOnBooting[i] = NULL;
	}
	SearchHDDOnBootingIndex = -1;

	////Merely 4 usb devices
	//addUEventForHDDOnBooting("sdd");
	//addUEventForHDDOnBooting("sdc");
	//addUEventForHDDOnBooting("sdb");
	//addUEventForHDDOnBooting("sda");
	char sdx_str[30];
	for (int i='z'; i >= 'a'; i--)
	{
		memset(sdx_str, 0, 30);
		sprintf(sdx_str, "sd%c", i);
		addUEventForHDDOnBooting(sdx_str);
	}
#endif

	current_usb_index_for_buf_block_sdx[0] = -1;
	current_usb_index_for_buf_block_sdx[1] = -1;

	ns = -1;
	iov.iov_base = buf;
	iov.iov_len = sizeof(buf);
	msg.msg_name = &sa;
	msg.msg_namelen = sizeof(sa);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	msg.msg_flags = 0;

	memset(&sa, 0, sizeof(sa));
	sa.nl_family = AF_NETLINK;
	sa.nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR;

	ns = socket(AF_NETLINK, SOCK_RAW, NETLINK_KOBJECT_UEVENT);
	if(ns < 0)
	{
		ALOGD("socket() error");
        return ;
	}

	tv.tv_sec = NETLINK_TV_SEC;
	tv.tv_usec = NETLINK_TV_USEC;
	setsockopt(ns, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv));

	if(bind(ns, (struct sockaddr *)&sa, sizeof(sa)))
	{
		ALOGD("bind() error");
		close(ns);
        return ;
	}
}

status_t DetectHDDThread::readyToRun()
{
	//ALOGD("DetectHDDThread::readyToRun()");
	selectedHDDIndex = -1;
	firstDetectedHDDIndex = 0;
	change_cnt = 0;
	mount_booting_flag = true;
	two_non_ext4_detect_booting_flag = true;
	formatSelected = false;
	tempFormatSelectedHDDIndex = -1;
	return 0;
}

bool DetectHDDThread::threadLoop()
{
	/*
	 * delay running until TVApp prepared
	 */
	char init[PROPERTY_VALUE_MAX] = {'\0'};
	if (property_get("tvs.tvapp.initialize", init, NULL) && !strcmp(init, "true"))
	{
		//ALOGD("tvs.tvapp.initialize == true!!");
	}
	else
	{
		//ALOGD("Waiting tvs.tvapp.initialize...");
		sleep(1);
		return true;
	}

	/*
	 * do work.
	 */

	char* pos = NULL;

	int len = recvmsg(ns, &msg, 0);
#if SEARCH_HDD_ON_BOOTING == 1
	if (len > 0 || SearchHDDOnBootingIndex >= 0)
#else
	if (len > 0)
#endif
	{
#if SEARCH_HDD_ON_BOOTING == 1
		if (len <= 0 && SearchHDDOnBootingIndex < 0)
		{
			return true;
		}
		else if (SearchHDDOnBootingIndex >= 0)
		{
			memset(buf, 0, 512);
			strcpy(buf, SearchHDDOnBooting[SearchHDDOnBootingIndex]);
			len = strlen(buf);
			ALOGD("SearchHDDOnBooting[%d]=[%s], len[%d]", SearchHDDOnBootingIndex, buf, len);
			if (SearchHDDOnBooting[SearchHDDOnBootingIndex] != NULL)
			{
				free(SearchHDDOnBooting[SearchHDDOnBootingIndex]);
				SearchHDDOnBooting[SearchHDDOnBootingIndex] = NULL;
			}
			SearchHDDOnBootingIndex--;
		}
#endif
		ALOGD("msg iov buf[%s], len[%d]", buf, len);

		int8_t current_usb_index = -1;
		char* pos_num = strstr(buf, POSITION_NUM_STR_FRONT1);
		if (pos_num)
		{
			current_usb_index = 1;
		}
		else
		{
			pos_num = strstr(buf, POSITION_NUM_STR_FRONT2);
			if (pos_num)
			{
				current_usb_index = 1;
			}
			else
			{
				pos_num = strstr(buf, POSITION_NUM_STR_REAR1);
				if (pos_num)
				{
					current_usb_index = 0;
				}
				else
				{
					pos_num = strstr(buf, POSITION_NUM_STR_REAR2);
					if (pos_num)
					{
						current_usb_index = 0;
					}
				}
			}
		}

		if (current_usb_index < 0)
		{
			return true;
		}

		char* pos_at = strchr(buf, '@');
		if (pos_at)
		{
			memset(buf_command, 0, 15);
			memcpy(buf_command, buf, pos_at-buf);
			if (strcmp(buf_command, "change") == 0)
			{
				if (CHANGE_EVENT_USE == 1)
				{
					pos = strstr(buf, BLOCK_SD);
					if (pos)
					{
						if (!hddInfoStore[current_usb_index].ChangeEventReceived)
						{
							hddInfoStore[current_usb_index].ChangeEventReceived = true;
							if (!ignore_change_msgiov_flag)
							{
								if (!thread_running_flag)
									change_cnt++;
								detectHDD(current_usb_index);
							}
						}
					}
				}
			}
			else if (strcmp(buf_command, "add") == 0)
			{
				char* pos_num = strstr(buf, POSITION_NUM_STR_FRONT1);
				if (pos_num)
				{
					firstDetectedHDDIndex = 1;
				}
				else
				{
					pos_num = strstr(buf, POSITION_NUM_STR_FRONT2);
					if (pos_num)
					{
						firstDetectedHDDIndex = 1;
					}
					else
					{
						pos_num = strstr(buf, POSITION_NUM_STR_REAR1);
						if (pos_num)
						{
							firstDetectedHDDIndex = 0;
						}
						else
						{
							pos_num = strstr(buf, POSITION_NUM_STR_REAR2);
							if (pos_num)
							{
								firstDetectedHDDIndex = 0;
							}
						}
					}
				}

				pos = strstr(buf, USB_DEVICE);
				if (pos)
				{
					if (!strncmp(USB_DEVICE, "usb_", 4))
					{
						pos[0] = '\0';
						memset(hddInfoStore[current_usb_index].buf_usb_device, 0, 200);
						memcpy(hddInfoStore[current_usb_index].buf_usb_device, "/sys", 4);
						memcpy(&(hddInfoStore[current_usb_index].buf_usb_device[4]), pos_at+1, pos-pos_at);
					}
					else
					{
						char* pos_cur = NULL;
						if (current_usb_index == 0)
						{
							pos_cur = strstr(pos_at+1, POSITION_NUM_STR_REAR1);
							if (pos_cur == NULL)
								pos_cur = strstr(pos_at+1, POSITION_NUM_STR_REAR2);
						}
						else if (current_usb_index == 1)
						{
							pos_cur = strstr(pos_at+1, POSITION_NUM_STR_FRONT1);
							if (pos_cur == NULL)
								pos_cur = strstr(pos_at+1, POSITION_NUM_STR_FRONT2);
						}

						if (pos_cur != NULL)
						{
							char* pos_colon = strstr(pos_cur+5, ":");
							char* pos_slash = strstr(pos_cur+5, "/");
							uint8_t shift_count = 0;
							if (pos_colon < pos_slash)
								shift_count = 5;
							else
								shift_count = pos_slash-pos_cur+1;
							pos_cur[shift_count] = '\0';
							memset(hddInfoStore[current_usb_index].buf_usb_device, 0, 200);
							memcpy(hddInfoStore[current_usb_index].buf_usb_device, "/sys", 4);
							memcpy(&(hddInfoStore[current_usb_index].buf_usb_device[4]), pos_at+1, pos_cur+shift_count-pos_at);
						}
					}
					ALOGD("====> hddInfoStore[%d].buf_usb_device[%s]", current_usb_index, hddInfoStore[current_usb_index].buf_usb_device);
				}

				pos = strstr(buf, BLOCK_SD);
				if (pos)
				{
					ignore_change_msgiov_flag = false;

					if (current_usb_index == 0 || current_usb_index == 1)
						hddInfoStore[current_usb_index].SlotConnected = true;

					char buf_tail[20];
					memset(buf_tail, 0, 20);
					strcpy(buf_tail, &pos[6]);
					char* pos_slash = strchr(buf_tail, '/');	// BLOCK_SDx/sdxx (block/sdx/sdxx)
					if (pos_slash)
					{
						int sdxx_len = strlen(buf_tail);
						if (buf_tail[sdxx_len-1]-'0' > hddInfoStore[current_usb_index].HDDInfo.partition_cnt)
							hddInfoStore[current_usb_index].HDDInfo.partition_cnt = buf_tail[sdxx_len-1]-'0';
						if (buf_tail[sdxx_len-1] == '1')
						{
							memset(hddInfoStore[current_usb_index].buf_block_sdx_sdx1, 0, 50);
							sprintf(hddInfoStore[current_usb_index].buf_block_sdx_sdx1, "/sys/%s/", pos);
							ALOGD("====> hddInfoStore[%d].buf_block_sdx_sdx1[%s]", current_usb_index, hddInfoStore[current_usb_index].buf_block_sdx_sdx1);
						}
						else
						{
							PartitionBlock tempPB;
							tempPB.partition_no = buf_tail[sdxx_len-1]-'0';
							memset(tempPB.buf_block_sdx_sdxx, 0, 50);
							memset(tempPB.dev_name, 0, 10);
							memset(tempPB.mounted_dir, 0, 80);
							sprintf(tempPB.buf_block_sdx_sdxx, "/sys/%s/", pos);
							hddInfoStore[current_usb_index].vPartitionBlock.push_back(tempPB);
							ALOGD("====> buf_block_sdx_sdxx[%s], partition_no[%d]", tempPB.buf_block_sdx_sdxx, tempPB.partition_no);
						}
						ALOGD("====> hddInfoStore[%d].HDDInfo.partition_cnt(%d)", current_usb_index, hddInfoStore[current_usb_index].HDDInfo.partition_cnt);
					}
					else // BLOCK_SD (block/sdx)
					{
						memset(hddInfoStore[current_usb_index].buf_block_sdx, 0, 50);
						sprintf(hddInfoStore[current_usb_index].buf_block_sdx, "/sys/%s/", pos);

						memset(hddInfoStore[current_usb_index].buf_block_sdx_buf_all, 0, 200);
						memcpy(hddInfoStore[current_usb_index].buf_block_sdx_buf_all, &buf[pos_at-buf+1], 199);
						current_usb_index_for_buf_block_sdx[current_usb_index] = current_usb_index;

						ALOGD("====> hddInfoStore[%d].buf_block_sdx[%s]", current_usb_index, hddInfoStore[current_usb_index].buf_block_sdx);
					}

					if (CHANGE_EVENT_USE == 0)
					{
						if (hddInfoStore[current_usb_index].buf_block_sdx_sdx1[0] != '\0')
						{
							ALOGD("===========> hddInfoStore[%d].HDDInfo.partition_cnt [%d], hddInfoStore[%d].system_partition_cnt [%d]", current_usb_index, hddInfoStore[current_usb_index].HDDInfo.partition_cnt, current_usb_index, hddInfoStore[current_usb_index].system_partition_cnt);
							if (hddInfoStore[current_usb_index].HDDInfo.partition_cnt == hddInfoStore[current_usb_index].system_partition_cnt)
							{
								if (!hddInfoStore[current_usb_index].ChangeEventReceived)
								{
									hddInfoStore[current_usb_index].ChangeEventReceived = true;
									if (!ignore_change_msgiov_flag)
									{
										if (!thread_running_flag)
											change_cnt++;
										detectHDD(current_usb_index);
									}
								}
							}
						}
						else if (hddInfoStore[current_usb_index].buf_block_sdx[0] != '\0')
						{
							char tmp_sdx_buf[4];
							memset(tmp_sdx_buf, 0, 4);
							uint8_t sdx_len = strlen(hddInfoStore[current_usb_index].buf_block_sdx);
							tmp_sdx_buf[2] = hddInfoStore[current_usb_index].buf_block_sdx[sdx_len-2];
							tmp_sdx_buf[1] = hddInfoStore[current_usb_index].buf_block_sdx[sdx_len-3];
							tmp_sdx_buf[0] = hddInfoStore[current_usb_index].buf_block_sdx[sdx_len-4];
							hddInfoStore[current_usb_index].system_partition_cnt = hddInfoStore[current_usb_index].getNumOfPartition(tmp_sdx_buf);
							ALOGD("====> tmp_sdx_buf [%s], hddInfoStore[%d].system_partition_cnt [%d]", tmp_sdx_buf, current_usb_index, hddInfoStore[current_usb_index].system_partition_cnt);
							if (hddInfoStore[current_usb_index].system_partition_cnt == 0)
							{
								if (!hddInfoStore[current_usb_index].ChangeEventReceived)
								{
									hddInfoStore[current_usb_index].ChangeEventReceived = true;
									if (!ignore_change_msgiov_flag)
									{
										if (!thread_running_flag)
											change_cnt++;
										detectHDD(current_usb_index);
									}
								}
							}
						}
					}
				}
			}
			else if (strcmp(buf_command, "remove") == 0)
			{
				pos = strstr(buf, BLOCK_SD);
				if (pos)
				{
                    LogHelperThread::stopLogService();

					if (strlen(pos) == strlen(BLOCK_SD)+1)
					{
						change_cnt = 0;

						ALOGD("==========> hddInfoStore[%d].SlotConnected [%d]", current_usb_index, hddInfoStore[current_usb_index].SlotConnected);
						if (hddInfoStore[current_usb_index].SlotConnected)
						{
							//yinohyinoh
							char sdx_str[30];
							char tmp_buf_prop[PROPERTY_VALUE_MAX] = {'\0'};
							for (int i='a'; i <= 'z'; i++)
							{
								memset(sdx_str, 0, 30);
								sprintf(sdx_str, "block/sd%c/", i);
								if (!strcmp(pos, sdx_str))
								{
									memset(sdx_str, 0, 30);
									sprintf(sdx_str, "tvs.hdd_ctrl.sd%c", i);
									property_set(sdx_str, "");
									break;
								}
							}

#if 0
							if (!strcmp(pos, "block/sda"))
							{
#if 0
								property_set("ctl.stop", "fuse_sdcard1");
							#if REDUNDANCY_MOUNT == 1
								char redundancy_unmount_buf[100];
								memset(redundancy_unmount_buf, 0, 100);
								sprintf(redundancy_unmount_buf, "umount %s", REDUNDANCY_MOUNT_DIR_1);
								system(redundancy_unmount_buf);
							#endif
#endif
								property_set("tvs.hdd_ctrl.sda", "");
							}
							else if (!strcmp(pos, "block/sdb"))
							{
#if 0
								property_set("ctl.stop", "fuse_usbdisk0");
							#if REDUNDANCY_MOUNT == 1
								char redundancy_unmount_buf[100];
								memset(redundancy_unmount_buf, 0, 100);
								sprintf(redundancy_unmount_buf, "umount %s", REDUNDANCY_MOUNT_DIR_2);
								system(redundancy_unmount_buf);
							#endif
#endif
								property_set("tvs.hdd_ctrl.sdb", "");
							}
#endif

							char buf_tail[20];
							memset(buf_tail, 0, 20);
							strcpy(buf_tail, &pos[6]);
							char* pos_slash = strchr(buf_tail, '/');
							if (pos_slash)
							{
								ALOGD("====> pos[%s], pos_slash[%s]", pos, pos_slash);
								ALOGD("====> hddInfoStore[%d].HDDInfo.sdx_name[%s]", current_usb_index, hddInfoStore[current_usb_index].HDDInfo.sdx_name);
								ALOGD("====> selectedHDDIndex[%d]\n", selectedHDDIndex);

								char pos_buf[30], tmp_buf[30];
								memset(pos_buf, 0, 30);
								memset(tmp_buf, 0, 30);
								memcpy(tmp_buf, buf_tail, pos_slash-buf_tail);
								sprintf(pos_buf, "block/%s/", tmp_buf);

								//if (selectedHDDIndex >= 0 && selectedHDDIndex == current_usb_index && strcmp(hddInfoStore[current_usb_index].HDDInfo.sdx_name, pos_buf) == 0 &&
								//	hddInfoStore[current_usb_index].HDDInfo.hdd_total_B_size > 1048576)	// > 1GB
								//if (hddInfoStore[current_usb_index].add_event_sent_flag && strcmp(hddInfoStore[current_usb_index].HDDInfo.sdx_name, pos_buf) == 0)
								if (selectedHDDIndex >= 0 && selectedHDDIndex == current_usb_index && strcmp(hddInfoStore[current_usb_index].HDDInfo.sdx_name, pos_buf) == 0)
								{
									selectedHDDIndex = -1;
									ignore_change_msgiov_flag = false;

									removeHDD(current_usb_index);
									if (hddInfoStore[current_usb_index].HDDInfo.partition_cnt > 1)
									{
										hddInfoStore[current_usb_index].add_event_sent_flag = false;
										ALOGD("==================> No sendRemoveEvent (multi-partition HDD): hddInfoStore[%d], hddInfoStore[%d].HDDInfo.partition_cnt[%d]\n", current_usb_index, current_usb_index, hddInfoStore[current_usb_index].HDDInfo.partition_cnt);
									}
									else
									{
										pDetectHDD->sendRemoveEvent(current_usb_index);
									}
								}
								else
								{
									if (hddInfoStore[current_usb_index].add_event_sent_flag && hddInfoStore[current_usb_index].HDDInfo.ext4_flag == 0)
									{
										if (hddInfoStore[current_usb_index].HDDInfo.partition_cnt > 1)
										{
											hddInfoStore[current_usb_index].add_event_sent_flag = false;
											ALOGD("==================> No sendRemoveEvent (multi-partition HDD): hddInfoStore[%d], hddInfoStore[%d].HDDInfo.partition_cnt[%d]\n", current_usb_index, current_usb_index, hddInfoStore[current_usb_index].HDDInfo.partition_cnt);
										}
										else
										{
											pDetectHDD->sendRemoveEvent(current_usb_index);
										}
									}
									if (hddInfoStore[current_usb_index].HDDInfo.ext4_flag == 1)
									{
										removeHDD(current_usb_index);
									}
								}
								hddInfoStore[current_usb_index].ClearHDDInfoStore();
							}
							else
							{
								ALOGD("====> pos[%s], pos_slash[NULL]\n", pos);
								ALOGD("====> hddInfoStore[%d].HDDInfo.sdx_name[%s]\n", current_usb_index, hddInfoStore[current_usb_index].HDDInfo.sdx_name);
								ALOGD("====> selectedHDDIndex[%d]\n", selectedHDDIndex);

								char pos_buf[30];
								memset(pos_buf, 0, 30);
								sprintf(pos_buf, "%s/", pos);

								//if (selectedHDDIndex >= 0 && selectedHDDIndex == current_usb_index && strcmp(hddInfoStore[current_usb_index].HDDInfo.sdx_name, pos_buf) == 0 &&
								//	hddInfoStore[current_usb_index].HDDInfo.hdd_total_B_size > 1048576)	// > 1GB
								//if (hddInfoStore[current_usb_index].add_event_sent_flag && strcmp(hddInfoStore[current_usb_index].HDDInfo.sdx_name, pos_buf) == 0)
								if (selectedHDDIndex >= 0 && selectedHDDIndex == current_usb_index && strcmp(hddInfoStore[current_usb_index].HDDInfo.sdx_name, pos_buf) == 0)
								{
									selectedHDDIndex = -1;
									ignore_change_msgiov_flag = false;

									removeHDD(current_usb_index);
									if (hddInfoStore[current_usb_index].HDDInfo.partition_cnt > 1)
									{
										hddInfoStore[current_usb_index].add_event_sent_flag = false;
										ALOGD("==================> No sendRemoveEvent (multi-partition HDD): hddInfoStore[%d], hddInfoStore[%d].HDDInfo.partition_cnt[%d]\n", current_usb_index, current_usb_index, hddInfoStore[current_usb_index].HDDInfo.partition_cnt);
									}
									else
									{
										pDetectHDD->sendRemoveEvent(current_usb_index);
									}
								}
								else
								{
									if (hddInfoStore[current_usb_index].add_event_sent_flag && hddInfoStore[current_usb_index].HDDInfo.ext4_flag == 0)
									{
										if (hddInfoStore[current_usb_index].HDDInfo.partition_cnt > 1)
										{
											hddInfoStore[current_usb_index].add_event_sent_flag = false;
											ALOGD("==================> No sendRemoveEvent (multi-partition HDD): hddInfoStore[%d], hddInfoStore[%d].HDDInfo.partition_cnt[%d]\n", current_usb_index, current_usb_index, hddInfoStore[current_usb_index].HDDInfo.partition_cnt);
										}
										else
										{
											pDetectHDD->sendRemoveEvent(current_usb_index);
										}
									}
									if (hddInfoStore[current_usb_index].HDDInfo.ext4_flag == 1)
									{
										removeHDD(current_usb_index);
									}
								}
								hddInfoStore[current_usb_index].ClearHDDInfoStore();
							}
							//removeHDD(current_usb_index, &tempRemoveHDDInfo[current_usb_index]);
						}
						else
						{
							//yinohyinoh
							if (!strcmp(pos, "block/sda"))
							{
								property_set("ctl.stop", "fuse_sdcard1");
							//#if REDUNDANCY_MOUNT == 1
							//	char redundancy_unmount_buf[100];
							//	memset(redundancy_unmount_buf, 0, 100);
							//	sprintf(redundancy_unmount_buf, "umount %s", REDUNDANCY_MOUNT_DIR_1);
							//	system(redundancy_unmount_buf);
							//#endif
								property_set("tvs.hdd_ctrl.sda", "");
							}
							else
							{
								char sdx_str[30];
								char tmp_buf_prop[PROPERTY_VALUE_MAX] = {'\0'};
								for (int i='b'; i <= 'z'; i++)
								{
									memset(sdx_str, 0, 30);
									sprintf(sdx_str, "block/sd%c/", i);
									if (!strcmp(pos, sdx_str))
									{
										memset(sdx_str, 0, 30);
										sprintf(sdx_str, "fuse_usbdisk%c", i);
										property_set("ctl.stop", sdx_str);
									//#if REDUNDANCY_MOUNT == 1
									//	char redundancy_unmount_buf[100];
									//	memset(redundancy_unmount_buf, 0, 100);
									//	sprintf(redundancy_unmount_buf, "umount %s", REDUNDANCY_MOUNT_DIR_2);
									//	system(redundancy_unmount_buf);
									//#endif
										memset(sdx_str, 0, 30);
										sprintf(sdx_str, "tvs.hdd_ctrl.sd%c", i);
										property_set(sdx_str, "");
										break;
									}
								}
							}

							removeHDD(current_usb_index, &tempRemoveHDDInfo[current_usb_index]);
							if (hddInfoStore[current_usb_index].HDDInfo.ext4_flag == 0)
							{
								hddInfoStore[current_usb_index].ClearHDDInfoStore();
							}
						}
						//if (!hddInfoStore[0].SlotConnected && !hddInfoStore[1].SlotConnected)
						//{
						//	char remove_vold_8_0_buf[80];
						//	memset(remove_vold_8_0_buf, 0, 80);
						//	strcpy(remove_vold_8_0_buf, "rm -rf /dev/block/vold/8:0");
						//	system(remove_vold_8_0_buf);
						//}
					}
				}
			}
		}
	}
	else
	{
		if (change_cnt > 0)
		{
			ALOGD("------------> change_cnt [%d]\n", change_cnt);
			change_cnt = 0;

			//for (int i = 0; i < USB_SLOT_COUNT; i++)
			//{
			//	if (hddInfoStore[i].SlotConnected)
			//	{
			//		ALOGD("-----------> hddInfoStore[%d].buf_block_sdx_sdx1[%s]\n", i, hddInfoStore[i].buf_block_sdx_sdx1);
			//		ALOGD("-----------> hddInfoStore[%d].buf_usb_device[%s]\n", i, hddInfoStore[i].buf_usb_device);
			//		ALOGD("-----------> hddInfoStore[%d].buf_block_sdx[%s]\n", i, hddInfoStore[i].buf_block_sdx);
			//		if (hddInfoStore[i].buf_block_sdx_sdx1[0] == '\0')
			//		{
			//			memset(hddInfoStore[i].buf_block_sdx_sdx1, 0, 50);
			//			strcpy(hddInfoStore[i].buf_block_sdx_sdx1, hddInfoStore[i].buf_block_sdx);
			//		}

			//		if (hddInfoStore[i].buf_usb_device[0] == '\0')
			//		{
			//			char temp_buf[200];
			//			memset(temp_buf, 0, 200);
			//			strcpy(temp_buf, hddInfoStore[i].buf_block_sdx_buf_all);
			//			char* cur_pos = strstr(temp_buf, "/host");
			//			if (cur_pos)
			//			{
			//				temp_buf[cur_pos-temp_buf] = '\0';
			//				char* bef_pos = NULL;
			//				cur_pos = strchr(temp_buf, '/');
			//				while (cur_pos != NULL)
			//				{
			//					bef_pos = cur_pos;
			//					cur_pos = strchr(bef_pos+1, '/');
			//				}
			//				if (bef_pos != NULL)
			//				{
			//					temp_buf[bef_pos-temp_buf+1] = '\0';
			//					memset(hddInfoStore[i].buf_usb_device, 0, 200);
			//					memcpy(hddInfoStore[i].buf_usb_device, "/sys", 4);
			//					strcpy(&(hddInfoStore[i].buf_usb_device[4]), temp_buf);
			//				}
			//			}
			//		}

			//		if (hddInfoStore[i].buf_usb_device[0] != '\0' &&
			//			hddInfoStore[i].buf_block_sdx_sdx1[0] != '\0' &&
			//			hddInfoStore[i].buf_block_sdx[0] != '\0')
			//		{
			//			uint8_t partition_cnt = hddInfoStore[i].HDDInfo.partition_cnt;
			//			memset(&(hddInfoStore[i].HDDInfo), 0, sizeof(hddInfoStore[i].HDDInfo));
			//			hddInfoStore[i].HDDInfo.partition_cnt = partition_cnt;

			//			hddInfoStore[i].GetHDDInfo(i);
			//		}
			//		else
			//		{
			//			hddInfoStore[i].ClearHDDInfoStore();
			//			//memset(hddInfoStore[i].buf_usb_device, 0, 200);
			//			//memset(hddInfoStore[i].buf_block_sdx_sdx1, 0, 50);
			//			//memset(hddInfoStore[i].buf_block_sdx, 0, 50);
			//			//memset(hddInfoStore[i].buf_block_sdx_buf_all, 0, 200);
			//			//hddInfoStore[i].vPartitionBlock.clear();
			//		}
			//	}
			//}

			mount_booting_flag = false;

			if (!thread_running_flag)
			{
				pthread_create(&hdd_detect_thread[0], NULL, &thread_hdd_detect_main, (void *)NULL);
			}
		}
	}
	return true;
}

void DetectHDDThread::removeHDD(uint8_t current_usb_index, TempRemoveHDDInfo* tempRemoveHDDInfo)
{
	if (tempRemoveHDDInfo == NULL)	// ext4
	{
		char mounted_dir_buf[200];
		if (hddInfoStore[current_usb_index].HDDInfo.mounted_dir[0] != '\0')
		{
			memset(mounted_dir_buf, 0, 200);
			sprintf(mounted_dir_buf, "busybox umount -l %s", hddInfoStore[current_usb_index].HDDInfo.mounted_dir);
			ALOGD("-----------> mounted_dir_buf(ext4:umount:mounted_dir) [%s]\n", mounted_dir_buf);
			system(mounted_dir_buf);

			if (!strstr(hddInfoStore[current_usb_index].HDDInfo.mounted_dir, "sdcard") && !strstr(hddInfoStore[current_usb_index].HDDInfo.mounted_dir, "usbdisk") && !strstr(hddInfoStore[current_usb_index].HDDInfo.mounted_dir, "usb.HDD"))
			{
				memset(mounted_dir_buf, 0, 200);
				sprintf(mounted_dir_buf, "rm -rf %s", hddInfoStore[current_usb_index].HDDInfo.mounted_dir);
				ALOGD("-----------> mounted_dir_buf(ext4:rm:mounted_dir) [%s]\n", mounted_dir_buf);
				system(mounted_dir_buf);
			}
		}
		else if (hddInfoStore[current_usb_index].HDDInfo.sdx_name[0] != '\0')
		{
			char tmp_buf[30];
			memset(tmp_buf, 0, 30);
			sprintf(tmp_buf, "/dev/%s", hddInfoStore[current_usb_index].HDDInfo.sdx_name);
			tmp_buf[strlen(tmp_buf)-1] = '\0';
			hddInfoStore[current_usb_index].unMountpoint(tmp_buf, true);
		}

		if (hddInfoStore[current_usb_index].dev_name[0] != '\0')
		{
			char tmp_dev_name[10];
			memset(tmp_dev_name, 0, 10);
			strcpy(tmp_dev_name, hddInfoStore[current_usb_index].dev_name);
			if (hddInfoStore[current_usb_index].GPTFlag)
			{
#if 0
				memset(mounted_dir_buf, 0, 200);
				sprintf(mounted_dir_buf, "rm -rf /dev/block/vold/%s", tmp_dev_name);
				system(mounted_dir_buf);
#endif

				int dev_name_len = strlen(tmp_dev_name);
				tmp_dev_name[dev_name_len-1] = hddInfoStore[current_usb_index].dev_name[dev_name_len-1]+1;
			}
			else if (atoi(tmp_dev_name) % 16)
			{
				int dev_name_len = strlen(tmp_dev_name);
				char tmp_dev_name2[10];
				memset(tmp_dev_name2, 0, 10);
				strcpy(tmp_dev_name2, tmp_dev_name);
				tmp_dev_name2[dev_name_len-1] = tmp_dev_name[dev_name_len-1]-1;
#if 0
				memset(mounted_dir_buf, 0, 200);
				sprintf(mounted_dir_buf, "rm -rf /dev/block/vold/%s", tmp_dev_name2);
				ALOGD("-----------> mounted_dir_buf(ext4:rm:dev_name) [%s]\n", mounted_dir_buf);
				system(mounted_dir_buf);
#endif
			}
#if 0
			memset(mounted_dir_buf, 0, 200);
			sprintf(mounted_dir_buf, "rm -rf /dev/block/vold/%s", tmp_dev_name);
			ALOGD("-----------> mounted_dir_buf(ext4:rm:dev_name) [%s]\n", mounted_dir_buf);
			system(mounted_dir_buf);
#endif

			Vector<PartitionBlock>::iterator iter;
			for (iter = hddInfoStore[current_usb_index].vPartitionBlock.begin(); iter != hddInfoStore[current_usb_index].vPartitionBlock.end(); iter++)
			{
				char tmpBuf[50];
				memset(tmpBuf, 0, 50);
				sprintf(tmpBuf, "rm -f /dev/block/vold/%s", iter->dev_name);
				ALOGD("---------> removeHDD(): PartitionBlock: tmpBuf [%s]", tmpBuf);
				system(tmpBuf);
			}
		}
	}
	else	// vfat
	{
		bool umount_flag_by_mounted_dir = false;
		char mounted_dir_buf[200];
		if (tempRemoveHDDInfo->HDDInfo.mounted_dir[0] != '\0')
		{
			memset(mounted_dir_buf, 0, 200);
			sprintf(mounted_dir_buf, "busybox umount -l %s", tempRemoveHDDInfo->HDDInfo.mounted_dir);
			ALOGD("-----------> mounted_dir_buf(vfat:umount:mounted_dir) [%s]\n", mounted_dir_buf);
			system(mounted_dir_buf);
			umount_flag_by_mounted_dir = true;

			if (!strstr(tempRemoveHDDInfo->HDDInfo.mounted_dir, "sdcard") && !strstr(tempRemoveHDDInfo->HDDInfo.mounted_dir, "usbdisk") && !strstr(tempRemoveHDDInfo->HDDInfo.mounted_dir, "usb.HDD"))
			{
				memset(mounted_dir_buf, 0, 200);
				sprintf(mounted_dir_buf, "rm -rf %s", tempRemoveHDDInfo->HDDInfo.mounted_dir);
				ALOGD("-----------> mounted_dir_buf(vfat:rm:mounted_dir) [%s]\n", mounted_dir_buf);
				system(mounted_dir_buf);
			}
		}

		if (tempRemoveHDDInfo->dev_name[0] != '\0')
		{
			char tmp_dev_name[10];
			memset(tmp_dev_name, 0, 10);
			strcpy(tmp_dev_name, tempRemoveHDDInfo->dev_name);
			if (tempRemoveHDDInfo->GPTFlag)
			{
#if 0
				memset(mounted_dir_buf, 0, 200);
				sprintf(mounted_dir_buf, "rm -rf /dev/block/vold/%s", tmp_dev_name);
				system(mounted_dir_buf);
#endif

				int dev_name_len = strlen(tmp_dev_name);
				tmp_dev_name[dev_name_len-1] = tempRemoveHDDInfo->dev_name[dev_name_len-1]+1;
			}

			if (!umount_flag_by_mounted_dir)
			{
				memset(mounted_dir_buf, 0, 200);
				sprintf(mounted_dir_buf, "busybox umount -l /dev/block/vold/%s", tmp_dev_name);
				ALOGD("-----------> mounted_dir_buf(vfat:umount:dev_name) [%s]\n", mounted_dir_buf);
				system(mounted_dir_buf);
			}

			if (atoi(tmp_dev_name) % 16)
			{
				int dev_name_len = strlen(tmp_dev_name);
				char tmp_dev_name2[10];
				memset(tmp_dev_name2, 0, 10);
				strcpy(tmp_dev_name2, tmp_dev_name);
				tmp_dev_name2[dev_name_len-1] = tmp_dev_name[dev_name_len-1]-1;
#if 0
				memset(mounted_dir_buf, 0, 200);
				sprintf(mounted_dir_buf, "rm -rf /dev/block/vold/%s", tmp_dev_name2);
				ALOGD("-----------> mounted_dir_buf(vfat:rm:dev_name) [%s]\n", mounted_dir_buf);
				system(mounted_dir_buf);
#endif
			}
#if 0
			memset(mounted_dir_buf, 0, 200);
			sprintf(mounted_dir_buf, "rm -rf /dev/block/vold/%s", tmp_dev_name);
			ALOGD("-----------> mounted_dir_buf(vfat:rm) [%s]\n", mounted_dir_buf);
			system(mounted_dir_buf);
#endif
			if (tempRemoveHDDInfo->partition_cnt > 1)
			{
				for (int i=1; i < tempRemoveHDDInfo->partition_cnt; i++)
				{
					memset(tmp_dev_name, 0, 10);
					strcpy(tmp_dev_name, tempRemoveHDDInfo->dev_name);
					int dev_name_len = strlen(tmp_dev_name);
					tmp_dev_name[dev_name_len-1] = tempRemoveHDDInfo->dev_name[dev_name_len-1]+i;
#if 0
					memset(mounted_dir_buf, 0, 200);
					sprintf(mounted_dir_buf, "rm -rf /dev/block/vold/%s", tmp_dev_name);
					ALOGD("-----------> mounted_dir_buf(vfat:rm) [%s]\n", mounted_dir_buf);
					system(mounted_dir_buf);
#endif
				}
			}
		}
		memset(tempRemoveHDDInfo, 0, sizeof(TempRemoveHDDInfo));
	}
}

void DetectHDDThread::detectHDD(int8_t current_usb_index)
{
	if (hddInfoStore[current_usb_index].SlotConnected)
	{
		ALOGD("-----------> hddInfoStore[%d].buf_block_sdx_sdx1[%s]\n", current_usb_index, hddInfoStore[current_usb_index].buf_block_sdx_sdx1);
		ALOGD("-----------> hddInfoStore[%d].buf_usb_device[%s]\n", current_usb_index, hddInfoStore[current_usb_index].buf_usb_device);
		ALOGD("-----------> hddInfoStore[%d].buf_block_sdx[%s]\n", current_usb_index, hddInfoStore[current_usb_index].buf_block_sdx);
		if (hddInfoStore[current_usb_index].buf_block_sdx_sdx1[0] == '\0')
		{
			memset(hddInfoStore[current_usb_index].buf_block_sdx_sdx1, 0, 50);
			strcpy(hddInfoStore[current_usb_index].buf_block_sdx_sdx1, hddInfoStore[current_usb_index].buf_block_sdx);
		}

		if (hddInfoStore[current_usb_index].buf_usb_device[0] == '\0')
		{
			char temp_buf[200];
			memset(temp_buf, 0, 200);
			strcpy(temp_buf, hddInfoStore[current_usb_index].buf_block_sdx_buf_all);
			char* cur_pos = strstr(temp_buf, "/host");
			if (cur_pos)
			{
				temp_buf[cur_pos-temp_buf] = '\0';
				char* bef_pos = NULL;
				cur_pos = strchr(temp_buf, '/');
				while (cur_pos != NULL)
				{
					bef_pos = cur_pos;
					cur_pos = strchr(bef_pos+1, '/');
				}
				if (bef_pos != NULL)
				{
					temp_buf[bef_pos-temp_buf+1] = '\0';
					memset(hddInfoStore[current_usb_index].buf_usb_device, 0, 200);
					memcpy(hddInfoStore[current_usb_index].buf_usb_device, "/sys", 4);
					strcpy(&(hddInfoStore[current_usb_index].buf_usb_device[4]), temp_buf);
				}
			}
		}

		if (hddInfoStore[current_usb_index].buf_usb_device[0] != '\0' &&
			hddInfoStore[current_usb_index].buf_block_sdx_sdx1[0] != '\0' &&
			hddInfoStore[current_usb_index].buf_block_sdx[0] != '\0')
		{
			uint8_t partition_cnt = hddInfoStore[current_usb_index].HDDInfo.partition_cnt;
			memset(&(hddInfoStore[current_usb_index].HDDInfo), 0, sizeof(hddInfoStore[current_usb_index].HDDInfo));
			hddInfoStore[current_usb_index].HDDInfo.partition_cnt = partition_cnt;

			hddInfoStore[current_usb_index].GetHDDInfo(current_usb_index);

			bool SlotHDDIndexQualified_tmp = pDetectHDD->IsHDDQualified(current_usb_index, hddInfoStore[current_usb_index].HDDInfo.idVendor, hddInfoStore[current_usb_index].HDDInfo.idProduct);
			if (!SlotHDDIndexQualified_tmp)
			{
				if (selectedHDDIndex < 0)
				{
					FILE *sys_fp;
					char sys_line[10];
					char tmp_sys_buf[50];
					memset(tmp_sys_buf, 0, 50);
					sprintf(tmp_sys_buf, "/sys/%sremovable", hddInfoStore[current_usb_index].HDDInfo.sdx_name);

					if (!(sys_fp = fopen(tmp_sys_buf, "r")))
					{
						ALOGE("Error opening %s (%s)", tmp_sys_buf, strerror(errno));
						return ;
					}

					fgets(sys_line, sizeof(sys_line), sys_fp);
					sys_line[strlen(sys_line)-1] = '\0';
					fclose(sys_fp);

					if (!strcmp(sys_line, "0"))
					{
						uint8_t sndMsgConfig[64];
						memset(sndMsgConfig, 0, 64);
						sndMsgConfig[0] = 1;
						sndMsgConfig[1] = WaitUserThread::HDD_NOT_SUPPORTED;
						sndMsgConfig[2] = 1;
						sndMsgConfig[3] = current_usb_index;
						pWaitUser->sendToThread(sndMsgConfig, 64);
						hddInfoStore[current_usb_index].add_event_sent_flag = true;
						ALOGD("==================> post hdd not supported event: hddInfoStore[%d]\n", current_usb_index);
					}
					else
					{
						uint8_t sndMsgConfig[64];
						memset(sndMsgConfig, 0, 64);
						sndMsgConfig[0] = 1;
						sndMsgConfig[1] = WaitUserThread::HDD_MEDIA_DISK;
						sndMsgConfig[2] = 1;
						sndMsgConfig[3] = current_usb_index;
						pWaitUser->sendToThread(sndMsgConfig, 64);
						//hddInfoStore[current_usb_index].add_event_sent_flag = true;
						ALOGD("==================> post hdd media disk event: hddInfoStore[%d]\n", current_usb_index);
					}
				}
			}
			else if (hddInfoStore[current_usb_index].HDDInfo.partition_cnt > 1)
			{
				hddInfoStore[current_usb_index].add_event_sent_flag = true;
				ALOGD("==================> No sendAddEvent (multi-partition HDD): hddInfoStore[%d], hddInfoStore[%d].HDDInfo.partition_cnt[%d]\n", current_usb_index, current_usb_index, hddInfoStore[current_usb_index].HDDInfo.partition_cnt);
			}
		}
		else
		{
			hddInfoStore[current_usb_index].ClearHDDInfoStore();
			//memset(hddInfoStore[current_usb_index].buf_usb_device, 0, 200);
			//memset(hddInfoStore[current_usb_index].buf_block_sdx_sdx1, 0, 50);
			//memset(hddInfoStore[current_usb_index].buf_block_sdx, 0, 50);
			//memset(hddInfoStore[current_usb_index].buf_block_sdx_buf_all, 0, 200);
			//hddInfoStore[current_usb_index].vPartitionBlock.clear();
		}
	}
}

bool DetectHDDThread::IsHDDQualified(int8_t index, char *idVendor, char *idProduct)
{
	if (index < 0 || index > 1)
		return false;

	bool flag = false;

	if (access(HDD_ALL, F_OK) >= 0)
	{
		if (hddInfoStore[index].SlotConnected)
			flag = true;
	}
	else if (access(HDD_WHITELIST, F_OK) >= 0)
	{
		FILE *wlfp;
		if ((wlfp = fopen(HDD_WHITELIST, "r")) != NULL)
		{
			char buff[80];
			char idVendorBuf[10];
			char idProductBuf[10];
			while (!feof(wlfp))
			{
				memset(buff, 0, 80);
				fgets(buff, 79, wlfp);
				if (buff[0] != '#')
				{
					buff[strlen(buff)-1] = '\0';
					char* wlpos = strchr(buff, ',');
					if (wlpos)
					{
						memset(idVendorBuf, 0, 10);
						memcpy(idVendorBuf, buff, wlpos-buff);
						memset(idProductBuf, 0, 10);
						strcpy(idProductBuf, wlpos+1);
						if (strcmp(idVendorBuf, "all") == 0 &&
							strcmp(idProductBuf, "all") == 0)
						{
							if (hddInfoStore[index].SlotConnected)
								flag = true;
						}
						else if (strcmp(idVendorBuf, idVendor) == 0 &&
								 strcmp(idProductBuf, idProduct) == 0)
						{
							if (hddInfoStore[index].SlotConnected)
								flag = true;
						}
					}
				}
			}
			fclose(wlfp);
		}
	}

	if ((strcmp(idVendor, "0411") == 0 &&
		 strcmp(idProduct, "01de") == 0) ||	// BUFFALO & HD-PCTU3 & 500GB
		(strcmp(idVendor, "1058") == 0 &&
		 strcmp(idProduct, "10a8") == 0) ||	// Western Digital & Elements 10A8 & 500GB
		(strcmp(idVendor, "1058") == 0 &&
		 strcmp(idProduct, "10b8") == 0))	// Western Digital & Elements 10B8 & 500GB
	{
		if (hddInfoStore[index].SlotConnected)
			flag = true;
	}

	return flag;
}

void DetectHDDThread::setHDDQualified()
{
	if (access(HDD_ALL, F_OK) >= 0)
	{
		for (int i = 0; i < USB_SLOT_COUNT; i++)
		{
			if (hddInfoStore[i].SlotConnected)
				hddInfoStore[i].hdd_qualified = true;
		}
	}
	else if (access(HDD_WHITELIST, F_OK) >= 0)
	{
		FILE *wlfp;
		if ((wlfp = fopen(HDD_WHITELIST, "r")) != NULL)
		{
			char buff[80];
			char idVendorBuf[10];
			char idProductBuf[10];
			while (!feof(wlfp))
			{
				memset(buff, 0, 80);
				fgets(buff, 79, wlfp);
				if (buff[0] != '#')
				{
					buff[strlen(buff)-1] = '\0';
					char* wlpos = strchr(buff, ',');
					if (wlpos)
					{
						memset(idVendorBuf, 0, 10);
						memcpy(idVendorBuf, buff, wlpos-buff);
						memset(idProductBuf, 0, 10);
						strcpy(idProductBuf, wlpos+1);
						if (strcmp(idVendorBuf, "all") == 0 &&
							strcmp(idProductBuf, "all") == 0)
						{
							for (int i = 0; i < USB_SLOT_COUNT; i++)
							{
								if (hddInfoStore[i].SlotConnected)
									hddInfoStore[i].hdd_qualified = true;
							}
						}
						else if (strcmp(idVendorBuf, hddInfoStore[0].HDDInfo.idVendor) == 0 &&
								 strcmp(idProductBuf, hddInfoStore[0].HDDInfo.idProduct) == 0)
						{
							if (hddInfoStore[0].SlotConnected)
								hddInfoStore[0].hdd_qualified = true;
						}
						else if (strcmp(idVendorBuf, hddInfoStore[1].HDDInfo.idVendor) == 0 &&
								 strcmp(idProductBuf, hddInfoStore[1].HDDInfo.idProduct) == 0)
						{
							if (hddInfoStore[1].SlotConnected)
								hddInfoStore[1].hdd_qualified = true;
						}
					}
				}
			}
			fclose(wlfp);
		}
	}

	for (int i = 0; i < USB_SLOT_COUNT; i++)
	{
		if (hddInfoStore[i].hdd_qualified == false)
		{
			if ((strcmp(hddInfoStore[i].HDDInfo.idVendor, "0411") == 0 &&
				 strcmp(hddInfoStore[i].HDDInfo.idProduct, "01de") == 0) ||	// BUFFALO & HD-PCTU3 & 500GB
				(strcmp(hddInfoStore[i].HDDInfo.idVendor, "1058") == 0 &&
				 strcmp(hddInfoStore[i].HDDInfo.idProduct, "10a8") == 0) ||	// Western Digital & Elements 10A8 & 500GB
				(strcmp(hddInfoStore[i].HDDInfo.idVendor, "1058") == 0 &&
				 strcmp(hddInfoStore[i].HDDInfo.idProduct, "10b8") == 0))	// Western Digital & Elements 10B8 & 500GB
			{
				if (hddInfoStore[i].SlotConnected)
					hddInfoStore[i].hdd_qualified = true;
			}
		}
	}
}

#if DEV_FTS_USE == 1
char* DetectHDDThread::getKeyValue(const char* key)
{
	static char buf[50];

	int fd, res;
	struct flash_ts_io_req req;

	memset(buf, 0, 50);

	fd = open("/dev/fts", O_RDONLY);
	if (fd < 0)
	{
		ALOGD("open('/dev/fts')\n");
		return buf;
	}

	memset(&req, 0, sizeof(req));
	strncpy(req.key, key, sizeof(req.key));

	res = ioctl(fd, FLASH_TS_IO_GET, &req);
	if (res)
	{
		ALOGD("ioctl error\n");
		close(fd);
		return buf;
	}

	memcpy(buf, req.val, 49);

	close(fd);

	return buf;
}

int8_t DetectHDDThread::setKeyValue(const char* key, const char* val)
{
	int fd, res;
	struct flash_ts_io_req req;

	fd = open("/dev/fts", O_WRONLY);
	if (fd < 0)
	{
		ALOGD("open('/dev/fts')\n");
		return -1;
	}

	memset(&req, 0, sizeof(req));
	strncpy(req.key, key, sizeof(req.key));
	strncpy(req.val, val, sizeof(req.val));

	res = ioctl(fd, FLASH_TS_IO_SET, &req);
	if (res)
	{
		ALOGD("ioctl error\n");
		close(fd);
		return -1;
	}

	close(fd);

	return 0;
}
#else
char* DetectHDDThread::getSerialNumber()
{
	static char buf[50];

	char serial_no_buf[100];
	memset(serial_no_buf, 0, 100);
	sprintf(serial_no_buf, "%sserial_number_for_PVR.txt", HDD_HOME);
	FILE* fp_serial_no = fopen(serial_no_buf, "r");
	if (fp_serial_no)
	{
		fgets(buf, sizeof(buf), fp_serial_no);
		buf[strlen(buf)] = '\0';
		fclose(fp_serial_no);
	}
	else
	{
		memset(buf, 0, 50);
	}

	ALOGD("==============> getSerialNumber: buf[%s]", buf);

	return buf;
}

int8_t DetectHDDThread::setSerialNumber(const char* val)
{
	char serial_no_buf[100];
	memset(serial_no_buf, 0, 100);
	sprintf(serial_no_buf, "%sserial_number_for_PVR.txt", HDD_HOME);
	FILE* fp_serial_no = fopen(serial_no_buf, "w");
	if (fp_serial_no)
	{
		fputs(val, fp_serial_no);
		fclose(fp_serial_no);
		return 0;
	}
	else
	{
		return -1;
	}
}
#endif

void DetectHDDThread::sendAddEvent(uint8_t index)
{
	if (index == 0 || index == 1)
	{
		if (!strcmp(hddInfoStore[index].HDDInfo.sdx_name, "block/sdz/"))
		{
			ALOGD("==================> sendAddEvent: hddInfoStore[%d].HDDInfo.sdx_name[%s]\n", index, hddInfoStore[index].HDDInfo.sdx_name);
			ALOGD("==================> sendAddEvent: reboot\n");
			system("reboot");
		}
		else
		{
			if (!hddInfoStore[index].add_event_sent_flag)
			{
				//if (/*fs_ext2_supported &&*/ hddInfoStore[index].HDDInfo.ext4_flag)
				//{
				//	char tmp_buf[200];
				//	memset(tmp_buf, 0, 200);
				//	sprintf(tmp_buf, "rm -rf %s/TimeShift*", hddInfoStore[index].HDDInfo.mounted_dir);
				//	system(tmp_buf);
				//}
				uint8_t sndMsgConfig[64];
				memset(sndMsgConfig, 0, 64);
				sndMsgConfig[0] = 1;
				sndMsgConfig[1] = WaitUserThread::HDD_ADD_DETECT;
				sndMsgConfig[2] = 1;
				sndMsgConfig[3] = index;
				pWaitUser->sendToThread(sndMsgConfig, 64);
				pWaitUser->sendToThread((uint8_t*)&(hddInfoStore[index].HDDInfo), sizeof(HDDInfoStruct));
				hddInfoStore[index].add_event_sent_flag = true;
				ALOGD("==================> post usb add event: hddInfoStore[%d]\n", index);
			}
		}
	}
}

void DetectHDDThread::sendRemoveEvent(uint8_t index)
{
	if (index == 0 || index == 1)
	{
		if (hddInfoStore[index].add_event_sent_flag)
		{
			uint8_t sndMsgConfig[64];
			memset(sndMsgConfig, 0, 64);
			sndMsgConfig[0] = 1;
			sndMsgConfig[1] = WaitUserThread::HDD_REMOVE_DETECT;
			sndMsgConfig[2] = 1;
			sndMsgConfig[3] = index;
			pWaitUser->sendToThread(sndMsgConfig, 64);
			hddInfoStore[index].add_event_sent_flag = false;
			ALOGD("==================> post usb remove event: hddInfoStore[%d]\n", index);
		}
	}
}

void DetectHDDThread::sendUpdateEvent(int8_t index)
{
	//if (index == 0 || index == 1)
	{
		//if (hddInfoStore[index].add_event_sent_flag)
		{
			uint8_t sndMsgConfig[64];
			memset(sndMsgConfig, 0, 64);
			sndMsgConfig[0] = 1;
			sndMsgConfig[1] = WaitUserThread::HDD_UPDATE_DETECT;
			sndMsgConfig[2] = 1;
			sndMsgConfig[3] = (int8_t)index;
			pWaitUser->sendToThread(sndMsgConfig, 64);
			pWaitUser->sendToThread((uint8_t*)&(hddInfoStore[index].HDDInfo), sizeof(hddInfoStore[index].HDDInfo));
			hddInfoStore[index].add_event_sent_flag = true;
			ALOGD("==================> post usb update event: hddInfoStore[%d]\n", (int)index);
		}
#if 0
        else
        {
			ALOGD("==================> post usb update event: index(%d) add_event_sent_flag false!\n", (int)index);
        }
#endif
	}
}

void DetectHDDThread::sendHDDErrEvent(uint8_t index)
{
	if (index == 0 || index == 1)
	{
		if (hddInfoStore[index].add_event_sent_flag)
		{
			uint8_t sndMsgConfig[64];
			memset(sndMsgConfig, 0, 64);
			sndMsgConfig[0] = 1;
			sndMsgConfig[1] = WaitUserThread::HDD_HDDERR_DETECT;
			sndMsgConfig[2] = 1;
			sndMsgConfig[3] = index;
			pWaitUser->sendToThread(sndMsgConfig, 64);
			hddInfoStore[index].add_event_sent_flag = true;
			ALOGD("==================> post usb hdd err event: hddInfoStore[%d]\n", index);
		}
	}
}

#if SEARCH_HDD_ON_BOOTING == 1
uint8_t DetectHDDThread::getPartitionCount(const char *sdxName)
{
	uint8_t partition_count = 0;

	char major[256];
	char minor[256];
	char blocks[256];
	char name[256];
	FILE *fp;
	char line[1024];
	uint8_t sdx_len = strlen(sdxName);

	if (!(fp = fopen("/proc/partitions", "r")))
	{
		ALOGE("Error opening /proc/partitions (%s)", strerror(errno));
		return partition_count;
	}

	while (fgets(line, sizeof(line), fp))
	{
		line[strlen(line)-1] = '\0';
		sscanf(line, "%255s %255s %255s %255s\n", major, minor, blocks, name);
		if (!strncmp(name, sdxName, sdx_len))
		{
			partition_count++;
		}
	}

	fclose(fp);
	return partition_count;
}

inline bool DetectHDDThread::setEventForHDDOnBooting(char *ptr)
{
	if (SearchHDDOnBootingIndex >= -1 && SearchHDDOnBootingIndex < HDD_EVENT_ON_BOOTING_MAX-1)
	{
		SearchHDDOnBooting[++SearchHDDOnBootingIndex] = ptr;
		return true;
	}
	else
	{
		return false;
	}
}

void DetectHDDThread::addUEventForHDDOnBooting(const char *sdxName)
{
	char sdxName_buf[30];

	memset(sdxName_buf, 0, 30);
	sprintf(sdxName_buf, "/dev/block/%s", sdxName);
	if (access(sdxName_buf, F_OK) >= 0)
	{
		DIR           *dp;
		struct dirent *dirp;

		char buf_pos_num[10];
		memset(buf_pos_num, 0, 10);
		strcpy(buf_pos_num, POSITION_NUM_STR_REAR_PATH1);

		char buf_dir[256];
		memset(buf_dir, 0, 256);
		sprintf(buf_dir, "%s/usb%c/%c-1%s%s:1.0", SEARCH_HDD_ON_BOOTING_PREFIX_REAR, buf_pos_num[1], buf_pos_num[1], POSITION_NUM_STR_REAR_PATH1, POSITION_NUM_STR_REAR_PATH1);

		uint8_t host_target_number = 0;
		if ((dp = opendir(buf_dir)) != NULL)
		{
			while ((dirp = readdir(dp)) != NULL)
			{
				if (strcmp(dirp->d_name, ".") == 0 ||
					strcmp(dirp->d_name, "..") == 0)
					continue;

				if (dirp->d_type == DT_DIR && !strncmp(dirp->d_name, "host", 4))
				{
					host_target_number = atoi(&(dirp->d_name[4]));
					break;
				}
			}

			if (closedir(dp) < 0)
				ALOGD("can't close directory %s\n", buf_dir);
		}

		if (host_target_number > 0)
		{
			char buf[256];
			memset(buf, 0, 256);
			sprintf(buf, "%s/host%d/target%d:0:0/%d:0:0:0/block/%s", buf_dir, host_target_number, host_target_number, host_target_number, sdxName);

			if (access(buf, F_OK) >= 0)
			{
				uint8_t get_partition_cnt = getPartitionCount(sdxName);
				for (int i=2; i < get_partition_cnt; i++)
				{
					memset(sdxName_buf, 0, 30);
					sprintf(sdxName_buf, "/dev/block/%s%c", sdxName, i);
					if (access(sdxName_buf, F_OK) >= 0)
					{
						uint8_t buf_len = strlen(buf) + 6;
						char *temp_ptr = (char*) malloc(buf_len+1);
						if (temp_ptr != NULL)
						{
							memset(temp_ptr, 0, buf_len+1);
							sprintf(temp_ptr, "add@%s/%s%d", &buf[4], sdxName, i);
							if (!setEventForHDDOnBooting(temp_ptr))
							{
								free(temp_ptr);
							}
						}
					}
				}

				memset(sdxName_buf, 0, 30);
				sprintf(sdxName_buf, "/dev/block/%s1", sdxName);
				if (access(sdxName_buf, F_OK) >= 0)
				{
					uint8_t buf_len = strlen(buf) + 5;
					char *temp_ptr = (char*) malloc(buf_len+1);
					if (temp_ptr != NULL)
					{
						memset(temp_ptr, 0, buf_len+1);
						sprintf(temp_ptr, "add@%s/%s1", &buf[4], sdxName);
						if (!setEventForHDDOnBooting(temp_ptr))
						{
							free(temp_ptr);
						}
					}
				}

				uint8_t buf_len = strlen(buf);
				char *temp_ptr = (char*) malloc(buf_len+1);
				if (temp_ptr != NULL)
				{
					memset(temp_ptr, 0, buf_len+1);
					sprintf(temp_ptr, "add@%s", &buf[4]);
					if (!setEventForHDDOnBooting(temp_ptr))
					{
						free(temp_ptr);
					}
				}

				char buf_usb_device[256];
				memset(buf_usb_device, 0, 256);
				sprintf(buf_usb_device, "%s/host%d/target%d:0:0/%d:0:0:0/%s%d:0:0:0", buf_dir, host_target_number, host_target_number, host_target_number, USB_DEVICE, host_target_number);
				buf_len = strlen(buf_usb_device);
				temp_ptr = (char*) malloc(buf_len+1);
				if (temp_ptr != NULL)
				{
					memset(temp_ptr, 0, buf_len+1);
					sprintf(temp_ptr, "add@%s", &buf_usb_device[4]);
					if (!setEventForHDDOnBooting(temp_ptr))
					{
						free(temp_ptr);
					}
				}
			}
		}

		memset(buf_pos_num, 0, 10);
		strcpy(buf_pos_num, POSITION_NUM_STR_FRONT_PATH1);

		memset(buf_dir, 0, 256);
		sprintf(buf_dir, "%s/usb%c/%c-1%s%s:1.0", SEARCH_HDD_ON_BOOTING_PREFIX_FRONT, buf_pos_num[1], buf_pos_num[1], POSITION_NUM_STR_FRONT_PATH1, POSITION_NUM_STR_FRONT_PATH1);

		host_target_number = 0;
		if ((dp = opendir(buf_dir)) != NULL)
		{
			while ((dirp = readdir(dp)) != NULL)
			{
				if (strcmp(dirp->d_name, ".") == 0 ||
					strcmp(dirp->d_name, "..") == 0)
					continue;

				if (dirp->d_type == DT_DIR && !strncmp(dirp->d_name, "host", 4))
				{
					host_target_number = atoi(&(dirp->d_name[4]));
					break;
				}
			}

			if (closedir(dp) < 0)
				ALOGD("can't close directory %s\n", buf_dir);
		}

		if (host_target_number > 0)
		{
			char buf[256];
			memset(buf, 0, 256);
			sprintf(buf, "%s/host%d/target%d:0:0/%d:0:0:0/block/%s", buf_dir, host_target_number, host_target_number, host_target_number, sdxName);

			if (access(buf, F_OK) >= 0)
			{
				uint8_t get_partition_cnt = getPartitionCount(sdxName);
				for (int i=2; i < get_partition_cnt; i++)
				{
					memset(sdxName_buf, 0, 30);
					sprintf(sdxName_buf, "/dev/block/%s%c", sdxName, i);
					if (access(sdxName_buf, F_OK) >= 0)
					{
						uint8_t buf_len = strlen(buf) + 6;
						char *temp_ptr = (char*) malloc(buf_len+1);
						if (temp_ptr != NULL)
						{
							memset(temp_ptr, 0, buf_len+1);
							sprintf(temp_ptr, "add@%s/%s%d", &buf[4], sdxName, i);
							if (!setEventForHDDOnBooting(temp_ptr))
							{
								free(temp_ptr);
							}
						}
					}
				}

				memset(sdxName_buf, 0, 30);
				sprintf(sdxName_buf, "/dev/block/%s1", sdxName);
				if (access(sdxName_buf, F_OK) >= 0)
				{
					uint8_t buf_len = strlen(buf) + 5;
					char *temp_ptr = (char*) malloc(buf_len+1);
					if (temp_ptr != NULL)
					{
						memset(temp_ptr, 0, buf_len+1);
						sprintf(temp_ptr, "add@%s/%s1", &buf[4], sdxName);
						if (!setEventForHDDOnBooting(temp_ptr))
						{
							free(temp_ptr);
						}
					}
				}

				uint8_t buf_len = strlen(buf);
				char *temp_ptr = (char*) malloc(buf_len+1);
				if (temp_ptr != NULL)
				{
					memset(temp_ptr, 0, buf_len+1);
					sprintf(temp_ptr, "add@%s", &buf[4]);
					if (!setEventForHDDOnBooting(temp_ptr))
					{
						free(temp_ptr);
					}
				}

				char buf_usb_device[256];
				memset(buf_usb_device, 0, 256);
				sprintf(buf_usb_device, "%s/host%d/target%d:0:0/%d:0:0:0/%s%d:0:0:0", buf_dir, host_target_number, host_target_number, host_target_number, USB_DEVICE, host_target_number);
				buf_len = strlen(buf_usb_device);
				temp_ptr = (char*) malloc(buf_len+1);
				if (temp_ptr != NULL)
				{
					memset(temp_ptr, 0, buf_len+1);
					sprintf(temp_ptr, "add@%s", &buf_usb_device[4]);
					if (!setEventForHDDOnBooting(temp_ptr))
					{
						free(temp_ptr);
					}
				}
			}
		}
	}
}
#endif
