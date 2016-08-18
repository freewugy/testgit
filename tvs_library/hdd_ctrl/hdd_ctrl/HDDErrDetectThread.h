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

#ifndef HDD_CTRL_HDDERRDETECTTHREAD_H_
#define HDD_CTRL_HDDERRDETECTTHREAD_H_

#include "hdd_ctrl.h"

class HDDErrDetectThread : public Thread
{
public:
    void onFirstRef();
    virtual status_t readyToRun();
    virtual bool threadLoop();

	bool IsHDDErr(const char *fsPath, const char *mountPoint);
};

#endif /* HDD_CTRL_HDDERRDETECTTHREAD_H_ */
