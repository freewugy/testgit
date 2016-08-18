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

#ifndef HDD_CTRL_WAITUSERTHREAD_H_
#define HDD_CTRL_WAITUSERTHREAD_H_

#include "hdd_ctrl.h"

class WaitUserThread : public Thread
{
	bool slashFlag;

    uint8_t MsgConfig256[256];
    int sockfd;
    int clilen;
    struct sockaddr_in clientaddr;
	int nSockOpt;

    int txSockfd;
    int txClilen;
    struct sockaddr_in txServeraddr;

    int txSockfd_thread;
    int txClilen_thread;
    struct sockaddr_in txServeraddr_thread;
public:
	enum {
		HDD_ADD_DETECT			= 1,
		HDD_REMOVE_DETECT		= 2,
		HDD_UPDATE_DETECT		= 3,
		HDD_HDDERR_DETECT		= 4,
		HDD_NOT_SUPPORTED		= 5,
		HDD_MEDIA_DISK			= 6,
		HDD_GET_HDD_INFO		= 11,
		HDD_GET_REAR_FRONT		= 12,
		HDD_FORMAT				= 21,
		HDD_USED_SIZE			= 31,
		HDD_FREE_SIZE			= 41,
		HDD_MOUNT				= 42,
		HDD_UNMOUNT				= 43,

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
		HDD_FILE_CHANGE_MODE	= 67,
		HDD_FILE_FILE_SIZE		= 68
	};

    ~WaitUserThread();

    void onFirstRef();
    virtual status_t readyToRun();
    virtual bool threadLoop();

    void sendTo(uint8_t *sndMsgConfig, size_t szMsgConfig);
    void sendToThread(uint8_t *sndMsgConfig, size_t szMsgConfig);

	uint8_t file_util_makedirs(char* pathname);
	uint8_t file_util_mkdirs(char* pathname);
	uint8_t file_util_exists(char* filename);
	char*	file_util_getPath(char* filename);
	char*	file_util_getParent(char* pathname);
	char*	file_util_spaceProcessing(char* filename);

	void file_canRead(char* filename);
	void file_canWrite(char* filename);
	void file_exists(char* filename);
	void file_mkdir(char* pathname);
	void file_mkdirs(char* pathname);
	void file_renameTo(char* src_filename, char* dest_filename);
	void file_newFile(char* filename);
	void file_newPathFile(char* pathname, char* filename);
	void file_delete(char* pathname);
	void file_isDirectory(char* pathname);
	void file_isFile(char* filename);
	void file_listFiles(char* pathname);
	void file_getParent(char* filename);
	void file_getName(char* filename);
	void file_getPath(char* filename);
	void file_lastModified(char* filename);
	void file_changeMode(uint16_t change_mode, char* filename);
	void file_fileSize(char* filename);

	void eth_renew(char* ethstr);

	void wifi_getWifiStatusLed();
	void wifi_setWifiStatusLed(uint8_t wifi_status);

	void setDocsisStatusLed(uint8_t docsis_status);

	void setPowerLed(uint8_t power_status);
};

extern sp<WaitUserThread> pWaitUser;

#endif /* HDD_CTRL_WAITUSERTHREAD_H_ */
