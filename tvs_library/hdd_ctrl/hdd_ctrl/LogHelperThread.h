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

#ifndef HDD_CTRL_LOGHELPERTHREAD_H_
#define HDD_CTRL_LOGHELPERTHREAD_H_

#include "hdd_ctrl.h"

class LogHelperThread : public Thread
{
enum HDDPosition {
	front = 0,
	rear,
	hdd_none
} ;

private:
    static const int sRetryCount = 300;
    static const int sInterval = 1;
    static const bool logDebug = false;
    static bool mRunning;
    HDDPosition mPosition;
    int mCount;
    char *path;

    void startLogService(HDDPosition position);
    void pvr_backup();
    void setPath(HDDPosition position);
    char* getPath();
    void deinit();
	bool isLogfile();
    int erase_mtd9(void);

public:
    static void stopLogService(void);
    void onFirstRef();
    virtual status_t readyToRun();
    virtual bool threadLoop();

    static bool isRunning();
    void setRunning(bool running);

};

#endif /* HDD_CTRL_LOGHELPERTHREAD_H_ */
