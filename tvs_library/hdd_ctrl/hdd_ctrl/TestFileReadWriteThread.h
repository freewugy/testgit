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

#ifndef HDD_CTRL_TESTFILEREADWRITETHREAD_H_
#define HDD_CTRL_TESTFILEREADWRITETHREAD_H_

#include "hdd_ctrl.h"

#define CMD_NONE		0x00
#define CMD_LIST    	0x01   
#define CMD_READ    	0x02   
#define CMD_WRITE   	0x03
#define CMD_ACK			0x04
#define CMD_NACK		0x05

#define STA_FAIL	   	0x00   
#define STA_OK       	0x01 
#define STA_CON			0x02

#define CMD_LEN			0x04

#define PAYLOAD_LEN		0x400   
#define HEADER_LEN		0x04

#define PACKET_LEN			(PAYLOAD_LEN)

class TestFileReadWriteThread : public Thread
{
	bool bWriteFlag;

	int server_sockfd, client_sockfd;
	int server_len, client_len;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	int nSockOpt;
	char recvbuf[PACKET_LEN];
	char sendbuf[PACKET_LEN];

	bool sendCmd(char cmd, char status);
	bool sendData(char *data, unsigned short len);
	
	bool hasPayload(void);
	char readCmd(void);
	unsigned short readData(char *data, unsigned short maxlen);

	void onFileList(void);
	void onFileRead(void);
	void onFileWrite(void);

public:
	~TestFileReadWriteThread();

    void onFirstRef();
    virtual status_t readyToRun();
    virtual bool threadLoop();
};

#endif /* HDD_CTRL_TESTFILEREADWRITETHREAD_H_ */
