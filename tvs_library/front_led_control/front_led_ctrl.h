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

#ifndef TVSTORM_FRONT_LED_CTRL_H
#define TVSTORM_FRONT_LED_CTRL_H

#include "nexus_types.h"
#include "nexus_platform.h"
#include "nexus_ipc_client_factory.h"
#include "nexus_gpio.h"
#include "bkni.h"

#include <stdio.h>
#include <utils/Log.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum
{
	TVSTORM_ERROR = -1,
	TVSTORM_OK = 0,
	TVSTORM_ERROR_NOT_IMPLEMENTED,
	TVSTORM_ERROR_INVALID_PARAMS,
} TVSTORM_Error;

typedef enum
{
	LED_POWER_GREEN = 0,
	LED_POWER_RED,
	LED_ETHERNET_GREEN,
	LED_ETHERNET_RED,
	LED_WIFI_GREEN,
	LED_WIFI_RED,
} TVS_LED_CONTROL;


enum {
	kNotFoundInterface = -1,
	kFoundInterface = 0,
	kNotConfiguredInterface,
	kConfiguredInterface,
	kInValidIPInterface,
	kValidIPInterface,
	kInterfaceStateMAX,
};

enum {
	TVSTORM_CM_USB_UPDATE_OFF = 0,
    TVSTORM_CM_USB_UPDATING,
	TVSTORM_CM_USB_UPDATED,
 	TVSTORM_CM_USB_SAME_VER,
 	TVSTORM_CM_USB_UPDATE_FAIL,
};

enum {
	TVSTORM_SYSTEM_STANDBY = 0,
	TVSTORM_SYSTEM_WAKEUP,
	TVSTORM_SYSTEM_PARTIAL_UPDATE
};
enum
{
	TVS_LED_OFF =0,
	TVS_LED_ON,
};

TVSTORM_Error TVSTORM_Front_Led_Init(void);
TVSTORM_Error TVSTORM_Front_Led_Uninit(void);
TVSTORM_Error TVSTORM_Front_Led_Control(TVS_LED_CONTROL tvs_led, int state); /* NEXUS_GpioValue gpio_value */

#ifdef __cplusplus
}
#endif

#endif /* TVSTORM_FRONT_LED_CTRL_H */
