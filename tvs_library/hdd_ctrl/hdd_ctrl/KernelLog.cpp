#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/klog.h>
#include <cutils/properties.h>

#include "KernelLog.h"

#define DEFAULT_LOG_ROTATE_SIZE_KBYTES 512 
#define DEFAULT_MAX_ROTATED_LOGS 5

#define FALLBACK_KLOG_BUF_SHIFT 17  /* CONFIG_LOG_BUF_SHIFT from our kernel */
#define FALLBACK_KLOG_BUF_LEN   (1 << FALLBACK_KLOG_BUF_SHIFT)

#define KLOGCAT_SERVICE_STATE "tvstorm.klogcat.enable"
/*
 * Commands to klogctl:
 *
 *      KLOG_CLOSE(0) -- Close the log.  Currently a NOP.
 *      KLOG_OPEN(1) -- Open the log. Currently a NOP.
 *      KLOG_READ(2) -- Read from the log.
 *      KLOG_READ_ALL(3) -- Read up to the last 4k of messages in the ring buffer.
 *      KLOG_READ_CLEAR(4) -- Read and clear last 4k of messages in the ring buffer
 *      KLOG_CLEAR(5) -- Clear ring buffer.
 *      KLOG_CONSOLE_OFF(6) -- Disable printk's to console
 *      KLOG_CONSOLE_ON(7) -- Enable printk's to console
 *      KLOG_CONSOLE_LEVEL(8) -- Set level of messages printed to console
 *      KLOG_SIZE_UNREAD(9) -- Return number of unread characters in the log buffer
 *		KLOG_SIZE_BUFFER(10) -- Return number of log buffer
*/


static int g_rotateKernelLogSizeKbytes = DEFAULT_LOG_ROTATE_SIZE_KBYTES;
static int g_maxRateLogs = DEFAULT_MAX_ROTATED_LOGS;
static char *g_outputLogFile = "/mnt/media_rw/sdcard1/log/kernel.log"; 
static int g_fd = 0;
static int g_totalBytes = 0;

static int startKernelLog(){
	struct stat statbuf; 

	g_fd = openKernelLog(g_outputLogFile);

	if(g_fd < 0){
		fprintf(stderr, "file handle is invalid.\n");
		return -1;
	}

	fstat(g_fd, &statbuf);
	
	g_totalBytes = statbuf.st_size;
	
	return 0;
}

static int openKernelLog(char *path){
	 return open(path, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
}

static void closeKernelLog(){
	close(g_fd);	
}

static int writeKernelLog(char *buf, int len){
	int writtenBytes = 0, num;

	num = klogctl(KLOG_READ_CLEAR, buf, len);

	//fprintf(stderr, "*^^* number of klog buffer : %d\n", num);
	if(num<0){
		fprintf(stderr, "The number of buffer is wrong.");
		return 0;
	}

	buf[num] = '\0';

	if(g_fd < 0){
		fprintf(stderr, "file handle(g_fd) is invalid.");
		return -1;
	}

	//writtenBytes = write(g_fd, buf, len);
	while(writtenBytes = write(g_fd, buf, num)){
		if(writtenBytes == -1){
			if(errno == EINTR)
				continue;
			else{
				fprintf(stderr, "Error : write is failed (errono : %d)\n", errno);
				return 0;
			}
		}

		buf += writtenBytes;
		num -= writtenBytes;

		g_totalBytes += writtenBytes;
	}

	//fprintf(stderr, "klogcat: %s[%s] - g_totalBytes : %d[g_rotateKernelLogSizeKbytes : %d]\n", __FILE__, __FUNCTION__, g_totalBytes/1024, g_rotateKernelLogSizeKbytes);
	if(g_rotateKernelLogSizeKbytes > 0 && (g_totalBytes / 1024) >= g_rotateKernelLogSizeKbytes){
		rotateKernelLog();
	}

	return 0;
}

static int rotateKernelLog(){
	int err;

	if(g_outputLogFile == NULL)
		return -1;

	closeKernelLog();

	for(int i = g_maxRateLogs; i >0;i--){
		char *file0, *file1;

		asprintf(&file1, "%s.%d", g_outputLogFile, i);

		if(i-1 == 0){
			asprintf(&file0, "%s", g_outputLogFile);
		}else{
			asprintf(&file0, "%s.%d", g_outputLogFile, i-1);
		}
//		fprintf(stderr,"klogcat: %s[%s] - file0 : %s\n", __FILE__, __FUNCTION__, file0);
//		fprintf(stderr,"klogcat: %s[%s] - file1 : %s\n", __FILE__, __FUNCTION__, file1);
		err = rename(file0, file1);

		if(err < 0 && errno != ENOENT){
			perror("while rotating log files.");
		}

		free(file0);
		free(file1);
	}

	g_fd = openKernelLog(g_outputLogFile);

	if(g_fd < 0){
		perror("cannot open output file.");
		fprintf(stderr , "cannot open output file.");
		return -1;
	}

	g_totalBytes = 0;

	return 0;
}

static bool isRunning(){
	char value[PROPERTY_VALUE_MAX] = {'\0'};
	
	if(property_get(KLOGCAT_SERVICE_STATE, value, NULL) && !strcmp(value, "on")){
		return true;
	}else{
		return false;
	}

}
int main(int argc, char *argv[]){
	int err, ret, len = 0;

	for(;;){
		ret = getopt(argc, argv, "n:v:r:f");

		if(ret <= 0)
			break;

		fprintf(stderr, "*^^* option : %c\n", ret);
		switch(ret){
			case 'n':{
				if(optarg != NULL){
					if(isdigit(optarg[0])){
						fprintf(stderr, "Invalid parameter to -n\n");
						fprintf(stderr, "*^^* optarg : %c\n", optarg[0]);
						return -1;
					}
					g_maxRateLogs = atoi(optarg);
				}
			}
			break;
	
			case 'f':{
				fprintf(stderr, "*^^* $$$$$$$");
				if(optarg != NULL){
					g_outputLogFile = optarg;					
					fprintf(stderr, "*^^* g_outputLogFile : %s\n", g_outputLogFile);
					fprintf(stderr, "*^^* optarg : %s\n", optarg);
					return -1;
				}
			}
			break;

			case 'v':{
			}
			break;

			case 'r':{
				if(optarg != NULL){
					if(isdigit(optarg[0])){
						fprintf(stderr, "Invalid parameter to -r\n");
						fprintf(stderr, "*^^* optarg : %c\n", optarg[0]); 
						return -1;
					}
					g_rotateKernelLogSizeKbytes = atoi(optarg); 
					fprintf(stderr, "*^^* g_rotateKernelLogSizeKbytes : %d\n", g_rotateKernelLogSizeKbytes);
				}
			}
			break;

			default:
			break;
		}
	}

	fprintf(stderr, "get buffer size of kernel buffer\n");
	/* Allocate buffer for kernel log */
	len = klogctl(KLOG_SIZE_BUFFER, NULL, 0);

	fprintf(stderr, "len : %d before starting kernel log\n", len);

	if(len < 0){
		fprintf(stderr, "Error : No log buffer size for kernel log.\n");
		len = FALLBACK_KLOG_BUF_LEN;
	}

	char *buf = new char[len+1]; 

	fprintf(stderr, " *^^* startKernelLog \n");
	if(startKernelLog() < 0){
		delete[] buf;
		buf= NULL;
	}

	while(isRunning()) { 
		memset(buf, 0x00, sizeof(buf));

		if(writeKernelLog(buf, len) < 0)
			break;

		usleep(1000);
	}

	fprintf(stderr, " *^^* closeKernelLog \n");
	delete[] buf;
	buf = NULL;

	closeKernelLog();

	return 0;
}
