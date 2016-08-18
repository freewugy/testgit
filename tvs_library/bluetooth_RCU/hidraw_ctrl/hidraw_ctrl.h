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
#ifndef _HIDRAW_CTRL_H_
#define _HIDRAW_CTRL_H_

/*
 * IDs.
 */
#define ID_BUS          0
#define ID_VENDOR       1
#define ID_PRODUCT      2
#define ID_VERSION      3

#define BUS_PCI         0x01
#define BUS_ISAPNP      0x02
#define BUS_USB         0x03
#define BUS_HIL         0x04
#define BUS_BLUETOOTH       0x05
#define BUS_VIRTUAL     0x06

#define BUS_ISA         0x10
#define BUS_I8042       0x11
#define BUS_XTKBD       0x12
#define BUS_RS232       0x13
#define BUS_GAMEPORT        0x14
#define BUS_PARPORT     0x15
#define BUS_AMIGA       0x16
#define BUS_ADB         0x17
#define BUS_I2C         0x18
#define BUS_HOST        0x19
#define BUS_GSC         0x1A
#define BUS_ATARI       0x1B
#define BUS_SPI         0x1C

#define HID_MAX_DESCRIPTOR_SIZE          4096

struct hidraw_report_descriptor {
	__u32 size;
	__u8 value[HID_MAX_DESCRIPTOR_SIZE];
};

struct hidraw_devinfo {
	__u32 bustype;
	__s16 vendor;
	__s16 product;
};

/* ioctl interface */
#define HIDIOCGRDESCSIZE	_IOR('H', 0x01, int)
#define HIDIOCGRDESC		_IOR('H', 0x02, struct hidraw_report_descriptor)
#define HIDIOCGRAWINFO		_IOR('H', 0x03, struct hidraw_devinfo)
#define HIDIOCGRAWNAME(len)     _IOC(_IOC_READ, 'H', 0x04, len)
#define HIDIOCGRAWPHYS(len)     _IOC(_IOC_READ, 'H', 0x05, len)
/* The first byte of SFEATURE and GFEATURE is the report number */
#define HIDIOCSFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x06, len)
#define HIDIOCGFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x07, len)

#define HIDRAW_FIRST_MINOR 0
#define HIDRAW_MAX_DEVICES 64
/* number of reports to buffer */
#define HIDRAW_BUFFER_SIZE 64


// TVSTORM HIDRAWCTRL Version
#define TVSTORM_HIDRAWCTRL_VERSION 1
#define TVSTORM_HIDRAWCTRL_PATCHLEVEL 2
#define TVSTORM_HIDRAWCTRL_SUBLEVEL 6

// TVSTORM HIDRAWCTRL date
#define TVSTORM_HIDRAWCTRL_YEAR 2015
#define TVSTORM_HIDRAWCTRL_MONTH 7
#define TVSTORM_HIDRAWCTRL_DAY 4

typedef struct TVSTORM_HIDRAWCTRL_VERSION_S
{
    uint16_t    version;
    uint16_t    patchlevel;
    uint16_t    sublevel;
}TVSTORM_HIDRAWCTRL_VERSION_T;

typedef struct TVSTORM_HIDRAWCTRL_DATE_S
{
    uint16_t    year;
    uint16_t    month;
    uint16_t    day;
}TVSTORM_HIDRAWCTRL_DATE_T;

const char *bus_str(int bus);

//hidraw open/close
static int  hidraw_open(const char *_dname, int _mode);
static void hidraw_close(void);

//snd_bt driver open/close
static int soundcard_open(int _mode);
static void soundcard_close(void);

//snd_bt poll
static int  snd_bt_card_capture_detecter_open(void);
static void  snd_bt_card_capture_detecter_close(void);

//hidraw receiver
static int  hidraw_thread_open(void);
static void  hidraw_thread_close(void);

//hidraw open/close
static int hidraw_inotify_open(int _mode);
static void hidraw_inotify_close(void);

//uinput open/close
static int virtual_uinput_open(void);
static void virtual_uinput_close(void);

void snd_bt_card_capture_detect(void * data);
void hidraw_receiver(void * data);

//inotify
void hidraw_inotify_get_event (int fd, const char * target);
void handle_error (int error);

// request RCU info
int hidraw_request_Rcu_Version(int fd);
int hidraw_request_Rcu_BatteryLevel(int fd);

void TVSTORM_HideawCtrl_Version(TVSTORM_HIDRAWCTRL_VERSION_T *hidctrl_version);
void TVSTORM_HideawCtrl_LastDate(TVSTORM_HIDRAWCTRL_DATE_T *hidctrll_date);

#endif  /* _HIDRAW_CTRL_H_ */
