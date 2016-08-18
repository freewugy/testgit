/******************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: $ cssong
 * $LastChangedDate: $ 2015.06.19
 * $LastChangedRevision: $ V1.1.0
 * Description: Analog Audio Mute Controler
 * Note:
 *         V1.1.0
 *                   check protperty
 *****************************************************************************/

#include "audio_mute_ctrl.h"
#include "nxclient.h"

#include "cutils/log.h"	//logcat 

#undef LOG_TAG
#define LOG_TAG	"AUDIOMUTE"

//#define SUPPORT_TVSTORM_AUDIOMUTE_DEBUG
//#define SUPPORT_TVSTORM_AUDIOMUTE_DEBUG
#ifndef SUPPORT_TVSTORM_AUDIOMUTE_DEBUG
//#undef ALOGE
//#define ALOGE(...)
#undef ALOGD
#define ALOGD(...)
//#undef ALOGI
//#define ALOGI(...)
#endif


// TVSTORM audio mute Version
#define TVSTORM_AUDIOMUTE_VERSION 1
#define TVSTORM_AUDIOMUTE_PATCHLEVEL 1
#define TVSTORM_AUDIOMUTE_SUBLEVEL 0

// TVSTORM audio mute date
#define TVSTORM_AUDIOMUTE_YEAR 2015
#define TVSTORM_AUDIOMUTE_MONTH 06
#define TVSTORM_AUDIOMUTE_DAY 19


#define PROPERTY_NEXUS_PLATFORMINIT   "hw.nexus.platforminit"
#define PROPERTY_TVSTORM_STANDBY   "persist.sys.tvstorm.standbymode"

bool g_initStatus = false;

NexusIPCClientBase *g_ipcClient = NULL;
NexusClientContext *g_nexusClient = NULL;

TVSTORM_Error TVSTORM_AUDIO_MUTE_Init(void)
{
    TVSTORM_Error errCode = TVSTORM_OK;
   
    if (g_initStatus) {
        ALOGE("TVSTORM_MUTE_CONTROL already initialized.\n");
        return TVSTORM_ERROR;
    }
    
#ifdef NXCLIENT_BINDER
    b_refsw_client_client_configuration config;
    b_refsw_client_client_info client_info;
 
    g_ipcClient = NexusIPCClientFactory::getClient("TVSTORM_MUTE_CONTROL");
    if (g_ipcClient == NULL) {
        ALOGE("could not get NexusIPCClient!\n");
        return TVSTORM_ERROR;
    }
    
    BKNI_Memset(&config, 0, sizeof(config));
    BKNI_Snprintf(config.name.string,sizeof(config.name.string),"TVSTORM_MUTE_CONTROL");
    g_nexusClient = g_ipcClient->createClientContext(&config);
    if (g_nexusClient == NULL) {
        ALOGE("%s: Could not create Nexus Client Context!!!", __FUNCTION__); 
        delete g_ipcClient;
    }
#else
    NEXUS_Error rc;

    NxClient_JoinSettings joinSettings;

    NxClient_GetDefaultJoinSettings(&joinSettings);

    snprintf(joinSettings.name, NXCLIENT_MAX_NAME, "TVSTORM_MUTE_CONTROL");
    rc = NxClient_Join(&joinSettings);
#endif
    
    g_initStatus = true;

    return TVSTORM_OK;

}

TVSTORM_Error TVSTORM_AUDIO_MUTE_Uninit(void)
{
    TVSTORM_Error errCode = TVSTORM_OK;
    
    if (g_initStatus) { 
#ifdef NXCLIENT_BINDER
        g_ipcClient->disconnectClientResources(g_nexusClient);
        g_ipcClient->destroyClientContext(g_nexusClient);
        g_nexusClient = NULL;
        delete g_ipcClient;
#else
        NxClient_Uninit();
#endif
        
        g_initStatus = false;
    } else {
        ALOGE("TVSTORM_MUTE_CONTROL already deinitialized.");
        return TVSTORM_ERROR;
    }
    
    return errCode;
}


TVSTORM_Error TVSTORM_AUDIO_MUTE_Control(int state)
{
	TVSTORM_Error errCode = TVSTORM_OK;
	NEXUS_GpioHandle 	AudioMuteGpioHandle;
	NEXUS_GpioSettings 	AudioMuteSettings;
	NEXUS_GpioValue n_gpio_value = NEXUS_GpioValue_eLow;

	if(state == AUD_MUTE_OFF){
		/*AON_GPIO_10 - Audio mute  off */
		NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType_eAonStandard, &AudioMuteSettings);
		ALOGI("Audio Mute Control - Off\n");
		AudioMuteSettings.mode  = NEXUS_GpioMode_eOutputPushPull;  /* eInput/eOutputOpenDrain/eOutputPushPull */
		n_gpio_value = NEXUS_GpioValue_eHigh;
		AudioMuteSettings.value = n_gpio_value;     /* eLow/eHigh */

		AudioMuteGpioHandle = NEXUS_Gpio_Open(NEXUS_GpioType_eAonStandard, 10, &AudioMuteSettings);
		NEXUS_Gpio_Close(AudioMuteGpioHandle);
	}else{
		/*AON_GPIO_10 - Audio mute  on */
		NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType_eAonStandard, &AudioMuteSettings);
		ALOGI("Audio Mute Control - On\n");
		n_gpio_value = NEXUS_GpioValue_eLow;
		AudioMuteSettings.value = n_gpio_value;     /* eLow/eHigh */

		AudioMuteGpioHandle = NEXUS_Gpio_Open(NEXUS_GpioType_eAonStandard, 10, &AudioMuteSettings);
		NEXUS_Gpio_Close(AudioMuteGpioHandle);
	}

	return TVSTORM_OK;
}

void TVSTORM_AUDIO_MUTE_version(TVSTORM_AUDIOMUTE_VERSION_S *audiomute_version)
{
    audiomute_version->version = TVSTORM_AUDIOMUTE_VERSION;
    audiomute_version->patchlevel = TVSTORM_AUDIOMUTE_PATCHLEVEL;
    audiomute_version->sublevel = TVSTORM_AUDIOMUTE_SUBLEVEL;

    return;
}

void TVSTORM_AUDIO_MUTE_last_date(TVSTORM_AUDIOMUTE_DATE_S *audiomute_date)
{
    audiomute_date->year = TVSTORM_AUDIOMUTE_YEAR;
    audiomute_date->month = TVSTORM_AUDIOMUTE_MONTH;
    audiomute_date->day = TVSTORM_AUDIOMUTE_DAY;

    return;
}

int main(int argc, char **argv)
{
	char property_nexus_platform_val[256] = {0, };
	bool done = false;
	int port = 0;
	int value =0;
	TVSTORM_AUDIOMUTE_VERSION_S audiomute_version;
	TVSTORM_AUDIOMUTE_DATE_S audiomute_date;
	char property_tvstorm_standby_val[256] = {0, };
	static bool is_standby_on = true, old_is_standby_on = true; //default is standy on
	TVSTORM_Error ret = TVSTORM_OK;

	TVSTORM_AUDIO_MUTE_version(&audiomute_version);
	ALOGI("audio mute version : V%d.%d.%d\n", audiomute_version.version,  audiomute_version.patchlevel, audiomute_version.sublevel);
	TVSTORM_AUDIO_MUTE_last_date(&audiomute_date);
	ALOGI("audio mute last date : year = %d, month = %d, day = %d\n", audiomute_date.year,  audiomute_date.month, audiomute_date.day);

	while(1)
	{
		__system_property_get(PROPERTY_NEXUS_PLATFORMINIT,property_nexus_platform_val );
		if(!strcmp("on", property_nexus_platform_val)){
			if (TVSTORM_AUDIO_MUTE_Init() != TVSTORM_OK) {
				ALOGE("TVSTORM_AUDIO_MUTE_Init error\n");
				return -1;
			}

			while (1){
				usleep(350000); // 350ms
				__system_property_get(PROPERTY_TVSTORM_STANDBY,property_tvstorm_standby_val );
				if(!strcmp("1", property_tvstorm_standby_val)) is_standby_on = true;
				else if(!strcmp("0", property_tvstorm_standby_val)) is_standby_on = false;

				if(old_is_standby_on != is_standby_on){
					old_is_standby_on = is_standby_on;				
					if(is_standby_on){	/* sleep on - standby */					
						ret = TVSTORM_AUDIO_MUTE_Control(AUD_MUTE_ON);
						if(ret != TVSTORM_OK){
							ALOGE("Nexus error : 0x%x", ret);
							goto nexus_error;
						}
					}else{ /* wake up - active */
						ret = TVSTORM_AUDIO_MUTE_Control(AUD_MUTE_OFF);
						if(ret != TVSTORM_OK){
							ALOGE("Nexus error : 0x%x", ret);
							goto nexus_error;
						}
					}
				}
			}		
		}
		else{
			sleep(1);
		}
	}

nexus_error:

	TVSTORM_AUDIO_MUTE_Uninit();
	return 0;
}
