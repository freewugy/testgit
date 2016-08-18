#ifndef HDD_CTRL_H
#define HDD_CTRL_H

#include <utils/Thread.h>
#include <utils/StrongPointer.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <utils/Vector.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>
#include <dlfcn.h>
#include "cutils/properties.h"
#include <errno.h>
#include <sys/statfs.h>
#include <pthread.h>

#include "flash_ts.h"

using namespace android;

#if defined(ST950I_LN)
//===== <LinkNet-IPTV: BCM7252, Android 5.0.0 Lollipop r2.0.1 + bcm patch> ======================================
#define WAIT_USER_HOST_PORT					0x5572
#define WAIT_USER_PROXY_PORT				0x5573
#define WAIT_USER_THREAD_PORT				0x5574

#define METAFILE_READ_WRITE_THREAD_PORT		0x5575

#define USB_SLOT_COUNT						2	//only 2 USB slots supported

#define NETLINK_TV_SEC						0			// 0s
#define NETLINK_TV_USEC						800000		// 800ms

#define DEV_FTS_USE							0
#define MOUNT_HERE_AGAINST_VOLD				0
#define REDUNDANCY_MOUNT                    1
#define REDUNDANCY_MOUNT_DIR_1              "/storage/sdcard1"
#define REDUNDANCY_MOUNT_DIR_2              "/storage/usbdisk0"
#define MAKE_DEV_BLOCK_VOLD_NODE			0
#define VOLUME_DEV_INFO_TO_VOLD_BY_FILE		1
#define RM_PREV_MOUNTED_DIR_ON_FORMAT		0

#define CHANGE_EVENT_USE					0
#define HDD_CTRL_TITLE						"Linknet-IPTV"
#define HDD_CTRL_VERSION					"1.1.13"
#define POSITION_NUM_STR_REAR1				"/4-1.1/"
#define POSITION_NUM_STR_REAR_PATH1			"/4-1.1"
#define POSITION_NUM_STR_REAR2				"/4-1.1/"
#define POSITION_NUM_STR_REAR_PATH2			"/4-1.1"
#define POSITION_NUM_STR_FRONT1				"/4-1.2/"
#define POSITION_NUM_STR_FRONT_PATH1		"/4-1.2"
#define POSITION_NUM_STR_FRONT2				"/4-1.2/"
#define POSITION_NUM_STR_FRONT_PATH2		"/4-1.2"
#define USB_DEVICE							"scsi_device/"
#define BLOCK_SD							"block/sd"
#define CUSTOMER_HOME						"/data/lkn_home/"
#define HDD_HOME							"/data/lkn_home/hdd_home/"
#define HDD_ALL								"/data/lkn_home/hdd_home/hdd_all"
#define HDD_WHITELIST						"/data/lkn_home/hdd_home/hdd_whitelist"
#define MNT_MEDIA_DIR						"/storage/"
#define TIMESHIFT_FILE_PREFIX				"timeshift"
#define TIMESHIFT_TS						"timeshift.ts"
#define TIMESHIFT_CTRL_DUMP					"timeshift_ctrl.dump"
#define TIMESHIFT_IDX_DUMP					"timeshift_idx.dump"
#define TIMESHIFT_SIZE					   	32212254720	// TimeShirt 30GB
#define SYSTEM_USED_SIZE					75509760	// System Used
#define ON_BOOTING_WAIT_TIME				0	//LinkNet-IPTV(Lollipop): 0 sec

#define SEARCH_HDD_ON_BOOTING				1

#if SEARCH_HDD_ON_BOOTING == 1
	#define SEARCH_HDD_ON_BOOTING_PREFIX_REAR		"/sys/devices/f0470500.ehci"
	#define SEARCH_HDD_ON_BOOTING_PREFIX_FRONT		"/sys/devices/f0470500.ehci"
#endif

#define FDISK_BIN							"busybox.lkn fdisk"
//===============================================================================================================
#elif defined(SH960C_LN)
//===== <LinkNet-IPTV: BCM7252, Android 5.0.0 Lollipop r2.0.1 + bcm patch> ======================================
#define WAIT_USER_HOST_PORT					0x5572
#define WAIT_USER_PROXY_PORT				0x5573
#define WAIT_USER_THREAD_PORT				0x5574

#define METAFILE_READ_WRITE_THREAD_PORT		0x5575

#define USB_SLOT_COUNT						2	//only 2 USB slots supported

#define NETLINK_TV_SEC						0			// 0s
#define NETLINK_TV_USEC						800000		// 800ms

#define DEV_FTS_USE							0
#define MOUNT_HERE_AGAINST_VOLD				0
#define REDUNDANCY_MOUNT                    1
#define REDUNDANCY_MOUNT_DIR_1              "/storage/sdcard1"
#define REDUNDANCY_MOUNT_DIR_2              "/storage/usbdisk0"
#define MAKE_DEV_BLOCK_VOLD_NODE			0
#define VOLUME_DEV_INFO_TO_VOLD_BY_FILE		1
#define RM_PREV_MOUNTED_DIR_ON_FORMAT		0

#define CHANGE_EVENT_USE					0
#define HDD_CTRL_TITLE						"Linknet-Cable 2nd"
#define HDD_CTRL_VERSION					"1.1.21"
#define POSITION_NUM_STR_REAR1				"/3-1.1/"
#define POSITION_NUM_STR_REAR_PATH1			"/3-1.1"
#define POSITION_NUM_STR_REAR2				"/3-1.1/"
#define POSITION_NUM_STR_REAR_PATH2			"/3-1.1"
#define POSITION_NUM_STR_FRONT1				"/3-1.2/"
#define POSITION_NUM_STR_FRONT_PATH1		"/3-1.2"
#define POSITION_NUM_STR_FRONT2				"/3-1.2/"
#define POSITION_NUM_STR_FRONT_PATH2		"/3-1.2"
#define USB_DEVICE							"scsi_device/"
#define BLOCK_SD							"block/sd"
#define CUSTOMER_HOME						"/data/lkn_home/"
#define HDD_HOME							"/data/lkn_home/hdd_home/"
#define HDD_ALL								"/data/lkn_home/hdd_home/hdd_all"
#define HDD_WHITELIST						"/data/lkn_home/hdd_home/hdd_whitelist"
#define MNT_MEDIA_DIR						"/storage/"
#define TIMESHIFT_FILE_PREFIX				"timeshift"
#define TIMESHIFT_TS						"timeshift.ts"
#define TIMESHIFT_CTRL_DUMP					"timeshift_ctrl.dump"
#define TIMESHIFT_IDX_DUMP					"timeshift_idx.dump"
#define TIMESHIFT_SIZE					   	32212254720	// TimeShirt 30GB
#define SYSTEM_USED_SIZE					75509760	// System Used
#define ON_BOOTING_WAIT_TIME				0	//LinkNet-IPTV(Lollipop): 0 sec

#define SEARCH_HDD_ON_BOOTING				1

#if SEARCH_HDD_ON_BOOTING == 1
	#define SEARCH_HDD_ON_BOOTING_PREFIX_REAR		"/sys/devices/f0470300.ehci"
	#define SEARCH_HDD_ON_BOOTING_PREFIX_FRONT		"/sys/devices/f0470300.ehci"
#endif

#define FDISK_BIN							"busybox.lkn fdisk"
//===============================================================================================================
#else
//===== <LinkNet-CABLE: Marvell BG2-CT, Android 4.2.2 JellyBean + marvell patch> ======================================
#define HDD_CTRL_TITLE						"Linknet-CABLE"
#define HDD_CTRL_VERSION					"1.0.0"

#define WAIT_USER_HOST_PORT					0x5562
#define WAIT_USER_PROXY_PORT				0x5563
#define WAIT_USER_THREAD_PORT				0x5564

#define METAFILE_READ_WRITE_THREAD_PORT		0x5565

#define USB_SLOT_COUNT						2	//only 2 USB slots supported

#define NETLINK_TV_SEC						0			// 0s
#define NETLINK_TV_USEC						500000		// 500ms

#define DEV_FTS_USE							1
#define MOUNT_HERE_AGAINST_VOLD				0
#define REDUNDANCY_MOUNT                    0
#define REDUNDANCY_MOUNT_DIR_1              ""
#define REDUNDANCY_MOUNT_DIR_2              ""
#define MAKE_DEV_BLOCK_VOLD_NODE			0
#define VOLUME_DEV_INFO_TO_VOLD_BY_FILE		1
#define RM_PREV_MOUNTED_DIR_ON_FORMAT		1

#define CHANGE_EVENT_USE					1
#define POSITION_NUM_STR_REAR1				"/1-1/"
#define POSITION_NUM_STR_REAR_PATH1			"/1-1"
#define POSITION_NUM_STR_REAR2				"/1-1/"
#define POSITION_NUM_STR_REAR_PATH2			"/1-1"
#define POSITION_NUM_STR_FRONT1				"/2-1/"
#define POSITION_NUM_STR_FRONT_PATH1		"/2-1"
#define POSITION_NUM_STR_FRONT2				"/2-1/"
#define POSITION_NUM_STR_FRONT_PATH2		"/2-1"
#define USB_DEVICE							"usb_device/"
#define BLOCK_SD							"block/sd"
#define CUSTOMER_HOME						"/data/lkn_home/"
#define HDD_HOME							"/data/lkn_home/hdd_home/"
#define HDD_ALL								"/data/lkn_home/hdd_home/hdd_all"
#define HDD_WHITELIST						"/data/lkn_home/hdd_home/hdd_whitelist"
#define MNT_MEDIA_DIR						"/mnt/media/"
#define TIMESHIFT_FILE_PREFIX				"TimeShift"
#define TIMESHIFT_TS						"TimeShift.ts"
#define TIMESHIFT_CTRL_DUMP					"TimeShift_ctrl.dump"
#define TIMESHIFT_IDX_DUMP					"TimeShift_idx.dump"
#define TIMESHIFT_SIZE						12884901888	// TimeShirt 12GB
#define SYSTEM_USED_SIZE					343945216	// System Used (209727488 + 134217728<128MB>)
#define ON_BOOTING_WAIT_TIME				5	//LinkNet-CABLE(JellyBean): 5 sec

#define SEARCH_HDD_ON_BOOTING				0

#if SEARCH_HDD_ON_BOOTING == 1
	#define SEARCH_HDD_ON_BOOTING_PREFIX_REAR		"/sys/devices/soc.0/f7ed0000.usb"
	#define SEARCH_HDD_ON_BOOTING_PREFIX_FRONT		"/sys/devices/soc.0/f7ee0000.usb"
#endif

#define FDISK_BIN							"busybox fdisk"
//===============================================================================================================
#endif

#if SEARCH_HDD_ON_BOOTING == 1
	#define	HDD_EVENT_ON_BOOTING_MAX	100
#endif

typedef struct _HDDInfo
{
	char	idProduct[10];
	char	idVendor[10];
	char	manufacturer[20];
	char	serial[30];
	char	product[20];

	long long	hdd_total_B_size;
	long long	hdd_actual_B_size;
	long long	hdd_used_B_size;
	long long	hdd_free_B_size;

	char	sdx_name[20];
	char	mounted_dir[80];
	char	fs_type[10];
	uint8_t	ext4_flag;
	uint8_t partition_cnt;
} HDDInfoStruct;

typedef struct _PartitionBlock
{
	uint8_t partition_no;
	char	buf_block_sdx_sdxx[50];
	char	dev_name[10];
	char	mounted_dir[80];
} PartitionBlock;

typedef struct _FileName
{
	char filename[100];
} FileName;

typedef struct _FileList
{
	int file_cnt;
	FileName filearr[256];
} FileList;

typedef struct _TempRemoveHDDInfo
{
	bool			GPTFlag;
	HDDInfoStruct	HDDInfo;
	char			dev_name[10];
	uint8_t			partition_cnt;
} TempRemoveHDDInfo;

//<FOR READ CASE>
//
//<Protocol: Java to CPP>
//
//  01: Start of heading
//  Command: 1 byte (read(0x11))
//  02: Start of text
//  Filename
//  03: End of text
//  04: End of transmit
//
//<Protocol: CPP to Java>
//
//  01: Start of heading
//  Command: ACK(0x06)
//  02: Start of text
//  03: End of text
//  Contents
//  04: End of transmit OR 0x15: NAK

//<FOR WRITE CASE>
//
//<Protocol: Java to CPP>
//
//  01: Start of heading
//  Command: 1 byte (write(0x12))
//  02: Start of text
//  Filename
//  03: End of text
//  Contents
//  04: End of transmit
//
//<Protocol: CPP to Java>
//
//  01: Start of heading
//  Command: ACK(0x06)
//  02: Start of text
//  03: End of text
//  04: End of transmit OR 0x15: NAK

extern bool thread_running_flag;

#endif	// HDD_CTRL_H
