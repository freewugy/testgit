// hdd_ctrl_jni_test.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>

enum {
	HDD_ADD_DETECT			= 1,
	HDD_REMOVE_DETECT		= 2,
	HDD_UPDATE_DETECT		= 3,
	HDD_GET_HDD_INFO        = 11,
	HDD_GET_REAR_FRONT      = 12,
	HDD_FORMAT          	= 21,
	HDD_USED_SIZE       	= 31,
	HDD_FREE_SIZE       	= 41,
	HDD_MOUNT		       	= 42,
	HDD_UNMOUNT		       	= 43,

	HDD_FILE_CANREAD        = 51,
	HDD_FILE_CANWRITE       = 52,
	HDD_FILE_EXISTS         = 53,
	HDD_FILE_MKDIR          = 54,
	HDD_FILE_MKDIRS         = 55,
	HDD_FILE_RENAMETO       = 56,
	HDD_FILE_NEWFILE        = 57,
	HDD_FILE_NEWPATHFILE    = 58,
	HDD_FILE_DELETE         = 59,
	HDD_FILE_ISDIRECTORY    = 60,
	HDD_FILE_ISFILE         = 61,
	HDD_FILE_LISTFILES      = 62,
	HDD_FILE_GETPARENT      = 63,
	HDD_FILE_GETNAME        = 64,
	HDD_FILE_GETPATH        = 65,
	HDD_FILE_LASTMODIFIED   = 66,
	HDD_FILE_CHANGE_MODE    = 67,
	HDD_FILE_FILE_SIZE	    = 68
};

enum {
	ETH_RENEW				= 115
};

enum {
	WIFI_GET_WIFI_STATUS_LED	= 125,
	WIFI_SET_WIFI_STATUS_LED	= 126,
	WIFI_WIFI_STATUS            = 127
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


void hdd_add_detect_jni_test(HDDInfoStruct* pHDDInfo, uint8_t rear_front_index)
{
	printf("------------------> hdd_add_detect_jni_test\n");
	printf("---------> idProduct [%s]\n", pHDDInfo->idProduct);
	printf("---------> idVendor [%s]\n", pHDDInfo->idVendor);
	printf("---------> manufacturer [%s]\n", pHDDInfo->manufacturer);
	printf("---------> serial [%s]\n", pHDDInfo->serial);
	printf("---------> product [%s]\n", pHDDInfo->product);
	printf("---------> hdd_total_B_size [%lld]\n", pHDDInfo->hdd_total_B_size);
	printf("---------> hdd_actual_B_size [%lld]\n", pHDDInfo->hdd_actual_B_size);
	printf("---------> hdd_Used_B_size [%lld]\n", pHDDInfo->hdd_Used_B_size);
	printf("---------> hdd_Free_B_size [%lld]\n", pHDDInfo->hdd_Free_B_size);
	printf("---------> sdx_name [%s]\n", pHDDInfo->sdx_name);
	printf("---------> mounted_dir [%s]\n", pHDDInfo->mounted_dir);
	printf("---------> fs_type [%s]\n", pHDDInfo->fs_type);
	printf("---------> ext4_flag [%d]\n", pHDDInfo->ext4_flag);
	printf("---------> partition_cnt [%d]\n", pHDDInfo->partition_cnt);
	printf("\n---------> rear_front_index=[%d], [%s]\n", rear_front_index, rear_front_index == 0 ? "rear" : "front");
}

void hdd_remove_detect_jni_test(uint8_t rear_front_index)
{
	printf("------------------> hdd_remove_detect_jni_test\n");
	printf("\n---------> rear_front_index=[%d], [%s]\n", rear_front_index, rear_front_index == 0 ? "rear" : "front");
}

void hdd_update_detect_jni_test(HDDInfoStruct* pHDDInfo, uint8_t rear_front_index)
{
	printf("------------------> hdd_update_detect_jni_test\n");
	printf("---------> idProduct [%s]\n", pHDDInfo->idProduct);
	printf("---------> idVendor [%s]\n", pHDDInfo->idVendor);
	printf("---------> manufacturer [%s]\n", pHDDInfo->manufacturer);
	printf("---------> serial [%s]\n", pHDDInfo->serial);
	printf("---------> product [%s]\n", pHDDInfo->product);
	printf("---------> hdd_total_B_size [%lld]\n", pHDDInfo->hdd_total_B_size);
	printf("---------> hdd_actual_B_size [%lld]\n", pHDDInfo->hdd_actual_B_size);
	printf("---------> hdd_Used_B_size [%lld]\n", pHDDInfo->hdd_Used_B_size);
	printf("---------> hdd_Free_B_size [%lld]\n", pHDDInfo->hdd_Free_B_size);
	printf("---------> sdx_name [%s]\n", pHDDInfo->sdx_name);
	printf("---------> mounted_dir [%s]\n", pHDDInfo->mounted_dir);
	printf("---------> fs_type [%s]\n", pHDDInfo->fs_type);
	printf("---------> ext4_flag [%d]\n", pHDDInfo->ext4_flag);
	printf("---------> partition_cnt [%d]\n", pHDDInfo->partition_cnt);
	printf("\n---------> rear_front_index=[%d], [%s]\n", rear_front_index, rear_front_index == 0 ? "rear" : "front");
}

void wifi_status_jni_test(int status)
{
	if (status == 0)
		printf("--------------------> [wifi_status_jni_test] status (OFF)\n");
	else if (status == 1)
		printf("--------------------> [wifi_status_jni_test] status (BLINKING)\n");
	else if (status == 2)
		printf("--------------------> [wifi_status_jni_test] status (ON)\n");
}

int main()
{
	char buf[10];

	void *handle;

	int     (*hdd_jni_open_func)();
	void    (*hdd_jni_close_func)();

	void	(*set_hdd_add_detect_callback_func)(void (*ptr_hdd_add_detect)(HDDInfoStruct* pHDDInfo, uint8_t rear_front_index));
	void	(*set_hdd_remove_detect_callback_func)(void (*ptr_hdd_remove_detect)(uint8_t rear_front_index));
	void	(*set_hdd_update_detect_callback_func)(void (*ptr_hdd_update_detect)(HDDInfoStruct* pHDDInfo, uint8_t rear_front_index));
	void	(*set_wifi_status_callback_func)(void (*ptr_wifi_status)(int));

	HDDInfoStruct*	(*hdd_get_hdd_info_func)();
	int				(*hdd_get_rear_front_func)();
	int				(*hdd_format_func)();
	long long		(*hdd_used_size_func)();
	long long		(*hdd_free_size_func)();
	uint8_t			(*hdd_mount_func)();
	uint8_t			(*hdd_unmount_func)();

	uint8_t		(*file_canRead_func)(char*);
	uint8_t		(*file_canWrite_func)(char*);
	uint8_t		(*file_exists_func)(char*);
	uint8_t		(*file_mkdir_func)(char*);
	uint8_t		(*file_mkdirs_func)(char*);
	uint8_t		(*file_renameTo_func)(char*, char*);
	uint8_t		(*file_newFile_func)(char*);
	uint8_t		(*file_newPathFile_func)(char*, char*);
	uint8_t		(*file_delete_func)(char*);
	uint8_t		(*file_isDirectory_func)(char*);
	uint8_t		(*file_isFile_func)(char*);
	FileList*	(*file_listFiles_func)(char*);
	char*		(*file_getParent_func)(char*);
	char*		(*file_getName_func)(char*);
	char*		(*file_getPath_func)(char*);
	long		(*file_lastModified_func)(char*);
	uint8_t		(*file_changeMode_func)(uint16_t, char*);
	long		(*file_fileSize_func)(char*);

	uint8_t		(*eth_renew_func)(char*);

	uint8_t		(*wifi_getWifiStatusLed_func)();
	uint8_t		(*wifi_setWifiStatusLed_func)(uint8_t);

	uint8_t		(*setPowerLed_func)(uint8_t);

	handle = dlopen("/system/lib/libhddctrljni2.so", RTLD_NOW);

	hdd_jni_open_func = (int(*)()) dlsym( handle, "hdd_jni_open" );
	hdd_jni_close_func = (void(*)()) dlsym( handle, "hdd_jni_close" );

	set_hdd_add_detect_callback_func = (void(*)(void (*)(HDDInfoStruct*, uint8_t))) dlsym( handle, "set_hdd_add_detect_callback" );
	set_hdd_remove_detect_callback_func = (void(*)(void (*)(uint8_t))) dlsym( handle, "set_hdd_remove_detect_callback" );
	set_hdd_update_detect_callback_func = (void(*)(void (*)(HDDInfoStruct*, uint8_t))) dlsym( handle, "set_hdd_update_detect_callback" );
	set_wifi_status_callback_func = (void(*)(void (*)(int))) dlsym( handle, "set_wifi_status_callback" );

	hdd_get_hdd_info_func = (HDDInfoStruct*(*)()) dlsym( handle, "hdd_get_hdd_info" );
	hdd_get_rear_front_func = (int(*)()) dlsym( handle, "hdd_get_rear_front" );
	hdd_format_func = (int(*)()) dlsym( handle, "hdd_format" );
	hdd_used_size_func = (long long(*)()) dlsym( handle, "hdd_used_size" );
	hdd_free_size_func = (long long(*)()) dlsym( handle, "hdd_free_size" );
	hdd_mount_func = (uint8_t(*)()) dlsym( handle, "hdd_mount" );
	hdd_unmount_func = (uint8_t(*)()) dlsym( handle, "hdd_unmount" );

	file_canRead_func		= (uint8_t(*)(char*)) dlsym( handle, "file_canRead" );
	file_canWrite_func		= (uint8_t(*)(char*)) dlsym( handle, "file_canWrite" );
	file_exists_func		= (uint8_t(*)(char*)) dlsym( handle, "file_exists" );
	file_mkdir_func			= (uint8_t(*)(char*)) dlsym( handle, "file_mkdir" );
	file_mkdirs_func		= (uint8_t(*)(char*)) dlsym( handle, "file_mkdirs" );
	file_renameTo_func		= (uint8_t(*)(char*,char*)) dlsym( handle, "file_renameTo" );
	file_newFile_func		= (uint8_t(*)(char*)) dlsym( handle, "file_newFile" );
	file_newPathFile_func	= (uint8_t(*)(char*,char*)) dlsym( handle, "file_newPathFile" );
	file_delete_func		= (uint8_t(*)(char*)) dlsym( handle, "file_delete" );
	file_isDirectory_func	= (uint8_t(*)(char*)) dlsym( handle, "file_isDirectory" );
	file_isFile_func		= (uint8_t(*)(char*)) dlsym( handle, "file_isFile" );
	file_listFiles_func		= (FileList*(*)(char*)) dlsym( handle, "file_listFiles" );
	file_getParent_func		= (char*(*)(char*)) dlsym( handle, "file_getParent" );
	file_getName_func		= (char*(*)(char*)) dlsym( handle, "file_getName" );
	file_getPath_func		= (char*(*)(char*)) dlsym( handle, "file_getPath" );
	file_lastModified_func	= (long(*)(char*)) dlsym( handle, "file_lastModified" );
	file_changeMode_func	= (uint8_t(*)(uint16_t, char*)) dlsym( handle, "file_changeMode" );
	file_fileSize_func		= (long(*)(char*)) dlsym( handle, "file_fileSize" );

	eth_renew_func			= (uint8_t(*)(char*)) dlsym( handle, "eth_renew" );

	wifi_getWifiStatusLed_func = (uint8_t(*)()) dlsym( handle, "wifi_getWifiStatusLed" );
	wifi_setWifiStatusLed_func = (uint8_t(*)(uint8_t)) dlsym( handle, "wifi_setWifiStatusLed" );

	setPowerLed_func = (uint8_t(*)(uint8_t)) dlsym( handle, "setPowerLed" );

	if(hdd_jni_open_func() != 1)
    {
        printf("[hdd_ctrl_jni_test] hdd_jni_open() error\n");
    }

	if (set_hdd_add_detect_callback_func != NULL)
		set_hdd_add_detect_callback_func(hdd_add_detect_jni_test);
	if (set_hdd_remove_detect_callback_func != NULL)
		set_hdd_remove_detect_callback_func(hdd_remove_detect_jni_test);
	if (set_hdd_update_detect_callback_func != NULL)
		set_hdd_update_detect_callback_func(hdd_update_detect_jni_test);

	if (set_wifi_status_callback_func != NULL)
		set_wifi_status_callback_func(wifi_status_jni_test);

	memset(buf, 0x00, 10);
	printf(">> ");
	while(fgets(buf, 10, stdin) != NULL)
	{
		switch(atoi(buf))
		{
	
			case HDD_GET_HDD_INFO:
				{
					HDDInfoStruct* pHDDInfo = hdd_get_hdd_info_func();
					printf("------------------> get_hdd_info\n");
					printf("---------> idProduct [%s]\n", pHDDInfo->idProduct);
					printf("---------> idVendor [%s]\n", pHDDInfo->idVendor);
					printf("---------> manufacturer [%s]\n", pHDDInfo->manufacturer);
					printf("---------> serial [%s]\n", pHDDInfo->serial);
					printf("---------> product [%s]\n", pHDDInfo->product);
					printf("---------> hdd_total_B_size [%lld]\n", pHDDInfo->hdd_total_B_size);
					printf("---------> hdd_actual_B_size [%lld]\n", pHDDInfo->hdd_actual_B_size);
					printf("---------> hdd_Used_B_size [%lld]\n", pHDDInfo->hdd_Used_B_size);
					printf("---------> hdd_Free_B_size [%lld]\n", pHDDInfo->hdd_Free_B_size);
					printf("---------> sdx_name [%s]\n", pHDDInfo->sdx_name);
					printf("---------> mounted_dir [%s]\n", pHDDInfo->mounted_dir);
					printf("---------> fs_type [%s]\n", pHDDInfo->fs_type);
					printf("---------> ext4_flag [%d]\n", pHDDInfo->ext4_flag);
					printf("---------> partition_cnt [%d]\n", pHDDInfo->partition_cnt);
				}
				break;
			case HDD_GET_REAR_FRONT:
				{
					int ret = hdd_get_rear_front_func();
					switch (ret)
					{
					case 0:
						printf("--------------------------------------> hdd get rear front: [REAR]\n");
						break;
					case 1:
						printf("--------------------------------------> hdd get rear front: [FRONT]\n");
						break;
					case 2:
						printf("--------------------------------------> hdd get rear front: [HDD NOT INSERTED]\n");
						break;
					default:
						printf("--------------------------------------> hdd get rear front: [ERROR]\n");
						break;
					}
				}
				break;
			case HDD_FORMAT:
				{
					int ret = hdd_format_func();
					printf("--------------------------------------> hdd format: [%d]\n", ret);
				}
				break;
			case HDD_USED_SIZE:
				{
					long long ret = hdd_used_size_func();
					printf("--------------------------------------> hdd used size: [%lld]\n", ret);
				}
				break;
			case HDD_FREE_SIZE:
				{
					long long ret = hdd_free_size_func();
					printf("--------------------------------------> hdd free size: [%lld]\n", ret);
				}
				break;
			case HDD_MOUNT:
				{
					uint8_t ret = hdd_mount_func();
					printf("--------------------------------------> hdd mount: [%d]\n", ret);
				}
				break;
			case HDD_UNMOUNT:
				{
					uint8_t ret = hdd_unmount_func();
					printf("--------------------------------------> hdd unmount: [%d]\n", ret);
				}
				break;
			case HDD_FILE_CANREAD:
				{
					int ret = file_canRead_func("/filetest/file test.txt");
					printf("--------------------------------------> file canRead: [%d]\n", ret);
				}
				break;
			case HDD_FILE_CANWRITE:
				{
					int ret = file_canWrite_func("/filetest/file test.txt");
					printf("--------------------------------------> file canWrite: [%d]\n", ret);
				}
				break;
			case HDD_FILE_EXISTS:
				{
					int ret = file_exists_func("/filetest/file test.txt");
					printf("--------------------------------------> file exists: [%d]\n", ret);
				}
				break;
			case HDD_FILE_MKDIR:
				{
					int ret = file_mkdir_func("/file test");
					printf("--------------------------------------> file mkdir: [%d]\n", ret);
				}
				break;
			case HDD_FILE_MKDIRS:
				{
					int ret = file_mkdirs_func("/file test/a/b/c");
					printf("--------------------------------------> file mkdirs: [%d]\n", ret);
				}
				break;
			case HDD_FILE_RENAMETO:
				{
					int ret = file_renameTo_func("/filetest/file test.txt", "/filetest/file test2.txt");
					printf("--------------------------------------> file renameTo: [%d]\n", ret);
				}
				break;
			case HDD_FILE_NEWFILE:
				{
					int ret = file_newFile_func("/filetest/file test.txt");
					printf("--------------------------------------> file newFile: [%d]\n", ret);
				}
				break;
			case HDD_FILE_NEWPATHFILE:
				{
					int ret = file_newPathFile_func("/filetest", "file test2.txt");
					printf("--------------------------------------> file newPathFile: [%d]\n", ret);
				}
				break;
			case HDD_FILE_DELETE:
				{
					int ret = file_delete_func("/filetest/file test.txt");
					printf("--------------------------------------> file delete: [%d]\n", ret);
				}
				break;
			case HDD_FILE_ISDIRECTORY:
				{
					int ret = file_isDirectory_func("/file test");
					printf("--------------------------------------> file isDirectory: [%d]\n", ret);
				}
				break;
			case HDD_FILE_ISFILE:
				{
					int ret = file_isFile_func("/file test/file test.txt");
					printf("--------------------------------------> file isFile: [%d]\n", ret);
				}
				break;
			case HDD_FILE_LISTFILES:
				{
					int i;
					FileList* ret = file_listFiles_func("/file test");
					for (i = 0; i < ret->file_cnt; i++)
					{
						printf("---------> file listFiles[%d]: [%s]\n", i, ret->filearr[i].filename);
					}
				}
				break;
			case HDD_FILE_GETPARENT:
				{
					char* ret = file_getParent_func("/file test");
					printf("---------> file getParent: [%s]\n", ret);
				}
				break;
			case HDD_FILE_GETNAME:
				{
					char* ret = file_getName_func("/filetest/file test.txt");
					printf("---------> file getName: [%s]\n", ret);
				}
				break;
			case HDD_FILE_GETPATH:
				{
					char* ret = file_getPath_func("/filetest/file test.txt");
					printf("---------> file getPath: [%s]\n", ret);
				}
				break;
			case HDD_FILE_LASTMODIFIED:
				{
					long st_mtime = file_lastModified_func("/filetest/file test.txt");
					printf("---------> file lastModified: [%ld]\n", st_mtime);
				}
				break;
			case HDD_FILE_CHANGE_MODE:
				{
					int ret = file_changeMode_func(754, "/file test/a aa");
					printf("--------------------------------------> file changeMode: [%d]\n", ret);
				}
				break;
			case HDD_FILE_FILE_SIZE:
				{
					long ret = file_fileSize_func("/filetest/file test.txt");
					printf("---------> file fileSize: [%ld]\n", ret);
				}
				break;

			case ETH_RENEW:
				{
					int ret = eth_renew_func("eth0");
					printf("---------> eth renew: [%d]\n", ret);
				}
				break;

			case WIFI_GET_WIFI_STATUS_LED:
				{
					int ret = wifi_getWifiStatusLed_func();
					printf("---------> wifi getWifiStatusLed: [%d]\n", ret);
				}
				break;
			case WIFI_SET_WIFI_STATUS_LED:
				{
					int ret = wifi_setWifiStatusLed_func(1);
					printf("---------> wifi setWifiStatusLed: [%d]\n", ret);
				}
				break;
			case SET_POWER_LED:
				{
					int ret = setPowerLed_func(0);  // POWER LED OFF
					printf("---------> setPowerLed: [%d]\n", ret);
				}
				break;
			default:
				break;
		}
		memset(buf, 0x00, 10);
		printf(">> ");
	}

	if(ferror(stdin))
		perror("input error");

	hdd_jni_close_func();

	dlclose(handle);

	return 0;
}
