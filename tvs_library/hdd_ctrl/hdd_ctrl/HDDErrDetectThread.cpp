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
#define LOG_TAG "HDDErrDetectThread"

#include "HDDErrDetectThread.h"
#include "DetectHDDThread.h"

void HDDErrDetectThread::onFirstRef()
{
	//ALOGD("HDDErrDetectThread::onFirstRef()");
}

status_t HDDErrDetectThread::readyToRun()
{
	//ALOGD("HDDErrDetectThread::readyToRun()");
	return 0;
}

bool HDDErrDetectThread::threadLoop()
{
	//ALOGD("HDDErrDetectThread::threadLoop()");

	if (pDetectHDD->selectedHDDIndex == 0 || pDetectHDD->selectedHDDIndex == 1)
	{
		if (!on_formatting_flag)
		{
			char tmp_dev_name[10];
			memset(tmp_dev_name, 0, 10);
			strcpy(tmp_dev_name, pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].dev_name);
			if (pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].GPTFlag)
			{
				int dev_name_len = strlen(tmp_dev_name);
				tmp_dev_name[dev_name_len-1] = pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].dev_name[dev_name_len-1]+1;
			}
			char fsPath[50];
			memset(fsPath, 0, 50);
			sprintf(fsPath, "/dev/block/vold/%s", tmp_dev_name);
			if (IsHDDErr(fsPath, pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir))
			{
				if (!on_formatting_flag)
				{
					pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDErrFlag = true;
					pDetectHDD->sendHDDErrEvent(pDetectHDD->selectedHDDIndex);
				}
			}
			else
			{
				memset(fsPath, 0, 50);
				sprintf(fsPath, "/dev/%s", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.sdx_name);
				fsPath[strlen(fsPath)-1] = '\0';
				if (IsHDDErr(fsPath, pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir))
				{
					if (!on_formatting_flag)
					{
						pDetectHDD->sendHDDErrEvent(pDetectHDD->selectedHDDIndex);
					}
				}
			}
		}
	}
	sleep(1);

	return true;
}

bool HDDErrDetectThread::IsHDDErr(const char *fsPath, const char *mountPoint)
{
	char device[256];
	char mount_path[256];
	char fs_type[256];
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
		sscanf(line, "%255s %255s %255s %255s\n", device, mount_path, fs_type, rest);
		if (!strcmp(device, fsPath) && !strcmp(mount_path, mountPoint))
		{
			fclose(fp);
			if (!strncmp(rest, "ro", 2))
			{
				ALOGD("=========> IsHDDErr(): fsPath[%s], mountPoint[%s], fsType[%s], rest[%s], return true", fsPath, mountPoint, fs_type, rest);
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	fclose(fp);
	return false;
}
