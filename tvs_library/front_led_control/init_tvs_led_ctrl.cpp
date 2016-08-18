/******************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: $ cssong
 * $LastChangedDate: $ 2015.07.14
 * $LastChangedRevision: $ V1.3.2
 * Description: Init Front LED Controler
 * Note: 
 *         V1.1.0 
 *                   Change Scenarios
 *                   remove Network valid status 
 *                   add eeprom property of passive standby mode
 *                   STB is not provisioned mode
 *         V1.1.1
 *                   Fix sleep
 *         V1.1.2
 *                   demo Scenarios
 *                   wifi on/off
 *         V1.2.0
 *                   wifi on/off
 *                   wifi wpa_supplicant.conf
 *         V1.2.1
 *                   fix deleted wifi wpa_supplicant.conf
 *         V1.2.2
 *                   fix arp to ifc_get_info
 *         V1.3.0
 *                   add rcu led control
 *         V1.3.1
 *                   add voice property - Voice search status is tvstorm.bt.rcu.voice.status
 *         V1.3.2
 *                   fix - property init
 *****************************************************************************/
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>

#include <netutils/ifc.h>
#include <linux/if.h>

#include "init_tvs_led_ctrl.h"
#include "eeprom.h"

// for hid rcu led
#include <pthread.h>	//thread
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/inotify.h>
#include <sys/limits.h>
#include <sys/poll.h>
#include <linux/input.h>
#include <errno.h>
#include "cutils/log.h"	//logcat 


// system property
#define PROPERTY_BOOTCOMPLETE		            "sys.boot_completed"

// tvstorm property
#define PROPERTY_TVSTORM_STANDBY                "persist.sys.tvstorm.standbymode"
#define PROPERTY_TVSTORM_EEPROM_STANDBY         "tvstorm.eeprom.standbymode"

#define PROPERTY_TVSTORM_NAGRA_AUTHENTICATION   "tvstorm.nagra.authentication"	// ok is 1 nok is 0
#define PROPERTY_TVSTORM_ETH_VALID              "tvstorm.eth.valid"							// vaild is 1 invalid is 0
#define PROPERTY_TVSTORM_CM_UPDATE_STATE		"tvstorm.cmusb.state"

#define PROPERTY_TVSTORM_PARTIAL_UPDATE         "tvstorm.partial.update"
#define DEVICE_NAME_ETHERNET                    "eth0"
#define DEVICE_NAME_WIFI                        "wlan0"

#undef LOG_TAG
#define LOG_TAG	"INITLED"

/* Debug cfg */
//#define INIT_TVS_LED_CTRL_DEBUG
#ifndef INIT_TVS_LED_CTRL_DEBUG
//#undef ALOGE
//#define ALOGE(...)
//#undef ALOGD
//#define ALOGD(...)
#undef ALOGI
#define ALOGI(...)
#endif


/*
 * HID RCU led
 */

 #ifndef PATH_MAX
 #define PATH_MAX 4096
 #endif
 
#define BT_RCU_BUS          0x05
#define BT_RCU_VENDOR       0xc4
#define BT_RCU_PRODUCT      0x7a44
#define BT_RCU_VERSION      0x213

#define IR_RCU_BUS          0x19

#define RCU_REPEAT_DELAY    32

#define EVENT_MAX	2	// ir + bt
#define POLL_MAX 3

// Voice open/close - Keyevent exception of BT RCU
#define PROPERTY_PREMIUM_VOICE_STATUS "tvstorm.bt.rcu.voice.status"

enum
{
    KEY_UP_STATE = 0,
    KEY_DOWN_STATE,
    KEY_REPEAT_STATE,
    KEY_MAX_STATE,
};

static struct pollfd ufds[POLL_MAX];
static char device_names[EVENT_MAX][PATH_MAX];
const char *device_path = "/dev/input";
const char *btrcu_name = "LinkNet RCU";
const char *nexuxir_name = "NexusIrHandler";

// inotify
static int fd_input_inotify = -1;
static int fd_input_wd = -1;
static int num_fds = 1;

static int key_value_status = KEY_MAX_STATE;
static int last_event_fd = -1;

// rcu led thread
pthread_t p_thread = 0, led_p_thread = 0;
int input_th_id, led_th_id;

/*
* input input device open
*/
static int open_input_device(const char *device)
{
    int version;
    int clkid = CLOCK_MONOTONIC;
    char dev_name[80];
    int input_fd = -1;
    int num;
    struct input_id id;

    input_fd = open(device, O_RDWR);
    if(input_fd < 0) {
            ALOGE("could not open %s, %s", device, strerror(errno));
        return -1;
    }
    
    if(ioctl(input_fd, EVIOCGVERSION, &version)) {
            ALOGE("could not get driver version for %s, %s", device, strerror(errno));
        return -1;
    }
    if(ioctl(input_fd, EVIOCGID, &id)) {
            ALOGE("could not get driver id for %s, %s", device, strerror(errno));
        return -1;
    }

    dev_name[sizeof(dev_name) - 1] = '\0';
    if(ioctl(input_fd, EVIOCGNAME(sizeof(dev_name) - 1), &dev_name) < 1) {
        ALOGE("could not get device name for %s, %s", device, strerror(errno));
        dev_name[0] = '\0';
    }

    ALOGD("device: %s,  bus: %04x,  vendor: %04x,  product: %04x, version: %04x, dev_name: %s", device, id.bustype, id.vendor, id.product, id.version, dev_name);
	
    if (ioctl(input_fd, EVIOCSCLOCKID, &clkid) != 0) {
        ALOGE("Can't enable monotonic clock reporting: %s\n", strerror(errno));
        // this is a non-fatal error
    }

    if(id.bustype == IR_RCU_BUS && strcmp(dev_name, nexuxir_name) == 0){
        ALOGD("found ir rcu input device %s, current fd = %d", device, num_fds);
        goto founded;
    }
	else if(id.bustype == BT_RCU_BUS && id.vendor == BT_RCU_VENDOR && id.product == BT_RCU_PRODUCT && strcmp(dev_name, btrcu_name) == 0/*&& id.version == BT_RCU_VERSION*/){
        ALOGD("found bt rcu input device %s, current fd = %d", device, num_fds);
        goto founded;
    }
    else{
        //ALOGE("could not found bt rcu input device %s", device);
        return -2;
    }

founded:
    if( num_fds > POLL_MAX){
        ALOGD("could not add device %s", device);
        return -2;
    }
//    else{
//        ALOGD("ufds.fd %d:%d", ufds[1].fd, ufds[2].fd);
//    }

     if(ufds[num_fds].fd < 0){
        ufds[num_fds].fd = input_fd;
        ufds[num_fds].events = POLLIN;
        strcpy(device_names[(num_fds - 1)], device);
        ALOGD("add input device : index = %d, device name = %s : %s", num_fds, device_names[(num_fds - 1)], device);
        num_fds++;
    }

    return 0;
}

/*
* input device close
*/
int close_input_device(const char *device)
{
    int num, loop;
    struct pollfd tfds;
    char device_name[PATH_MAX];

    for(num = 1; num < num_fds; num++){
	  //ALOGD("num = %d : %s : %s ", num, device_names[(num -1)], device);
        if(strcmp(device_names[(num -1)], device) == 0){
            ALOGD("remote find index = %d, device: %s", num, device);
            ufds[num].fd = -1;
            ufds[num].events = 0;
            memset(device_names[(num -1)], 0x00, sizeof(PATH_MAX));
            num_fds--;
            ALOGD("remote index = %d, device: %s", num, device);
            goto relocation;
        }
    }

    return -1;

relocation:
    for(loop=1; loop < EVENT_MAX; loop++){
         if(ufds[loop].fd < 0 && ufds[loop+1].fd > 0){
             ALOGD("relocation index %d from %d to , device: %s", loop+1, loop, device_names[loop]);
             ufds[loop].fd       = ufds[loop+1].fd;
             ufds[loop].events = ufds[loop+1].events;
             strcpy(device_names[(loop - 1)], device_names[loop]);

             ufds[loop+1].fd = -1;
             ufds[loop+1].events = 0;
             memset(device_names[loop], 0x00, sizeof(PATH_MAX));
         }
    }
    return 0;
	
}

/*
* input inotify
*/
static int read_notify(const char *dirname, int nfd)
{
    int res;
    char devname[PATH_MAX];
    char *filename;
    char event_buf[512];
    int event_size;
    int event_pos = 0;
    struct inotify_event *event;

    res = read(nfd, event_buf, sizeof(event_buf));
    if(res < (int)sizeof(*event)) {
        ALOGE("could not get event, %d", res);
        return -1;
    }
    //printf("got %d bytes of event information\n", res);

    strcpy(devname, dirname);
    filename = devname + strlen(devname);
    *filename++ = '/';

    while(res >= (int)sizeof(*event)) {
        event = (struct inotify_event *)(event_buf + event_pos);
        ALOGD("%d: %08x %s", event->wd, event->mask, event->len ? event->name : "");
        if(event->len) {
            strcpy(filename, event->name);
            if(event->mask & IN_CREATE) {
                open_input_device(devname);
            }
            else {
                close_input_device(devname);
            }
        }
        event_size = sizeof(*event) + event->len;
        res -= event_size;
        event_pos += event_size;
    }
    return 0;
}

/*
* input scan dir
*/
static int scan_input_dir(const char *dirname)
{
    char devname[PATH_MAX];
    char *filename;
    DIR *dir;
    struct dirent *de;
    dir = opendir(dirname);
    if(dir == NULL)
        return -1;
    strcpy(devname, dirname);
    filename = devname + strlen(devname);
    *filename++ = '/';
    while((de = readdir(dir))) {
        if(de->d_name[0] == '.' &&
           (de->d_name[1] == '\0' ||
            (de->d_name[1] == '.' && de->d_name[2] == '\0')))
            continue;
        strcpy(filename, de->d_name);
        open_input_device(devname);
    }
    closedir(dir);
    return 0;
}

/*
* input inotify open
*/
static int input_inotify_open(void)
{
	int ret;

	fd_input_inotify = inotify_init();
	if (fd_input_inotify < 0) {
		ALOGE("could not open input inotify");
		return -1;
	}

	fd_input_wd = inotify_add_watch(fd_input_inotify, device_path, IN_DELETE | IN_CREATE);
	if (fd_input_wd < 0) {
		close(fd_input_inotify);
		fd_input_inotify = -1;
		ALOGE("could not add input watch faile!");
	}
	ALOGD("open input_inotify_open");
	return  fd_input_inotify;
}

/*
* input inotify close
*/
static void input_inotify_close(void)
{
	ALOGD("close input_inotify_close");
	inotify_rm_watch( fd_input_inotify, fd_input_wd );
	close(fd_input_inotify);
	fd_input_inotify = -1;
}

/*
* input event task
*/
void *input_event_task(void * data)
{
    int64_t last_sync_time = 0;
    struct input_event event;
    int res = -1;
    int i;
    int *key_status = (int *)data;

    ufds[0].fd = fd_input_inotify;
    ufds[0].events = POLLIN;

    while(1) {
        poll(ufds, num_fds, -1);
        //ALOGD("poll %d, returned %d", num_fds);
        if(ufds[0].revents & POLLIN) {
            read_notify(device_path, ufds[0].fd);
        }
        for(i = 1; i < num_fds; i++) {
            if(ufds[i].revents) {
                if(ufds[i].revents & POLLIN) {
                    res = read(ufds[i].fd, &event, sizeof(event));
                    if(res < (int)sizeof(event)) {
                        ALOGE("could not get event");
                        return (void *)0;
                    }

                    if(event.type == 0x1 && event.value == 0x1){
                        ALOGD("event [type %04x, code %04x,  value %08x] : %s", event.type, event.code, event.value, device_names[i-1]);
						last_event_fd = (i -1);
                        *key_status = KEY_DOWN_STATE;
                    }else if(event.type == 0x1 && event.value == 0x0){
                        ALOGD("event [type %04x, code %04x,  value %08x] : %s", event.type, event.code, event.value, device_names[i-1]);
						last_event_fd = -1;
                        *key_status = KEY_UP_STATE;
                    }
                }
            }
        }
    }
    return (void *)0;
}


/*
* input rcu led task
*/
void *input_led_task(void * data)
{
    char p_tvstorm_standby_val[PROPERTY_VALUE_MAX] = {0, };
    char p_bootcomplete_val[PROPERTY_VALUE_MAX] = {0, };
    char p_nagra_auth_val[PROPERTY_VALUE_MAX] = {0, };
    char p_voice_status_val[PROPERTY_VALUE_MAX] = {0, };
    unsigned int repeat_cnt = 0;

    while(1) {
        __system_property_get(PROPERTY_TVSTORM_STANDBY,p_tvstorm_standby_val );
        __system_property_get(PROPERTY_TVSTORM_NAGRA_AUTHENTICATION, p_nagra_auth_val);
        __system_property_get(PROPERTY_BOOTCOMPLETE, p_bootcomplete_val);
        __system_property_get(PROPERTY_PREMIUM_VOICE_STATUS, p_voice_status_val);
        //reset wrong keyevent
        if(strcmp("1", p_voice_status_val) == 0 && key_value_status == KEY_DOWN_STATE){
            key_value_status = KEY_UP_STATE;
            repeat_cnt = 0;
            ALOGD("voice search status is on");
        }

        if(strcmp("0", p_tvstorm_standby_val) == 0 
			&& strcmp("1", p_bootcomplete_val) == 0 
			&& strcmp("1", p_nagra_auth_val) == 0 ){	/* wake up - active */
            //bt rcu led
            if(key_value_status == KEY_DOWN_STATE){
                //ALOGD("key status = %d, repeat_cnt = %d", key_value_status, repeat_cnt);
                if(repeat_cnt == 1 || repeat_cnt > RCU_REPEAT_DELAY){
                    TVSTORM_Front_Led_Control(LED_POWER_GREEN, TVS_LED_OFF);
                    usleep(108 * 1000); // 108ms
                    TVSTORM_Front_Led_Control(LED_POWER_GREEN, TVS_LED_ON);
                    if(repeat_cnt > RCU_REPEAT_DELAY)
                        usleep(98 * 1000); // 98ms

					if (last_event_fd >= 0 && last_event_fd < (num_fds - 1))
						ALOGD("repeat from %s", device_names[last_event_fd]);
                }
                repeat_cnt++;
            }else if(key_value_status == KEY_UP_STATE && repeat_cnt != 0){
                //ALOGD("key status = %d", key_value_status);
                repeat_cnt = 0;
            }
        }
        usleep(10 * 1000); // 10ms
    }

    return (void *)0;
}


int input_led_init(void)
{
    int res = -1;
    int i;

    num_fds = 1;

    for(i=0; i <POLL_MAX; i++){
	ufds[i].fd = -1;
	ufds[i].events = 0;
	ufds[i].revents = 0;
    }

    res = input_inotify_open();
    if(res < 0) {
        ALOGE("input inotify open failed for %d\n", fd_input_inotify);
        return -1;
    }
    
    res = scan_input_dir(device_path);
    if(res < 0) {
        ALOGD("scan dir failed for %s\n", device_path);
    }

    //thread create
    input_th_id = pthread_create(&p_thread, NULL, input_event_task, &key_value_status);
    if (input_th_id < 0){
    	ALOGE("event thread create error : %d\n", input_th_id);
    	return -1;
    }
    else{
    	ALOGD("open input event thread create\n");
    }

    //thread create
    led_th_id = pthread_create(&led_p_thread, NULL, input_led_task, NULL);
    if (led_th_id < 0){
    	ALOGE("led thread create error : %d\n", led_th_id);
    	return -1;
    }
    else{
    	ALOGD("open input led thread create\n");
    }

    return 0;
}

void input_led_uninit(void)
{
    pthread_detach(p_thread);
    input_th_id = -1;
    pthread_detach(led_p_thread);
    led_th_id = -1;	
    input_inotify_close();
}

// init tvs led ctrl

int checkInterfaceIP(const char *name)
{
	struct in_addr addr;
	int prefixLength;
	unsigned flags = 0;
	int ret = -1;

	ifc_init();
	
	if (ifc_get_info(name, &addr.s_addr, &prefixLength, &flags)) {
	    ifc_close();
	    return -1;
	}

	char *addr_s = strdup(inet_ntoa(addr));

	if( strncmp( addr_s, "0.0.0.0", strlen(addr_s) ) != 0){
		ret = 0;
	}

	free(addr_s);
	ifc_close();

	return ret;
}

int getNetworkStateOfInterface(const char *ifname){
	int index = 0;
	unsigned flags, current_state = kFoundInterface;
	in_addr_t addr = 0;
	const char *iface = ifname;

	ifc_init();

	if(ifc_get_ifindex(iface, &index) < 0){
		return kNotFoundInterface;
	}

	ifc_get_info(iface, &addr, NULL, &flags);

	if(flags & IFF_UP){
		if(addr > 0){
			char value[PROPERTY_VALUE_MAX] = {0, }; 	
			bool valid = false;

			if(!strcmp(iface, DEVICE_NAME_WIFI)){
				if(checkInterfaceIP(iface) >= 0)
					valid = true;
			}else{
				/* Ethernet */
				__system_property_get(PROPERTY_TVSTORM_ETH_VALID, value );
				if(strcmp(value, "1") == 0)
					valid = true;
			}
			if(valid)
				current_state = kValidIPInterface;
			else
				current_state = kInValidIPInterface;
		}else{
			current_state = kNotConfiguredInterface;
		}
	}

	return current_state;
}

/*
 * static connect -First get current standby mode
 * return ture is sleep, other is active
*/
static bool get_from_eeprom_tvstorm_standby(void)
{
	int ret = 0;
	STB_POWER_STATUS_T ps;

	ret = TVSTORM_Get_StbPowerStaus(&ps);
	ALOGI("%s: ret = %d", __FUNCTION__, ret);

	if(ret == 0 && ps == STB_POWER_STATUS_STANDBY){
          return true;
	}

	return false;
}

/*
 * static nagra authentication - valid or invalid
 * return ture is valid, other is invalid
*/
static bool get_nagra_authentication_status(void)
{
    char property_nagra_valid_val[PROPERTY_VALUE_MAX] = {0, };

    __system_property_get(PROPERTY_TVSTORM_NAGRA_AUTHENTICATION,property_nagra_valid_val );
    if(strcmp("1", property_nagra_valid_val) == 0)
        return true;
    else
        return false;
}

/* static cm usb update state 
* return 0: off, 1: updating cm bin, 2: the same version, 3: update fail
*/

static int get_cm_usb_update_state(void)
{
	char property_cm_usb_state_val[PROPERTY_VALUE_MAX] = {0, };
	int val;
	__system_property_get(PROPERTY_TVSTORM_CM_UPDATE_STATE, property_cm_usb_state_val);

	val = atoi(property_cm_usb_state_val);

	return val;
}

void TVSTORM_InitTvsLedCtrl_version(TVSTORM_INITTVSLEDCTRL_VERSION_T *inittvsled_version)
{
    inittvsled_version->version = TVSTORM_INITTVSLEDCTRL_VERSION;
    inittvsled_version->patchlevel = TVSTORM_INITTVSLEDCTRL_PATCHLEVEL;
    inittvsled_version->sublevel = TVSTORM_INITTVSLEDCTRL_SUBLEVEL;

    return;
}

void TVSTORM_InitTvsLedCtrl_last_date(TVSTORM_INITTVSLEDCTRL_DATE_T *inittvsled_date)
{
    inittvsled_date->year = TVSTORM_INITTVSLEDCTRL_YEAR;
    inittvsled_date->month = TVSTORM_INITTVSLEDCTRL_MONTH;
    inittvsled_date->day = TVSTORM_INITTVSLEDCTRL_DAY;

    return;
}

void initProperty(){
    TVSTORM_INITTVSLEDCTRL_VERSION_T initled_version;
    TVSTORM_INITTVSLEDCTRL_DATE_T initled_date;

    TVSTORM_InitTvsLedCtrl_version(&initled_version);
    ALOGD("TVSTORM Front Led Init version : V%d.%d.%d", initled_version.version,  initled_version.patchlevel, initled_version.sublevel);

    TVSTORM_InitTvsLedCtrl_last_date(&initled_date);
    ALOGD("TVSTORM Front Led Init date : year = %d, month = %d, day = %d", initled_date.year,  initled_date.month, initled_date.day);
}

void initLedVersion(){
    char property_set_valid_val[PROPERTY_VALUE_MAX] = {0, };

    __system_property_get(PROPERTY_TVSTORM_NAGRA_AUTHENTICATION,property_set_valid_val );

    if(strcmp("1", property_set_valid_val) != 0 && strcmp("0", property_set_valid_val) != 0){
    	__system_property_set(PROPERTY_TVSTORM_NAGRA_AUTHENTICATION,"1" );
        ALOGD("%s is init = 1", PROPERTY_TVSTORM_NAGRA_AUTHENTICATION);
    }

    __system_property_get(PROPERTY_TVSTORM_ETH_VALID,property_set_valid_val );

	if(strcmp("1", property_set_valid_val) != 0 && strcmp("0", property_set_valid_val) != 0){
         __system_property_set(PROPERTY_TVSTORM_ETH_VALID,"1" );
         ALOGD("%s is init = 1", PROPERTY_TVSTORM_ETH_VALID);
    }
}

void LEDStateInit(){
	/* init led - only first time */
	TVSTORM_Front_Led_Control(LED_ETHERNET_GREEN, TVS_LED_OFF);
	TVSTORM_Front_Led_Control(LED_ETHERNET_RED, TVS_LED_OFF);
	TVSTORM_Front_Led_Control(LED_WIFI_GREEN, TVS_LED_OFF);
	TVSTORM_Front_Led_Control(LED_WIFI_RED, TVS_LED_OFF);
			

	if(get_from_eeprom_tvstorm_standby()){	/* passive standby mode */
		TVSTORM_Front_Led_Control(LED_POWER_GREEN, TVS_LED_OFF);
		TVSTORM_Front_Led_Control(LED_POWER_RED, TVS_LED_ON);
		__system_property_set(PROPERTY_TVSTORM_EEPROM_STANDBY,"on" );
	}
	else{	/* active standby mode */
		TVSTORM_Front_Led_Control(LED_POWER_GREEN, TVS_LED_ON);
		TVSTORM_Front_Led_Control(LED_POWER_RED, TVS_LED_OFF);
		__system_property_set(PROPERTY_TVSTORM_EEPROM_STANDBY,"off" );
	}
}

int frontLEDInit(){
	if (TVSTORM_Front_Led_Init() == TVSTORM_OK) {
		if(input_led_init() <0){
			ALOGE("input_led_init is failed!!");
			input_led_uninit();
		    TVSTORM_Front_Led_Uninit();
			return -1;
		}

		initLedVersion();

		initProperty();

		LEDStateInit();
	
		ALOGE("frontLEDInit is success!!!\n");	
		return 0;
	}

	ALOGE("frontLEDInit is failed...\n");

	return -1;
}

void changeNagraLEDStatus(int led_blinking){
	ALOGV("STB Provisioned mode +++");
	TVSTORM_Front_Led_Control(LED_POWER_RED, TVS_LED_OFF);

	if(led_blinking){
		TVSTORM_Front_Led_Control(LED_POWER_GREEN, TVS_LED_ON);
	}else{
		TVSTORM_Front_Led_Control(LED_POWER_GREEN, TVS_LED_OFF);
	}
}

void changePowerLEDStatus(bool standby){
	if(standby){
			TVSTORM_Front_Led_Control(LED_POWER_GREEN, TVS_LED_OFF);
			TVSTORM_Front_Led_Control(LED_POWER_RED, TVS_LED_ON);

			// network led off		
			TVSTORM_Front_Led_Control(LED_ETHERNET_GREEN, TVS_LED_OFF);
			TVSTORM_Front_Led_Control(LED_ETHERNET_RED, TVS_LED_OFF);
			TVSTORM_Front_Led_Control(LED_WIFI_GREEN, TVS_LED_OFF);
			TVSTORM_Front_Led_Control(LED_WIFI_RED, TVS_LED_OFF);
	}else{

			TVSTORM_Front_Led_Control(LED_POWER_GREEN, TVS_LED_ON);
			TVSTORM_Front_Led_Control(LED_POWER_RED, TVS_LED_OFF);
	}
}

void changeEthernetLEDStatus(int led_blinking){
	int eth_status = kNotFoundInterface;    
	static int count = 0;

	eth_status = getNetworkStateOfInterface(DEVICE_NAME_ETHERNET);

	if(count == 100)
		ALOGV("eth_status = 0x%x", eth_status);

	switch(eth_status){
		case kNotConfiguredInterface:{
			TVSTORM_Front_Led_Control(LED_ETHERNET_GREEN, TVS_LED_OFF);

			if(led_blinking){
			 	TVSTORM_Front_Led_Control(LED_ETHERNET_RED, TVS_LED_ON);
			}else{
				TVSTORM_Front_Led_Control(LED_ETHERNET_RED, TVS_LED_OFF);
			}
		}
		break;

		case kInValidIPInterface:{
			TVSTORM_Front_Led_Control(LED_ETHERNET_RED, TVS_LED_OFF);
			if(led_blinking){
				TVSTORM_Front_Led_Control(LED_ETHERNET_GREEN, TVS_LED_ON);
			}else{
				TVSTORM_Front_Led_Control(LED_ETHERNET_GREEN, TVS_LED_OFF);
			}
		}
		break;

		case kValidIPInterface:{
			TVSTORM_Front_Led_Control(LED_ETHERNET_RED, TVS_LED_OFF);
			TVSTORM_Front_Led_Control(LED_ETHERNET_GREEN, TVS_LED_ON);		
		}
		break;

		case kNotFoundInterface:
		case kFoundInterface:
		default:{
			TVSTORM_Front_Led_Control(LED_ETHERNET_GREEN, TVS_LED_OFF);
			TVSTORM_Front_Led_Control(LED_ETHERNET_RED, TVS_LED_OFF);
		}
		break;
	}

	if(count > 99)
		count = 0;
	else
		count++;
}

void changeWifiLEDStatus(int led_blinking){
	int wifi_status = kNotFoundInterface;    
	static int count = 0;

	wifi_status = getNetworkStateOfInterface(DEVICE_NAME_WIFI);

	ALOGV("wifi_status = 0x%x", wifi_status);

	switch(wifi_status){
		case kNotConfiguredInterface:{
			TVSTORM_Front_Led_Control(LED_WIFI_GREEN, TVS_LED_OFF);

			if(led_blinking){
			 	TVSTORM_Front_Led_Control(LED_WIFI_RED, TVS_LED_ON);
			}else{
				TVSTORM_Front_Led_Control(LED_WIFI_RED, TVS_LED_OFF);
			}
		}
		break;

		case kInValidIPInterface:{
			TVSTORM_Front_Led_Control(LED_WIFI_RED, TVS_LED_OFF);
			if(led_blinking){
				TVSTORM_Front_Led_Control(LED_WIFI_GREEN, TVS_LED_ON);
			}else{
				TVSTORM_Front_Led_Control(LED_WIFI_GREEN, TVS_LED_OFF);
			}
		}
		break;

		case kValidIPInterface:{
			TVSTORM_Front_Led_Control(LED_WIFI_RED, TVS_LED_OFF);
			TVSTORM_Front_Led_Control(LED_WIFI_GREEN, TVS_LED_ON);		
		}
		break;

		case kFoundInterface:{
			TVSTORM_Front_Led_Control(LED_WIFI_GREEN, TVS_LED_OFF);
			TVSTORM_Front_Led_Control(LED_WIFI_RED, TVS_LED_ON);
		}
		break;

		case kNotFoundInterface:
		default:{
			TVSTORM_Front_Led_Control(LED_WIFI_GREEN, TVS_LED_OFF);
			TVSTORM_Front_Led_Control(LED_WIFI_RED, TVS_LED_OFF);
		}
		break;
	}

	if(count > 99)
		count = 0;
	else
		count++;
}

#if SH960C_LN
void changeCMUpdateStatus(int led_blinking){
	int cm_usb_update_state = 0; 
	static int logCount = 0;
	/* CM USB Update Status */
	cm_usb_update_state = get_cm_usb_update_state();

	if(logCount == 100){
		ALOGI("cm_usb_update_state = 0x%x", cm_usb_update_state);
	}

	if(cm_usb_update_state > TVSTORM_CM_USB_UPDATE_OFF){
		static int count = 0;

		switch(cm_usb_update_state){
			case TVSTORM_CM_USB_UPDATING:{
				if(led_blinking){
					TVSTORM_Front_Led_Control(LED_ETHERNET_RED, TVS_LED_OFF);  
					TVSTORM_Front_Led_Control(LED_ETHERNET_GREEN, TVS_LED_ON);
					count++;
				}else{
					TVSTORM_Front_Led_Control(LED_ETHERNET_RED, TVS_LED_OFF);  
					TVSTORM_Front_Led_Control(LED_ETHERNET_GREEN, TVS_LED_OFF);
					count++;
				}
					
				/* led green blinking is ended after blinking twice */
				if(count == 4){
					__system_property_set(PROPERTY_TVSTORM_CM_UPDATE_STATE, "0");
					count = 0;
				}
			}
			break;

			case TVSTORM_CM_USB_UPDATED:{
				if(led_blinking){
					TVSTORM_Front_Led_Control(LED_ETHERNET_RED, TVS_LED_OFF);  
					TVSTORM_Front_Led_Control(LED_ETHERNET_GREEN, TVS_LED_ON);
					count++;
				}else{
					TVSTORM_Front_Led_Control(LED_ETHERNET_RED, TVS_LED_OFF);  
					TVSTORM_Front_Led_Control(LED_ETHERNET_GREEN, TVS_LED_OFF);
					count++;
				}

				/* led green blinking is ended after blinking twice */
				if(count == 4){
					__system_property_set(PROPERTY_TVSTORM_CM_UPDATE_STATE, "0");
					count = 0;
				}
			}
			break;

			case TVSTORM_CM_USB_SAME_VER:{
				if(led_blinking){
					TVSTORM_Front_Led_Control(LED_ETHERNET_RED, TVS_LED_OFF);  
					TVSTORM_Front_Led_Control(LED_ETHERNET_GREEN, TVS_LED_ON);
					count++;
				}else{
					TVSTORM_Front_Led_Control(LED_ETHERNET_RED, TVS_LED_OFF);  
					TVSTORM_Front_Led_Control(LED_ETHERNET_GREEN, TVS_LED_OFF);
					count++;
				}

				/* led green blinking is ended after blinking one time */
				if(count == 2){
					__system_property_set(PROPERTY_TVSTORM_CM_UPDATE_STATE, "0");	
					count = 0;
				}
			}
			break;

			case TVSTORM_CM_USB_UPDATE_FAIL:{
				if(led_blinking){
					TVSTORM_Front_Led_Control(LED_ETHERNET_GREEN, TVS_LED_OFF);
					TVSTORM_Front_Led_Control(LED_ETHERNET_RED, TVS_LED_ON);
					count++;
				}else{
					TVSTORM_Front_Led_Control(LED_ETHERNET_GREEN, TVS_LED_OFF);
					TVSTORM_Front_Led_Control(LED_ETHERNET_RED, TVS_LED_OFF);
					count++;
				}

				/* led green blinking is ended after blinking twice */
				if(count == 4){
					__system_property_set(PROPERTY_TVSTORM_CM_UPDATE_STATE, "0");
					count = 0;
				}
			}
			break;

			default:
			break;
		}
	}
}
#endif

void changeUpdateLEDStatus(int led_blinking)
{
	// Off RED LED 
	TVSTORM_Front_Led_Control(LED_POWER_RED, TVS_LED_OFF);
	TVSTORM_Front_Led_Control(LED_ETHERNET_RED, TVS_LED_OFF);
	TVSTORM_Front_Led_Control(LED_WIFI_RED, TVS_LED_OFF);

	if(led_blinking){
		TVSTORM_Front_Led_Control(LED_WIFI_GREEN, TVS_LED_ON);
		TVSTORM_Front_Led_Control(LED_ETHERNET_GREEN, TVS_LED_ON);
		TVSTORM_Front_Led_Control(LED_POWER_GREEN, TVS_LED_ON);
	}else{
		TVSTORM_Front_Led_Control(LED_WIFI_GREEN, TVS_LED_OFF);
		TVSTORM_Front_Led_Control(LED_ETHERNET_GREEN, TVS_LED_OFF);
		TVSTORM_Front_Led_Control(LED_POWER_GREEN, TVS_LED_OFF);
	}
}

int checkCurrentSystemState()
{
	 char property_tvstorm_standby_val[PROPERTY_VALUE_MAX] = {0, };
	 char property_tvstorm_update_val[PROPERTY_VALUE_MAX] = {0, };

	__system_property_get(PROPERTY_TVSTORM_STANDBY,property_tvstorm_standby_val);

	if(!strcmp("1", property_tvstorm_standby_val)){
		// Standby mode
		return TVSTORM_SYSTEM_STANDBY;
	}else{
		__system_property_get(PROPERTY_TVSTORM_PARTIAL_UPDATE, property_tvstorm_update_val);
		
		if(!strcmp("1", property_tvstorm_update_val)){
			//Partial update mode
			return TVSTORM_SYSTEM_PARTIAL_UPDATE; 
		}
	}

	//Wake up mode
	return TVSTORM_SYSTEM_WAKEUP;
}

int main(int argc, char **argv)
{
	static bool led_blink_status = false;

	frontLEDInit();

	while(true)
	{
		/* delay for 500ms */
		usleep(500 * 1000);

		// led blink status setting
		led_blink_status = !led_blink_status; 

		int currentState = checkCurrentSystemState();

		ALOGD(" currentState : %d", currentState);
		switch(currentState){
			case TVSTORM_SYSTEM_STANDBY:{
				__system_property_set(PROPERTY_TVSTORM_EEPROM_STANDBY,"on" );
				changePowerLEDStatus(true); 
			}
			break;

			case TVSTORM_SYSTEM_PARTIAL_UPDATE:{
				changeUpdateLEDStatus(led_blink_status);				
			}
			break;

			case TVSTORM_SYSTEM_WAKEUP:
			default: {
				/* wake up - active */
				__system_property_set(PROPERTY_TVSTORM_EEPROM_STANDBY,"off" );

				if(get_nagra_authentication_status() == false){
					/* Change Nagra LED Status */
					changeNagraLEDStatus(led_blink_status); 
				}else{
					/* Change Power LED Status */
					changePowerLEDStatus(false);
				}

				/* Change Ethernet LED Status */
				changeEthernetLEDStatus(led_blink_status);

				/* Only useful for Cable Modem */
#if SH960C_LN
				/* Change CM USB Update Status */
				changeCMUpdateStatus(led_blink_status);
#endif
				/* Change Wi-Fi LED Status */
				changeWifiLEDStatus(led_blink_status);
			}
		}
	}

	input_led_uninit();

	TVSTORM_Front_Led_Uninit();

	return 0;
}
