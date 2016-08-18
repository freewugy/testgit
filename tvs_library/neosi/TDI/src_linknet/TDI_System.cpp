/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: ksh78 $
 * $LastChangedDate: 2014-05-30 22:14:29 +0900 (금, 30 5월 2014) $
 * $LastChangedRevision: 823 $
 * Description:
 * Note:
 *****************************************************************************/

#include <stdio.h>

#include "TDI.h" 
#include "Logger.h"
#include "OhPlayerClientSIWrapper.h"
#include <pthread.h>

using namespace tvstorm;

tvstorm::OhPlayerClientSIWrapper* mOhPlayerClient = NULL;
pthread_mutex_t playerClinentMutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;

void playerLock()
{
        //pthread_mutex_lock(&playerClinentMutex);
}
void playerUnlock()
{
        //pthread_mutex_unlock(&playerClinentMutex);
}

TDI_Error TDI_Open_System()
{
        TDI_Error err = TDI_SUCCESS;
        TDI_Demux_Open(0, 0);
        if(mOhPlayerClient == NULL) {
                L_TEST("begin_OhPlayerClient_open\n");
                mOhPlayerClient = new OhPlayerClientSIWrapper;
                L_TEST("end_OhPlayerClient_open\n");
        }
        return err;
}

TDI_Error TDI_Close_System()
{
        TDI_Error err = TDI_SUCCESS;
        if(mOhPlayerClient) {
                L_TEST("begin_OhPlayerClient_close\n");
                delete mOhPlayerClient;
                L_TEST("end_OhPlayerClient_close\n");
                mOhPlayerClient = NULL;
        }
        return err;
}

TDI_Error TDI_System_Open(IN bool bUseDirectFB)
{
        TDI_Error err = TDI_SUCCESS;

        TDI_Open_System();

        return err;
}
