/*
 * utils.c
 *
 *  Created on: Feb 16, 2015
 *      Author: jhseo
 */

#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "utils.h"

//const int LOG_ENABLE = 0;
//const int LOG_FILE_MODE = 0;
//FILE * sFp = NULL;

static const char *LOG = "/sdcard/log/susclient.log";
static const char *LOG_MODE = "/sdcard/log/log_mode";
int sLogFile = -1;

int openLogFile(void)
{
    int file;

    if(access(LOG_MODE, R_OK))
    {
        printf("%s doesn't exist! \n", LOG_MODE);
        sLogFile = -1;
        return -1;
    }
    file = open(LOG, (O_CREAT  | O_APPEND | O_WRONLY | O_SYNC), 0644);
    if(file < 0)
    {
        perror("open error:");
        printf("log_file_open error %d \n", file);
        return file;
    }

    sLogFile = file;

    return file;
}


void toLowerString(char * str, int size)
{
    if(str == NULL) {
        return;
    }

    int idx = 0;

    for (idx = 0; idx < size; idx++) {
        str[idx] = tolower(str[idx]);
    }
}

void Log(const char* format, ...) {
   
    char buf[256];

    if (sLogFile == -1) {
        openLogFile();
    }

    if (sLogFile < 0) {
        return;
    } 

    va_list arglist;

    va_start(arglist, format);
    vsnprintf(buf, 255, format, arglist);
    va_end(arglist);

    write(sLogFile, buf, strlen(buf));
    fsync(sLogFile);
}

