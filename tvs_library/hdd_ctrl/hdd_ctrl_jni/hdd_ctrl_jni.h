// hdd_ctrl_jni.h

#ifndef HDD_CTRL_JNI_H
#define HDD_CTRL_JNI_H

#include <sys/types.h>

enum {
	HDD_ADD_DETECT			= 1,
	HDD_REMOVE_DETECT		= 2,
	HDD_UPDATE_DETECT		= 3,
	HDD_HDDERR_DETECT		= 4,
	HDD_NOT_SUPPORTED		= 5,
	HDD_MEDIA_DISK			= 6,
	HDD_GET_HDD_INFO        = 11,
	HDD_GET_REAR_FRONT      = 12,
	HDD_FORMAT          	= 21,
	HDD_USED_SIZE       	= 31,
	HDD_FREE_SIZE       	= 41,
	HDD_MOUNT		       	= 42,
	HDD_UNMOUNT		       	= 43,

	HDD_FILE_CANREAD		= 51,
	HDD_FILE_CANWRITE		= 52,
	HDD_FILE_EXISTS			= 53,
	HDD_FILE_MKDIR			= 54,
	HDD_FILE_MKDIRS			= 55,
	HDD_FILE_RENAMETO		= 56,
	HDD_FILE_NEWFILE		= 57,
	HDD_FILE_NEWPATHFILE	= 58,
	HDD_FILE_DELETE			= 59,
	HDD_FILE_ISDIRECTORY	= 60,
	HDD_FILE_ISFILE			= 61,
	HDD_FILE_LISTFILES		= 62,
	HDD_FILE_GETPARENT		= 63,
	HDD_FILE_GETNAME		= 64,
	HDD_FILE_GETPATH		= 65,
	HDD_FILE_LASTMODIFIED	= 66,
	HDD_FILE_CHANGE_MODE    = 67,
	HDD_FILE_FILE_SIZE    	= 68
};

enum {
	ETH_RENEW				= 115
};

enum {
	WIFI_GET_WIFI_STATUS_LED	= 125,
	WIFI_SET_WIFI_STATUS_LED	= 126,
	WIFI_WIFI_STATUS			= 127
};

enum {
	ECM_SET_DOCSIS_STATUS_LED   = 252
};

enum {
	    SET_POWER_LED           = 253
};


typedef struct _HDDInfo
{
	char    idProduct[10];
	char    idVendor[10];
	char    manufacturer[20];
	char    serial[30];
	char    product[20];

	long long	hdd_total_B_size;
	long long	hdd_actual_B_size;
	long long	hdd_Used_B_size;
	long long	hdd_Free_B_size;

	char    sdx_name[20];
	char    mounted_dir[80];
	char    fs_type[10];
	uint8_t ext4_flag;
	uint8_t partition_cnt;
} HDDInfoStruct;

typedef struct _FileName
{
	char filename[100];
} FileName;

typedef struct _FileList
{
	int file_cnt;
	FileName filearr[256];
} FileList;


uint8_t	hdd_jni_open();
void	hdd_jni_close();

void set_hdd_add_detect_callback(void (*ptr_hdd_add_detect)(HDDInfoStruct* pHDDInfo, uint8_t rear_front_index));
void set_hdd_remove_detect_callback(void (*ptr_hdd_remove_detect)(uint8_t rear_front_index));
void set_hdd_update_detect_callback(void (*ptr_hdd_update_detect)(HDDInfoStruct* pHDDInfo, uint8_t rear_front_index));
void set_hdd_hdderr_detect_callback(void (*ptr_hdd_hdderr_detect)(uint8_t rear_front_index));
void set_hdd_not_supported_callback(void (*ptr_hdd_not_supported)(uint8_t rear_front_index));
void set_hdd_media_disk_callback(void (*ptr_hdd_media_disk)(uint8_t rear_front_index));
void set_wifi_status_callback(void (*ptr_Wifi_status)(int status));

HDDInfoStruct*	hdd_get_hdd_info();
uint8_t		hdd_get_rear_front();
uint8_t		hdd_format();
long long	hdd_used_size();
long long	hdd_free_size();
uint8_t		hdd_mount();
uint8_t		hdd_unmount();

uint8_t file_canRead(char* filename);
uint8_t file_canWrite(char* filename);
uint8_t file_exists(char* filename);
uint8_t file_mkdir(char* pathname);
uint8_t file_mkdirs(char* pathname);
uint8_t file_renameTo(char* src_filename, char* dest_filename);
uint8_t file_newFile(char* pathname);
uint8_t file_newPathFile(char* pathname, char* filename);
uint8_t file_delete(char* pathname);
uint8_t file_isDirectory(char* pathname);
uint8_t file_isFile(char* filename);
FileList* file_listFiles(char* pathname);
char* file_getParent(char* pathname);
char* file_getName(char* filename);
char* file_getPath(char* filename);
long  file_lastModified(char* filename);
uint8_t file_changeMode(uint16_t change_mode, char* filename);
long file_fileSize(char* filename);

uint8_t eth_renew(char* ethstr);

uint8_t wifi_getWifiStatusLed();
uint8_t wifi_setWifiStatusLed(uint8_t wifi_status);

uint8_t setDocsisStatusLed(uint8_t docsis_status);

uint8_t setPowerLed(uint8_t power_status);

#endif	// HDD_CTRL_JNI_H
