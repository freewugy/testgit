/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: eschoi $
 * $LastChangedDate: 2014-08-12 17:10:52 +0900 (화, 12 8월 2014) $
 * $LastChangedRevision: 980 $
 * Description:
 * Note:
 *****************************************************************************/

#ifndef _TIMER_H_
#define _TIMER_H

///////////////////////////////////////////////////////////////
// header
///////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include <semaphore.h>


///////////////////////////////////////////////////////////////
// define
///////////////////////////////////////////////////////////////

// Maximum timer number
#define MAX_TIMER_NUM	128

#define ID_TIMER_CHECK_CURRENT_PROGRAM_CHANGED  1000
#define ID_TIMER_CHECK_WEPG_UPDATE              2000
#define ID_TIMER_CHECK_SCAN_MANAGER             3000
#define ID_TIMER_CHECK_WEBSI_UPDATE             4000
////////////////////////////////////////////////////////////////
// prototype API
///////////////////////////////////////////////////////////////

// callback
typedef void (*TimerHandler)(int id);

// INIT
int	init_timer(void);

// SET
int	set_timer(int id, long intv, TimerHandler *tmFn);

// DELETE
int	delete_timer(int id);

// ALL CLEAR
int	deinit_timer(void);

// PRINT ALL TIMER
void	printf_timer();

void getSITime_t(time_t* intime);
int siSleep(int sec);
int siMiliSleep(int msec);
int siMicroSleep(int usec);

#endif

