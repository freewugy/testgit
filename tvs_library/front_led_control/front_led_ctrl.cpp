/******************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: $ cssong
 * $LastChangedDate: $ 2015.05.28
 * $LastChangedRevision: $ V1.2.0
 * Description: Front LED Controler Library
 * Note:
 *         V1.1.0 
 *                  add TVS_NETWORK_STATUS
 *         V1.2.0 
 *                  add TVSTORM_NETWORK_CONFIGURED
 *****************************************************************************/
#include "front_led_ctrl.h"
#include "nxclient.h"

bool g_initStatus = false;

NexusIPCClientBase *g_ipcClient = NULL;
NexusClientContext *g_nexusClient = NULL;

TVSTORM_Error TVSTORM_Front_Led_Init(void)
{
    TVSTORM_Error errCode = TVSTORM_OK;
    
    if (g_initStatus) {
        ALOGE("TVSTORM_FRONT_LED_CONTROL already initialized.\n");
        return TVSTORM_ERROR;
    }
    
#ifdef NXCLIENT_BINDER
    b_refsw_client_client_configuration config;
    b_refsw_client_client_info client_info;

    g_ipcClient = NexusIPCClientFactory::getClient("TVSTORM_FRONT_LED_CONTROL");
    if (g_ipcClient == NULL) {
        ALOGE("could not get NexusIPCClient!\n");
        return TVSTORM_ERROR;
    }
    
    BKNI_Memset(&config, 0, sizeof(config));
    BKNI_Snprintf(config.name.string,sizeof(config.name.string),"TVSTORM_FRONT_LED_CONTROL");
    g_nexusClient = g_ipcClient->createClientContext(&config);
    if (g_nexusClient == NULL) {
        ALOGE("%s: Could not create Nexus Client Context!!!", __FUNCTION__); 
        delete g_ipcClient;
		g_ipcClient = NULL;
    }
#else
    NEXUS_Error rc;

    NxClient_JoinSettings joinSettings;

    NxClient_GetDefaultJoinSettings(&joinSettings);

    snprintf(joinSettings.name, NXCLIENT_MAX_NAME, "TVSTORM_FRONT_LED_CONTROL");
    rc = NxClient_Join(&joinSettings);
#endif
   
    g_initStatus = true;

    return TVSTORM_OK;

}

TVSTORM_Error TVSTORM_Front_Led_Uninit(void)
{
    TVSTORM_Error errCode = TVSTORM_OK;
    
    if (g_initStatus) { 
#ifdef NXCLIENT_BINDER
        g_ipcClient->disconnectClientResources(g_nexusClient);
        g_ipcClient->destroyClientContext(g_nexusClient);
        g_nexusClient = NULL;
        delete g_ipcClient;
		g_ipcClient = NULL;
#else
        NxClient_Uninit();
#endif
        g_initStatus = false;
    } else {
        ALOGE("TVSTORM_FRONT_LED_CONTROL already deinitialized.");
        return TVSTORM_ERROR;
    }
    
    return errCode;
}


TVSTORM_Error TVSTORM_Front_Led_Control(TVS_LED_CONTROL tvs_led, int state)
{
	TVSTORM_Error errCode = TVSTORM_OK;
	NEXUS_GpioValue n_gpio_value = NEXUS_GpioValue_eLow;	/* eLow/eHigh */
	NEXUS_GpioHandle 	n_GpioHandle;
	NEXUS_GpioSettings 	n_GpioSettings;
	
	
	if (state == TVS_LED_OFF){	/* off is high */
		n_gpio_value = NEXUS_GpioValue_eHigh;
	}
	else if(state == TVS_LED_ON){	/* on is low */
		n_gpio_value = NEXUS_GpioValue_eLow;
	}

	switch(tvs_led)
	{
		case LED_POWER_GREEN:
			/* AON_GPIO_001 - Power Green LED */
			NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType_eAonStandard, &n_GpioSettings);
			n_GpioSettings.mode  = NEXUS_GpioMode_eOutputPushPull;  /* eInput/eOutputOpenDrain/eOutputPushPull */
			n_GpioSettings.value = n_gpio_value;     /* eLow/eHigh */

			n_GpioHandle = NEXUS_Gpio_Open(NEXUS_GpioType_eAonStandard, 1, &n_GpioSettings);
			NEXUS_Gpio_Close(n_GpioHandle);
            if(NULL == n_GpioHandle)
            {
                ALOGE("POWER_LED_GREEN GPIO handle failed \n");
            }
			break;			
		case LED_POWER_RED:
			/* AON_GPIO_017 - Power Red LED */
			NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType_eAonStandard, &n_GpioSettings);
			n_GpioSettings.mode  = NEXUS_GpioMode_eOutputPushPull;  /* eInput/eOutputOpenDrain/eOutputPushPull */
			n_GpioSettings.value = n_gpio_value;     /* eLow/eHigh */

			n_GpioHandle = NEXUS_Gpio_Open(NEXUS_GpioType_eAonStandard, 17, &n_GpioSettings);
            if(NULL == n_GpioHandle)
            {
                ALOGE("POWER_LED_RED GPIO handle failed \n");
            }
			NEXUS_Gpio_Close(n_GpioHandle);
			break;
		case LED_ETHERNET_GREEN:
			/* GPIO_000 - Ethernet Green LED */
			NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType_eStandard, &n_GpioSettings);
			n_GpioSettings.mode  = NEXUS_GpioMode_eOutputPushPull;  /* eInput/eOutputOpenDrain/eOutputPushPull */
			n_GpioSettings.value = n_gpio_value;     /* eLow/eHigh */
#ifdef SH960C_LN
			n_GpioHandle = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 60, &n_GpioSettings);
#else /*ST950I_LN */
			n_GpioHandle = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 0, &n_GpioSettings);
#endif
            if(NULL == n_GpioHandle)
            {
                ALOGE("CM_LED_GREEN GPIO handle failed \n");
            }
			NEXUS_Gpio_Close(n_GpioHandle);
			break;
		case LED_ETHERNET_RED:
			/* GPIO_003 - Ethernet Red LED*/
			NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType_eStandard, &n_GpioSettings);
			n_GpioSettings.mode  = NEXUS_GpioMode_eOutputPushPull;  /* eInput/eOutputOpenDrain/eOutputPushPull */
			n_GpioSettings.value = n_gpio_value;     /* eLow/eHigh */
#ifdef SH960C_LN
			n_GpioHandle = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 61, &n_GpioSettings);
#else		/* ST950I_LN */
			n_GpioHandle = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 3, &n_GpioSettings);
#endif
            if(NULL == n_GpioHandle)
            {
                ALOGE("CM_LED_RED GPIO handle failed \n");
            }
			NEXUS_Gpio_Close(n_GpioHandle);
			break;
		case LED_WIFI_GREEN:
			/*GPIO_048 - Wi-Fi Green LED */
			NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType_eStandard, &n_GpioSettings);
			n_GpioSettings.mode  = NEXUS_GpioMode_eOutputPushPull;  /* eInput/eOutputOpenDrain/eOutputPushPull */
			n_GpioSettings.value = n_gpio_value;     /* eLow/eHigh */

			n_GpioHandle = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 48, &n_GpioSettings);
            if(NULL == n_GpioHandle)
            {
                ALOGE("WIFI_LED_GREEN GPIO handle failed \n");
            }
			NEXUS_Gpio_Close(n_GpioHandle);
			break;
		case LED_WIFI_RED:
			/* GPIO_049 - Wi-Fi Red LED */
			NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType_eStandard, &n_GpioSettings);
			n_GpioSettings.mode  = NEXUS_GpioMode_eOutputPushPull;  /* eInput/eOutputOpenDrain/eOutputPushPull */
			n_GpioSettings.value = n_gpio_value;     /* eLow/eHigh */

			n_GpioHandle = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 49, &n_GpioSettings);
            if(NULL == n_GpioHandle)
            {
                ALOGE("WIFI_LED_RED GPIO handle failed \n");
            }
			NEXUS_Gpio_Close(n_GpioHandle);
			break;		
		default:
			break;
	}

	return TVSTORM_OK;
}
