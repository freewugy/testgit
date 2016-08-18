/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: ksh78 $
 * $LastChangedDate: 2014-07-07 15:08:00 +0900 (월, 07 7월 2014) $
 * $LastChangedRevision: 914 $
 * Description:
 * Note:
 *****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>

#include "TDI.h" 
#include "TDI_System.h"


TDI_SYSTEM_RESULT TDI_Open_System()
{
	return TDI_SYSTEM_RESULT_OK;
}



TDI_Error TDI_System_Open(IN bool bUseDirectFB)
{
	TDI_Open_System();
	return (TDI_Error)0;
}

int siUSleep(int usec)
{
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = usec;
        
        return select(0, NULL, NULL, NULL, &tv);
}