/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: eschoi $
 * $LastChangedDate: 2014-06-03 13:58:35 +0900 (화, 03 6월 2014) $
 * $LastChangedRevision: 837 $
 * Description:
 * Note:
 *****************************************************************************/

#include "Logger.h"
#include "TimeConvertor.h"
#include <fstream>
#include <string.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <unistd.h>

void TSL_Printf(const char *Format, ...)
{
    int str_len;
    va_list va;
    char MsgBuf[256];

    va_start(va, Format);
    vsprintf((char*) MsgBuf, Format, va);
    va_end(va);

    str_len = strlen(MsgBuf);

    ///////////////////////////////////////////////
    // Get Current Time
    ///////////////////////////////////////////////
    time_t currTime;
    struct tm * st_time;
    char time_buf[128];
    char log_buf[512];

    time(&currTime);
    st_time = localtime(&currTime);
    strftime(time_buf, 128, "%Y-%m-%dT%H:%M:%S", st_time);

    sprintf(log_buf, "[%s] %s", time_buf, MsgBuf);

    ///////////////////////////////////////////////
    // Write log file
    ///////////////////////////////////////////////
    FILE * fp = NULL;
#ifdef ANDROID
    const char *DVB_SI_LOG_DIR_PATH = "/data/lkn_home/run/si/";
    const char *DVB_SI_LOG_FILE_PATH = "/data/lkn_home/run/si/DVBSI.log";
#else
    const char *DVB_SI_LOG_DIR_PATH = "./log";
    const char *DVB_SI_LOG_FILE_PATH = "./log/DVBSI.log";
#endif
    struct stat file_info;

    if (access(DVB_SI_LOG_DIR_PATH, F_OK) != 0)
    {
        char cmdBuf[128];
        sprintf(cmdBuf, "mkdir -p %s", DVB_SI_LOG_DIR_PATH);

        system(cmdBuf);
    }

    if (stat(DVB_SI_LOG_FILE_PATH, &file_info) >= 0)
    {
        if (file_info.st_size > 1000000)
        {
            char cmd_buf[128];

            sprintf(cmd_buf, "rm %s", DVB_SI_LOG_FILE_PATH);
            system(cmd_buf);
        }

        fp = fopen(DVB_SI_LOG_FILE_PATH, "a+");
        if (fp)
        {
            fwrite(log_buf, 1, strlen(log_buf), fp );
            fflush(fp);
            fclose(fp);
#ifdef ANDROID
            __android_log_print(ANDROID_LOG_INFO, __APP_NAME__, "%s",  log_buf);
#else
            printf("%s", log_buf);
#endif
        }
    }
    else
    {
        char cmd_buf[128];
        sprintf(cmd_buf, "touch %s", DVB_SI_LOG_FILE_PATH);
        system(cmd_buf);
    }

}
