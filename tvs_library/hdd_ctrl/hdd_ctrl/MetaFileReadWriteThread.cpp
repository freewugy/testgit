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
#define LOG_TAG "MetaFileReadWriteThread"

#include "MetaFileReadWriteThread.h"
#include "DetectHDDThread.h"
#include "WaitUserThread.h"

MetaFileReadWriteThread::~MetaFileReadWriteThread()
{
	close( server_sockfd );
}

void MetaFileReadWriteThread::onFirstRef()
{
	//ALOGD("MetaFileReadWriteThread::onFirstRef()\n");

	bWriteFlag = true;

	server_sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr( "127.0.0.1" );
	server_address.sin_port = htons( METAFILE_READ_WRITE_THREAD_PORT );

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

status_t MetaFileReadWriteThread::readyToRun()
{
	//ALOGD("MetaFileReadWriteThread::readyToRun()\n");
	return 0;
}

bool MetaFileReadWriteThread::threadLoop()
{
	//ALOGD("MetaFileReadWriteThread::threadLoop()\n");

	while ( 1 )
	{
		client_len = sizeof( client_address );
		client_sockfd = accept( server_sockfd, ( struct sockaddr *)&client_address, &client_len );
		printf( "Client connected\n" );

		/////////////////////////
		FILE *fp;

		char current_mode = NONE_MODE;

		char command = 0;
		char databuf[256];

		int  filename_index = 0;
		char filename[100];

		while ( 1 )
		{
			memset(databuf, 0, 256);
			int read_cnt = read( client_sockfd, databuf, 256 );

			int index;
			for (index = 0; index < read_cnt; index++)
			{
				if (databuf[index] == 0x01)	// Start of heading
				{
					filename_index = 0;
					memset(filename, 0, 100);
					current_mode = COMMAND_MODE;
					continue;
				}
				else if (databuf[index] == 0x02)	// Start of text
				{
					//current_mode = FILENAME_MODE;
					if (command == 0x11)	// read(0x11)
					{
						memset(filename, 0, 100);
						char rd_fn_buf[100];
						memset(rd_fn_buf, 0, 100);
						sprintf(rd_fn_buf, "%srd_fn.txt", HDD_HOME);
						FILE* fp_rf_fn = fopen(rd_fn_buf, "r");
						if (fp_rf_fn)
						{
							fgets(filename, 100, fp_rf_fn);
							fclose(fp_rf_fn);
						}
						char system_buf[100];
						memset(system_buf, 0, 100);
						sprintf(system_buf, "rm -f %srd_fn.txt", HDD_HOME);
						system(system_buf);
					}
					else if (command == 0x12)	// write(0x12)
					{
						memset(filename, 0, 100);
						char wr_fn_buf[100];
						memset(wr_fn_buf, 0, 100);
						sprintf(wr_fn_buf, "%swr_fn.txt", HDD_HOME);
						FILE* fp_wr_fn = fopen(wr_fn_buf, "r");
						if (fp_wr_fn)
						{
							fgets(filename, 100, fp_wr_fn);
							fclose(fp_wr_fn);
						}
						char system_buf[100];
						memset(system_buf, 0, 100);
						sprintf(system_buf, "rm -f %swr_fn.txt", HDD_HOME);
						system(system_buf);
					}
					continue;
				}
				else if (databuf[index] == 0x03)	// End of text
				{
					if (command == 0x12)	// write(0x12)
					{
						//current_mode = CONTENTS_MODE;
						if (bWriteFlag)
						{
							if (pDetectHDD->selectedHDDIndex == 0 || pDetectHDD->selectedHDDIndex == 1)
							{
								char tempFilename[256];
								memset(tempFilename, 0, 256);
								sprintf(tempFilename, "%s%s", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, filename);
								ALOGD("=========> write(0x12): tempFilename[%s]", tempFilename);
								if (pWaitUser->file_util_makedirs(filename))
								{
									fp = fopen(tempFilename, "w");
									if (fp)
									{
										char wr_cn_buf[100];
										memset(wr_cn_buf, 0, 100);
										sprintf(wr_cn_buf, "%swr_cn.txt", HDD_HOME);
										FILE* fp_wr_cn = fopen(wr_cn_buf, "r");
										if (fp_wr_cn)
										{
											char contents[512];
											while(!feof(fp_wr_cn))
											{
												memset(contents, 0, 512);
												fgets(contents, 512, fp_wr_cn);
												fputs(contents, fp);
											}
											fclose(fp_wr_cn);
											char system_buf[100];
											memset(system_buf, 0, 100);
											sprintf(system_buf, "rm -f %swr_cn.txt", HDD_HOME);
											system(system_buf);
										}
										//fputc(databuf[index], fp);
										bWriteFlag = true;
										fclose(fp);
									}
									else
									{
										bWriteFlag = false;
									}
								}
								else
								{
									bWriteFlag = false;
								}
							}
						}
					}
					continue;
				}
				else if (databuf[index] == 0x04)	// End of transmit
				{
					if (command == 0x11)	// read(0x11)
					{
						char ch = 0x01;
						write( client_sockfd, &ch, 1 );
						ch = 0x06;
						write( client_sockfd, &ch, 1 );

						ch = 0x02;
						write( client_sockfd, &ch, 1 );

						if (pDetectHDD->selectedHDDIndex == 0 || pDetectHDD->selectedHDDIndex == 1)
						{
							char tempFilename[256];
							memset(tempFilename, 0, 256);
							sprintf(tempFilename, "%s%s", pDetectHDD->hddInfoStore[pDetectHDD->selectedHDDIndex].HDDInfo.mounted_dir, filename);
							if (pWaitUser->file_util_makedirs(filename))
							{
								fp = fopen(tempFilename, "r");
								if (fp)
								{
									char rd_cn_buf[100];
									memset(rd_cn_buf, 0, 100);
									sprintf(rd_cn_buf, "%srd_cn.txt", HDD_HOME);
									FILE* fp_rd_cn = fopen(rd_cn_buf, "w");
									if (fp_rd_cn)
									{
										char tempbuf[256];
										memset(tempbuf, 0, 256);
										while (fgets(tempbuf, 255, fp) != NULL)
										{
											fputs(tempbuf, fp_rd_cn);
											//write( client_sockfd, tempbuf, strlen(tempbuf));
											memset(tempbuf, 0, 256);
										}
										fclose(fp_rd_cn);
										char system_buf[100];
										memset(system_buf, 0, 100);
										sprintf(system_buf, "chmod 777 %srd_cn.txt", HDD_HOME);
										system(system_buf);
									}
									fclose(fp);

									ch = 0x03;
									write( client_sockfd, &ch, 1 );
									ch = 0x04;
									write( client_sockfd, &ch, 1 );
								}
								else
								{
									ch = 0x03;
									write( client_sockfd, &ch, 1 );
									ch = 0x15;
									write( client_sockfd, &ch, 1 );
								}
							}
							else
							{
								ch = 0x03;
								write( client_sockfd, &ch, 1 );
								ch = 0x15;
								write( client_sockfd, &ch, 1 );
							}
						}
						else
						{
							ch = 0x03;
							write( client_sockfd, &ch, 1 );
							ch = 0x15;
							write( client_sockfd, &ch, 1 );
						}
					}
					else if (command == 0x12)	// write(0x12)
					{
						char ch = 0x01;
						write( client_sockfd, &ch, 1 );
						ch = 0x06;
						write( client_sockfd, &ch, 1 );
						ch = 0x02;
						write( client_sockfd, &ch, 1 );
						ch = 0x03;
						write( client_sockfd, &ch, 1 );
						if (bWriteFlag)
							ch = 0x04;
						else
							ch = 0x15;
						write( client_sockfd, &ch, 1 );
						bWriteFlag = true;
					}
					command = 0;
					index = read_cnt + 1;
					filename_index = 0;
					memset(filename, 0, 100);
					current_mode = NONE_MODE;
					continue;
				}

				switch (current_mode)
				{
					case COMMAND_MODE:
						{
							command = databuf[index];
						}
						break;
					case FILENAME_MODE:
						{
							filename[filename_index++] = databuf[index];
						}
						break;
					case CONTENTS_MODE:
						{
							if (command == 0x12)	// write(0x12)
							{
								//
							}
						}
						break;
				}
			}
			if (index >= read_cnt + 1)
				break;
		}

		close( client_sockfd );
	}

	return false;
}

