/******************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: $ cssong
 * $LastChangedDate: $ 2015.07.04
 * $LastChangedRevision: $ V1.2.6
 * Description: Hidraw controler
 * Note: 
 *         V1.1.0 
 *                   add RCU Un-paring request protocol
 *         V1.2.0 
 *                   add RCU RCU Battery level Request protocol
 *         V1.2.1 
 *                   logcat
 *                   time to modify RCU Version and RCU battery level coming
 *         V1.2.2 
 *                   fix - Version Information call after unparing
 *         V1.2.3
 *                   add property tvstorm.bt.rcu.status and verion and battery request time
 *         V1.2.4
 *                   remove property tvstorm.bt.rcu.status
 *         V1.2.5
 *                   fix - Voice search Exceptions - back, home, power key
 *         V1.2.6
 *                   add voice property - Voice search status is tvstorm.bt.rcu.voice.status
 *****************************************************************************/
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>		// O_WRONLY
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>	// write(), close()

#include <cutils/properties.h>

#include <time.h>		//time


#include <sys/poll.h>	//poll
#include <pthread.h>	//thread

#include <sys/inotify.h>	//inotify 
#include <errno.h>

#include "csr_adpcm.h"
#include "hidraw_ctrl.h"

#include <linux/input.h>	//uinput
#include <linux/uinput.h>	// uinput

#include "cutils/log.h"	//logcat 

#undef LOG_TAG
#define LOG_TAG	"HIDRAW"


/* Debug cfg */
#define HIDRAW_DEBUG	0

//#define SUPPORT_TVSTORM_HIDRAWCTRL_DEBUG
#ifndef SUPPORT_TVSTORM_HIDRAWCTRL_DEBUG
//#undef ALOGE
//#define ALOGE(...)
#undef ALOGD
#define ALOGD(...)
//#undef ALOGI
//#define ALOGI(...)
#endif

/* dump audio file */
#define AUDIO_FILE_DUMP 0
#if AUDIO_FILE_DUMP
static const char *AUDIO_DUMP_PATH  = "/data/pcm/";   //"/mnt/media/usb.5A51-1CDF/pcm/";
static const char *AUDIO_DUMP_NAME = "acapture";
static const char *AUDIO_DUMP_COMMA = ".";
static const char *AUDIO_DUMP_PCM_EXTENSION = "pcm";
static const char *AUDIO_DUMP_ADPCM_EXTENSION = "adpcm";
#endif

// rcu version try counter
#define PREMIUM_VERSION_TRY_COUNT 1
#define PREMIUM_VERSION_MAX_COUNT 2

#define PREMIUM_REQUEST_WAIT_TIME 2000
#define PREMIUM_REQUEST_WAIT_MAX_TIME 3000

//snd_bt ioctl
#define SND_BT_IOCTL_CREATE_DEV  0x100
#define SND_BT_IOCTL_DESTORY_DEV 0x110

//Voice
#define VOICE_RAW_DATA_LEN 21
#define VOICE_OPEN_CMD 1
#define VOICE_CLOSE_CMD 0


//Premium RCU info
#define PREMIUM_BUS_TYPE BUS_BLUETOOTH//BUS_USB
#define PREMIUM_VENDOR_ID 0x00c4//0x043e 
#define PREMIUM_PRODUCT_ID 0x7a44

#define PROPERTY_PREMIUM_VERSION "tvstorm.bt.rcu.version"
#define PROPERTY_PREMIUM_BATTEL_LEVEL "tvstorm.bt.rcu.level"
#define PROPERTY_PREMIUM_STATUS "tvstorm.bt.rcu.status"

// Voice open/close - Keyevent exception of BT RCU
#define PROPERTY_PREMIUM_VOICE_STATUS "tvstorm.bt.rcu.voice.status"

#define PREMIUM_RCU_VERSION_REPORTID 0x3
#define PREMIUM_RCU_VERSION_HEADER 0x5c

// Battery Reportid
#define PREMIUM_RCU_BATTERY_REPORTID 0x4

// uinput
#define TVSTORM_UINPUT_DEVICE_PATH		"/dev/uinput"
#define PREMIUM_RCU_UNPARING_REQUEST_REPORTID 0x3
#define PREMIUM_RCU_UNPARING_REQUEST_DATA01 0xB
#define PREMIUM_RCU_UNPARING_REQUEST_DATA02 0x0

// dummy - capture stop workground
#define DUMMY_DATA_RETRY_MAX 409 // 3 * 1024 / 80

static int fd_hidraw = -1;
static int fd_soundcard = -1;

static int fd_hidraw_inotify = -1;
static int fd_hidraw_wd = -1;

// voice cmd status
static char voc_status = -1;
	  
//Driver name
static const char *SND_DEVICE_NAME = "/dev/snd_bt";
static const char *HIDRAW_DEVICE_PREFIXNAME = "/dev/hidraw";

//inotify /dev/hidrawXX
static const char *HIDRAW_PREFIXNAME_NAME = "hidraw";
static const char *HIDRAW_INOTIFY_NAME = "/dev";
static const char *HIDRAW_INOTIFY_PREFIXNAME = "/dev/";

//The OPEN/CLOSE command packet content: (Report ID from GTV is 0x78 as below)
static const unsigned char mrvl_mic_open[6] = {0x78, 0x00, 0x00, 0x00, 0x00, 0x01};
static const unsigned char mrvl_mic_close[6] = {0x78, 0x00, 0x00, 0x00, 0x00, 0x00};

// RCU Version request
static const unsigned char mrvl_version_request[6] = {0x78, 0x00, 0x00, 0x00, 0x00, 0x61};

// RCU Battery level request
static const unsigned char mrvl_battery_level_request[6] = {0x78, 0x00, 0x00, 0x00, 0x00, 0x62};


// Consumer(Remote) key data protocol
static const unsigned char cusumer_back_key[3] = {0x3, 0x24, 0x2};
static const unsigned char cusumer_home_key[3] = {0x3, 0x23, 0x2};
static const unsigned char cusumer_power_key[3] = {0x3, 0x32, 0x2};


//snd_bt poll
pthread_t p_thread = 0;
int snd_th_id;

enum{
	CAPTURE_IDLE_STATE 		= 0,
	CAPTURE_START_STATE 	= 1,
	CAPTURE_STOP_STATE 	= 2,
};

// status 0 is noting, 1 is capture start, 2 is capture stop
static  int capture_card_state = CAPTURE_IDLE_STATE;

//hid reveiver status
//hidraw driver name
static char  hidraw_devname[128];
static  int voice_cmd_status = CAPTURE_IDLE_STATE;

// battery level
static  int battery_level = 0;

//hidraw thread
pthread_t hidraw_thread = 0;
int hidraw_th_id;

//hidraw battery level thread
pthread_t battery_thread = 0;
int battery_th_id;

//ADPCM first packet
int bFirstPacket = 1;


// battery level value
int rlvl = -1, orlvl = -1;

// version value
int rver = -1;
int rcnt = -1;

// uinput
int uinput_fd = -1;

// timer
unsigned int wait_time = 0, curr_time = 0;

const char * bus_str(int bus)
{
	switch (bus) {
	case BUS_USB:
		return "USB";
		break;
	case BUS_HIL:
		return "HIL";
		break;
	case BUS_BLUETOOTH:
		return "Bluetooth";
		break;
	case BUS_VIRTUAL:
		return "Virtual";
		break;
	default:
		return "Other";
		break;
	}
}
unsigned int GetTickCount(void)
{
    struct timeval gettick;
    unsigned int tick;
    int ret;
    gettimeofday(&gettick, NULL);

    tick = gettick.tv_sec*1000 + gettick.tv_usec/1000;

    return tick;
}


//hidraw receive voice data
static int  hidraw_thread_open(void)
{
	//card detect thread create
	hidraw_th_id = pthread_create(&hidraw_thread, NULL, (void *)hidraw_receiver, (void*)&voice_cmd_status);

	ALOGD("thread create  hidraw_th_id %d, hidraw_thread %d\n", hidraw_th_id, hidraw_thread);
	if (hidraw_th_id < 0){
		ALOGE("thread create error : %d\n", hidraw_th_id);
		return -EBADF;
	}
	else{
		ALOGI("create hidraw thread\n");
	}
		
	return 0;
}


static void  hidraw_thread_close(void)
{
	ALOGI("%s, close \n", __func__);
	ALOGD("close hidraw thread\n");
	pthread_detach(hidraw_thread);
	hidraw_th_id = -1;
}


//snd_bt poll
static int  snd_bt_card_capture_detecter_open(void)
{
	//card detect thread create
	snd_th_id = pthread_create(&p_thread, NULL, (void *)snd_bt_card_capture_detect, (void*)&capture_card_state);

	ALOGD("thread create  snd_th_id %d, p_thread %d\n", snd_th_id, p_thread);
	if (snd_th_id < 0){
		ALOGE("thread create error : %d\n", snd_th_id);
		return -EBADF;
	}
	else{
		ALOGI("open capture detecter\n");
	}
		
	return 0;
}


static void  snd_bt_card_capture_detecter_close(void)
{
	ALOGD("%s, close \n", __func__);
	ALOGI("close capture detecter\n");
	pthread_detach(p_thread);
	snd_th_id = -1;
}

/*
* snd driver open
*/
static int soundcard_open(int _mode)
{
	int ret;
	char data = 0;
	char *snd_devname = SND_DEVICE_NAME;
	
	fd_soundcard = open(snd_devname, _mode);

	if (fd_soundcard < 0) {
		//handle_error (errno);
		ALOGE("Unable to open device %s\n", SND_DEVICE_NAME);
	}

	ret = ioctl(fd_soundcard, SND_BT_IOCTL_CREATE_DEV, &data);
	if(ret == 0){
		ALOGI("open device snd_bt\n");
		ALOGD("create snd_bt device : write ret : %d, data : %d\n", ret, data);
	}else{
		ALOGE("create snd_bt device faile!!%d\n", ret);
	}
	
	return fd_soundcard;
}

/*
* snd driver close
*/
static void soundcard_close(void)
{
	int ret;
	char data = 0;

	ret = ioctl(fd_soundcard, SND_BT_IOCTL_DESTORY_DEV, &data);
	if(ret == 0){
		ALOGI("close device snd_bt\n");
		ALOGD("destory snd_bt device : write ret : %d, data : %d\n", ret, data);
	}else{
		ALOGE("destory snd_bt device faile!!%d\n", ret);
	}

	close(fd_soundcard);

	fd_soundcard = -1;
}


/*
* hidraw inotify open
*/
static int hidraw_inotify_open(int _mode)
{
	int ret;

	fd_hidraw_inotify = inotify_init();
	if (fd_hidraw_inotify < 0) {
		//handle_error (errno);
		ALOGE("Unable to open hidraw inotify\n");
	}

	fd_hidraw_wd = inotify_add_watch (fd_hidraw_inotify, HIDRAW_INOTIFY_NAME, _mode);
	if (fd_hidraw_wd < 0) {
		//handle_error (errno);
		close(fd_hidraw_inotify);
		fd_hidraw_inotify = -1;
		ALOGE("add hidraw inotify faile!\n");
	}
   
	ALOGI("open device hidraw inotify\n");
	return  fd_hidraw_inotify;
}

/*
* hidraw inotify close
*/
static void hidraw_inotify_close(void)
{
	ALOGI("close device  hidraw inotify\n");
	inotify_rm_watch( fd_hidraw_inotify, fd_hidraw_wd );
	close(fd_hidraw_inotify);
	fd_hidraw_inotify = -1;
}


/*
* uinput driver open
*/
static int virtual_uinput_open(void)
{
	int ret;
	struct uinput_user_dev uinput;
	
	uinput_fd = open(TVSTORM_UINPUT_DEVICE_PATH, O_WRONLY| O_NDELAY);

	if (uinput_fd < 0) {
		//handle_error (errno);
		ALOGE("Unable to open device %s\n", TVSTORM_UINPUT_DEVICE_PATH);
	}

	memset(&uinput,0, sizeof(uinput)); // Initialize the uInput device to NULL
	strncpy(uinput.name, "BtButton", UINPUT_MAX_NAME_SIZE);
	uinput.id.version = 1;
	uinput.id.bustype = BUS_VIRTUAL;
	uinput.id.product = 0;
	uinput.id.vendor = 0;
	ioctl(uinput_fd, UI_SET_PHYS, "BtButton");
	ioctl(uinput_fd, UI_SET_EVBIT, EV_KEY);
	ioctl(uinput_fd, UI_SET_KEYBIT, KEY_PC);
	write(uinput_fd, &uinput, sizeof(uinput));

      ret = ioctl(uinput_fd, UI_DEV_CREATE);
	if(ret == 0){
		ALOGE("unable to create UINPUT device. \n");
	}else{
		ALOGE("create UINPUT device faile!!%d\n", ret);
	}
	
	return uinput_fd;
}

/*
* uinput driver close
*/
static void virtual_uinput_close(void)
{
	int ret;

	ret = ioctl(uinput_fd, UI_DEV_DESTROY);
	if(ret == 0){
		ALOGI("close device UINPUT\n");
	}else{
		ALOGE("destory UINPUT device faile!!%d\n", ret);
	}

	if(uinput_fd != -1){
		close(uinput_fd);
	}

	uinput_fd = -1;
}

/*
* uinput driver close
*/
int virtual_uinput_send_event(uint16_t type, uint16_t code, int32_t value)
{
	struct input_event event;
	struct timeval tv;
	int ret =0;

	memset(&event, 0, sizeof(event));

	gettimeofday(&tv, NULL);
	event.type = type;
	event.code = code;
	event.value = value;
	event.time = tv;

	ret = write(uinput_fd, &event, sizeof(event));
	ALOGD("%s : type=%04x code=%04x, value=%04x, ret=%d\n", __FUNCTION__, type, code, value, ret);

	return ret;
}

/*
* hidraw driver open
*/
static int hidraw_open(const char *_dname, int _mode)
{
	int res, i;
	int fd_hid = -1;
	struct hidraw_devinfo info;
#if HIDRAW_DEBUG	
	struct hidraw_report_descriptor rpt_desc;
	int desc_size = 0;
	unsigned char buf[256];
#endif
	
	memset(&info, 0x0, sizeof(info));

	fd_hid = open(_dname, _mode);
	if (fd_hid < 0) {
		//handle_error (errno);
		ALOGD("Unable to open device %s\n", _dname);
		return (-1);
	}

	/* Get Raw Info */
	res = ioctl(fd_hid, HIDIOCGRAWINFO, &info);

	if (res < 0) {
		close(fd_hid);
#if HIDRAW_DEBUG	
		ALOGD("HIDIOCGRAWINFO faile");
#endif
		return (-2);
	} else {
		if(info.bustype == PREMIUM_BUS_TYPE 
		&& info.vendor == PREMIUM_VENDOR_ID 
		&& info.product == PREMIUM_PRODUCT_ID){
		ALOGD("open device %s\n", _dname);

#if HIDRAW_DEBUG	
			ALOGD("Raw Info:\n");
			ALOGD("\tbustype: %d (%s)\n",
				info.bustype, bus_str(info.bustype));
			ALOGD("\tvendor: 0x%04hx\n", info.vendor);
			ALOGD("\tproduct: 0x%04hx\n", info.product);

			memset(buf, 0x0, sizeof(buf));
			memset(&rpt_desc, 0x0, sizeof(rpt_desc));

			/* Get Report Descriptor Size */
			res = ioctl(fd_hid, HIDIOCGRDESCSIZE, &desc_size);
			if (res < 0)
				ALOGD("HIDIOCGRDESCSIZE faile");
			else
				ALOGD("Report Descriptor Size: %d\n", desc_size);

			/* Get Report Descriptor */
			rpt_desc.size = desc_size;
			res = ioctl(fd_hid, HIDIOCGRDESC, &rpt_desc);
			if (res < 0) {
				ALOGD("HIDIOCGRDESC faile");
			} else {
				ALOGD("Report Descriptor:\n");
				for (i = 0; i < rpt_desc.size; i++)
					ALOGD("%hhx ", rpt_desc.value[i]);
			}

			/* Get Raw Name */
			res = ioctl(fd_hid, HIDIOCGRAWNAME(256), buf);
			if (res < 0)
				ALOGD("HIDIOCGRAWNAME faile");
			else
				ALOGD("Raw Name: %s\n", buf);

			/* Get Physical Location */
			res = ioctl(fd_hid, HIDIOCGRAWPHYS(256), buf);
			if (res < 0)
				ALOGD("HIDIOCGRAWPHYS faile");
			else
				ALOGD("Raw Phys: %s\n", buf);

#endif
			fd_hidraw = fd_hid;
			memset(hidraw_devname, 0x0, sizeof(hidraw_devname));
			strncpy(hidraw_devname, _dname, strlen(_dname));

			//captuer card state init
			capture_card_state = CAPTURE_IDLE_STATE;
			voice_cmd_status = CAPTURE_IDLE_STATE;

			//Rcu ready
//			__system_property_set(PROPERTY_PREMIUM_STATUS, "on");	//default setting
//			ALOGI("%s : %s\n", PROPERTY_PREMIUM_STATUS, "on");

                   //Rcu voice status
                   voc_status = -1;
                   __system_property_set(PROPERTY_PREMIUM_VOICE_STATUS, "0");	//default voice off setting
                   ALOGI("%s : %s\n", PROPERTY_PREMIUM_VOICE_STATUS, "off");
			
			wait_time = GetTickCount();
			curr_time = 0;

			//get version
			rcnt = 0;
			rver = -1;
			__system_property_set(PROPERTY_PREMIUM_VERSION, "00");		//default setting

			//get battery level
			rlvl = orlvl = -1;
			__system_property_set(PROPERTY_PREMIUM_BATTEL_LEVEL, "100");	//default setting
			ALOGI("hid device  foune %s\n", hidraw_devname);
		}
		else{
			return (-4);
		}
	}

	return (fd_hid);
}

/*
* hidraw driver close
*/
static void hidraw_close(void)
{
	ALOGI("close device %s\n", hidraw_devname);
	memset(hidraw_devname, 0x0, sizeof(hidraw_devname));
	close(fd_hidraw);
	fd_hidraw = -1;

	// init battery level
	rlvl = -1, orlvl = -1;
	__system_property_set(PROPERTY_PREMIUM_BATTEL_LEVEL, "0");
	// init version
	rver = -1;
	__system_property_set(PROPERTY_PREMIUM_VERSION, "00");
//	__system_property_set(PROPERTY_PREMIUM_STATUS, "off");
//	ALOGI("%s : %s\n", PROPERTY_PREMIUM_STATUS, "off");

      //Rcu voice status
      voc_status = -1;
      __system_property_set(PROPERTY_PREMIUM_VOICE_STATUS, "0");	//default voice off setting
      ALOGI("%s : %s\n", PROPERTY_PREMIUM_VOICE_STATUS, "off");
}

/*
* input cmd : 1 is open, 0 is close
*/
int Voice_OpenClose_write(int fd, unsigned char cmd)
{
	int res = -1;
	int i; 
	unsigned char *buff;

	if(fd < 0){
		ALOGE("hidraw write fd is null!!\n");
		return -2;
	}

      if(voc_status ==  cmd ){
          ALOGD("hidraw cmd reject [property %s, cmd %s] !!", voc_status==1?"open":"close", cmd==1?"open":"close");
          return 0;
      }

	/* Send a Report to the Device */
	if(cmd == 1){
		buff = mrvl_mic_open;
             //Rcu voice status
             __system_property_set(PROPERTY_PREMIUM_VOICE_STATUS, "1");	//default voice off setting
             ALOGI("%s : %s\n", PROPERTY_PREMIUM_VOICE_STATUS, "on");
	}
	else{
		buff =  mrvl_mic_close;
             //Rcu voice status
             __system_property_set(PROPERTY_PREMIUM_VOICE_STATUS, "0");	//default voice off setting
             ALOGI("%s : %s\n", PROPERTY_PREMIUM_VOICE_STATUS, "off");
	}
	res = write(fd, buff, 6);

#if HIDRAW_DEBUG
	printf("Voice %s ", cmd==0?"Close":"Open");
	for(i=0; i< 6; i++){
		printf("0x%x ", buff[i]);
	}

	if (res <= 0) {
		printf("\nwrite faile!\n");
	} else {
		printf("write() wrote %d bytes\n", res);
	}
#endif
	voc_status = cmd;

	return res;
}

/*
* input cmd : 1 is open, 0 is close
*         rdata : point of read buffer
*/
int Voice_raw_data_read(int fd, unsigned char *rdata)
{
	int res = -1;
	int i;

	if(fd < 0){
		ALOGE("hidraw read fd is null!!\n");
		return -2;
	}

	if(rdata == NULL){
		ALOGE("read buff is null!!\n");
		return -3;
	}
	
	/* Get a report from the device */
	res = read(fd, rdata, VOICE_RAW_DATA_LEN);

#if HIDRAW_DEBUG
	printf("Voice read ");
	if (res <= 0) {
		printf("read faile!\n");
	} else {
		printf("read() read %d bytes:\n\t", res);
		for (i = 0; i < res; i++)
			printf("%hhx ", rdata[i]);
	}
#endif

	return res;
}

int hidraw_request_Rcu_Version(int fd)
{
	int res = -1;
	unsigned char *buff;

	if(fd < 0){
		ALOGE("%s : hidraw read fd is null!!\n", __FUNCTION__);
		return -2;
	}

	/* Send a Report to the Device */
	buff = mrvl_version_request;

	res = write(fd, buff, 6);

	return res;
}

int hidraw_request_Rcu_BatteryLevel(int fd)
{
	int res = -1;
	unsigned char *buff;

	if(fd < 0){
		ALOGE("%s : hidraw read fd is null!!\n", __FUNCTION__);
		return -2;
	}

	/* Send a Report to the Device */
	buff = mrvl_battery_level_request;

	res = write(fd, buff, 6);

	return res;
}


int hidraw_ctrl_init(void)
{
	char  hidr_name[128];
	int i;

	// init property
	__system_property_set(PROPERTY_PREMIUM_BATTEL_LEVEL, "0");
	__system_property_set(PROPERTY_PREMIUM_VERSION, "00");	
//	__system_property_set(PROPERTY_PREMIUM_STATUS, "off");
//	ALOGI("%s : %s\n", PROPERTY_PREMIUM_STATUS, "off");
       //Rcu voice status
       voc_status = -1;
       __system_property_set(PROPERTY_PREMIUM_VOICE_STATUS, "0");	//default voice off setting
       ALOGI("%s : %s\n", PROPERTY_PREMIUM_VOICE_STATUS, "off");

	//virtual uinput open
	if(virtual_uinput_open() < 0){
		return -1;
	}
	
	//snd_bt open
	if(soundcard_open(O_RDWR) < 0){
		return -2;
	}

	//snd_bt poll
	if(snd_bt_card_capture_detecter_open() < 0){
		return -3;
	}

	//hidraw inotify
	if(hidraw_inotify_open(/*IN_ALL_EVENTS*/ IN_CREATE | IN_DELETE) < 0){
		return -4;
	}

	//hidraw receive thread open
	if(hidraw_thread_open() < 0){	
		return -5;
	}			

	//first time hidraw load
	memset(hidraw_devname, 0x0, sizeof(hidraw_devname));
	for(i=0; i<HIDRAW_MAX_DEVICES; i++){
		memset(hidr_name, 0x0, sizeof(hidr_name));
		sprintf(hidr_name, "%s%d", HIDRAW_DEVICE_PREFIXNAME, i);
		if(hidraw_open(hidr_name, O_RDWR | O_NONBLOCK) > 0){
			break;
		}
	}

	return 0;
}

void hidraw_ctrl_Terminate(void)
{
	hidraw_close();
	snd_bt_card_capture_detecter_close();
	soundcard_close();
	virtual_uinput_close();
	hidraw_inotify_close();
	hidraw_thread_close();
}


int Adpcm_to_Pcm(unsigned char* adpcmData, int adpcmDataSize, signed short* pcmData, unsigned int *pcmDataSize)
{
	//ADPCM PCM
	unsigned short firtPcmData;
	
	if(adpcmData == NULL || pcmData == NULL){
		ALOGE("Adpcm or pcm is null !!\n");
		return -1;
	}
	
	if(bFirstPacket == 1)
	{
		ALOGD("/nAdpcmDataHandler()::bFirstPacket\n");
		bFirstPacket = 0;
		firtPcmData = (((unsigned short)adpcmData[0]) << 8) | adpcmData[1];

		InitAdpcmDecoder();
		AdpcmSetFirstSample(firtPcmData);
		AdpcmDecode(&adpcmData[2], adpcmDataSize -2, pcmData, pcmDataSize);
		ALOGD("/nfirst pcmDataSize=%d, adpcmDataSize=%d\n",*pcmDataSize, adpcmDataSize);
	}else{
		AdpcmDecode(adpcmData, adpcmDataSize, pcmData, pcmDataSize);
		ALOGD("/npcmDataSize=%d, adpcmDataSize=%d\n", *pcmDataSize, adpcmDataSize);
	}

	return *pcmDataSize;
}

//snd_bt poll
void snd_bt_card_capture_detect(void * data)
{
	int ret=0;
	unsigned int *card_status = (unsigned int *)data;
	
	struct pollfd     poll_events;      // this struct have event infromation
	int    poll_state;

	poll_events.fd        = fd_soundcard;
	poll_events.events    = POLLIN | POLLERR | POLLRDNORM;     //check input
	poll_events.revents   = 0;			

	while ( 1)
	{
		//usleep(10000);	//10 msec
		
	      poll_state = poll(
		  			(struct pollfd*)&poll_events,
										1,  // check nubmer - pollfd 
										100000   // time out
	                       );
		if(poll_state <= 0){
			continue;
		}
		else                             // have a event
		{
			if ( poll_events.revents & (POLLIN | POLLRDNORM))     // input event
			{
				//MANDATORY_PRINT("capture detect event on : capture on capture_card_state = %d, voice_cmd_status = %d !!!\n", *card_status, voice_cmd_status);
				if(*card_status == CAPTURE_STOP_STATE || *card_status == CAPTURE_IDLE_STATE){
					*card_status = CAPTURE_START_STATE;
					ALOGD("capture start : capture_card_state = %d, voice_cmd_status = %d !!!\n", *card_status, voice_cmd_status);
				}
				else if(*card_status == CAPTURE_START_STATE){
					*card_status = CAPTURE_STOP_STATE;
					ALOGD("capture end : capture_card_state = %d, voice_cmd_status = %d !!!\n", *card_status, voice_cmd_status);
				}
			}
		}
	}	

}


//hidraw receive voice data
//snd_bt poll
void hidraw_receiver(void * data)
{
//	unsigned int *card_status = (unsigned int *)data;
	int i, j, res, ret;
	unsigned char buf[256];

	unsigned int *voice_status = (unsigned int *)data;

	//ADPCM PCM
	unsigned int pcmDataSize;
	unsigned short firtPcmData;
	short pPcmData[256];

	//time
	unsigned int current_time = 0, temp_time = 0, during_time = 0;

	// battery level
	unsigned char rlevel[16] = {0,};

#if AUDIO_FILE_DUMP
	//file write
	char  adump_name[256];
	static int anum = 0;
	FILE *write_pcm_ptr, *write_adpcm_ptr;
#endif

	ALOGD("hidraw_receiver Start (%ld)\n", pthread_self());

	memset(buf, 0x0, sizeof(buf));
	memset(pPcmData, 0x0, sizeof(pPcmData));
	while(1)
	{
		usleep(10000);	//10 msec

		if(fd_hidraw < 0)
			continue;

		if(capture_card_state == CAPTURE_START_STATE){	//capture start

			if(capture_card_state == CAPTURE_STOP_STATE){
				ALOGD("voice open is stop : because state is change - capture_card_state = %d\n", capture_card_state);
				break;
			}
			
			*voice_status = CAPTURE_START_STATE;
			/* Send a Report to the Device */
			res = Voice_OpenClose_write(fd_hidraw, VOICE_OPEN_CMD);
			if(res <= 0){
				ALOGD("voice open send faile - %d\n", res);
				break;
			}
			
		#if AUDIO_FILE_DUMP
			//file open
			memset(adump_name, 0x0, sizeof(adump_name));
			sprintf(adump_name, "%s%s%04d%s%s", AUDIO_DUMP_PATH, AUDIO_DUMP_NAME, anum, AUDIO_DUMP_COMMA, AUDIO_DUMP_PCM_EXTENSION);
			write_pcm_ptr = fopen(adump_name,"wb");  // w for write, b for binary
			if (write_pcm_ptr < 0){
				ALOGD("file pcm open faile\n");
			}
			
			memset(adump_name, 0x0, sizeof(adump_name));
			sprintf(adump_name, "%s%s%04d%s%s", AUDIO_DUMP_PATH, AUDIO_DUMP_NAME, anum, AUDIO_DUMP_COMMA, AUDIO_DUMP_ADPCM_EXTENSION);
			write_adpcm_ptr = fopen(adump_name,"wb");  // w for write, b for binary
			if (write_adpcm_ptr < 0){
				ALOGD("file admpcm open faile\n");
			}
			anum++;
		#endif
		
			bFirstPacket = 1;
			
			while(1)
			{
				usleep(1000);	// 1 msec

				if(capture_card_state == CAPTURE_STOP_STATE){
					ALOGD("voice read is stop : because state is change - capture_card_state = %d\n", capture_card_state);
					res = Voice_OpenClose_write(fd_hidraw, VOICE_CLOSE_CMD);
					ALOGD("snd_bt closed - voice close cmd send %d\n", res);
					*voice_status = CAPTURE_STOP_STATE;
					while(1){
						res = Voice_raw_data_read(fd_hidraw, buf);
						ALOGD("read report id res %d:\n", res);
						if(res < 0) break;
					}
					break;
				}

				memset(buf, 0x0, sizeof(buf));
				memset(pPcmData, 0x0, sizeof(pPcmData));

				res = Voice_raw_data_read(fd_hidraw, buf);
				ALOGD("read report id res %d:\n", res);

				if(res <= -2){
					ALOGE("voice read null!!\n");
					break;
				}

				if(res < 0) continue;

				//ADPCM to PCM start
				if(buf[0] == 0xb){
					if(Adpcm_to_Pcm(&buf[1], res-1, pPcmData, &pcmDataSize) <= 0){
						continue;
					}

					//snd driver - pcm data writting
					ret = write(fd_soundcard, (void *)pPcmData, pcmDataSize*sizeof(unsigned short));
					ALOGD("snd_bt write PCM res %d:\n", res);

					if(ret < 0){
						res = Voice_OpenClose_write(fd_hidraw, VOICE_CLOSE_CMD);
						ALOGD("snd_bt closed - voice close cmd send %d\n", res);
						*voice_status = CAPTURE_STOP_STATE;

						while(1){
							res = Voice_raw_data_read(fd_hidraw, buf);
							ALOGD("read report id res %d:\n", res);
							if(res < 0) break;
						}
						break;
					}

				#if AUDIO_FILE_DUMP
					//dump file PCM data
					fwrite((void *)pPcmData,pcmDataSize*sizeof(unsigned short),1,write_pcm_ptr); // write n*2 bytes to our buffer
					//dump file ADPCM data
					fwrite(&buf[0],(res-1),1,write_adpcm_ptr); // write n bytes to our buffer
				#endif

				} //ADPCM to PCM end
                          else{
                              //debug
//                              if(res > 0){
//                                  ALOGI("\n receive %d\n", res);
//                                  for(i=0; i< res; i++) ALOGI("0x%x ", buf[i]);
//                              }
                              if(res == 3
                                      && (cusumer_back_key[0] == buf[0]
                                      &&  cusumer_back_key[1] == buf[1]
                                      &&  cusumer_back_key[2] == buf[2])
                                      ||  (cusumer_home_key[0] == buf[0]
                                      &&  cusumer_home_key[1] == buf[1]
                                      &&  cusumer_home_key[2] == buf[2])
                                      || (cusumer_power_key[0] == buf[0]
                                      &&  cusumer_power_key[1] == buf[1]
                                      &&  cusumer_power_key[2] == buf[2]) )
                              {
                                  memset(pPcmData, 0x00, pcmDataSize*sizeof(unsigned short));
                                  i = 0;
                                  while( i < DUMMY_DATA_RETRY_MAX){
                                      usleep(1000);	// 1 msec
                                      ret = write(fd_soundcard, (void *)pPcmData, pcmDataSize*sizeof(unsigned short));
                                      if(ret < 0){
                                          ALOGI("\n write dummy data = %d, try = %d\n", ret, i);
                                          break;
                                      }
                                      i++;
                                  }
                              }
                          }
			}

			#if AUDIO_FILE_DUMP
				//file close
				fclose(write_pcm_ptr);
				fclose(write_adpcm_ptr);
			#endif			
		}
		else if(capture_card_state == CAPTURE_STOP_STATE || capture_card_state == CAPTURE_IDLE_STATE){
			if (*voice_status == CAPTURE_START_STATE){
				ALOGD("receive stop : voice_cmd_status = %d !!!\n", voice_cmd_status);
				res = Voice_OpenClose_write(fd_hidraw, VOICE_CLOSE_CMD);
				*voice_status = CAPTURE_STOP_STATE;

				while(1){
					res = Voice_raw_data_read(fd_hidraw, buf);
					ALOGD("read report id res %d:\n", res);
					if(res < 0) break;
				}
			}
			else{
				res = read(fd_hidraw, buf, sizeof(buf));

				//debug
//				if(res > 0){
//					ALOGI("\n receive %d\n", res);
//					for(i=0; i< res; i++) ALOGI("0x%x ", buf[i]);
//				}

				// get RCU Version request protocol
				if(res ==3  && buf[0] == PREMIUM_RCU_VERSION_REPORTID && buf[1] > 0 && buf[2] == PREMIUM_RCU_VERSION_HEADER){
					sprintf(rlevel, "%02x", buf[1]);
					rver = atoi(rlevel);
					__system_property_set(PROPERTY_PREMIUM_VERSION, rlevel);
					ALOGI("%s : found version 0x%x : 0x%x : 0x%x , rcu_ver =%s, rver = %d\n",  __FUNCTION__, buf[0], buf[1], buf[2], rlevel, rver);
				}
				
				// get RCU battery Volume request protocol
				if(res == 2 && buf[0] == PREMIUM_RCU_BATTERY_REPORTID && buf[1] <= 0x64 && buf[1] >= 0x0 ){
					sprintf(rlevel, "%d", buf[1]);
					rlvl = atoi(rlevel);
					ALOGI ("%s : rlevel = %s, rlvl =%d, orlvl = %d \n",__FUNCTION__, rlevel, rlvl, orlvl);
					if(rlvl != orlvl){
						__system_property_set(PROPERTY_PREMIUM_BATTEL_LEVEL, rlevel);
						orlvl = rlvl;
						ALOGI ("%s : update baterry level = %d\n",__FUNCTION__, rlvl);
					}
				}
				// get RCU Un-pairing request protocol
				if(res == 3 && \
					buf[0] == PREMIUM_RCU_UNPARING_REQUEST_REPORTID && \
					buf[1] == PREMIUM_RCU_UNPARING_REQUEST_DATA01 && \
					buf[2] == PREMIUM_RCU_UNPARING_REQUEST_DATA02){
					ALOGI ("%s : get test : res = %d, buf[%x,%x,%x]\n",__FUNCTION__, res, buf[0], buf[1], buf[2]);
					ALOGI ("Send KEY_PC !! is unparing key\n");
					virtual_uinput_send_event(EV_KEY, KEY_PC, 1);
					virtual_uinput_send_event(EV_SYN, SYN_REPORT, 0);
					usleep(280 * 1000);
					virtual_uinput_send_event(EV_KEY, KEY_PC, 0);
					virtual_uinput_send_event(EV_SYN, SYN_REPORT, 0);					
				}

				if (fd_hidraw > 0 && rver == -1 || rlvl == -1 ) curr_time = GetTickCount();
				// set request Version
				if(fd_hidraw > 0 && rver == -1 && rcnt < PREMIUM_VERSION_TRY_COUNT && (curr_time - wait_time > PREMIUM_REQUEST_WAIT_TIME)){
					ALOGI("Request timer !! curr_time%ld, wait_time = %ld\n", curr_time, wait_time);
					//usleep(100000);	// 100 msec
					hidraw_request_Rcu_Version(fd_hidraw );
					rcnt++;
					ALOGI("Request Rcu Version!! rcnt = %d\n", rcnt);
				}
				// set request battery volume
				else	 if(fd_hidraw > 0 &&  rver != -1 && rlvl == -1 && rcnt < PREMIUM_VERSION_MAX_COUNT && (curr_time - wait_time > PREMIUM_REQUEST_WAIT_MAX_TIME)){
					ALOGI("Request timer !! curr_time%ld, wait_time = %ld\n", curr_time, wait_time);
					//usleep(500000);	// 500 msec
					hidraw_request_Rcu_BatteryLevel(fd_hidraw);
					rcnt++;
					ALOGI("Request Battery Level!! rcnt = %d\n", rcnt);
				}
			}
		}
	}

}

/* Allow for 1024 simultanious events */
#define BUFF_SIZE ((sizeof(struct inotify_event)+FILENAME_MAX)*1024)
void hidraw_inotify_get_event (int fd, const char * target)
{
	ssize_t len, i = 0;
	char hidraw_name[FILENAME_MAX] = {0};
#if HIDRAW_DEBUG	
	char action[128+FILENAME_MAX] = {0};
#endif
	char buff[BUFF_SIZE] = {0};

	len = read (fd, buff, BUFF_SIZE);

	while (i < len) {
		struct inotify_event *uevent = (struct inotify_event *)&buff[i];
#if HIDRAW_DEBUG
		char action[128+FILENAME_MAX] = {0,};

		if (uevent->len) strcpy (action, uevent->name);
		else 	strcpy (action, target);
#endif

		if (uevent->mask & IN_CREATE){
#if HIDRAW_DEBUG
			strcat(action, " created in watched directory");
#endif
			ALOGD("%s, %s\n", __func__, uevent->name);
			if (uevent->len && strstr(uevent->name, HIDRAW_PREFIXNAME_NAME) != NULL){
				memset(hidraw_name, 0x0, sizeof(hidraw_name));
				if (uevent->len){
					strcat (hidraw_name, HIDRAW_INOTIFY_PREFIXNAME);
					strcat(hidraw_name, uevent->name);
					ALOGD("hiddv name %s\n", hidraw_name);
				}
				
				/* Open the Device with non-blocking reads. In real life,
				don't use a hard coded path; use libudev instead. */
				//	fd = open("/dev/hidraw0", O_RDWR|O_NONBLOCK);
				if(hidraw_open(hidraw_name, O_RDWR | O_NONBLOCK) < 0){
					ALOGD("not match hidraw name\n");
				}
			}
		}
		else if (uevent->mask & IN_DELETE){
#if HIDRAW_DEBUG
			strcat(action, " deleted from watched directory");
#endif
			//check plug out
			if (uevent->len) ALOGD("%s, %s\n", __func__, uevent->name);
			else 	ALOGD("null %s, %s\n", __func__, target);

			if (uevent->len && strstr(uevent->name, HIDRAW_PREFIXNAME_NAME) != NULL){
				memset(hidraw_name, 0x0, sizeof(hidraw_name));
				strcat(hidraw_name, HIDRAW_INOTIFY_PREFIXNAME);
				strcat(hidraw_name, uevent->name);
				ALOGD("hiddv name %s\n", hidraw_name);

				if(strcmp(hidraw_devname, hidraw_name) == 0){
					hidraw_close();
				}
			}

		}
		else if (uevent->mask & IN_ACCESS){
#if HIDRAW_DEBUG
			strcat(action, " was read");
#endif
		}
		else if (uevent->mask & IN_ATTRIB){
#if HIDRAW_DEBUG
			strcat(action, " Metadata changed");
#endif
		}
		else if (uevent->mask & IN_CLOSE_WRITE){
#if HIDRAW_DEBUG
			strcat(action, " opened for writing was closed");
#endif
		}
		else if (uevent->mask & IN_CLOSE_NOWRITE){
#if HIDRAW_DEBUG
			strcat(action, " not opened for writing was closed");
#endif
		}
		else if (uevent->mask & IN_DELETE_SELF){
#if HIDRAW_DEBUG
			strcat(action, "Watched file/directory was itself deleted");
#endif
		}
		else if (uevent->mask & IN_MODIFY){
#if HIDRAW_DEBUG
			strcat(action, " was modified");
#endif
		}
		else if (uevent->mask & IN_MOVE_SELF){
#if HIDRAW_DEBUG
			strcat(action, "Watched file/directory was itself moved");
#endif
		}
		else if (uevent->mask & IN_MOVED_FROM){
#if HIDRAW_DEBUG
			strcat(action, " moved out of watched directory");
#endif
		}
		else if (uevent->mask & IN_MOVED_TO){
#if HIDRAW_DEBUG
			strcat(action, " moved into watched directory");
#endif
		}
		else if (uevent->mask & IN_OPEN){
#if HIDRAW_DEBUG
			strcat(action, " was opened");
#endif
		}

#if HIDRAW_DEBUG
		printf ("wd=%d mask=%d cookie=%d len=%d dir=%s\n",
			uevent->wd, uevent->mask, uevent->cookie, uevent->len, 
			(uevent->mask & IN_ISDIR)?"yes":"no");

		if (uevent->len) ALOGD ("name=%s\n", uevent->name);

		printf ("%s\n", action);
#endif
		i += sizeof(struct inotify_event) + uevent->len;
	}
}  /* hidraw_inotify_get_event */

void handle_error (int error)
{
   fprintf (stderr, "Error: %s\n", strerror(error));

}  /* handle_error */


void TVSTORM_HideawCtrl_Version(TVSTORM_HIDRAWCTRL_VERSION_T *hidctrl_version)
{
    hidctrl_version->version = TVSTORM_HIDRAWCTRL_VERSION;
    hidctrl_version->patchlevel = TVSTORM_HIDRAWCTRL_PATCHLEVEL;
    hidctrl_version->sublevel = TVSTORM_HIDRAWCTRL_SUBLEVEL;

    return;
}

void TVSTORM_HideawCtrl_LastDate(TVSTORM_HIDRAWCTRL_DATE_T *hidctrll_date)
{
    hidctrll_date->year = TVSTORM_HIDRAWCTRL_YEAR;
    hidctrll_date->month = TVSTORM_HIDRAWCTRL_MONTH;
    hidctrll_date->day = TVSTORM_HIDRAWCTRL_DAY;

    return;
}

int main(int argc, char **argv)
{
    TVSTORM_HIDRAWCTRL_VERSION_T hidctrl_version;
    TVSTORM_HIDRAWCTRL_DATE_T hidctrll_date;
    int res;

    TVSTORM_HideawCtrl_Version(&hidctrl_version);
    ALOGI("TVSTORM VOICESEARCH version : V%d.%d.%d\n", hidctrl_version.version,  hidctrl_version.patchlevel, hidctrl_version.sublevel);

    TVSTORM_HideawCtrl_LastDate(&hidctrll_date);
    ALOGI("TVSTORM VOICESEARCH  date : year = %d, month = %d, day = %d\n", hidctrll_date.year,  hidctrll_date.month, hidctrll_date.day);

    usleep(500000);	//500 msec

    //init - open devices
    res = hidraw_ctrl_init();
    if(res < 0){
    	ALOGE("hidraw_ctrl init faile!!\n");
    	return res;
    }
    
    while (1) {
    	usleep(100000);	//100 msec
    	hidraw_inotify_get_event(fd_hidraw_inotify, HIDRAW_INOTIFY_NAME);
    }
    
    hidraw_ctrl_Terminate();
    
    return 0;
}
