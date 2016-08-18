// hdd_ctrl.cpp

#include "hdd_ctrl.h"

#undef LOG_TAG
#define LOG_TAG	"HDD_CTRL"
#include "cutils/log.h"

//#undef ALOGD
//#define ALOGD(...)

#include <cutils/properties.h>
#include "flash_ts.h"

#include "HDDInfoStore.h"
#include "DetectHDDThread.h"
#include "HDDErrDetectThread.h"
#include "WaitUserThread.h"
#include "WifiStatusThread.h"
#include "MetaFileReadWriteThread.h"
#include "TestFileReadWriteThread.h"
#include "LogHelperThread.h"

bool thread_running_flag = false;


bool isCTSMode()
{
	const char* ctsFileName = "/data/lkn_home/cts_mode";
	return access(ctsFileName, 0) == 0;
}

int Dummyloop()
{
	while(1)
	{
		usleep(5000000);
	}

	return 0;
}

int main()
{
	if (isCTSMode())
	{
		return 0;//Dummyloop();
	}
	else
	{
		char volume_dev_buf[100]; 
		char format_mode_buf[100];

		memset(volume_dev_buf, 0, 100);
		sprintf(volume_dev_buf, "touch %sext4_volume_dev.txt", HDD_HOME);
		system(volume_dev_buf);
		memset(volume_dev_buf, 0, 100);
		sprintf(volume_dev_buf, "touch %snon_ext4_volume_dev.txt", HDD_HOME);
		system(volume_dev_buf);
		
		memset(fs_ext_str, 0, 5);
		memset(format_mode_buf, 0, 100);
		sprintf(format_mode_buf, "%sext2_mode", CUSTOMER_HOME);

		if (access(format_mode_buf, F_OK) >= 0)
		{
			fs_ext2_supported = true;
			strcpy(fs_ext_str, "ext2");
		}
		else
		{
			fs_ext2_supported = false;
			strcpy(fs_ext_str, "ext4");
		}

		/* doing in init.tvstorm.rc
		DIR             *dp;
		struct dirent   *dirp;
		if ((dp = opendir(MNT_MEDIA_DIR)) != NULL)
		{
			while ((dirp = readdir(dp)) != NULL)
			{
				if (strcmp(dirp->d_name, ".") == 0 ||
						strcmp(dirp->d_name, "..") == 0)
					continue;

				if (dirp->d_type == DT_DIR && !strncmp(dirp->d_name, "usb.", 4))
				{
					char temp_path_d_name_buf[100];
					memset(temp_path_d_name_buf, 0, 100);
					sprintf(temp_path_d_name_buf, "rmdir %s%s", MNT_MEDIA_DIR, dirp->d_name);
					ALOGD("=====(on running)=========> temp_path_d_name_buf[%s]", temp_path_d_name_buf);
					system(temp_path_d_name_buf);
				}
			}
		
			if (closedir(dp) < 0)
				ALOGE("can't close directory %s\n", MNT_MEDIA_DIR);
		}
		*/

		pDetectHDD = new DetectHDDThread; 
		pDetectHDD->run();

		pWaitUser = new WaitUserThread; 
		pWaitUser->run();

		pWifiStatus = new WifiStatusThread; 
		pWifiStatus->run();

		sp<MetaFileReadWriteThread> pMetaFileReadWrite = new MetaFileReadWriteThread;
		pMetaFileReadWrite->run();

#if 0
		sp<TestFileReadWriteThread> pTestFileReadWrite = new TestFileReadWriteThread;
		pTestFileReadWrite->run();
#endif

		sp<HDDErrDetectThread> pHDDErrDetect = new HDDErrDetectThread;
		pHDDErrDetect->run();

		sp<LogHelperThread> pLogHelper = new LogHelperThread;
		pLogHelper->run();

		pDetectHDD->join();
		pWaitUser->join();
		pWifiStatus->join();
		pMetaFileReadWrite->join();
		pHDDErrDetect->join();
		pLogHelper->join();
	}

	return 0;
}
