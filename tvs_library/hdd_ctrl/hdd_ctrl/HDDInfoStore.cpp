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
#define LOG_TAG "HDDInfoStore"

#include "HDDInfoStore.h"
#include "DetectHDDThread.h"

#include <sys/mount.h>
#include <blkid/blkid.h>
#include "make_ext4fs.h"

#define MOUNT_OPTS      "errors=continue,context=u:object_r:oemfs:s0"

bool fs_ext2_supported = false;
char fs_ext_str[5];

bool on_formatting_flag = false;

pthread_t hdd_format_thread[1];
void* thread_hdd_format_main(void*);

HDDInfoStore::HDDInfoStore()
{
	SlotConnected = false;
	ChangeEventReceived = false;
	GPTFlag = false;
	HDDErrFlag = false;
	memset(&HDDInfo, 0, sizeof(HDDInfo));
	memset(dev_name, 0, 10);
	vPartitionBlock.clear();
	memset(buf_usb_device, 0, 200);
    memset(buf_block_sdx, 0, 50);
	memset(buf_block_sdx_buf_all, 0, 200);
    memset(buf_block_sdx_sdx1, 0, 50);
	hdd_qualified = false;
	add_event_sent_flag = false;
}

HDDInfoStore::~HDDInfoStore()
{
	ClearHDDInfoStore();
}

void HDDInfoStore::ClearHDDInfoStore()
{
	SlotConnected = false;
	ChangeEventReceived = false;
	GPTFlag = false;
	HDDErrFlag = false;
	memset(&HDDInfo, 0, sizeof(HDDInfo));
	memset(dev_name, 0, 10);
	vPartitionBlock.clear();
	memset(buf_usb_device, 0, 200);
    memset(buf_block_sdx, 0, 50);
	pDetectHDD->current_usb_index_for_buf_block_sdx[0] = -1;
	pDetectHDD->current_usb_index_for_buf_block_sdx[1] = -1;
	memset(buf_block_sdx_buf_all, 0, 200);
    memset(buf_block_sdx_sdx1, 0, 50);
	hdd_qualified = false;
	add_event_sent_flag = false;
}

void HDDInfoStore::searchMountInfo(uint8_t hdd_index, char* mounted_dir, char* fs_type, char* rw_mode, char* serial, char* sdx_name, char* dev_name, bool &ext4_flag)
{
	ALOGD("=========================> searchMountInfo(): hdd_index[%d], mounted_dir[%s], fs_type[%s], rw_mode[%s], serial[%s], sdx_name[%s], dev_name[%s], ext4_flag[%s]", hdd_index, mounted_dir, fs_type, rw_mode, serial, sdx_name, dev_name, ext4_flag ? "true" : "false");

	FILE *fp;
	int  waiting_cnt = 0;
	char buff[512];
	char *first_space, *second_space;
	bool dev_or_sdx_flag = true;
	memset(mounted_dir, 0, 80);
	memset(fs_type, 0, 10);
	memset(rw_mode, 0, 5);
	while (waiting_cnt < 300 && mounted_dir[0] == '\0' && (fp = fopen("/proc/mounts", "r")) != NULL)
	{
		while (!feof(fp))
		{
			memset(buff, 0, 512);
			fgets(buff, 511, fp);
			char tmp_sdx_name[20];
			memset(tmp_sdx_name, 0, 20);
			strcpy(tmp_sdx_name, sdx_name);
			tmp_sdx_name[strlen(sdx_name)-1] = '\0';
			char *pos_sdx_name = strstr(buff, tmp_sdx_name);
			char *pos = strstr(buff, dev_name);
			if (pos && buff[pos-buff+strlen(dev_name)] == ' ')
			{
				first_space = strchr(buff, ' ');
				second_space = strchr(&buff[first_space-buff+1], ' ');
				memcpy(mounted_dir, &buff[first_space-buff+1], second_space-first_space-1);
				first_space = strchr(&buff[second_space-buff+1], ' ');
				memcpy(fs_type, &buff[second_space-buff+1], first_space-second_space-1);
				if (strcmp(fs_type, fs_ext_str) == 0)
					ext4_flag = true;
				else
					ext4_flag = false;
				second_space = strchr(&buff[first_space-buff+1], ',');
				memcpy(rw_mode, &buff[first_space-buff+1], second_space-first_space-1);
				if (strcmp(rw_mode, "ro") == 0)
				{
					if (ext4_flag)
						ext4_flag = false;
				}
				dev_or_sdx_flag = true;
				ALOGD("=========================> (dev_name) mounted_dir [%s]", mounted_dir);
				ALOGD("=========================> (dev_name) fs_type [%s]", fs_type);
				ALOGD("=========================> (dev_name) ext4_flag [%s]", ext4_flag ? "true" : "false");
			}
			else if (pos_sdx_name && buff[pos_sdx_name-buff+strlen(tmp_sdx_name)] == ' ')
			{
				first_space = strchr(buff, ' ');
				second_space = strchr(&buff[first_space-buff+1], ' ');
				memcpy(mounted_dir, &buff[first_space-buff+1], second_space-first_space-1);
				first_space = strchr(&buff[second_space-buff+1], ' ');
				memcpy(fs_type, &buff[second_space-buff+1], first_space-second_space-1);
				if (strcmp(fs_type, fs_ext_str) == 0)
					ext4_flag = true;
				else
					ext4_flag = false;
				second_space = strchr(&buff[first_space-buff+1], ',');
				memcpy(rw_mode, &buff[first_space-buff+1], second_space-first_space-1);
				if (strcmp(rw_mode, "ro") == 0)
				{
					if (ext4_flag)
						ext4_flag = false;
				}
				dev_or_sdx_flag = false;
				ALOGD("=========================> (sdx_name) mounted_dir [%s]", mounted_dir);
				ALOGD("=========================> (sdx_name) fs_type [%s]", fs_type);
				ALOGD("=========================> (sdx_name) ext4_flag [%s]", ext4_flag ? "true" : "false");
			}
			else
			{
				Vector<PartitionBlock>::iterator iter;
				for (iter = vPartitionBlock.begin(); iter != vPartitionBlock.end(); iter++)
				{
					char *pos = strstr(buff, iter->dev_name);
					if (pos && buff[pos-buff+strlen(iter->dev_name)] == ' ')
					{
						first_space = strchr(buff, ' ');
						second_space = strchr(&buff[first_space-buff+1], ' ');
						memcpy(iter->mounted_dir, &buff[first_space-buff+1], second_space-first_space-1);
						first_space = strchr(&buff[second_space-buff+1], ' ');
						ALOGD("===============> iter->partition_no [%d], iter->mounted_dir [%s]", iter->partition_no, iter->mounted_dir);
					}
				}
			}
		}
		fclose(fp);
		waiting_cnt++;

		if ((pDetectHDD->mount_booting_flag && waiting_cnt >= 5 && hdd_index == 1 &&
			 pDetectHDD->hddInfoStore[0].HDDInfo.ext4_flag == 1 &&
			 pDetectHDD->hddInfoStore[0].SlotConnected && pDetectHDD->hddInfoStore[1].SlotConnected) ||
			(pDetectHDD->mount_booting_flag && waiting_cnt >= 5 && hdd_index == 0 &&
			 pDetectHDD->hddInfoStore[1].HDDInfo.ext4_flag == 1 &&
			 pDetectHDD->hddInfoStore[0].SlotConnected && pDetectHDD->hddInfoStore[1].SlotConnected))
		{
			pDetectHDD->mount_booting_flag = false;
			memset(HDDInfo.serial, 0, 30);
			memcpy(HDDInfo.serial, serial, 30);
			memset(HDDInfo.sdx_name, 0, 20);
			memcpy(HDDInfo.sdx_name, sdx_name, 20);
			strcpy(HDDInfo.fs_type, fs_ext_str);
			ext4_flag = true;
			memset(mounted_dir, 0, 80);
			sprintf(mounted_dir, "%susb.%s", MNT_MEDIA_DIR, HDDInfo.serial);
			MountHDD(false);
			break;
		}

		if (waiting_cnt >= 300 || mounted_dir[0] != '\0') break;
		usleep(500000); // 500ms
	}
}

void HDDInfoStore::convertSerialNumberToASCII(char* serialNo)
{
	for (uint8_t i=0; i < strlen(serialNo); i++)
	{
		if ((serialNo[i] < 48					   ) ||
			(serialNo[i] > 57 && serialNo[i] < 65) ||
			(serialNo[i] > 90 && serialNo[i] < 97) ||
			(serialNo[i] > 122					   ))
		{
			uint8_t mod_val = serialNo[i] % 62;
			if (mod_val < 10)
				serialNo[i] = 48 + mod_val;
			else if (mod_val >= 10 && mod_val < 36)
				serialNo[i] = 65 + mod_val - 10;
			else if (mod_val >= 36 && mod_val < 62)
				serialNo[i] = 97 + mod_val - 36;
		}
	}
}

void HDDInfoStore::GetHDDInfo(uint8_t hdd_index)
{
	int		fd;
	ssize_t	rd_size;
	char	filename[80];

	char	idProduct[10];
	char	idVendor[10];
	char	manufacturer[20];
	char	serial[30];
	char	product[20];

	long long	hdd_size = 0L;

	char	dev_name[10];
	char	mounted_dir[80];
	char	fs_type[10];
	char	rw_mode[5];
	bool	ext4_flag = false;

	memset(idProduct, 0, 10);
	memset(filename, 0, 80);
	sprintf(filename, "%sidProduct", buf_usb_device);
	if ((fd = open(filename, O_RDONLY)) > 0)
	{
		if ((rd_size = read(fd, idProduct, 9)) > 0)
			idProduct[rd_size-1] = '\0';
		ALOGD("=========================> idProduct [%s]", idProduct);
		close(fd);
	}

	memset(idVendor, 0, 10);
	memset(filename, 0, 80);
	sprintf(filename, "%sidVendor", buf_usb_device);
	if ((fd = open(filename, O_RDONLY)) > 0)
	{
		if ((rd_size = read(fd, idVendor, 9)) > 0)
			idVendor[rd_size-1] = '\0';
		ALOGD("=========================> idVendor [%s]", idVendor);
		close(fd);
	}

	memset(manufacturer, 0, 20);
	memset(filename, 0, 80);
	sprintf(filename, "%smanufacturer", buf_usb_device);
	if ((fd = open(filename, O_RDONLY)) > 0)
	{
		if ((rd_size = read(fd, manufacturer, 19)) > 0)
			manufacturer[rd_size-1] = '\0';
		ALOGD("=========================> manufacturer [%s]", manufacturer);
		close(fd);
	}

	memset(serial, 0, 30);
	memset(filename, 0, 80);
	sprintf(filename, "%sserial", buf_usb_device);
	if ((fd = open(filename, O_RDONLY)) > 0)
	{
		if ((rd_size = read(fd, serial, 29)) > 0)
			serial[rd_size-1] = '\0';
		convertSerialNumberToASCII(serial);
		ALOGD("=========================> serial [%s]", serial);
		close(fd);
	}

	memset(product, 0, 20);
	memset(filename, 0, 80);
	sprintf(filename, "%sproduct", buf_usb_device);
	if ((fd = open(filename, O_RDONLY)) > 0)
	{
		if ((rd_size = read(fd, product, 19)) > 0)
			product[rd_size-1] = '\0';
		ALOGD("=========================> product [%s]", product);
		close(fd);
	}

	hdd_size = GetHDDSize(buf_block_sdx);

	ALOGD("=========================> hdd_size (%lld) Bytes", hdd_size);

	memset(dev_name, 0, 10);
	memset(filename, 0, 80);
	ALOGD("=========================> buf_block_sdx [%s]", buf_block_sdx);
	ALOGD("=========================> buf_block_sdx_sdx1 [%s]", buf_block_sdx_sdx1);
	if (!strcmp(buf_block_sdx, buf_block_sdx_sdx1))
	{
		uint8_t block_sdx_len = strlen(buf_block_sdx);
		if (block_sdx_len >= 2)
		{
			char block_sdx_ch = buf_block_sdx[block_sdx_len-2];
			sprintf(filename, "%ssd%c1/dev", buf_block_sdx_sdx1, block_sdx_ch);
			if ((fd = open(filename, O_RDONLY)) > 0)
			{
				if ((rd_size = read(fd, dev_name, 9)) > 0)
					dev_name[rd_size-1] = '\0';
				ALOGD("=========================> dev_name [%s]", dev_name);
				close(fd);
			}
			else
			{
				sprintf(filename, "%sdev", buf_block_sdx_sdx1);
				if ((fd = open(filename, O_RDONLY)) > 0)
				{
					if ((rd_size = read(fd, dev_name, 9)) > 0)
						dev_name[rd_size-1] = '\0';
					ALOGD("=========================> dev_name [%s]", dev_name);
					close(fd);
				}
			}
		}
	}
	else
	{
		sprintf(filename, "%sdev", buf_block_sdx_sdx1);
		if ((fd = open(filename, O_RDONLY)) > 0)
		{
			if ((rd_size = read(fd, dev_name, 9)) > 0)
				dev_name[rd_size-1] = '\0';
			ALOGD("=========================> dev_name [%s]", dev_name);
			close(fd);
		}
	}

	char sdx_name[20];
	memset(sdx_name, 0, 20);
	char* sdx_pos = strstr(buf_block_sdx, "block/sd");
	if (sdx_pos)
	{
		strcpy(sdx_name, sdx_pos);
		ALOGD("=========================> sdx_name [%s]", sdx_name);
	}

	Vector<PartitionBlock>::iterator iter;
	for (iter = vPartitionBlock.begin(); iter != vPartitionBlock.end(); iter++)
	{
		memset(filename, 0, 80);
		sprintf(filename, "%sdev", iter->buf_block_sdx_sdxx);
		ALOGD("===============> partition part: filename [%s]", filename);
		if ((fd = open(filename, O_RDONLY)) > 0)
		{
			if ((rd_size = read(fd, iter->dev_name, 9)) > 0)
				iter->dev_name[rd_size-1] = '\0';
			ALOGD("===============> iter->partition_no [%d], iter->dev_name [%s]", iter->partition_no, iter->dev_name);
			close(fd);
		}
	}

	ALOGD("=========================> partition_cnt [%d]", HDDInfo.partition_cnt);

	bool on_booting = false;
	char dev_bootcomplete_prop[PROPERTY_VALUE_MAX] = {'\0'};
	if (!property_get("dev.bootcomplete", dev_bootcomplete_prop, NULL) ||
		strcmp(dev_bootcomplete_prop, "1"))
	{
		on_booting = true;
	}
	while (1)
	{
		char dev_bootcomplete_prop[PROPERTY_VALUE_MAX] = {'\0'};
		if (property_get("dev.bootcomplete", dev_bootcomplete_prop, NULL) &&
			!strcmp(dev_bootcomplete_prop, "1"))
		{
			ALOGD("========================================");
			ALOGD("%s HDD_CTRL Version (%s)", HDD_CTRL_TITLE, HDD_CTRL_VERSION);
			ALOGD("========================================");

			break;
		}
		usleep(1000000);	//1000ms
	}

	if (on_booting)
	{
		sleep(ON_BOOTING_WAIT_TIME);
	}

	//searchMountInfo(hdd_index, mounted_dir, fs_type, rw_mode, serial, sdx_name, dev_name, ext4_flag);

#if MAKE_DEV_BLOCK_VOLD_NODE == 1
	MakeNodeToVoldDir(dev_name, 0);
#endif

	bool fstype_gpt_flag = false;

#if 0
	char *fstype = NULL;
	char devpath[256];
	memset(devpath, 0, 256);
	sprintf(devpath, "/dev/block/vold/%s", dev_name);
	//blkid_cache cache = NULL;
	//blkid_get_cache(&cache, NULL);
	//char *fstype = blkid_get_tag_value(cache, "TYPE", devpath);
	fstype = blkid_get_tag_value(NULL, "TYPE", devpath);
	//blkid_put_cache(cache);
	ALOGD("=========(dev_name)===========> fstype[%s]", fstype);
	if (fstype == NULL)
	{
		memset(devpath, 0, 256);
		sprintf(devpath, "/dev/%s", sdx_name);
		devpath[strlen(devpath)-1] = '\0';
		//blkid_cache cache = NULL;
		//blkid_get_cache(&cache, NULL);
		//fstype = blkid_get_tag_value(cache, "TYPE", devpath);
		fstype = blkid_get_tag_value(NULL, "TYPE", devpath);
		//blkid_put_cache(cache);
		ALOGD("=========(sdx_name)===========> fstype[%s]", fstype);
	}
	if (fstype == NULL)
	{
		fstype_gpt_flag = true;
		GPTFlag = true;

		char tmp_dev_name[10];
		memset(tmp_dev_name, 0, 10);
		strcpy(tmp_dev_name, dev_name);
		int dev_name_len = strlen(tmp_dev_name);
		tmp_dev_name[dev_name_len-1] = dev_name[dev_name_len-1]+1;

		memset(devpath, 0, 256);
		sprintf(devpath, "/dev/block/vold/%s", tmp_dev_name);
		//blkid_cache cache = NULL;
		//blkid_get_cache(&cache, NULL);
		//fstype = blkid_get_tag_value(cache, "TYPE", devpath);
		fstype = blkid_get_tag_value(NULL, "TYPE", devpath);
		//blkid_put_cache(cache);
		ALOGD("=========(dev_name+1)===========> fstype[%s]", fstype);
	}
#else
	sleep(1);

	char fstype[10];
	memset(fstype, 0, 10);

	char sdx_str[30];
	char tmp_buf_prop[PROPERTY_VALUE_MAX] = {'\0'};
	for (int i='a'; i <= 'z'; i++)
	{
		memset(sdx_str, 0, 30);
		sprintf(sdx_str, "block/sd%c/", i);
		if (!strcmp(sdx_name, sdx_str))
		{
			memset(sdx_str, 0, 30);
			sprintf(sdx_str, "tvs.hdd_ctrl.sd%c", i);
			property_get(sdx_str, tmp_buf_prop, NULL);
			ALOGD("=============> tmp_buf_prop (block/sd%c/) [%s]", i, tmp_buf_prop);
			break;
		}
	}
	memcpy(fstype, tmp_buf_prop, 9);
	ALOGD("=============> fstype [%s]", fstype);
	if (strlen(fstype) == 0)
	{
		fstype_gpt_flag = true;
		GPTFlag = true;
	}

	bool SlotHDDIndexQualified_tmp = pDetectHDD->IsHDDQualified(hdd_index, idVendor, idProduct);
	if (SlotHDDIndexQualified_tmp)
	{
		if (HDDInfo.partition_cnt == 0)
		{
			ALOGD("============> dev_name[%s], HDDInfo.partition_cnt[%d]", dev_name, HDDInfo.partition_cnt);
			char fsPath_tmp[30];
			memset(fsPath_tmp, 0, 30);
			sprintf(fsPath_tmp, "/dev/block/vold/%s", dev_name);

			MakeFsByFdisk(fsPath_tmp, HDDInfo.partition_cnt);
			memset(fstype, 0, 10);
			strcpy(fstype, "ntfs");
		}
		/*
		else if (HDDInfo.partition_cnt > 1)
		{
			ALOGD("============> dev_name[%s], HDDInfo.partition_cnt[%d]", dev_name, HDDInfo.partition_cnt);
			//char tmp_dev_name[10];
			//memset(tmp_dev_name, 0, 10);
			//strcpy(tmp_dev_name, dev_name);
			//int dev_name_len = strlen(tmp_dev_name);
			//tmp_dev_name[dev_name_len-1] = dev_name[dev_name_len-1]-1;

			//char fsPath_tmp[30];
			//memset(fsPath_tmp, 0, 30);
			//sprintf(fsPath_tmp, "/dev/block/vold/%s", tmp_dev_name);

			//MakeFsByFdisk(fsPath_tmp, HDDInfo.partition_cnt);
		}
		*/
		else if (HDDInfo.partition_cnt >= 1)
		//else
		{
			if ((HDDInfo.partition_cnt == 1 && fstype_gpt_flag && GPTFlag) || HDDInfo.partition_cnt > 1)
			//if (fstype_gpt_flag && GPTFlag)
			{
				fstype_gpt_flag = false;
				GPTFlag = false;

				char *fstype_tmp = NULL;
				char devpath[256];
				memset(devpath, 0, 256);
				sprintf(devpath, "/dev/block/vold/%s", dev_name);
				//blkid_cache cache = NULL;
				//blkid_get_cache(&cache, NULL);
				//char *fstype_tmp = blkid_get_tag_value(cache, "TYPE", devpath);
				fstype_tmp = blkid_get_tag_value(NULL, "TYPE", devpath);
				//blkid_put_cache(cache);
				ALOGD("=========(dev_name)===========> fstype_tmp[%s]", fstype_tmp);
				if (fstype_tmp == NULL)
				{
					memset(devpath, 0, 256);
					sprintf(devpath, "/dev/%s", sdx_name);
					devpath[strlen(devpath)-1] = '\0';
					//blkid_cache cache = NULL;
					//blkid_get_cache(&cache, NULL);
					//fstype_tmp = blkid_get_tag_value(cache, "TYPE", devpath);
					fstype_tmp = blkid_get_tag_value(NULL, "TYPE", devpath);
					//blkid_put_cache(cache);
					ALOGD("=========(sdx_name)===========> fstype_tmp[%s]", fstype_tmp);
				}

				if (fstype_tmp == NULL)
				{
					fstype_gpt_flag = true;
					GPTFlag = true;

					char tmp_dev_name[10];
					memset(tmp_dev_name, 0, 10);
					strcpy(tmp_dev_name, dev_name);
					int dev_name_len = strlen(tmp_dev_name);
					tmp_dev_name[dev_name_len-1] = dev_name[dev_name_len-1]+1;

					memset(devpath, 0, 256);
					sprintf(devpath, "/dev/block/vold/%s", tmp_dev_name);
					//blkid_cache cache = NULL;
					//blkid_get_cache(&cache, NULL);
					//fstype_tmp = blkid_get_tag_value(cache, "TYPE", devpath);
					fstype_tmp = blkid_get_tag_value(NULL, "TYPE", devpath);
					//blkid_put_cache(cache);
					ALOGD("=========(dev_name+1)===========> fstype_tmp[%s]", fstype_tmp);

					memset(fstype, 0, 10);
				}
				else
				{
					memset(fstype, 0, 10);
					strcpy(fstype, fstype_tmp);
				}
			}
		}
	}
#endif

#if 0
	if (fstype != NULL && !strcmp(fstype, fs_ext_str))
#else
	if (strlen(fstype) > 0 && !strcmp(fstype, fs_ext_str))
#endif
	{
		bool HDDSerialFlag = false;
		bool SlotHDDIndexQualified = false;
		bool SlotOtherQualified = false;
		if (pDetectHDD->hddInfoStore[0].SlotConnected &&
			pDetectHDD->hddInfoStore[1].SlotConnected)
		{
			SlotHDDIndexQualified = pDetectHDD->IsHDDQualified(hdd_index, idVendor, idProduct);
			if (hdd_index == 0)
				SlotOtherQualified = pDetectHDD->IsHDDQualified(1, idVendor, idProduct);
			else if (hdd_index == 1)
				SlotOtherQualified = pDetectHDD->IsHDDQualified(0, idVendor, idProduct);
			if (SlotHDDIndexQualified && SlotOtherQualified)
			{
#if DEV_FTS_USE == 1
				char* key_val = pDetectHDD->getKeyValue("hdd.serial.number");
#else
				char* key_val = pDetectHDD->getSerialNumber(); //"hdd.serial.number"
#endif
				ALOGD("====================================> key_val [%s]", key_val);
				if (key_val != NULL && strcmp(HDDInfo.serial, key_val) == 0)
				{
					HDDSerialFlag = true;
				}
			}

#if MAKE_DEV_BLOCK_VOLD_NODE == 1
			MakeNodeToVoldDir(dev_name, 1);
#endif

#if MOUNT_HERE_AGAINST_VOLD == 1
			Ext4Mount(serial, fstype_gpt_flag, dev_name, sdx_name, /*on_booting,*/ mounted_dir, fs_type, fstype, ext4_flag);
			char timeshift_buf[256];
			memset(timeshift_buf, 0, 256);
			sprintf(timeshift_buf, "rm -f %s/%s*", mounted_dir, TIMESHIFT_FILE_PREFIX);
			system(timeshift_buf);
#else
			if (HDDSerialFlag || (SlotHDDIndexQualified && pDetectHDD->selectedHDDIndex < 0))
			{
				Ext4Mount(serial, fstype_gpt_flag, dev_name, sdx_name, /*on_booting,*/ mounted_dir, fs_type, fstype, ext4_flag);
				char timeshift_buf[256];
				memset(timeshift_buf, 0, 256);
				sprintf(timeshift_buf, "rm -f %s/%s*", mounted_dir, TIMESHIFT_FILE_PREFIX);
				system(timeshift_buf);
			}
			else
			{
				memset(mounted_dir, 0, 80);
				memset(fs_type, 0, 10);
				strcpy(fs_type, fstype);
				ext4_flag = true;

				//yinohyinoh
				//if (!strcmp(HDDInfo.sdx_name, "block/sda/"))
				//	property_set("ctl.start", "fuse_sdcard1");
				//else if (!strcmp(HDDInfo.sdx_name, "block/sdb/"))
				//	property_set("ctl.start", "fuse_usbdisk0");
			}
#endif
		}
		else
		{
#if MAKE_DEV_BLOCK_VOLD_NODE == 1
			MakeNodeToVoldDir(dev_name, 1);
#endif

			//ALOGD("=========================> pDetectHDD->IsHDDQualified(%d, %s, %s) [%d]", hdd_index, idVendor, idProduct, pDetectHDD->IsHDDQualified(hdd_index, idVendor, idProduct));
			//ALOGD("=========================> pDetectHDD->selectedHDDIndex [%d]", pDetectHDD->selectedHDDIndex);
#if MOUNT_HERE_AGAINST_VOLD == 1
			Ext4Mount(serial, fstype_gpt_flag, dev_name, sdx_name, /*on_booting,*/ mounted_dir, fs_type, fstype, ext4_flag);
			char timeshift_buf[256];
			memset(timeshift_buf, 0, 256);
			sprintf(timeshift_buf, "rm -f %s/%s*", mounted_dir, TIMESHIFT_FILE_PREFIX);
			system(timeshift_buf);
#else
			if (pDetectHDD->IsHDDQualified(hdd_index, idVendor, idProduct) && pDetectHDD->selectedHDDIndex < 0)
			{
				Ext4Mount(serial, fstype_gpt_flag, dev_name, sdx_name, /*on_booting,*/ mounted_dir, fs_type, fstype, ext4_flag);
				char timeshift_buf[256];
				memset(timeshift_buf, 0, 256);
				sprintf(timeshift_buf, "rm -f %s/%s*", mounted_dir, TIMESHIFT_FILE_PREFIX);
				system(timeshift_buf);
			}
			else
			{
				memset(mounted_dir, 0, 80);
				memset(fs_type, 0, 10);
				strcpy(fs_type, fstype);
				ext4_flag = true;

				//yinohyinoh
				//if (!strcmp(HDDInfo.sdx_name, "block/sda/"))
				//	property_set("ctl.start", "fuse_sdcard1");
				//else if (!strcmp(HDDInfo.sdx_name, "block/sdb/"))
				//	property_set("ctl.start", "fuse_usbdisk0");
			}
#endif
		}
	}
	else
	{
#if MAKE_DEV_BLOCK_VOLD_NODE == 1
		MakeNodeToVoldDir(dev_name, 2);
#endif
#if MOUNT_HERE_AGAINST_VOLD == 1
		VfatMount(serial, fstype_gpt_flag, dev_name, sdx_name, /*on_booting,*/ mounted_dir, fs_type, fstype, ext4_flag);
#else
		memset(mounted_dir, 0, 80);
		memset(fs_type, 0, 10);
#if 0
		if (fstype == NULL || fstype_gpt_flag)
#else
		if (strlen(fstype) == 0 || fstype_gpt_flag)
#endif
			strcpy(fs_type, "GPT");
		else
			strcpy(fs_type, fstype);
		ext4_flag = false;

		//yinohyinoh
		//if (!strcmp(HDDInfo.sdx_name, "block/sda/"))
		//	property_set("ctl.start", "fuse_sdcard1");
		//else if (!strcmp(HDDInfo.sdx_name, "block/sdb/"))
		//	property_set("ctl.start", "fuse_usbdisk0");
#endif
	}

#if 0
	//if (HDDInfo.partition_cnt > 1 || fstype == NULL)
#else
	//if (HDDInfo.partition_cnt > 1 || strlen(fstype) == 0)
#endif
	//{
	//	ALOGD("============> sdx_name[%s], HDDInfo.partition_cnt[%d]", sdx_name, HDDInfo.partition_cnt);
	//	char fsPath_tmp[30];
	//	memset(fsPath_tmp, 0, 30);
	//	sprintf(fsPath_tmp, "/dev/%s", sdx_name);
	//	fsPath_tmp[strlen(fsPath_tmp)-1] = '\0';

	//	MakeFsByFdisk(fsPath_tmp, HDDInfo.partition_cnt);
	//}

#if 0
	if (fstype != NULL)
		free(fstype);
#endif

	ALOGD("=========================> mounted_dir [%s]", mounted_dir);
	ALOGD("=========================> fs_type [%s]", fs_type);
	ALOGD("=========================> ext4_flag [%s]", ext4_flag ? "true" : "false");

	uint8_t partition_cnt = HDDInfo.partition_cnt;
	memset(&HDDInfo, 0, sizeof(HDDInfo));
	HDDInfo.partition_cnt = partition_cnt;

	memcpy(HDDInfo.idProduct, idProduct, 10);
	memcpy(HDDInfo.idVendor, idVendor, 10);
	memcpy(HDDInfo.manufacturer, manufacturer, 20);
	memcpy(HDDInfo.serial, serial, 30);
	memcpy(HDDInfo.product, product, 20);
	HDDInfo.hdd_total_B_size = hdd_size;

	memcpy(HDDInfo.sdx_name, sdx_name, 20);
	memcpy(this->dev_name, dev_name, 10);
	memcpy(HDDInfo.mounted_dir, mounted_dir, 80);
	memcpy(HDDInfo.fs_type, fs_type, 10);
	if (ext4_flag)
		HDDInfo.ext4_flag = 1;
	else
		HDDInfo.ext4_flag = 0;

	if (mounted_dir[0] != '\n' && ext4_flag == 1)
		GetHDDActualSize(true, false);
	else
		GetHDDActualSize(false, false);

	//if (!ext4_flag)
	//{
		memset(&pDetectHDD->tempRemoveHDDInfo[hdd_index], 0, sizeof(TempRemoveHDDInfo));
		pDetectHDD->tempRemoveHDDInfo[hdd_index].GPTFlag = GPTFlag;
		memcpy(pDetectHDD->tempRemoveHDDInfo[hdd_index].dev_name, dev_name, 10);
		memcpy(&pDetectHDD->tempRemoveHDDInfo[hdd_index].HDDInfo, &HDDInfo, sizeof(HDDInfoStruct));
		pDetectHDD->tempRemoveHDDInfo[hdd_index].partition_cnt = HDDInfo.partition_cnt;
	//}

#if DEV_FTS_USE == 1
	//char* key_val = pDetectHDD->getKeyValue("hdd.format.error.SN");
#else
	//char* key_val = pDetectHDD->getSerialNumber(); //"hdd.format.error.SN"
#endif
	//ALOGD("====(hdd.format.error.SN)=====> key_val [%s]", key_val);
	//if (strcmp(HDDInfo.fs_type, fs_ext_str) == 0 && key_val != NULL && strcmp(key_val, HDDInfo.serial) == 0)
	//{
	//	//char temp_buf[200];
	//	//memset(temp_buf, 0, 200);
	//	//sprintf(temp_buf, "busybox umount -l /dev/block/vold/%s", this->dev_name);
	//	//system(temp_buf);
	//	//memset(temp_buf, 0, 200);
	//	//sprintf(temp_buf, "busybox mkfs.vfat /dev/block/vold/%s", this->dev_name);
	//	//system(temp_buf);
	//	////memset(HDDInfo.mounted_dir, 0, 80);
	//	//strcpy(HDDInfo.fs_type, "vfat");
	//	//HDDInfo.ext4_flag = 0;

	//	FormatHDDToExt4();
	//	strcpy(HDDInfo.fs_type, fs_ext_str);
	//	HDDInfo.ext4_flag = 1;
	//	HDDInfo.partition_cnt = 0;
	//}
}

void HDDInfoStore::MakeNodeToVoldDir(char *dev_name, char mount_case)
{
	char make_node_buf[80];
	char dev_major_number[3];
	char dev_minor_number[3];

	memset(make_node_buf, 0, 80);
	memset(dev_major_number, 0, 3);
	memset(dev_minor_number, 0, 3);

	//sprintf(make_node_buf, "/dev/block/vold/%s", dev_name);
	//if (access(make_node_buf, F_OK) >= 0)
	//	return;

	bool colon_flag = false;
	for (uint8_t i=0, j=0; i < strlen(dev_name); i++)
	{
		if (dev_name[i] == ':')
		{
			colon_flag = true;
			j = 0;
		}
		else
		{
			if (j < 2)
			{
				if (!colon_flag)
					dev_major_number[j] = dev_name[i];
				else
					dev_minor_number[j] = dev_name[i];
				j++;
			}
		}
	}
	sprintf(make_node_buf, "mknod /dev/block/vold/%s b %s %s", dev_name, dev_major_number, dev_minor_number);
	if (mount_case == 0)
		ALOGD("==============> GetHDDInfo: make node [%s]", make_node_buf);
	else if (mount_case == 1)
		ALOGD("==============> Ext4Mount: make node [%s]", make_node_buf);
	else if (mount_case == 2)
		ALOGD("==============> VfatMount: make node [%s]", make_node_buf);
	system(make_node_buf);

	Vector<PartitionBlock>::iterator iter;
	for (iter = vPartitionBlock.begin(); iter != vPartitionBlock.end(); iter++)
	{
		memset(make_node_buf, 0, 80);
		memset(dev_major_number, 0, 3);
		memset(dev_minor_number, 0, 3);

		bool colon_flag = false;
		for (uint8_t i=0, j=0; i < strlen(iter->dev_name); i++)
		{
			if (iter->dev_name[i] == ':')
			{
				colon_flag = true;
				j = 0;
			}
			else
			{
				if (j < 2)
				{
					if (!colon_flag)
						dev_major_number[j] = iter->dev_name[i];
					else
						dev_minor_number[j] = iter->dev_name[i];
					j++;
				}
			}
		}
		sprintf(make_node_buf, "mknod /dev/block/vold/%s b %s %s", iter->dev_name, dev_major_number, dev_minor_number);
		if (mount_case == 0)
			ALOGD("==============> GetHDDInfo(PartitionBlock): make node [%s]", make_node_buf);
		else if (mount_case == 1)
			ALOGD("==============> Ext4Mount(PartitionBlock): make node [%s]", make_node_buf);
		else if (mount_case == 2)
			ALOGD("==============> VfatMount(PartitionBlock): make node [%s]", make_node_buf);
		system(make_node_buf);
	}
}

void HDDInfoStore::Ext4Mount(char *serial, bool fstype_gpt_flag, char *dev_name, char *sdx_name, /*bool on_booting,*/ char *mounted_dir, char *fs_type, char *fstype, bool &ext4_flag)
{
	bool media_usb_flag = false;
	uint8_t tmp_media_usb_num = 0;
	char mounted_dir_path[80];
	memset(mounted_dir_path, 0, 80);
	sprintf(mounted_dir_path, "%susb.HDD", MNT_MEDIA_DIR);
	ALOGD("==============> mounted_dir_path[%s]", mounted_dir_path);

	/*
	do
	{
		char media_usb_buf[80];
		if (tmp_media_usb_num == 0)
		{
			memset(mounted_dir_path, 0, 80);
			sprintf(mounted_dir_path, "%susb.%s", MNT_MEDIA_DIR, serial);
			ALOGD("==============> mounted_dir_path[%s]", mounted_dir_path);
			memset(media_usb_buf, 0, 80);
			sprintf(media_usb_buf, "usb.%s", serial);
		}
		else
		{
			memset(mounted_dir_path, 0, 80);
			sprintf(mounted_dir_path, "%susb.%s%d", MNT_MEDIA_DIR, serial, tmp_media_usb_num);
			ALOGD("==============> mounted_dir_path[%s]", mounted_dir_path);
			memset(media_usb_buf, 0, 80);
			sprintf(media_usb_buf, "usb.%s%d", serial, tmp_media_usb_num);
		}
		tmp_media_usb_num++;

		DIR             *dp;
		struct dirent   *dirp;
		media_usb_flag = false;
		if ((dp = opendir(MNT_MEDIA_DIR)) != NULL)
		{
			while ((dirp = readdir(dp)) != NULL)
			{
				if (strcmp(dirp->d_name, ".") == 0 ||
					strcmp(dirp->d_name, "..") == 0)
					continue;

				if (dirp->d_type == DT_DIR)
				{
					char temp_path_d_name_buf[100];
					if (!strcmp(media_usb_buf, dirp->d_name))
					{
						media_usb_flag = true;

						memset(temp_path_d_name_buf, 0, 100);
						sprintf(temp_path_d_name_buf, "rmdir %s", mounted_dir_path);
						ALOGD("==============> temp_path_d_name_buf[%s]", temp_path_d_name_buf);
						system(temp_path_d_name_buf);
					}
					if (!strncmp(dirp->d_name, "usb.", 4))
					{
						memset(temp_path_d_name_buf, 0, 100);
						sprintf(temp_path_d_name_buf, "rmdir %s%s", MNT_MEDIA_DIR, dirp->d_name);
						ALOGD("==============> temp_path_d_name_buf[%s]", temp_path_d_name_buf);
						system(temp_path_d_name_buf);
					}
				}
			}

			if (closedir(dp) < 0)
				ALOGE("can't close directory %s\n", MNT_MEDIA_DIR);
		}
	}
	while (media_usb_flag);
	*/

	char tmp_buf[256];
	/*
	memset(tmp_buf, 0, 256);
	sprintf(tmp_buf, "mkdir %s", mounted_dir_path);
	system(tmp_buf);
	ALOGD("==============> tmp_buf[%s]", tmp_buf);
	*/

	char tmp_dev_name[10];
	memset(tmp_dev_name, 0, 10);
	strcpy(tmp_dev_name, dev_name);
	if (fstype_gpt_flag)
	{
		int dev_name_len = strlen(tmp_dev_name);
		tmp_dev_name[dev_name_len-1] = dev_name[dev_name_len-1]+1;
	}

	memset(tmp_buf, 0, 256);
	sprintf(tmp_buf, "/dev/block/vold/%s", tmp_dev_name);

#if VOLUME_DEV_INFO_TO_VOLD_BY_FILE == 1
	char volume_dev_buf[100];
	memset(volume_dev_buf, 0, 100);
	sprintf(volume_dev_buf, "%sext4_volume_dev.txt", HDD_HOME);
	FILE* fp_vol_dev = fopen(volume_dev_buf, "w");
	if (fp_vol_dev)
	{
		fputs(tmp_buf, fp_vol_dev);
		fclose(fp_vol_dev);
	}
#endif

	if (fs_ext2_supported)
	{
		memset(tmp_buf, 0, 256);
		sprintf(tmp_buf, "/dev/%s", sdx_name);
		tmp_buf[strlen(tmp_buf)-1] = '\0';
	}

	ALOGD("==============> tmp_buf[%s], mounted_dir_path[%s], fs_ext_str[%s]", tmp_buf, mounted_dir_path, fs_ext_str);

	//yinoh(2015.02.03:Issue => Mounting HDD doesn't work on STB booting)
	//if (on_booting)
	//{
	//	unMountpoint(tmp_buf, false);
	//}

	char tmp_buf_sdx_name[40];
	memset(tmp_buf_sdx_name, 0, 40);
	sprintf(tmp_buf_sdx_name, "/dev/%s", sdx_name);
	tmp_buf_sdx_name[strlen(tmp_buf_sdx_name)-1] = '\0';
	ALOGD("==============>  tmp_buf_sdx_name [%s]", tmp_buf_sdx_name);

	int rc_waiting_cnt = 0;
	int rc = -1;
	unsigned long flags;
    const char *mnt_opts = MOUNT_OPTS;

	flags = MS_NODEV | MS_NOSUID | MS_DIRSYNC | MS_NOATIME | MS_NOEXEC;
	while (rc_waiting_cnt < 20 && rc)
	{
		rc = mount(tmp_buf, mounted_dir_path, fs_ext_str, flags, mnt_opts);
		if (!rc) break;
        ALOGE("mount failed(%s)", strerror(errno));
		if (access(tmp_buf_sdx_name, F_OK))
			break;
		usleep(200000);	// 200ms
		rc_waiting_cnt++;
	}
	ALOGD("==============> rc_waiting_cnt[%d], rc[%d]", rc_waiting_cnt, rc);
	if (rc)
	{
		rc_waiting_cnt = 0;
		while (rc_waiting_cnt < 30 && rc)
		{
			rc = mount(tmp_buf, mounted_dir_path, fs_ext_str, flags, mnt_opts);
			if (!rc) break;
			if (access(tmp_buf_sdx_name, F_OK))
				break;
			usleep(200000);	// 200ms
			rc_waiting_cnt++;
		}
		ALOGD("======(second)========> rc_waiting_cnt[%d], rc[%d]", rc_waiting_cnt, rc);

		if (rc)
		{
			// HDD is busy
			sleep(2);

			char tmp_buf_dev_name[50];
			memset(tmp_buf_dev_name, 0, 50);
			sprintf(tmp_buf_dev_name, "/dev/block/vold/%s", dev_name);
			memset(tmp_buf, 0, 256);
			sprintf(tmp_buf, "busybox mount -t ext4 %s %s", tmp_buf_dev_name, mounted_dir_path);
			char* mounted_point = NULL;
			for (int i=0; i < 10; i++)
			{
				if (access(tmp_buf_sdx_name, F_OK))
					break;
				system(tmp_buf);
				ALOGD("======(try[%d])========>  (mount) tmp_buf [%s]", i+1, tmp_buf);
				for (int j=0; j < 2; j++)
				{
					if (access(tmp_buf_sdx_name, F_OK))
						break;
					sleep(1);
				}
				mounted_point = getMountpoint(tmp_buf_dev_name);
				if (access(tmp_buf_sdx_name, F_OK))
					break;
				ALOGD("======(try[%d])========>  mounted_point [%s]", i+1, mounted_point);
				if (mounted_point[0] != '\0')
					break;
			}
			ALOGD("======(third)========>  (mount) tmp_buf [%s]", tmp_buf);
		}
	}

	memset(tmp_buf, 0, 256);
	sprintf(tmp_buf, "chown system %s", mounted_dir_path);
	system(tmp_buf);
	ALOGD("==============> tmp_buf[%s]", tmp_buf);

	memset(mounted_dir, 0, 80);
	strcpy(mounted_dir, mounted_dir_path);
	memset(fs_type, 0, 10);
	strcpy(fs_type, fstype);
	ext4_flag = true;
}

void HDDInfoStore::VfatMount(char *serial, bool fstype_gpt_flag, char *dev_name, char *sdx_name, /*bool on_booting,*/ char *mounted_dir, char *fs_type, char *fstype, bool &ext4_flag)
{
	bool media_usb_flag = false;
	uint8_t tmp_media_usb_num = 0;
	char mounted_dir_path[80];

	do
	{
		char media_usb_buf[80];
		if (tmp_media_usb_num == 0)
		{
			memset(mounted_dir_path, 0, 80);
			sprintf(mounted_dir_path, "%susb.%s", MNT_MEDIA_DIR, serial);
			ALOGD("==============> mounted_dir_path[%s]", mounted_dir_path);
			memset(media_usb_buf, 0, 80);
			sprintf(media_usb_buf, "usb.%s", serial);
		}
		else
		{
			memset(mounted_dir_path, 0, 80);
			sprintf(mounted_dir_path, "%susb.%s%d", MNT_MEDIA_DIR, serial, tmp_media_usb_num);
			ALOGD("==============> mounted_dir_path[%s]", mounted_dir_path);
			memset(media_usb_buf, 0, 80);
			sprintf(media_usb_buf, "usb.%s%d", serial, tmp_media_usb_num);
		}
		tmp_media_usb_num++;

		DIR             *dp;
		struct dirent   *dirp;
		media_usb_flag = false;
		if ((dp = opendir(MNT_MEDIA_DIR)) != NULL)
		{
			while ((dirp = readdir(dp)) != NULL)
			{
				if (strcmp(dirp->d_name, ".") == 0 ||
					strcmp(dirp->d_name, "..") == 0)
					continue;

				if (dirp->d_type == DT_DIR)
				{
					char temp_path_d_name_buf[100];
					if (!strcmp(media_usb_buf, dirp->d_name))
					{
						media_usb_flag = true;

						memset(temp_path_d_name_buf, 0, 100);
						sprintf(temp_path_d_name_buf, "rmdir %s", mounted_dir_path);
						ALOGD("==============> temp_path_d_name_buf[%s]", temp_path_d_name_buf);
						system(temp_path_d_name_buf);
					}
					if (!strncmp(dirp->d_name, "usb.", 4))
					{
						memset(temp_path_d_name_buf, 0, 100);
						sprintf(temp_path_d_name_buf, "rmdir %s%s", MNT_MEDIA_DIR, dirp->d_name);
						ALOGD("==============> temp_path_d_name_buf[%s]", temp_path_d_name_buf);
						system(temp_path_d_name_buf);
					}
				}
			}

			if (closedir(dp) < 0)
				ALOGE("can't close directory %s\n", MNT_MEDIA_DIR);
		}
	}
	while (media_usb_flag);

	char tmp_buf[256];
	memset(tmp_buf, 0, 256);
	sprintf(tmp_buf, "mkdir %s", mounted_dir_path);
	system(tmp_buf);
	ALOGD("==============> tmp_buf[%s]", tmp_buf);

	char tmp_dev_name[10];
	memset(tmp_dev_name, 0, 10);
	strcpy(tmp_dev_name, dev_name);
	if (fstype_gpt_flag)
	{
		int dev_name_len = strlen(tmp_dev_name);
		tmp_dev_name[dev_name_len-1] = dev_name[dev_name_len-1]+1;
	}

	memset(tmp_buf, 0, 256);
	sprintf(tmp_buf, "/dev/block/vold/%s", tmp_dev_name);

#if VOLUME_DEV_INFO_TO_VOLD_BY_FILE == 1
	char volume_dev_buf[100];
	memset(volume_dev_buf, 0, 100);
	sprintf(volume_dev_buf, "%snon_ext4_volume_dev.txt", HDD_HOME);
	FILE* fp_vol_dev = fopen(volume_dev_buf, "w");
	if (fp_vol_dev)
	{
		fputs(tmp_buf, fp_vol_dev);
		fclose(fp_vol_dev);
	}
#endif

	if (fs_ext2_supported)
	{
		memset(tmp_buf, 0, 256);
		sprintf(tmp_buf, "/dev/%s", sdx_name);
		tmp_buf[strlen(tmp_buf)-1] = '\0';
	}

	ALOGD("==============> tmp_buf[%s], mounted_dir_path[%s], fs_ext_str[%s]", tmp_buf, mounted_dir_path, fs_ext_str);

	//yinoh(2015.02.03:Issue => Mounting HDD doesn't work on STB booting)
	//if (on_booting)
	//{
	//	unMountpoint(tmp_buf, false);
	//}

	//int rc_waiting_cnt = 0;
	//int rc = -1;
	//unsigned long flags;

	//flags = MS_NODEV | MS_NOSUID | MS_DIRSYNC | MS_NOATIME | MS_NOEXEC;

	//while (rc_waiting_cnt < 20 && rc)
	//{
	//	rc = mount(tmp_buf, mounted_dir_path, "vfat", flags, NULL);
	//	if (!rc) break;
	//	usleep(200000);	// 200ms
	//	rc_waiting_cnt++;
	//}
	//ALOGD("==============> tmp_buf[%s], rc[%d], mounted_dir_path[%s]", tmp_buf, rc, mounted_dir_path);

	//if (rc)
	//{
	//	rc_waiting_cnt = 0;
	//	while (rc_waiting_cnt < 30 && rc)
	//	{
	//		rc = mount(tmp_buf, mounted_dir_path, fs_ext_str, flags, NULL);
	//		if (rc_waiting_cnt >= 30 || !rc) break;
	//		usleep(200000);	// 200ms
	//		rc_waiting_cnt++;
	//	}
	//	ALOGD("======(second)========> rc_waiting_cnt[%d], rc[%d]", rc_waiting_cnt, rc);
		//sleep(2);
		//char tmp_sdx_name[20];
		//memset(tmp_sdx_name, 0, 20);
		//strcpy(tmp_sdx_name, sdx_name);
		//tmp_sdx_name[strlen(tmp_sdx_name)-1] = '\0';
		//memset(tmp_buf, 0, 256);
		////sprintf(tmp_buf, "busybox mount -t ext4 -w /dev/%s %s", tmp_sdx_name, mounted_dir_path);
		//sprintf(tmp_buf, "busybox mount -t ext4 -w /dev/block/vold/%s %s", dev_name, mounted_dir_path);
		//system(tmp_buf);
		//ALOGD("============> (mount) tmp_buf [%s]", tmp_buf);
	//}

	memset(tmp_buf, 0, 256);
	sprintf(tmp_buf, "busybox umount -l /dev/block/vold/%s", dev_name);
	system(tmp_buf);
	memset(tmp_buf, 0, 256);
	if (!strcmp(fstype, "vfat"))
	{
		sprintf(tmp_buf, "busybox mount -t vfat -o uid=1000,gid=1000,rw,umask=002 /dev/block/vold/%s %s", dev_name, mounted_dir_path);
	}
	else if (!strcmp(fstype, "ntfs"))
	{
		sprintf(tmp_buf, "busybox mount -t ntfs -o uid=1000,gid=1000,rw,umask=002 /dev/block/vold/%s %s", dev_name, mounted_dir_path);
	}
	else if (!strcmp(fstype, "exfat"))
	{
		sprintf(tmp_buf, "busybox mount -t fusectl -o uid=1000,gid=1000,rw,umask=002 /dev/block/vold/%s %s", dev_name, mounted_dir_path);
	}
	else
	{
		sprintf(tmp_buf, "busybox mount -t fuseblk -o uid=1000,gid=1000,rw,umask=002 /dev/block/vold/%s %s", dev_name, mounted_dir_path);
	}
	system(tmp_buf);
	ALOGD("============> (mount) tmp_buf [%s]", tmp_buf);

	memset(mounted_dir, 0, 80);
	strcpy(mounted_dir, mounted_dir_path);
	//memset(fs_type, 0, 10);
	//strcpy(fs_type, fstype);
#if 0
	if (fstype == NULL || fstype_gpt_flag)
#else
	if (strlen(fstype) == 0 || fstype_gpt_flag)
#endif
		strcpy(fs_type, "GPT");
	else
		strcpy(fs_type, fstype);
	ext4_flag = false;
}

void HDDInfoStore::MakeFsByFdisk(const char *fsPath, uint8_t partition_cnt)
{
	ALOGD("MakeFsByFdisk: ---------> fsPath[%s], partition_cnt[%d]", fsPath, partition_cnt);

	char tmp_buf[100];
	FILE *fp;

	memset(tmp_buf, 0, 100);
	sprintf(tmp_buf, "%s %s", FDISK_BIN, fsPath);
	ALOGD("MakeFsByFdisk: tmp_buf[%s]", tmp_buf);

	fp = popen(tmp_buf, "w");
	if (fp == NULL)
	{
		ALOGE("MakeFsByFdisk: popen error");
		return;
	}

	for (int i=partition_cnt; i > 0; i--)
	{
		fputs("d\n", fp);
		if (i > 1)
		{
			char numstr[4];
			memset(numstr, 0, 4);
			sprintf(numstr, "%d\n", i);
			fputs(numstr, fp);
		}
	}
	fputs("n\n", fp);
	fputs("p\n", fp);
	fputs("1\n", fp);
	fputs("\n", fp);
	fputs("\n", fp);
	fputs("t\n", fp);	// Reserved
	fputs("7\n", fp);
	fputs("w\n", fp);

	pclose(fp);

	vPartitionBlock.clear();
	HDDInfo.partition_cnt = 1;
}

long long HDDInfoStore::GetHDDSize(char* buf_block_sdx)
{
	int		fd;
	ssize_t	rd_size;
	char filename[80];

	char  block_count_str[21];
	long  long block_count = 0L;
	char  logical_block_size_str[11];
	short logical_block_size = 512;

	memset(block_count_str, 0, 21);
	memset(filename, 0, 80);
	sprintf(filename, "%ssize", buf_block_sdx);
	if ((fd = open(filename, O_RDONLY)) > 0)
	{
		if ((rd_size = read(fd, block_count_str, 20)) > 0)
		{
			block_count_str[rd_size-1] = '\0';
			block_count = atoll(block_count_str);
		}
		close(fd);
	}

	memset(logical_block_size_str, 0, 11);
	memset(filename, 0, 80);
	sprintf(filename, "%squeue/logical_block_size", buf_block_sdx);
	if ((fd = open(filename, O_RDONLY)) > 0)
	{
		if ((rd_size = read(fd, logical_block_size_str, 10)) > 0)
		{
			logical_block_size_str[rd_size-1] = '\0';
			logical_block_size = atoi(logical_block_size_str);
		}
		close(fd);
	}

	ALOGD("=====> block_count[%lld], block_count_str[%s], logical_block_size[%d], logical_block_size_str[%s]", block_count, block_count_str, logical_block_size, logical_block_size_str);
	return (long long)(block_count * logical_block_size);
}

void HDDInfoStore::GetHDDActualSize(bool subtract_flag, bool from_format_flag)
{
	struct statfs st;

	ALOGD("======================> HDDInfo.mounted_dir [%s]", HDDInfo.mounted_dir);
	if (statfs(HDDInfo.mounted_dir, &st) < 0)
	{
		ALOGE("%s: %s", HDDInfo.mounted_dir, strerror(errno));
		HDDInfo.hdd_actual_B_size = 0;
		HDDInfo.hdd_used_B_size = 0;
		HDDInfo.hdd_free_B_size = 0;
	}
	else
	{
		//ALOGD("-----------> GetHDDActualSize(): st.f_blocks[%lld], st.f_bsize[%d], st.f_blocks * st.f_bsize[%lld]", st.f_blocks, st.f_bsize, st.f_blocks * st.f_bsize);
		//ALOGD("-----------> GetHDDActualSize(): st.f_blocks[%lld], st.f_bsize[%d], st.f_bfree[%lld], (st.f_blocks - st.f_bfree) * st.f_bsize[%lld]", st.f_blocks, st.f_bsize, st.f_bfree, (st.f_blocks - st.f_bfree) * st.f_bsize);

		HDDInfo.hdd_actual_B_size = (long long)st.f_blocks * (long long)st.f_bsize;
		HDDInfo.hdd_used_B_size = ((long long)st.f_blocks - (long long)st.f_bfree) * st.f_bsize;
		HDDInfo.hdd_free_B_size = (long long)st.f_bfree * (long long)st.f_bsize;
		ALOGD("(statfs)======================> HDDInfo.hdd_actual_B_size [%lld]", HDDInfo.hdd_actual_B_size);
		ALOGD("(statfs)======================> HDDInfo.hdd_used_B_size [%lld]", HDDInfo.hdd_used_B_size);
		ALOGD("(statfs)======================> HDDInfo.hdd_free_B_size [%lld]", HDDInfo.hdd_free_B_size);
		//if (subtract_flag)
		//{
		//  HDDInfo.hdd_actual_B_size -= TIMESHIFT_SIZE;   // TimeShirt 12GB or 30GB
		//  //HDDInfo.hdd_free_B_size -= TIMESHIFT_SIZE;     // TimeShirt 12GB or 30GB
		//}
		long long current_timeshift_size = 0L;
		if (!from_format_flag)
			current_timeshift_size = GetSizeOfTimeShift(HDDInfo.mounted_dir);
		ALOGD("===========> current_timeshift_size[%lld]", current_timeshift_size);
		HDDInfo.hdd_used_B_size -= current_timeshift_size;
		HDDInfo.hdd_used_B_size -= SYSTEM_USED_SIZE;
		if (subtract_flag)
		{
			HDDInfo.hdd_actual_B_size -= TIMESHIFT_SIZE;   // TimeShirt 12GB or 30GB
			//HDDInfo.hdd_actual_B_size -= (TIMESHIFT_SIZE - current_timeshift_size);   // TimeShirt 12GB or 30GB
			HDDInfo.hdd_free_B_size -= (TIMESHIFT_SIZE - current_timeshift_size);     // TimeShirt 12GB or 30GB
		}
		if (HDDInfo.hdd_used_B_size <= st.f_bsize || HDDInfo.hdd_used_B_size < 102400)
			HDDInfo.hdd_used_B_size = 0;
		if (HDDInfo.hdd_actual_B_size < HDDInfo.hdd_free_B_size)
			HDDInfo.hdd_free_B_size = HDDInfo.hdd_actual_B_size - HDDInfo.hdd_used_B_size;
		if (HDDInfo.hdd_free_B_size < 0)
		{
			ALOGD("===========> GetHDDActualSize(): HDDInfo.hdd_free_B_size [%lld] < 0", HDDInfo.hdd_free_B_size);
			//HDDInfo.hdd_free_B_size = 0;
			//HDDInfo.hdd_used_B_size = HDDInfo.hdd_actual_B_size;
			HDDInfo.hdd_free_B_size = HDDInfo.hdd_actual_B_size - HDDInfo.hdd_used_B_size;
		}
		if (HDDInfo.hdd_actual_B_size < HDDInfo.hdd_used_B_size)
		{
			HDDInfo.hdd_used_B_size = HDDInfo.hdd_actual_B_size;
			HDDInfo.hdd_free_B_size = 0;
		}

		/*
		HDDInfo.hdd_actual_B_size = (long long)st.f_blocks * (long long)st.f_bsize;
		HDDInfo.hdd_used_B_size = ((long long)st.f_blocks - (long long)st.f_bfree) * st.f_bsize;
		ALOGD("Default: ======================> HDDInfo.hdd_actual_B_size [%lld]", HDDInfo.hdd_actual_B_size);
		ALOGD("Default: ======================> HDDInfo.hdd_used_B_size [%lld]", HDDInfo.hdd_used_B_size);
		if (subtract_flag)
			HDDInfo.hdd_used_B_size -= SYSTEM_USED_SIZE;   // System Used
		if (HDDInfo.hdd_used_B_size <= st.f_bsize)
			HDDInfo.hdd_used_B_size = 0;
		HDDInfo.hdd_free_B_size = (long long)st.f_bfree * (long long)st.f_bsize;
		ALOGD("Default: ======================> HDDInfo.hdd_free_B_size [%lld]", HDDInfo.hdd_free_B_size);
		if (subtract_flag)
			HDDInfo.hdd_free_B_size += SYSTEM_USED_SIZE;   // System Used
		if (HDDInfo.hdd_actual_B_size < HDDInfo.hdd_free_B_size)
			HDDInfo.hdd_free_B_size = HDDInfo.hdd_actual_B_size - HDDInfo.hdd_used_B_size;

		if (subtract_flag)
		{
			HDDInfo.hdd_actual_B_size -= TIMESHIFT_SIZE;   // TimeShirt 12GB or 30GB
			HDDInfo.hdd_free_B_size -= TIMESHIFT_SIZE;     // TimeShirt 12GB or 30GB
		}

		//ALOGD("-----------> GetHDDActualSize(): st.f_blocks[%lld], st.f_bsize[%d], st.f_blocks * st.f_bsize[%lld]", st.f_blocks, st.f_bsize, st.f_blocks * st.f_bsize);
		//ALOGD("-----------> GetHDDActualSize(): st.f_blocks[%lld], st.f_bsize[%d], st.f_bfree[%lld], (st.f_blocks - st.f_bfree) * st.f_bsize[%lld]", st.f_blocks, st.f_bsize, st.f_bfree, (st.f_blocks - st.f_bfree) * st.f_bsize);

		//long long current_timeshift_size = GetSizeOfTimeShift(HDDInfo.mounted_dir);
		//ALOGD("===========> current_timeshift_size[%lld]", current_timeshift_size);
		*/
	}

	ALOGD("======================> HDDInfo.hdd_actual_B_size [%lld]", HDDInfo.hdd_actual_B_size);
	ALOGD("======================> HDDInfo.hdd_used_B_size [%lld]", HDDInfo.hdd_used_B_size);
	ALOGD("======================> HDDInfo.hdd_free_B_size [%lld]", HDDInfo.hdd_free_B_size);
}

long long HDDInfoStore::GetSizeOfTimeShift(const char *mounted_dir)
{
	long long current_timeshift_size = 0L;

	char buf[256];
	memset(buf, 0, 256);
	sprintf(buf, "%s/%s", mounted_dir, TIMESHIFT_TS);
	ALOGD("-----------> GetHDDActualSize(): buf[%s]", buf);

	int mReadFD = ::open(buf, O_RDONLY);
	int64_t bytes = lseek64(mReadFD, 0, SEEK_END);
	::close(mReadFD);
	if (bytes > 0)
	{
		current_timeshift_size = bytes;
	}

	memset(buf, 0, 256);
	sprintf(buf, "%s/%s", mounted_dir, TIMESHIFT_CTRL_DUMP);
	ALOGD("-----------> GetHDDActualSize(): buf[%s]", buf);

	mReadFD = ::open(buf, O_RDONLY);
	bytes = lseek64(mReadFD, 0, SEEK_END);
	::close(mReadFD);
	if (bytes > 0)
	{
		current_timeshift_size += bytes;
	}

	memset(buf, 0, 256);
	sprintf(buf, "%s/%s", mounted_dir, TIMESHIFT_IDX_DUMP);
	ALOGD("-----------> GetHDDActualSize(): buf[%s]", buf);

	mReadFD = ::open(buf, O_RDONLY);
	bytes = lseek64(mReadFD, 0, SEEK_END);
	::close(mReadFD);
	if (bytes > 0)
	{
		current_timeshift_size += bytes;
	}

	return current_timeshift_size;
}

void* thread_hdd_format_main(void* arg)
{
	on_formatting_flag = true;

	char dev_name[10];
	memset(dev_name, 0, 10);
	strcpy(dev_name, (char*)arg);

	if (pDetectHDD->selectedHDDIndex >= 0)
	{
		bool gpt_flag = false;
		if (!strcmp(pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.fs_type, "GPT"))
		{
			gpt_flag = true;
		}

		char mounted_dir_buf[200];
		char remount_mounted_dir[80];
		ALOGD("---------> pDetectHDD->hddInfoStore[%d].vPartitionBlock.size() [%d] fs_type [%s]", 
		        pDetectHDD->selectedHDDIndex, 
		        pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].vPartitionBlock.size(),
		        gpt_flag?"GPT":"?");
		Vector<PartitionBlock>::iterator iter;
		for (iter = pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].vPartitionBlock.begin(); iter != pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].vPartitionBlock.end(); iter++)
		{
			char tmpBuf[30];
			memset(tmpBuf, 0, 30);
			sprintf(tmpBuf, "/dev/block/vold/%s", iter->dev_name);
			ALOGD("---------> PartitionBlock: tmpBuf [%s]", tmpBuf);
			int waiting_cnt = 0;
			while (waiting_cnt < 12)
			{
				if (pDetectHDD->selectedHDDIndex >= 0 && pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].unMountpoint(tmpBuf, !gpt_flag))
					break;
				if (pDetectHDD->selectedHDDIndex < 0)
					break;
				waiting_cnt++;
				if (waiting_cnt >= 12) break;
				usleep(1000000);	// 1000ms
			}

			//memset(mounted_dir_buf, 0, 200);
			//sprintf(mounted_dir_buf, "busybox umount -l %s", iter->mounted_dir);
			//ALOGD("---------> vPartitionBlock: (umount) mounted_dir_buf [%s]", mounted_dir_buf);
			//if (system(mounted_dir_buf) != -1)
			//{
			//	memset(mounted_dir_buf, 0, 200);
			//	sprintf(mounted_dir_buf, "rm -rf %s", iter->mounted_dir);
			//	ALOGD("---------> vPartitionBlock: (rm -rf) mounted_dir_buf [%s]", mounted_dir_buf);
			//	system(mounted_dir_buf);
			//}
		}
#if DEV_FTS_USE == 1
		//pDetectHDD->setKeyValue("hdd.format.error.SN", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.serial);
#else
		//pDetectHDD->setSerialNumber(pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.serial); //"hdd.format.error.SN"
#endif

		char tmpFsPath[30];
		memset(tmpFsPath, 0, 30);
		if (gpt_flag)
		{
			char tmp_dev_name[10];
			memset(tmp_dev_name, 0, 10);
			strcpy(tmp_dev_name, dev_name);
			int dev_name_len = strlen(tmp_dev_name);
			tmp_dev_name[dev_name_len-1] = dev_name[dev_name_len-1]+1;
			sprintf(tmpFsPath, "/dev/block/vold/%s", tmp_dev_name);
		}
		else
		{
			sprintf(tmpFsPath, "/dev/block/vold/%s", dev_name);
		}
		bool device_rm_flag;
		if (fs_ext2_supported)
			device_rm_flag = true;
		else
			device_rm_flag = false;
		int waiting_cnt = 0;
		while (waiting_cnt < 12)
		{
			if (pDetectHDD->selectedHDDIndex >= 0 && pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].unMountpoint(tmpFsPath, device_rm_flag))
				break;
			if (pDetectHDD->selectedHDDIndex < 0)
				break;
			waiting_cnt++;
			if (waiting_cnt >= 12) break;
			usleep(1000000);	// 1000ms
		}

		//yinohyinoh
		if (!strcmp(pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.sdx_name, "block/sda/"))
		{
			property_set("ctl.stop", "fuse_sdcard1");
		//#if REDUNDANCY_MOUNT == 1
		//	char redundancy_unmount_buf[100];
		//	memset(redundancy_unmount_buf, 0, 100);
		//	sprintf(redundancy_unmount_buf, "umount %s", REDUNDANCY_MOUNT_DIR_1);
		//	system(redundancy_unmount_buf);
		//#endif
		}
		else
		{
			char sdx_str[30];
			for (int i='b'; i <= 'z'; i++)
			{
				memset(sdx_str, 0, 30);
				sprintf(sdx_str, "block/sd%c/", i);
				if (!strcmp(pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.sdx_name, sdx_str))
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
					break;
				}
			}
		}

		//char* tmpMountpoint = pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].getMountpoint(tmpFsPath);
		//memset(mounted_dir_buf, 0, 200);
		//sprintf(mounted_dir_buf, "busybox umount -l %s", tmpMountpoint);
		//system(mounted_dir_buf);
		//ALOGD("---------> (umount) mounted_dir_buf [%s]", mounted_dir_buf);
		//memset(mounted_dir_buf, 0, 200);
		//sprintf(mounted_dir_buf, "rm -rf %s", tmpMountpoint);
		//system(mounted_dir_buf);
		//ALOGD("---------> (rm -rf) mounted_dir_buf [%s]", mounted_dir_buf);

		char buff[50];
		char buff_vfat[50];
		memset(buff, 0, 50);
		memset(buff_vfat, 0, 50);
		//if (gpt_flag || pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].vPartitionBlock.size() > 0)
		if (gpt_flag)
		{
			char tmp_sdx_name[20];
			memset(tmp_sdx_name, 0, 20);
			strcpy(tmp_sdx_name, pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.sdx_name);
			tmp_sdx_name[strlen(tmp_sdx_name)-1] = '\0';
			if (fs_ext2_supported)
			{
				sprintf(buff, "busybox mke2fs -F /dev/%s", tmp_sdx_name);
				sprintf(buff_vfat, "busybox mkfs.vfat /dev/%s", tmp_sdx_name);
			}
			else
			{
				//sprintf(buff, "make_ext4fs -j 65536 /dev/%s", tmp_sdx_name);
				//sprintf(buff, "make_ext4fs /dev/%s", tmp_sdx_name);
				char tmp_dev_name[10];
				memset(tmp_dev_name, 0, 10);
				strcpy(tmp_dev_name, dev_name);
				int dev_name_len = strlen(tmp_dev_name);
				tmp_dev_name[dev_name_len-1] = dev_name[dev_name_len-1]+1;
				sprintf(buff, "make_ext4fs -j 65536 /dev/block/vold/%s", tmp_dev_name);
				//sprintf(buff, "make_ext4fs /dev/block/vold/%s", tmp_dev_name);
				sprintf(buff_vfat, "busybox mkfs.vfat /dev/block/vold/%s", tmp_dev_name);
			}
		}
		else
		{
			char tmp_sdx_name[20];
			memset(tmp_sdx_name, 0, 20);
			strcpy(tmp_sdx_name, pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.sdx_name);
			tmp_sdx_name[strlen(tmp_sdx_name)-1] = '\0';
			if (fs_ext2_supported)
			{
				sprintf(buff, "busybox mke2fs -F /dev/%s", tmp_sdx_name);
				sprintf(buff_vfat, "busybox mkfs.vfat /dev/%s", tmp_sdx_name);
			}
			else
			{
				//sprintf(buff, "make_ext4fs -j 65536 /dev/%s", tmp_sdx_name);
				//sprintf(buff, "make_ext4fs /dev/%s", tmp_sdx_name);
				sprintf(buff, "make_ext4fs -j 65536 /dev/block/vold/%s", dev_name);
				//sprintf(buff, "make_ext4fs /dev/block/vold/%s", dev_name);
				sprintf(buff_vfat, "busybox mkfs.vfat /dev/block/vold/%s", dev_name);
			}
		}

#if VOLUME_DEV_INFO_TO_VOLD_BY_FILE == 1
		char volume_dev_buf[100];
		memset(volume_dev_buf, 0, 100);
		sprintf(volume_dev_buf, "%sext4_volume_dev.txt", HDD_HOME);
		FILE* fp_vol_dev = fopen(volume_dev_buf, "w");
		if (fp_vol_dev)
		{
			fputs(tmpFsPath, fp_vol_dev);
			fclose(fp_vol_dev);
		}
#endif

		if (pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDErrFlag)
		{
			ALOGD("buff_vfat [%s]", buff_vfat);
			pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDErrFlag = false;
			system(buff_vfat);
		}
		//else
		//{
		//	ALOGD("buff_vfat [%s]", buff_vfat);
		//	system(buff_vfat);
		//}

		ALOGD("buff [%s]", buff);
		ALOGD("===========> format begin");
#if 1
        char blk_dev[256];
        sprintf(blk_dev, "/dev/block/vold/%s", dev_name);
        int ret_system = make_ext4fs(blk_dev, 0, NULL, NULL);
		if (0 == ret_system)
#else
		int ret_system = system(buff);
		if (ret_system != 127 && ret_system != -1)
#endif
		{
		    ALOGD("===========> format finished!");
#if 1
			// for some corrupted HDD
			for (int i=0; i < 5; i++)
			{
				sleep(1);
				if (pDetectHDD->selectedHDDIndex < 0)
					break;
			}
#endif

			if (pDetectHDD->selectedHDDIndex >= 0 && pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].SlotConnected)
			{
				memset(remount_mounted_dir, 0, 80);
				if (pDetectHDD->selectedHDDIndex >= 0)
				{
					//char* mounted_dir_pos = strrchr(pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, '/');
					//if (mounted_dir_pos)
					//{
					//	char mounted_dir_tail[80];
					//	memset(mounted_dir_tail, 0, 80);
					//	strcpy(mounted_dir_tail, mounted_dir_pos+1);
					//	sprintf(remount_mounted_dir, "%s%s", MNT_MEDIA_DIR, mounted_dir_tail);
					//	char tmp_buf[256];
					//	memset(tmp_buf, 0, 256);
					//	sprintf(tmp_buf, "mkdir %s", remount_mounted_dir);
					//	system(tmp_buf);
					//}
					//else
					//{
						//if (pDetectHDD->selectedHDDIndex >= 0)
							memcpy(remount_mounted_dir, pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, 80);
					//}
				}

				if (pDetectHDD->selectedHDDIndex >= 0 && pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir[0] == '\0')
				{
#if RM_PREV_MOUNTED_DIR_ON_FORMAT == 1
					memset(mounted_dir_buf, 0, 200);
					sprintf(mounted_dir_buf, "rm -rf %s", remount_mounted_dir);
					system(mounted_dir_buf);
					ALOGD("---------> (mount): (rm -rf) mounted_dir_buf [%s]", mounted_dir_buf);
#endif

					if (pDetectHDD->selectedHDDIndex >= 0)
					{
						//sprintf(remount_mounted_dir, "%susb.%s", MNT_MEDIA_DIR, pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.serial);
						sprintf(remount_mounted_dir, "%susb.HDD", MNT_MEDIA_DIR);
					}
					if (pDetectHDD->selectedHDDIndex >= 0)
						memset(pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, 0, 80);
					if (pDetectHDD->selectedHDDIndex >= 0)
						strcpy(pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, remount_mounted_dir);

					memset(mounted_dir_buf, 0, 200);
					if (pDetectHDD->selectedHDDIndex >= 0)
						sprintf(mounted_dir_buf, "mkdir %s", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir);
					system(mounted_dir_buf);
					ALOGD("---------> (mount): (mkdir) mounted_dir_buf [%s]", mounted_dir_buf);
				}

				//confirm: busybox umount -l fsPath
				int tmpFd;
				char tmp_dev_name[10];
				char tmp_sdx_name[20];
				char tmp_filename[80];
				memset(tmp_dev_name, 0, 10);
				memset(tmp_sdx_name, 0, 20);
				memset(tmp_filename, 0, 80);
				if (pDetectHDD->selectedHDDIndex >= 0)
				{
					strcpy(tmp_sdx_name, pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.sdx_name);
					tmp_sdx_name[strlen(tmp_sdx_name)-1] = '\0'; // 마지막 '/' 제거
				}
				uint8_t block_sdx_len = strlen(tmp_sdx_name);
				if (block_sdx_len >= 1)
				{
					char block_sdx_ch = tmp_sdx_name[block_sdx_len-1];
					sprintf(tmp_filename, "/sys/%s/dev", tmp_sdx_name);
					if ((tmpFd = open(tmp_filename, O_RDONLY)) > 0)
					{
						int rd_size;
						if ((rd_size = read(tmpFd, tmp_dev_name, 9)) > 0)
							tmp_dev_name[rd_size-1] = '\0';
						ALOGD("=========================> tmp_dev_name [%s]", tmp_dev_name);
						char tmpFsPath[50];
						memset(tmpFsPath, 0, 50);
						sprintf(tmpFsPath, "/dev/block/vold/%s", tmp_dev_name);
						if (pDetectHDD->selectedHDDIndex >= 0)
							pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].unMountpoint(tmpFsPath, device_rm_flag);
						close(tmpFd);
					}

					int tmp_partition_num = 1;
					do
					{
						sprintf(tmp_filename, "/sys/%s/sd%c%d/dev", tmp_sdx_name, block_sdx_ch, tmp_partition_num++);
						if ((tmpFd = open(tmp_filename, O_RDONLY)) > 0)
						{
							int rd_size;
							if ((rd_size = read(tmpFd, tmp_dev_name, 9)) > 0)
								tmp_dev_name[rd_size-1] = '\0';
							ALOGD("=========================> tmp_dev_name [%s]", tmp_dev_name);
							char tmpFsPath[50];
							memset(tmpFsPath, 0, 50);
							sprintf(tmpFsPath, "/dev/block/vold/%s", tmp_dev_name);
							if (pDetectHDD->selectedHDDIndex >= 0)
								pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].unMountpoint(tmpFsPath, device_rm_flag);
							close(tmpFd);
						}
						if (pDetectHDD->selectedHDDIndex < 0)
						{
							break;
						}
					}
					while (tmpFd != -1);
				}
				////confirm: busybox umount -l fsPath again
				////char tmp_sdx_name[20];
				//memset(tmp_sdx_name, 0, 20);
				//if (pDetectHDD->selectedHDDIndex >= 0)
				//{
				//	//pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].unMountpoint(tmpFsPath, device_rm_flag);
				//	usleep(300000);	//300ms
				//	char* pos_tmp_sdx_name = strstr(pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.sdx_name, "/sd");
				//	if (pos_tmp_sdx_name)
				//	{
				//		strcpy(tmp_sdx_name, pos_tmp_sdx_name+1);
				//		tmp_sdx_name[strlen(tmp_sdx_name)-1] = '\0';
				//		char* tmpMajorMinor = NULL;
				//		if (pDetectHDD->selectedHDDIndex >= 0)
				//			tmpMajorMinor = pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].getMajorMinor(tmp_sdx_name);
				//		uint8_t tmp_partition_num = 1;
				//		while (tmpMajorMinor != NULL)
				//		{
				//			char tmpFsPath2[30];
				//			memset(tmpFsPath2, 0, 30);
				//			sprintf(tmpFsPath2, "/dev/block/vold/%s", tmpMajorMinor);
				//			if (pDetectHDD->selectedHDDIndex >= 0)
				//				pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].unMountpoint(tmpFsPath2, device_rm_flag);
				//			char tmp_sdx_name2[20];
				//			memset(tmp_sdx_name2, 0, 20);
				//			sprintf(tmp_sdx_name2, "%s%d", tmp_sdx_name, tmp_partition_num++);
				//			if (pDetectHDD->selectedHDDIndex >= 0)
				//				tmpMajorMinor = pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].getMajorMinor(tmp_sdx_name2);
				//		}
				//	}
				//	usleep(300000);	//300ms
				//}

				int rc_waiting_cnt = 0;
				int rc = -1;
				//if (gpt_flag || (pDetectHDD->selectedHDDIndex >= 0 && pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].vPartitionBlock.size() > 0))
				if (gpt_flag)
				{
					char tmp_sdx_name[20];
					memset(tmp_sdx_name, 0, 20);
					if (pDetectHDD->selectedHDDIndex >= 0)
						strcpy(tmp_sdx_name, pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.sdx_name);
					tmp_sdx_name[strlen(tmp_sdx_name)-1] = '\0';
					memset(mounted_dir_buf, 0, 200);
					if (fs_ext2_supported)
					{
						sprintf(mounted_dir_buf, "busybox mount -t ext2 -w /dev/%s %s", tmp_sdx_name, remount_mounted_dir);
					}
					else
					{
						//sprintf(mounted_dir_buf, "busybox mount -t ext4 -w /dev/%s %s", tmp_sdx_name, remount_mounted_dir);
						char tmp_dev_name[10];
						memset(tmp_dev_name, 0, 10);
						strcpy(tmp_dev_name, dev_name);
						int dev_name_len = strlen(tmp_dev_name);
						tmp_dev_name[dev_name_len-1] = dev_name[dev_name_len-1]+1;
						sprintf(mounted_dir_buf, "busybox mount -t ext4 -w /dev/block/vold/%s %s", tmp_dev_name, remount_mounted_dir);
					}
					rc = system(mounted_dir_buf);
					ALOGD("============> (format:mount:gpt_flag) mounted_dir_buf [%s]", mounted_dir_buf);

					if (rc)
					{
						// HDD is busy
						sleep(2);

						char try_buf[256];
						char try_buf_dev_name[50];
						memset(try_buf, 0, 256);
						memset(try_buf_dev_name, 0, 50);
						if (fs_ext2_supported)
							sprintf(try_buf_dev_name, "/dev/%s", tmp_sdx_name);
						else
							sprintf(try_buf_dev_name, "/dev/block/vold/%s", dev_name);
						memset(try_buf, 0, 256);
						sprintf(try_buf, "busybox mount -t ext4 %s %s", try_buf_dev_name, remount_mounted_dir);
						char* mounted_point = NULL;
						for (int i=0; i < 10; i++)
						{
							if (pDetectHDD->selectedHDDIndex < 0)
								break;

							system(try_buf);
							ALOGD("======(try[%d])========>  (mount) try_buf [%s]", i+1, try_buf);
							for (int j=0; j < 2; j++)
							{
								if (pDetectHDD->selectedHDDIndex < 0)
									break;
								sleep(1);
							}
							if (pDetectHDD->selectedHDDIndex >= 0)
							{
								mounted_point = pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].getMountpoint(try_buf_dev_name);
							}
							else
							{
								break;
							}
							ALOGD("======(try[%d])========>  mounted_point [%s]", i+1, mounted_point);
							if (mounted_point[0] != '\0')
							{
								rc = 0;
								break;
							}
						}
						ALOGD("==============>  (mount) try_buf [%s]", try_buf);
					}
				}
				else
				{
					//unsigned long flags;

					//flags = MS_NODEV | MS_NOSUID | MS_DIRSYNC | MS_NOATIME | MS_NOEXEC;
					//while (rc_waiting_cnt < 20 && rc)
					//{
					//	if (fs_ext2_supported)
					//		rc = mount(tmpFsPath, remount_mounted_dir, "ext2", flags, NULL);
					//	else
					//		rc = mount(tmpFsPath, remount_mounted_dir, "ext4", flags, NULL);
					//	if (rc_waiting_cnt >= 20 || !rc) break;
					//	usleep(200000);	// 200ms
					//	rc_waiting_cnt++;
					//}
					//ALOGD("==============> format: rc_waiting_cnt[%d], rc[%d]", rc_waiting_cnt, rc);

					//if (rc)
					//{
						char tmp_sdx_name[20];
						memset(tmp_sdx_name, 0, 20);
						if (pDetectHDD->selectedHDDIndex >= 0)
							strcpy(tmp_sdx_name, pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.sdx_name);
						tmp_sdx_name[strlen(tmp_sdx_name)-1] = '\0';
						memset(mounted_dir_buf, 0, 200);
#if 0
						if (fs_ext2_supported)
						{
							sprintf(mounted_dir_buf, "busybox mount -t ext2 -w /dev/%s %s", tmp_sdx_name, remount_mounted_dir);
						}
						else
						{
							//sprintf(mounted_dir_buf, "busybox mount -t ext4 -w /dev/%s %s", tmp_sdx_name, remount_mounted_dir);
							sprintf(mounted_dir_buf, "busybox mount -t ext4 -w /dev/block/vold/%s %s", dev_name, remount_mounted_dir);
						}
						rc = system(mounted_dir_buf);
						ALOGD("============> (format:mount) mounted_dir_buf [%s]", mounted_dir_buf);
#else

                        unsigned long flags;
                        const char *mnt_opts = MOUNT_OPTS;
                        char source[256];
                        sprintf(source, "/dev/block/vold/%s", dev_name);

                        flags = MS_NODEV | MS_NOSUID | MS_DIRSYNC | MS_NOATIME | MS_NOEXEC;
                        rc = mount(source, remount_mounted_dir, fs_ext_str, flags, mnt_opts);
                        ALOGD("mount return %d (%s)\n", rc, rc?strerror(errno):"OK");
#endif

						ALOGD("============> (format:mount) mounted_dir_buf [%s -> %s]", source, remount_mounted_dir);

						if (rc)
						{
							// HDD is busy
							sleep(2);
							char try_buf[256];
							char try_buf_dev_name[50];
							memset(try_buf, 0, 256);
							memset(try_buf_dev_name, 0, 50);
							if (fs_ext2_supported)
								sprintf(try_buf_dev_name, "/dev/%s", tmp_sdx_name);
							else
								sprintf(try_buf_dev_name, "/dev/block/vold/%s", dev_name);
							memset(try_buf, 0, 256);
							sprintf(try_buf, "busybox mount -t ext4 %s %s", try_buf_dev_name, remount_mounted_dir);
							char* mounted_point = NULL;
							for (int i=0; i < 10; i++)
							{
								if (pDetectHDD->selectedHDDIndex < 0)
									break;

								system(try_buf);
								ALOGD("======(try[%d])========>  (mount) try_buf [%s]", i+1, try_buf);
								for (int j=0; j < 2; j++)
								{
									if (pDetectHDD->selectedHDDIndex < 0)
										break;
									sleep(1);
								}
								if (pDetectHDD->selectedHDDIndex >= 0)
								{
									mounted_point = pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].getMountpoint(try_buf_dev_name);
								}
								else
								{
									break;
								}
								ALOGD("======(try[%d])========>  mounted_point [%s]", i+1, mounted_point);
								if (mounted_point[0] != '\0')
								{
									rc = 0;
									break;
								}
							}
							ALOGD("==============>  (mount) try_buf [%s]", try_buf);
						}
                        else
                        {
                            ALOGD("remounted successfully!");
                        }
					//}
				}

				if (!rc)
				{
					if (fs_ext2_supported)
					{
						if (!strstr(remount_mounted_dir, "sdcard") && !strstr(remount_mounted_dir, "usbdisk") && !strstr(remount_mounted_dir, "usb.HDD"))
						{
							memset(mounted_dir_buf, 0, 200);
							sprintf(mounted_dir_buf, "rm -rf %s/*", remount_mounted_dir);
							system(mounted_dir_buf);
						}
					}
					else
					{
						char timeshift_buf[256];
						memset(timeshift_buf, 0, 256);
						sprintf(timeshift_buf, "rm -f %s/%s*", remount_mounted_dir, TIMESHIFT_FILE_PREFIX);
						system(timeshift_buf);
					}

					if (pDetectHDD->selectedHDDIndex >= 0)
						pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].vPartitionBlock.clear();
					char remount_block_sdx_name[50];
					memset(remount_block_sdx_name, 0, 50);
					if (pDetectHDD->selectedHDDIndex >= 0)
					{
						memcpy(pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, remount_mounted_dir, 80);

						char tmp_buf[256];
						memset(tmp_buf, 0, 256);
						sprintf(tmp_buf, "chown system %s", remount_mounted_dir);
						system(tmp_buf);
						ALOGD("==============> tmp_buf[%s]", tmp_buf);
					}

					if (pDetectHDD->selectedHDDIndex >= 0)
						sprintf(remount_block_sdx_name, "/sys/%s", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.sdx_name);
					if (pDetectHDD->selectedHDDIndex >= 0)
						pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.hdd_total_B_size = pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].GetHDDSize(remount_block_sdx_name);
					if (pDetectHDD->selectedHDDIndex >= 0)
						ALOGD("=========================> (format:mount) hdd_size (%lld) Bytes", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.hdd_total_B_size);

					if (gpt_flag || (pDetectHDD->selectedHDDIndex >= 0 && pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].vPartitionBlock.size() > 0))
					{
						sleep(3);
					}

					if (pDetectHDD->selectedHDDIndex >= 0)
						pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].GetHDDActualSize(true, true);
					if (pDetectHDD->selectedHDDIndex >= 0)
						memset(pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.fs_type, 0, 10);
					if (pDetectHDD->selectedHDDIndex >= 0)
						strcpy(pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.fs_type, fs_ext_str);
					if (pDetectHDD->selectedHDDIndex >= 0)
						pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.ext4_flag = 1;
					if (pDetectHDD->selectedHDDIndex >= 0)
						pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.partition_cnt = 1;

					//if (pDetectHDD->selectedHDDIndex >= 0)
					//	pDetectHDD->sendRemoveEvent(pDetectHDD->selectedHDDIndex);
					//sleep(5);
					//if (pDetectHDD->selectedHDDIndex >= 0)
					//	pDetectHDD->sendAddEvent(pDetectHDD->selectedHDDIndex);

					//if (pDetectHDD->selectedHDDIndex >= 0)
						pDetectHDD->sendUpdateEvent(pDetectHDD->selectedHDDIndex);

					if (pDetectHDD->selectedHDDIndex >= 0)
					{
#if DEV_FTS_USE == 1
						pDetectHDD->setKeyValue("hdd.serial.number", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.serial);
						//pDetectHDD->setKeyValue("hdd.format.error.SN", "");
#else
						pDetectHDD->setSerialNumber(pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.serial); //"hdd.serial.number"
						//pDetectHDD->setSerialNumber(""); //"hdd.format.error.SN"
#endif
					}
				}
                else
                {
                    ALOGD("rc %d \n", rc);

                    // send update event << ERROR >>
			        pDetectHDD->sendUpdateEvent(-1);
                }
			}
		}
		else
		{
		    // format fail !
		    ALOGD("===========> format fail!!!");
			if (pDetectHDD->selectedHDDIndex >= 0)
			{
				pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.ext4_flag = 0;
			}
			pDetectHDD->sendUpdateEvent(-1);
		}
		ALOGD("===========> format end");
	}

	on_formatting_flag = false;

	return NULL;
}

bool HDDInfoStore::FormatHDDToExt4()
{
	if (dev_name[0] != '\0')
	{
		ignore_change_msgiov_flag = true;
		pthread_create(&hdd_format_thread[0], NULL, &thread_hdd_format_main, (void*)dev_name);
		return true;
	}
	return false;
}

char* HDDInfoStore::getMountpoint(const char *fsPath)
{
	static char retMountpoint[256];

	memset(retMountpoint, 0, 256);

	char device[256];
	char mount_path[256];
	char rest[256];
	FILE *fp;
	char line[1024];

	if (!(fp = fopen("/proc/mounts", "r")))
	{
		ALOGE("Error opening /proc/mounts (%s)", strerror(errno));
		return retMountpoint;
	}

	while (fgets(line, sizeof(line), fp))
	{
		line[strlen(line)-1] = '\0';
		sscanf(line, "%255s %255s %255s\n", device, mount_path, rest);
		if (!strcmp(device, fsPath))
		{
			fclose(fp);
			strcpy(retMountpoint, mount_path);
			ALOGD("=========> getMountpoint(): retMountpoint[%s]", retMountpoint);
			return retMountpoint;
		}
	}

	fclose(fp);
	return retMountpoint;
}

bool HDDInfoStore::unMountpoint(const char *fsPath, bool device_rm_flag)
{
	char device[256];
	char mount_path[256];
	char rest[256];
	FILE *fp;
	char line[1024];

	if (!(fp = fopen("/proc/mounts", "r")))
	{
		ALOGE("Error opening /proc/mounts (%s)", strerror(errno));
		return false;
	}

	while (fgets(line, sizeof(line), fp))
	{
		line[strlen(line)-1] = '\0';
		sscanf(line, "%255s %255s %255s\n", device, mount_path, rest);
		if (!strcmp(device, fsPath))
		{
			fclose(fp);
			char tmp_buf[256];
			memset(tmp_buf, 0, 256);
			sprintf(tmp_buf, "busybox umount -l %s", mount_path);
			system(tmp_buf);
			if (!strstr(mount_path, "sdcard") && !strstr(mount_path, "usbdisk") && !strstr(mount_path, "usb.HDD"))
			{
				memset(tmp_buf, 0, 256);
				sprintf(tmp_buf, "rm -rf %s", mount_path);
				system(tmp_buf);
			}
			if (device_rm_flag)
			{
				memset(tmp_buf, 0, 256);
				sprintf(tmp_buf, "rm -rf %s", device);
				system(tmp_buf);
			}
			return true;
		}
	}

	fclose(fp);
	return false;
}

char* HDDInfoStore::getMajorMinor(const char *sdxName)
{
	char major[256];
	char minor[256];
	char blocks[256];
	char name[256];
	FILE *fp;
	char line[1024];

	if (!(fp = fopen("/proc/partitions", "r")))
	{
		ALOGE("Error opening /proc/partitions (%s)", strerror(errno));
		return NULL;
	}

	while (fgets(line, sizeof(line), fp))
	{
		line[strlen(line)-1] = '\0';
		sscanf(line, "%255s %255s %255s %255s\n", major, minor, blocks, name);
		if (!strcmp(name, sdxName))
		{
			fclose(fp);
			static char tmp_dev_name[10];
			memset(tmp_dev_name, 0, 10);
			sprintf(tmp_dev_name, "%s:%s", major, minor);
			return tmp_dev_name;
		}
	}

	fclose(fp);
	return NULL;
}

bool HDDInfoStore::MountHDD(bool send_add_event_flag)
{
	bool flag = false;

	ALOGD("---------> MountHDD: HDDInfo.mounted_dir[%s], HDDInfo.sdx_name[%s], dev_name[%s], HDDInfo.ext4_flag[%d]", HDDInfo.mounted_dir, HDDInfo.sdx_name, dev_name, HDDInfo.ext4_flag);
	if (HDDInfo.ext4_flag)
	{
		if (HDDInfo.mounted_dir[0] == '\0' && dev_name[0] != '\0')
		{
			if (HDDInfo.mounted_dir[0] == '\0')
			{
				char remount_mounted_dir[80];
				memset(remount_mounted_dir, 0, 80);
				sprintf(remount_mounted_dir, "%susb.%s", MNT_MEDIA_DIR, HDDInfo.serial);
				memset(HDDInfo.mounted_dir, 0, 80);
				strcpy(HDDInfo.mounted_dir, remount_mounted_dir);
			}

			char tmp_dev_name[10];
			memset(tmp_dev_name, 0, 10);
			strcpy(tmp_dev_name, dev_name);
			if (GPTFlag)
			{
				int dev_name_len = strlen(tmp_dev_name);
				tmp_dev_name[dev_name_len-1] = dev_name[dev_name_len-1]+1;
			}
			char mount_dev_name[30];
			memset(mount_dev_name, 0, 30);
			sprintf(mount_dev_name, "/dev/block/vold/%s", tmp_dev_name);

			char mounted_dir_buf[200];
			//if (vPartitionBlock.size() > 0)
			//{
			//	ALOGD("---------> MountHDD: vPartitionBlock.size() [%d]", vPartitionBlock.size());
			//	Vector<PartitionBlock>::iterator iter;
			//	for (iter = vPartitionBlock.begin(); iter != vPartitionBlock.end(); iter++)
			//	{
			//		memset(mounted_dir_buf, 0, 200);
			//		sprintf(mounted_dir_buf, "mkdir %s", iter->mounted_dir);
			//		system(mounted_dir_buf);
			//		ALOGD("---------> MountHDD: vPartitionBlock: (mkdir) mounted_dir_buf [%s]", mounted_dir_buf);

			//		memset(mounted_dir_buf, 0, 200);
			//		sprintf(mounted_dir_buf, "chmod 775 %s", iter->mounted_dir);
			//		system(mounted_dir_buf);
			//		ALOGD("---------> MountHDD: vPartitionBlock: (chmod 775) mounted_dir_buf [%s]", mounted_dir_buf);

			//		memset(mounted_dir_buf, 0, 200);
			//		sprintf(mounted_dir_buf, "busybox mount -t %s -w /dev/block/vold/%s %s", HDDInfo.fs_type, iter->dev_name, iter->mounted_dir);
			//		system(mounted_dir_buf);
			//		ALOGD("---------> MountHDD: vPartitionBlock: (mount) mounted_dir_buf [%s]", mounted_dir_buf);
			//	}
			//}
			memset(mounted_dir_buf, 0, 200);
			sprintf(mounted_dir_buf, "mkdir %s", HDDInfo.mounted_dir);
			ALOGD("---------> MountHDD: (mkdir) mounted_dir_buf [%s]", mounted_dir_buf);
			if (system(mounted_dir_buf) != -1)
			{
				memset(mounted_dir_buf, 0, 200);
				sprintf(mounted_dir_buf, "chmod 775 %s", HDDInfo.mounted_dir);
				system(mounted_dir_buf);
				ALOGD("---------> MountHDD: (chmod 775) mounted_dir_buf [%s]", mounted_dir_buf);

				memset(mounted_dir_buf, 0, 200);
				sprintf(mounted_dir_buf, "busybox mount -t %s -w %s %s", HDDInfo.fs_type, mount_dev_name, HDDInfo.mounted_dir);
				system(mounted_dir_buf);
				ALOGD("---------> MountHDD: (mount) mounted_dir_buf [%s]", mounted_dir_buf);

				flag = true;

				GetHDDActualSize(true, false);

				if (send_add_event_flag)
				{
					pDetectHDD->sendAddEvent(pDetectHDD->selectedHDDIndex);
				}
			}
		}
	}
	return flag;
}

bool HDDInfoStore::UnmountHDD()
{
	bool flag = false;

	ALOGD("---------> MountHDD: HDDInfo.mounted_dir[%s], HDDInfo.sdx_name[%s], dev_name[%s], HDDInfo.ext4_flag[%d]", HDDInfo.mounted_dir, HDDInfo.sdx_name, dev_name, HDDInfo.ext4_flag);
	if (HDDInfo.mounted_dir[0] != '\0' && dev_name[0] != '\0')
	{
		//if (HDDInfo.ext4_flag)
		//{
			char mounted_dir_buf[200];
			if (vPartitionBlock.size() > 0)
			{
				ALOGD("---------> UnmountHDD: vPartitionBlock.size() [%d]", vPartitionBlock.size());
				Vector<PartitionBlock>::iterator iter;
				for (iter = vPartitionBlock.begin(); iter != vPartitionBlock.end(); iter++)
				{
					memset(mounted_dir_buf, 0, 200);
					sprintf(mounted_dir_buf, "busybox umount -l %s", iter->mounted_dir);
					system(mounted_dir_buf);
					ALOGD("---------> UnmountHDD: vPartitionBlock: (umount) mounted_dir_buf [%s]", mounted_dir_buf);

					if (!strstr(iter->mounted_dir, "sdcard") && !strstr(iter->mounted_dir, "usbdisk") && !strstr(iter->mounted_dir, "usb.HDD"))
					{
						memset(mounted_dir_buf, 0, 200);
						sprintf(mounted_dir_buf, "rm -rf %s", iter->mounted_dir);
						system(mounted_dir_buf);
						ALOGD("---------> UnmountHDD: vPartitionBlock: (rm -rf mounted_dir) mounted_dir_buf [%s]", mounted_dir_buf);
					}

					if (!GPTFlag)
					{
						memset(mounted_dir_buf, 0, 200);
						sprintf(mounted_dir_buf, "rm -rf /dev/block/vold/%s", iter->dev_name);
						system(mounted_dir_buf);
						ALOGD("---------> UnmountHDD: vPartitionBlock: (rm -rf dev_name) mounted_dir_buf [%s]", mounted_dir_buf);
					}
				}
			}
			memset(mounted_dir_buf, 0, 200);
			sprintf(mounted_dir_buf, "busybox umount -l %s", HDDInfo.mounted_dir);
			ALOGD("---------> UnmountHDD: (umount) mounted_dir_buf [%s]", mounted_dir_buf);
			if (system(mounted_dir_buf) != -1)
			{
				if (!strstr(HDDInfo.mounted_dir, "sdcard") && !strstr(HDDInfo.mounted_dir, "usbdisk") && !strstr(HDDInfo.mounted_dir, "usb.HDD"))
				{
					memset(mounted_dir_buf, 0, 200);
					sprintf(mounted_dir_buf, "rm -rf %s", HDDInfo.mounted_dir);
					memset(HDDInfo.mounted_dir, 0, 80);
					system(mounted_dir_buf);
					ALOGD("---------> UnmountHDD: (rm -rf mounted_dir) mounted_dir_buf [%s]", mounted_dir_buf);
				}

				//memset(mounted_dir_buf, 0, 200);
				//sprintf(mounted_dir_buf, "rm -rf /dev/block/vold/%s", dev_name);
				//system(mounted_dir_buf);
				//ALOGD("---------> UnmountHDD: (rm -rf dev_name) mounted_dir_buf [%s]", mounted_dir_buf);

				flag = true;

				//GetHDDActualSize(true, false);

				ignore_change_msgiov_flag = true;

				ALOGD("UnmountHDD: ");
				pDetectHDD->sendRemoveEvent(pDetectHDD->selectedHDDIndex);

				sleep(2);
				ignore_change_msgiov_flag = false;
			}
		//}
	}
	return flag;
}

int HDDInfoStore::getNumOfPartition(const char *sdx)
{
	int  numOfPartition = -1;

	char major[256];
	char minor[256];
	char blocks[256];
	char name[256];
	FILE *fp;
	char line[1024];

	if (!(fp = fopen("/proc/partitions", "r")))
	{
		ALOGE("Error opening /proc/partitions (%s)", strerror(errno));
		return -1;
	}

	while (fgets(line, sizeof(line), fp))
	{
		line[strlen(line)-1] = '\0';
		sscanf(line, "%255s %255s %255s %255s\n", major, minor, blocks, name);
		if ((!strcmp(major, "8") && !strncmp(name, sdx, 3)) ||
			(!strcmp(major, "65") && !strncmp(name, sdx, 3)))
		{
			numOfPartition++;
		}
	}

	fclose(fp);
	return numOfPartition;
}

char* HDDInfoStore::MountHDDExt4(char* sdx_name, char* serial)
{
	bool flag = false;
	static char remount_mounted_dir[80];

	memset(remount_mounted_dir, 0, 80);

	if (sdx_name != NULL && sdx_name[0] != '\0' && serial != NULL)
	{
		memset(remount_mounted_dir, 0, 80);
		sprintf(remount_mounted_dir, "%susb.%s", MNT_MEDIA_DIR, serial);

		char mount_dev_name[30];
		memset(mount_dev_name, 0, 30);
		sprintf(mount_dev_name, "/dev/%s", sdx_name);

		char mounted_dir_buf[200];
		memset(mounted_dir_buf, 0, 200);
		sprintf(mounted_dir_buf, "mkdir %s", remount_mounted_dir);
		ALOGD("---------> MountHDD: (mkdir) mounted_dir_buf [%s]", mounted_dir_buf);
		if (system(mounted_dir_buf) != -1)
		{
			memset(mounted_dir_buf, 0, 200);
			sprintf(mounted_dir_buf, "chmod 775 %s", remount_mounted_dir);
			system(mounted_dir_buf);
			ALOGD("---------> MountHDD: (chmod 775) mounted_dir_buf [%s]", mounted_dir_buf);

			memset(mounted_dir_buf, 0, 200);
			if (fs_ext2_supported)
				sprintf(mounted_dir_buf, "busybox mount -t ext2 -w %s %s", mount_dev_name, remount_mounted_dir);
			else
				sprintf(mounted_dir_buf, "busybox mount -t ext4 -w %s %s", mount_dev_name, remount_mounted_dir);
			system(mounted_dir_buf);
			ALOGD("---------> MountHDD: (mount) mounted_dir_buf [%s]", mounted_dir_buf);

			flag = true;

		}
	}

	return remount_mounted_dir;
}

bool HDDInfoStore::UnmountHDDExt4(char* mounted_dir, char* dev_name)
{
	bool flag = false;

	if (mounted_dir != NULL && mounted_dir[0] != '\0' && dev_name != NULL && dev_name[0] != '\0')
	{
		char mounted_dir_buf[200];
		memset(mounted_dir_buf, 0, 200);
		sprintf(mounted_dir_buf, "busybox umount -l %s", mounted_dir);
		ALOGD("---------> UnmountHDDExt4: (umount) mounted_dir_buf [%s]", mounted_dir_buf);
		if (system(mounted_dir_buf) != -1)
		{
			if (!strstr(mounted_dir, "sdcard") && !strstr(mounted_dir, "usbdisk") && !strstr(mounted_dir, "usb.HDD"))
			{
				memset(mounted_dir_buf, 0, 200);
				sprintf(mounted_dir_buf, "rm -rf %s", mounted_dir);
				ALOGD("---------> UnmountHDDExt4: (rm -rf) mounted_dir_buf [%s]", mounted_dir_buf);
				system(mounted_dir_buf);
			}

			flag = true;
		}
	}
	return flag;
}
