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

#ifndef HDD_CTRL_DETECTHDDTHREAD_H_
#define HDD_CTRL_DETECTHDDTHREAD_H_

#include "hdd_ctrl.h"
#include "HDDInfoStore.h"

class DetectHDDThread : public Thread
{
	int ns;
	struct sockaddr_nl	sa;
	struct timeval		tv;
	char buf_command[15];
	char buf[512];
	struct iovec iov;
	struct msghdr msg;
public:
	uint8_t				change_cnt;
	bool				mount_booting_flag;
	bool				two_non_ext4_detect_booting_flag;
	bool				formatSelected;

	HDDInfoStore		hddInfoStore[2];
	TempRemoveHDDInfo	tempRemoveHDDInfo[2];
	int8_t				selectedHDDIndex;
	int8_t				tempFormatSelectedHDDIndex;
	uint8_t				firstDetectedHDDIndex;

	int8_t				current_usb_index_for_buf_block_sdx[2];

#if SEARCH_HDD_ON_BOOTING == 1
	char				*SearchHDDOnBooting[HDD_EVENT_ON_BOOTING_MAX];
	int8_t				SearchHDDOnBootingIndex;
#endif

    ~DetectHDDThread();

    void onFirstRef();
    virtual status_t readyToRun();
    virtual bool threadLoop();

	void removeHDD(uint8_t current_usb_index, TempRemoveHDDInfo* tempRemoveHDDInfo = NULL);
	void detectHDD(int8_t current_usb_index);
	bool IsHDDQualified(int8_t index, char *idVendor, char *idProduct);
	void setHDDQualified();
#if DEV_FTS_USE == 1
	char* getKeyValue(const char* key);
	int8_t setKeyValue(const char* key, const char* val);
#else
	char* getSerialNumber();
	int8_t setSerialNumber(const char* val);
#endif
	void sendAddEvent(uint8_t index);
	void sendRemoveEvent(uint8_t index);
	void sendUpdateEvent(int8_t index);
	void sendHDDErrEvent(uint8_t index);

#if SEARCH_HDD_ON_BOOTING == 1
	uint8_t		getPartitionCount(const char *sdxName);
	inline bool	setEventForHDDOnBooting(char *ptr);
	void		addUEventForHDDOnBooting(const char *sdxName);
#endif
};

extern sp<DetectHDDThread> pDetectHDD;

extern bool ignore_change_msgiov_flag;

#endif /* HDD_CTRL_DETECTHDDTHREAD_H_ */
