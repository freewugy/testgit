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
#define LOG_TAG "LogHelperThread"

#include <mtd/mtd-user.h>
#include "LogHelperThread.h"

bool LogHelperThread::mRunning = false;

static char *cmd_front_usb[] = {"logcat", "-f", "/mnt/media_rw/sdcard1/log/main_log.log", "-v",
							"threadtime", "-r", "20000", "-n", "200" };
static char *cmd_rear_usb[] = {"logcat", "-f", "/mnt/media_rw/usbdiskb/log/main_log.log", "-v",
							"threadtime", "-r", "20000", "-n", "200" };

void LogHelperThread::onFirstRef()
{
	if(logDebug) ALOGD("LogHelperThread::onFirstRef()");
}

status_t LogHelperThread::readyToRun()
{
	if(logDebug) ALOGD("LogHelperThread::readyToRun()");
	mPosition = hdd_none;
	mRunning = true;
	mCount = 0;
	return 0;
}

void LogHelperThread::setPath(HDDPosition position){
	int len;

	if(logDebug)  ALOGD("LogHelperThread::setPath : %d", position);
	if(position == front){
		char front_usb_path[]="sdcard1";
		len = strlen(front_usb_path);
		path = new char[len+1];
		strcpy(path, front_usb_path);
	}else{
		char rear_usb_path[]="usbdiskb";
		len = strlen(rear_usb_path);
		path = new char[len+1];
		strcpy(path, rear_usb_path);
	}
	if(logDebug)  ALOGD("LogHelperThread::setPath len : %d", len);
}

char* LogHelperThread::getPath(){
	if(path !=NULL){
		if(logDebug) ALOGD("LogHelperThread::getPath : %s", path);
		return path;
	}
	else{
		if(logDebug) ALOGD("LogHelperThread::getPath : NULL");
		return NULL;
	}
}

void LogHelperThread::deinit(){
	if(path != NULL){
		if(logDebug) ALOGD("LogHelperThread::destroy path : %s", path);
		delete[] path;
		path = NULL;
	}
}

void LogHelperThread::stopLogService(void)
{
    mRunning = false;
#if USE_DUMP_KERNEL_OOPS
	/* Stop klocat service */
	property_set("tvstorm.klogcat.enable","off");
#endif
}

int logcat_main(int argc, char **argv);

int LogHelperThread::erase_mtd9(void)
{
    int fd;
    unsigned int end_offset;
    mtd_info_t mtd_info;
    erase_info_t ei;

    ALOGI("mtd_erase \n");

    fd = open("/dev/mtd/mtd9", O_RDWR | O_SYNC);
    if(fd < 0) return -1;

    ioctl(fd, MEMGETINFO, &mtd_info);
    //LOGI("mtd: erase size 0x%08x(%d) \n", mtd_info.erasesize, mtd_info.erasesize);

    ei.start = 0;
    end_offset = ei.start + mtd_info.size;

    ei.length = mtd_info.erasesize;

    ALOGI("ei.start %x, ei.length %x, mtd_info.size %x\n", ei.start, ei.length, mtd_info.size);

    for(; ei.start < mtd_info.size; ei.start += mtd_info.erasesize) 
    {
        ioctl(fd, MEMUNLOCK, &ei);
        if (ioctl(fd, MEMERASE, &ei) < 0) {
            ALOGE("mtd: erase failure at 0x%08x (%s)\n",
                    ei.start, strerror(errno));
            return -2;
        }

        ALOGI("mtd: erase 0x%08x bytes at 0x%08x \n", mtd_info.erasesize, ei.start);
    }

    close(fd);

    return 0;
}

void* thread_logcat_kmsg(void* arg)
{
    char kmsg[2048], line[2048];

    FILE *f = fopen("/proc/kmsg", "r");
    if (!f) {
        ALOGE("Could not open /proc/kmsg : %s\n", strerror(errno));
        return NULL;
    }

#if 0
    FILE *logcat = fopen("/dev/log/system", "w");
    if (!logcat) {
        ALOGE("Could not open /dev/log/system : %s\n", strerror(errno));
        return NULL;
    }
#endif

    while (fgets(kmsg, 2048, f)) {
        __android_log_print(ANDROID_LOG_DEBUG, "kernel", kmsg);
    }

    return NULL;
}

void LogHelperThread::startLogService(HDDPosition position){
    char s1[] = "/data/anr/traces.txt";
    char command[256]; 
    setPath(position);

    pvr_backup();
    if(logDebug) ALOGD("LogHelperThread startLogcatService...");

    /* move old log file to oldLog folder */
    memset(command, 0x00, sizeof(command));
    sprintf(command, "rm -rf /mnt/media_rw/%s/oldLog", getPath());
    if(logDebug) ALOGD("LogHelperThread:: %s - cmd : %s", __FUNCTION__, command);
    system(command);

	memset(command, 0x00, sizeof(command));
    sprintf(command, "mv /mnt/media_rw/%s/log /mnt/media_rw/%s/oldLog", getPath(), getPath());
    if(logDebug) ALOGD("LogHelperThread:: %s - cmd : %s", __FUNCTION__, command);
    system(command);

    memset(command, 0x00, sizeof(command));
    sprintf(command, "mkdir /mnt/media_rw/%s/log", getPath());
    if(logDebug) ALOGD("LogHelperThread:: %s - cmd : %s", __FUNCTION__, command);
    system(command);

	memset(command, 0x00, sizeof(command));
    sprintf(command, "mv /mnt/media_rw/%s/oldLog/log_mode /mnt/media_rw/%s/log", getPath(), getPath());
    if(logDebug) ALOGD("LogHelperThread:: %s - cmd : %s", __FUNCTION__, command); 
    system(command);

    /* Copy traces.txt */
    if(access(s1, F_OK) >= 0){ 
	    memset(command, 0x00, sizeof(command));
        sprintf(command, "cp -f /data/anr/* /mnt/media_rw/%s/oldLog/", getPath());
        system(command);
    }   

    pthread_t logcat_kmsg_thread;
	pthread_create(&logcat_kmsg_thread, NULL, &thread_logcat_kmsg, (void *)NULL);

#if USE_DUMP_KERNEL_OOPS
    /* Copy kernel oops */
	memset(command, 0x00, sizeof(command));
    //sprintf(command, "/system/bin/oopslog /dev/mtd/mtd9ro > /mnt/media_rw/%s/log/oops.log", getPath(), getPath());
    sprintf(command, "/system/bin/dd if=/dev/mtd/mtd9ro of=/mnt/media_rw/%s/log/oops.dump", getPath(), getPath());
    if(logDebug) ALOGD("LogHelperThread:: %s - cmd : %s", __FUNCTION__, command); 
    system(command);
    erase_mtd9();
    system("sync");
#endif

    if(isLogfile() == false){
         memset(command, 0x00, sizeof(command));
         sprintf(command, "rm -rf /mnt/media_rw/%s/oldLog", getPath());
         if(logDebug) ALOGD("LogHelperThread:: %s - cmd : %s because there is no log file in oldLog folder.", __FUNCTION__, command);
         system(command);
    }   

#if USE_DUMP_KERNEL_OOPS
	/* start klogcat service */
	property_set("tvstorm.klogcat.enable","on");
#endif

    /* start logcat */
    if(logDebug) ALOGD("LogHelperThread:: %s - start logcat[Position : %d]", __FUNCTION__, mPosition);

	if(mPosition == front){
    	logcat_main(9, cmd_front_usb);
	}else if(mPosition == rear){
		logcat_main(9, cmd_rear_usb);
	}
}

bool LogHelperThread::isLogfile(){
    DIR *dp;
    struct dirent *dirp;
    char buf_dir[256];
    bool isFindFile = false;

    memset(buf_dir, 0, 256);
    sprintf(buf_dir, "/mnt/media_rw/%s/oldLog", getPath());

    if((dp = opendir(buf_dir)) != NULL){
	    while ((dirp = readdir(dp)) != NULL){
	        if (strcmp(dirp->d_name, ".") != 0 && 
                strcmp(dirp->d_name, "..") != 0){
                isFindFile = true;
            }
		}

        if (closedir(dp) < 0)
	        if(logDebug) ALOGD("closedir is failed....");
	}

    return isFindFile;
}

void LogHelperThread::pvr_backup(){
	char s1[]="/data/data/com.linknet.service.tv/databases/ServiceManager.db";
	char s2[]="/data/lkn_home/config/pvr.error";

	if(logDebug) ALOGD("LogHelperThread %s ", __FUNCTION__);

	if(access(s1, F_OK) >=0){
		char command1[200];
		char command2[150];
		time_t currentTime;

		memset(command1, 0x00, sizeof(command1));
		memset(command2, 0x00, sizeof(command2));

		time(&currentTime);
		sprintf(command1, "cp -f %s /mnt/media_rw/%s/pvr/ServiceMgr_%s.db", s1, getPath(), ctime(&currentTime));

		if(logDebug) ALOGD("LogHelperThread:: %s - cmd : %s", __FUNCTION__, command1);
		system(command1);

		sprintf(command2, "cp -f %s /mnt/media_rw/%s/pvr/pvr_%s.error", s2, getPath(), ctime(&currentTime));
		if(logDebug) ALOGD("LogHelperThread:: %s - cmd : %s", __FUNCTION__, command2);
		system(command2);
	}

	if(logDebug) ALOGD("LogHelperThread %s End ", __FUNCTION__);
}

bool LogHelperThread::isRunning(){
	return mRunning;
}

void LogHelperThread::setRunning(bool running){
	mRunning = running;
}

bool LogHelperThread::threadLoop()
{
	if(logDebug) ALOGD("LogHelperThread::threadLoop()");

	while(isRunning()){
		/* File check */
		char USB_FRONT[] = "/mnt/media_rw/sdcard1/log/log_mode";
		char USB_REAR[] ="/mnt/media_rw/usbdiskb/log/log_mode";
		if(access(USB_FRONT, F_OK) >= 0){
			mPosition = front;
		}
		else if(access(USB_REAR, F_OK) >= 0){
			mPosition = rear;
		}
		if(logDebug) ALOGD("LogHelperThread mPosition : %d , count : %d", mPosition, mCount);
		if(mPosition != hdd_none){
			startLogService(mPosition);
			setRunning(false);
		}

		sleep(this->sInterval);

		if(mCount > this->sRetryCount){
			setRunning(false);
		}
		mCount++;
	}

	deinit();
	return false;
}
