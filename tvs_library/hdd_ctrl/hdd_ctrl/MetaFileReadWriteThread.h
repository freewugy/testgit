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

#ifndef HDD_CTRL_METAFILEREADWRITETHREAD_H_
#define HDD_CTRL_METAFILEREADWRITETHREAD_H_

#include "hdd_ctrl.h"

class MetaFileReadWriteThread : public Thread
{
	bool bWriteFlag;

	int server_sockfd, client_sockfd;
	int server_len, client_len;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	int nSockOpt;
public:
	enum {
		NONE_MODE		= 0,
		COMMAND_MODE	= 1,
		FILENAME_MODE	= 2,
		CONTENTS_MODE	= 3
	};

	~MetaFileReadWriteThread();

    void onFirstRef();
    virtual status_t readyToRun();
    virtual bool threadLoop();
};

#endif /* HDD_CTRL_METAFILEREADWRITETHREAD_H_ */
