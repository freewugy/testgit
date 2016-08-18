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

#ifndef HDD_CTRL_HDDINFOSTORE_H_
#define HDD_CTRL_HDDINFOSTORE_H_

#include "hdd_ctrl.h"

class HDDInfoStore : public RefBase
{
public:
	bool					SlotConnected;
	bool					ChangeEventReceived;
	bool					GPTFlag;
	bool					HDDErrFlag;
	HDDInfoStruct			HDDInfo;
	char					dev_name[10];
	Vector<PartitionBlock>	vPartitionBlock;
	uint8_t					system_partition_cnt;

	char buf_usb_device[200];
	char buf_block_sdx[50];
	char buf_block_sdx_buf_all[200];
	char buf_block_sdx_sdx1[50];

	bool hdd_qualified;
	bool add_event_sent_flag;

	HDDInfoStore();
	~HDDInfoStore();

	void ClearHDDInfoStore();

	void searchMountInfo(uint8_t hdd_index, char* mounted_dir, char* fs_type, char* rw_mode, char* serial, char* sdx_name, char* dev_name, bool &ext4_flag);
	void convertSerialNumberToASCII(char* serialNo);
	void GetHDDInfo(uint8_t hdd_index);
	void MakeNodeToVoldDir(char *dev_name, char mount_case);
	void Ext4Mount(char *serial, bool fstype_gpt_flag, char *dev_name, char *sdx_name, /*bool on_booting,*/ char *mounted_dir, char *fs_type, char *fstype, bool &ext4_flag);
	void VfatMount(char *serial, bool fstype_gpt_flag, char *dev_name, char *sdx_name, /*bool on_booting,*/ char *mounted_dir, char *fs_type, char *fstype, bool &ext4_flag);
	void MakeFsByFdisk(const char *fsPath, uint8_t partition_cnt);
	long long GetHDDSize(char* buf_block_sdx);
	void GetHDDActualSize(bool subtract_flag, bool from_format_flag);
	long long GetSizeOfTimeShift(const char *mounted_dir);
	bool FormatHDDToExt4();
	char* getMountpoint(const char *fsPath);
	bool unMountpoint(const char *fsPath, bool device_rm_flag);
	char* getMajorMinor(const char *sdxName);
	bool MountHDD(bool send_add_event_flag);
	bool UnmountHDD();
	char* MountHDDExt4(char* dev_name, char* serial);
	bool UnmountHDDExt4(char* mounted_dir, char* dev_name);
	int  getNumOfPartition(const char *sdx);
};

extern bool fs_ext2_supported;
extern char fs_ext_str[5];

extern bool on_formatting_flag;

#endif /* HDD_CTRL_HDDINFOSTORE_H_ */
