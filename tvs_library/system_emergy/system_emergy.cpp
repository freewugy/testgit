/******************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: $ cssong
 * $LastChangedDate: $ 2015.04.01
 * $LastChangedRevision: $ V1.1.1
 * Description: eeprom controler test
 * Note:
 *****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tvstorm_eeprom.h"
#define LOG_TAG "system_emergy"
#include "cutils/log.h"

int main(int argc, char **argv)
{

	TVSTORM_Error errCode = TVSTORM_OK;
	SW_DOWNLOAD_MODE_T dm;
	TVS_DOWNLOAD_MODE_T tvs_dm;
	int lval, download_mode;

#if 0
    errCode = TVSTORM_Get_DownloadMode(&dm, &tvs_dm);
    if(errCode != TVSTORM_OK){
        printf("errCode %d\n", errCode);
    }
    else{
        printf("download mode : 0x%08x, tvs_download_mode : 0x%08x\n", dm, tvs_dm);
    }

    tvs_dm = TVS_DOWNLOAD_MODE_EMERG;
#endif
    errCode = TVSTORM_Set_DownloadMode(SW_DOWNLOAD_MODE_ENABLED, TVS_DOWNLOAD_MODE_EMERG);

    if(errCode != TVSTORM_OK){
        printf("errCode %d\n", errCode);
    }

    sleep(1);

    printf("reboot ... \n");
    system("reboot");
    return 0;
}
