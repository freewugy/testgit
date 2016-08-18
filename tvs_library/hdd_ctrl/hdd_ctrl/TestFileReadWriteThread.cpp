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

#define TVS_LOG_ENABLED
#define LOG_TAG "TestFileReadWriteThread"

#include "TestFileReadWriteThread.h"
#include "DetectHDDThread.h"
#include "WaitUserThread.h"


TestFileReadWriteThread::~TestFileReadWriteThread()
{
	close( server_sockfd );
}

void TestFileReadWriteThread::onFirstRef()
{
	ALOGD("TestFileReadWriteThread::onFirstRef()\n");

	client_sockfd = -1;
	
	bWriteFlag = true;

	server_sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr( "127.0.0.1" );
//	server_address.sin_addr.s_addr = htonl( INADDR_ANY);
	server_address.sin_port = htons( METAFILE_READ_WRITE_THREAD_PORT + 1);
	

	server_len = sizeof( server_address );

	// prevent bind error
	nSockOpt = 1;
	setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &nSockOpt, sizeof(nSockOpt));

	if ( bind(server_sockfd, ( struct sockaddr *)&server_address, server_len) != 0 )
	{
		ALOGE("bind error : ");
		return ;
	}

	listen( server_sockfd, 5 );

	ALOGD( "server wait...\n" );
}

status_t TestFileReadWriteThread::readyToRun()
{
	ALOGD("TestFileReadWriteThread::readyToRun()\n");
	return 0;
}


bool TestFileReadWriteThread::sendCmd(char cmd, char status)
{
	const char CMD_KEY[] = {'C','M'};
		
	if (client_sockfd < 0)
		return false;

	sendbuf[0] = CMD_KEY[0];
	sendbuf[1] = CMD_KEY[1];
	sendbuf[2] = cmd;
	sendbuf[3] = status;

	if (0 > write(client_sockfd, sendbuf, CMD_LEN))
		return false;

	return true;
}

bool TestFileReadWriteThread::sendData(char* data, unsigned short len)
{
	const char PAY_KEY[] = {'P', 'A'};

	if (client_sockfd < 0)
		return false;

	if (len > (PAYLOAD_LEN - HEADER_LEN))
		return false;

	sendbuf[0] = PAY_KEY[0];
	sendbuf[1] = PAY_KEY[1];
	sendbuf[2] = (len >> 8) & 0xFF;
	sendbuf[3] = len & 0xFF;
	
	if (len)
		memcpy(sendbuf + 4, data, len);

	if (0 > write(client_sockfd, sendbuf, len + 4))
		return false;

	return true;
}

unsigned short TestFileReadWriteThread::readData(char *data, unsigned short buflen)
{
	const char PAY_KEY[] = {'P', 'A'}; 
	unsigned short payload = 0;
	int read_cnt = 0;
	struct timeval tv;
    fd_set readfds;
	int result;

	if (client_sockfd < 0)
		return 0;

    FD_ZERO(&readfds);
    FD_SET(client_sockfd, &readfds);
 
    tv.tv_sec = 5;
    tv.tv_usec = 0;

	result = select(client_sockfd+1, &readfds,(fd_set *)0, (fd_set *)0, &tv);
          
	if (result == 0)
	{
		ALOGD("client timeout");
		return 0;
	}
	else if (result < 0)
	{
		ALOGD("Select Error");
		return 0;
	}


	if (CMD_LEN > (read_cnt = read( client_sockfd, recvbuf, PACKET_LEN)))
		return 0;
	
	if (recvbuf[0] != PAY_KEY[0] || recvbuf[1] != PAY_KEY[1])
		return 0;

	payload = (recvbuf[2] << 8) | recvbuf[3]; 

	if (payload != (read_cnt - 4))
		return 0;

	if (buflen < payload)
		payload = buflen;

	if (payload)
		memcpy(data, recvbuf + 4, payload);

	return payload; 
}

char TestFileReadWriteThread::readCmd()
{
	const char CMD_KEY[] = {'C','M'};
    int read_cnt = 0;          
    struct timeval tv;
	fd_set readfds;
	int result;

	if (client_sockfd < 0)
		return CMD_NONE;

    FD_ZERO(&readfds);
    FD_SET(client_sockfd, &readfds);
		  
    tv.tv_sec = 5;
    tv.tv_usec = 0;

	result = select(client_sockfd+1, &readfds,(fd_set *)0, (fd_set *)0, &tv);
					          
	if (result == 0)
	{
	    ALOGD("client timeout");        
	    return 0;
	}
	else if (result < 0)       
	{
	    ALOGD("Select Error"); 
	    return 0;
	}

    if (CMD_LEN > (read_cnt = read( client_sockfd, recvbuf, PACKET_LEN)))
        return CMD_NONE; 

	if (recvbuf[0] != CMD_KEY[0] || recvbuf[1] != CMD_KEY[1])
        return CMD_NONE;; 

	return recvbuf[2]; 
}

bool TestFileReadWriteThread::hasPayload()
{
	if (client_sockfd < 0)
		return false;

	return (recvbuf[3] == STA_CON);
}

void TestFileReadWriteThread::onFileList()
{
	DIR *dp = NULL;
	struct dirent *dirp = NULL;
	char tempDir[256], filename[256];
	int found = 0;

	ALOGD("%s", __FUNCTION__);

	memset(filename, 0, sizeof(filename));
	memset(tempDir, 0, sizeof(tempDir));
	
	if (!hasPayload() || 0 == readData(filename, sizeof(filename)))
		return;

	if (pDetectHDD->selectedHDDIndex == 0 || pDetectHDD->selectedHDDIndex == 1)
	{
		sprintf(tempDir, "%s%s", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, filename);

		if ((dp = opendir(tempDir)) != NULL)
		{
			while ((dirp = readdir(dp)) != NULL)
			{
				if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
					continue;

				if (dirp->d_type == DT_REG || dirp->d_type == DT_DIR)
				{
					found++;
					break;
				}
			}

			if (found > 0)
			{
				if (!sendCmd(CMD_LIST, STA_CON) || CMD_ACK != readCmd())
					goto cleanup;
			}
			else
			{
				sendCmd(CMD_LIST, STA_OK);
				goto cleanup;
			}			
			
			rewinddir(dp);
					
			while ((dirp = readdir(dp)) != NULL)
			{
				if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
					continue;
										
				if (dirp->d_type == DT_REG || dirp->d_type == DT_DIR)
				{
					if (!sendData(dirp->d_name, strlen(dirp->d_name) + 1) || CMD_ACK != readCmd())
						goto cleanup;
				}
			}

			if (found > 0)
				sendData(NULL, 0);
		}
	}

cleanup:

	if (dp)
		closedir(dp);
}

void TestFileReadWriteThread::onFileRead()
{
	bool read_ok = false;
	char filename[256];
	char tempFilename[256];
	int read_len = 0;
	char tempBuf[PAYLOAD_LEN];
	FILE *fp;

	memset(tempFilename, 0, sizeof(tempFilename));
	memset(filename, 0, sizeof(filename));

	ALOGD("%s", __FUNCTION__);

	if (!hasPayload() || 0 == readData(filename, sizeof(filename)))
		return;

	if (pDetectHDD->selectedHDDIndex == 0 || pDetectHDD->selectedHDDIndex == 1)
	{
		sprintf(tempFilename, "%s%s", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, filename);

		if (pWaitUser->file_util_makedirs(filename))
		{
			fp = fopen(tempFilename, "r");

			if (fp)
			{
				if (!sendCmd(CMD_READ, STA_CON) || CMD_ACK != readCmd())
					goto cleanup;

				while(0 < (read_len = fread(tempBuf, 1, PAYLOAD_LEN - HEADER_LEN, fp)))
				{
					if (!sendData(tempBuf, read_len) || CMD_ACK != readCmd())
						goto cleanup;
				}

				if (!sendData(NULL, 0))
					goto cleanup;

				read_ok = true;
			}
		}
	}

	if (!read_ok)
		sendCmd(CMD_READ, STA_FAIL);

cleanup:

	if (fp)
		fclose(fp);
}

void TestFileReadWriteThread::onFileWrite()
{
	bool write_ok = false;
	char filename[256];
	char tempFilename[256];
	char tempBuf[PAYLOAD_LEN - HEADER_LEN];
	int read_cnt = 0;
	FILE *fp = NULL;

	ALOGD("%s", __FUNCTION__);

	memset(filename, 0, sizeof(filename));
	memset(tempFilename, 0, sizeof(tempFilename));

	if (!hasPayload() || 0 == readData(filename, sizeof(filename)))
		return;

	if (pDetectHDD->selectedHDDIndex == 0 || pDetectHDD->selectedHDDIndex == 1)
	{
		sprintf(tempFilename, "%s%s", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, filename);

		if (pWaitUser->file_util_makedirs(filename))
		{
			fp = fopen(tempFilename, "w");

			if (fp)
			{
				if (!sendCmd(CMD_WRITE, STA_OK))
					goto cleanup;

				while(0 < (read_cnt = readData(tempBuf, PAYLOAD_LEN - HEADER_LEN)))
				{
					fwrite(tempBuf, 1, read_cnt, fp);

					if (!sendCmd(CMD_ACK, STA_OK))
						goto cleanup;
				}

				write_ok = true;
			}
		}
	}

	if (!write_ok)
		sendCmd(CMD_WRITE, STA_FAIL);

cleanup:

	if (fp)
		fclose(fp);
}

bool TestFileReadWriteThread::threadLoop()
{
	char cmd_read = CMD_NONE;

	ALOGD("%S", __FUNCTION__);

	while ( 1 )
	{
		client_len = sizeof( client_address );
		client_sockfd = accept( server_sockfd, ( struct sockaddr *)&client_address, &client_len );
		
		ALOGD("%s Client Connected", __FUNCTION__);

		while ( 1 )
		{
			cmd_read = readCmd();

			if (cmd_read == CMD_LIST)
				onFileList();
			else if (cmd_read == CMD_READ)
				onFileRead();
			else if (cmd_read == CMD_WRITE)
				onFileWrite();
			else
				break;
		}

		ALOGD("%s client closed", __FUNCTION__);

		close( client_sockfd );
		client_sockfd = -1;
	}

	return false;
}

