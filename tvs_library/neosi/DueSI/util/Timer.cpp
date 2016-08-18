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

#include "Timer.h"
#include "Logger.h"

static const char* TAG = "Timer";
//#define __DEBUG_LOG_UTIL__

////////////////////////////////////////////////////////////////
// prototype
///////////////////////////////////////////////////////////////

// thread
void *time_thread_func(void *arg);

int getStateOfTimer_array(int array_index);
int getStateOfTimer_id(int id);
int getArrayIndex_Empty();
int getArrayIndex_SearchID(int id);

void setTimer_STOP(int array_index);
int checkExistID(int id);

///////////////////////////////////////////////////////////////
// structure
///////////////////////////////////////////////////////////////

enum {
    FALSE = 0, TRUE = 1
};

enum {
    RET_ERROR = -1, RET_OK = 0
};

enum {
    TIMER_STOP = 0, TIMER_RUN = 1, TIMER_USED = 2
};

typedef struct {
    int timer_id; // timer id
    long timer_time; // timer time in milliseconds
    pthread_t timer_thread; // timer thread
    TimerHandler timer_callback_func; // timer callback
    int nRepeat; // not yet,
    int bUse; // 0:not use, 1:use
} timer_info_data;

typedef struct {
    timer_info_data sTimer_data[MAX_TIMER_NUM];
    int nTotal_count;
} timer_info_table;

///////////////////////////////////////////////////////////////
// global
///////////////////////////////////////////////////////////////

// total timer data
static timer_info_table sTimer_table;

// mutex
pthread_mutex_t mutx;

// semaphore
sem_t bin_sem;

///////////////////////////////////////////////////////////////
// local timer function
///////////////////////////////////////////////////////////////

// thread function
void *time_thread_func(void *arg)
{
    int array_index;
    struct timespec start_time_val, check_time_val;

    int run_thread = TIMER_RUN;
    timer_info_data sTimer_local_data;

    memset(&sTimer_local_data, 0x00, sizeof(sTimer_local_data));
    memcpy(&sTimer_local_data, (timer_info_data *) arg, sizeof(sTimer_local_data));

    // semaphore increase
    sem_post(&bin_sem);

    array_index = getArrayIndex_SearchID(sTimer_local_data.timer_id); // get array index of current timer structure

#ifdef __DEBUG_LOG_UTIL__
    L_DEBUG(TAG, "## timer id[%d] in thread\n", array_index);
#endif

    // disable pthread_cancel
#ifndef ANDROID
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
#else
    struct sigaction actions;
    memset(&actions, 0, sizeof(actions));
    sigemptyset(&actions.sa_mask);
    actions.sa_flags = 0;
    actions.sa_handler = SIG_IGN; /* SIGIGN */
    sigaction(SIGINT,&actions,NULL);
#endif

    memset(&start_time_val, 0x00, sizeof(start_time_val));
    memset(&check_time_val, 0x00, sizeof(check_time_val));

    clock_gettime(CLOCK_MONOTONIC, &start_time_val); // start_time

    long diff_time = 0;
    while (run_thread == TIMER_RUN) {
        clock_gettime(CLOCK_MONOTONIC, &check_time_val); // check time

        // for break thread
        if(getStateOfTimer_array(array_index) == TIMER_STOP)
            run_thread = TIMER_STOP;

        diff_time = (long)(1000 * (check_time_val.tv_sec - start_time_val.tv_sec));
        //printf("diff_time : %ld, timer_time : %ld\n", diff_time, sTimer_local_data.timer_time);
        if(diff_time >= sTimer_local_data.timer_time) {
#ifdef __DEBUG_LOG_UTIL__
            L_DEBUG(TAG, "## finish timer thread id[%d], time[%ld], diff[%ld]\n", sTimer_local_data.timer_id, sTimer_local_data.timer_time, diff_time);
            L_DEBUG(TAG, "## start callback func!!\n\n");
#endif

            sTimer_local_data.timer_callback_func(sTimer_local_data.timer_id); // call callback function after timer
            clock_gettime(CLOCK_MONOTONIC, &start_time_val);
        }
        siSleep(1); // 1 second
    }
    return 0;
}

// get timer state
int getStateOfTimer_array(int array_index)
{
    return sTimer_table.sTimer_data[array_index].bUse;
}

// get timer state
int getStateOfTimer_id(int id)
{
    int res = RET_ERROR, array_index = -1;

    array_index = getArrayIndex_SearchID(id);
    if(array_index == -1) {
        // not exist timer of id
        L_ERROR(TAG, "## Not exist id(%d)!!!!\n", id);
        return res;
    }

    return sTimer_table.sTimer_data[array_index].bUse;
}

//
// get empty array index of Timer
int getArrayIndex_Empty()
{
    int i = 0;
    int array_index = RET_ERROR;

    if(sTimer_table.nTotal_count == MAX_TIMER_NUM) {
        // all used timer...
        printf("## Error!!! all timer used\n");
        return array_index;
    }

    for (i = 0; i < MAX_TIMER_NUM; i++) {
        if(sTimer_table.sTimer_data[i].bUse == TIMER_STOP) {
            array_index = i;
            break;
        }
    }
    return array_index;
}

//
// get array index of searching ID
int getArrayIndex_SearchID(int id)
{
    int i = 0;
    int array_index = RET_ERROR;

    if(sTimer_table.nTotal_count == 0) {
        // all empty timer...
        printf("## Noting timer!!!!\n");
        return array_index;
    }

    for (i = 0; i < MAX_TIMER_NUM; i++) {
        if((sTimer_table.sTimer_data[i].bUse == 1) && (sTimer_table.sTimer_data[i].timer_id == id)) {
            array_index = i;
            break;
        }
    }

    return array_index;
}

// set timer state
void setTimer_STOP(int array_index)
{
    if(sTimer_table.sTimer_data[array_index].bUse == TIMER_RUN) {
        sTimer_table.sTimer_data[array_index].bUse = TIMER_STOP;
        memset(&sTimer_table.sTimer_data[array_index], 0x00, sizeof(sTimer_table.sTimer_data[array_index]));
        sTimer_table.nTotal_count--;
    }
}

//
// check exist Timer ID
int checkExistID(int id)
{
    int i = 0, res = RET_ERROR;

    for (i = 0; i < MAX_TIMER_NUM; i++) {
        if(sTimer_table.sTimer_data[i].timer_id == id) {
            res = i;
            break;
        }
    }
    return res;
}

///////////////////////////////////////////////////////////////
// timer api
///////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////
// init timer
// 	Description : must use initialize timer.
//	Parameter : noting
//	Return :  
//		-1 : error
//		  1 : ok
int init_timer()
{
    int res = RET_ERROR;

    memset(&sTimer_table, 0x00, sizeof(sTimer_table));

    // mutex init
    if(pthread_mutex_init(&mutx, NULL) != 0) {
        // failed mutex
        printf("## Error!!!! failed mutex init\n");
        return res;
    }

    // semaphore init
    if(sem_init(&bin_sem, 0, 0) != 0) {
        // failed semaphore
        L_ERROR(TAG, "## Error!!!! failed semaphore init\n");
        return res;
    }

    res = RET_OK;
    return res;
}

//////////////////////////////////////////////////////
// set timer
//	Description : setting timer
//	Parameter   :
//	        int id       : timer id
//	        long intv    : timer time
//	        TimerHandler : callback func after timer
//	Return :
//		-1 : error
//		 1 : ok
int set_timer(int id, long intv, TimerHandler *tmFn)
{
    // mutex lock
    pthread_mutex_lock(&mutx);

    int res = RET_ERROR, array_index = -1;
    timer_info_data sTimer_local_data; // local timer data

#ifdef __DEBUG_LOG_UTIL__
    L_DEBUG(TAG, "#####################################\n");
    L_DEBUG(TAG, "## Start set_timer ID[%d], time[%ld]\n", id, intv);
    printf_timer();
#endif

    if(!(id) || !(tmFn)) { // NULL check of input parameter
        L_ERROR(TAG, "## Error!!! id or callback funcion is NULL\n");
        pthread_mutex_unlock(&mutx);
        return res;
    }

    // already eixst Timer case, re-setting timer.
    if(checkExistID(id) >= 0) {
        // mutex unlock
        pthread_mutex_unlock(&mutx);
        if(delete_timer(id) == RET_ERROR) {
            printf("## Error!!! timer delete\n");
            return res;
        }
        pthread_mutex_lock(&mutx);
    }

    array_index = getArrayIndex_Empty(); // check remain array index of timer
    if(array_index == -1) {
        //not exist empty timer...
        L_ERROR(TAG, "## Error!!! not exist timer array...\n");
        printf_timer();
        pthread_mutex_unlock(&mutx);
        return res;
    }

    memset(&sTimer_local_data, 0x00, sizeof(sTimer_local_data));

    sTimer_local_data.timer_id = id;
    sTimer_local_data.timer_time = intv;
    sTimer_local_data.bUse = TIMER_RUN;
    sTimer_local_data.timer_callback_func = (TimerHandler) tmFn;

    // registor timer structure
    memcpy(&sTimer_table.sTimer_data[array_index], &sTimer_local_data, sizeof(sTimer_local_data));
    sTimer_table.nTotal_count++;

#ifdef __DEBUG_LOG_UTIL__
    L_DEBUG(TAG, "## set timer id[%d], time[%ld]\n", sTimer_local_data.timer_id, sTimer_local_data.timer_time);
#endif

    // create thread
    res = pthread_create(&sTimer_local_data.timer_thread, NULL, time_thread_func, (void *) &sTimer_local_data);
    if(res != 0) {
        printf("## Error!!! create time thread failed!!\n");

        // if thread problem then delete timer structure
        memset(&sTimer_table.sTimer_data[array_index], 0x00, sizeof(sTimer_table.sTimer_data));
        sTimer_table.nTotal_count--;

        printf_timer();
        pthread_mutex_unlock(&mutx);
        res = RET_ERROR;
        return res;
    }

    // semaphore waiting
    sem_wait(&bin_sem);

#ifdef __DEBUG_LOG_UTIL__
    L_DEBUG(TAG, "## registed timer id[%d], time[%ld]\n", sTimer_table.sTimer_data[array_index].timer_id, sTimer_table.sTimer_data[array_index].timer_time);
#endif

    // mutex unlock
    pthread_mutex_unlock(&mutx);

#ifdef __DEBUG_LOG_UTIL__
    L_DEBUG(TAG, "#####################################\n");
    L_DEBUG(TAG, "## Finish set_timer ID[%d], time[%ld]\n", id, intv);
    L_DEBUG(TAG, "#####################################\n");
    printf_timer();
#endif

    res = RET_OK;
    return res;
}

//////////////////////////////////////////////////////
// delete to selected timer 
int delete_timer(int id)
{
    int res = RET_ERROR, array_index = -1;

#ifdef __DEBUG_LOG_UTIL__
    L_DEBUG(TAG, "\n## start delete timer (%d)\n", id);
    printf_timer();
#endif

    // mutex lock
    pthread_mutex_lock(&mutx);

    array_index = getArrayIndex_SearchID(id);
    if(array_index == -1) {
        // not exist timer of id
        L_ERROR(TAG, "## Not exist id(%d)!!!!\n", id);
        // mutex unlock
        pthread_mutex_unlock(&mutx);

        return res;
    }

#ifdef __DEBUG_LOG_UTIL__
    L_DEBUG(TAG, "find ID [%d] timer for delete, array[%d]\n", sTimer_table.sTimer_data[array_index].timer_id, array_index);
#endif

    setTimer_STOP(array_index);

    // mutex unlock
    pthread_mutex_unlock(&mutx);

#ifdef __DEBUG_LOG_UTIL__
    L_DEBUG(TAG, "## finish delete timer (%d)\n", id);
    printf_timer();
#endif

    res = RET_OK;
    return res;
}

//////////////////////////////////////////////////////
// all delete timer & destory semaphore, mutex
int deinit_timer()
{
    int i = 0, res = RET_ERROR;

#ifdef __DEBUG_LOG_UTIL__
    L_DEBUG(TAG, "\n## destory timer\n\n");
#endif

    for (i = 0; i < MAX_TIMER_NUM; i++) {
        if(delete_timer(sTimer_table.sTimer_data[i].timer_id) == RET_ERROR)
            L_ERROR(TAG, "## Error!! [%d] ID[%d] timer[%ld] timer delete failed...\n", i, sTimer_table.sTimer_data[i].timer_id,
                    sTimer_table.sTimer_data[i].timer_time);
    }
    sTimer_table.nTotal_count = 0;

    sem_destroy(&bin_sem); // destroy semaphore
    if(pthread_mutex_destroy(&mutx) != 0) { // destory mutex
        L_ERROR(TAG, "## Error!!!! pthread destory\n");
        return res;
    }

    res = RET_OK;
    return res;
}

//////////////////////////////////////////////////////
// print all timer
void printf_timer()
{
    int i = 0;

    L_DEBUG(TAG, "\n###################################\n");
    L_DEBUG(TAG, "## registed timer list\n");
    L_DEBUG(TAG, "## total timer = %d, MAX = %d\n", sTimer_table.nTotal_count, MAX_TIMER_NUM);
    for (i = 0; i < MAX_TIMER_NUM; i++)
        if(sTimer_table.sTimer_data[i].bUse == 1) {
            L_DEBUG(TAG, "## printf [%d] ID[%d], time[%ld], bUsed[%d]\n", i, sTimer_table.sTimer_data[i].timer_id,
                    sTimer_table.sTimer_data[i].timer_time, sTimer_table.sTimer_data[i].bUse);
        }
    L_DEBUG(TAG, "###################################\n\n");
}


void getSITime_t(time_t* intime)
{
        struct timespec tp;
        
        clock_gettime(CLOCK_MONOTONIC, &tp);
        *intime = tp.tv_sec;
}

int siSleep(int sec)
{
        struct timeval tv;
        tv.tv_sec = sec;
        tv.tv_usec = 0;//(msec%1000)*1000;
        
        return select(0, NULL, NULL, NULL, &tv);
}

int siMiliSleep(int msec)
{
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = msec*1000;
        
        return select(0, NULL, NULL, NULL, &tv);
}

int siMicroSleep(int usec)
{
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = usec;

        return select(0, NULL, NULL, NULL, &tv);
}
