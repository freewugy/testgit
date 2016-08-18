/******************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: $ cssong
 * $LastChangedDate: $ 2015.04.21
 * $LastChangedRevision: $ V1.0.1
 * Description: Fan control
 * Note:
 *****************************************************************************/

#ifndef FAN_PWM_CTRL_H
#define FAN_PWM_CTRL_H

#include "nexus_types.h"
#include "nexus_platform.h"
#include "nexus_ipc_client_factory.h"
#include "nexus_temp_monitor.h"
#include "nexus_gpio.h"
#include "nexus_pwm.h"
#include "bkni.h"

#include <stdio.h>
#include <utils/Log.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

//#define TVSTORM_PWM_TEST

//#define SUPPORT_TVSTORM_TEMP_MONITOR

#ifndef CONSTANT_FREQUENCY_PWM_MODE
	#define CONSTANT_FREQUENCY_PWM_MODE
#else
	#define VARIABLE_FREQUENCY_PWM_MODE
#endif

typedef enum
{
	TVSTORM_ERROR = -1,
	TVSTORM_OK = 0,
	TVSTORM_ERROR_NOT_IMPLEMENTED,
	TVSTORM_ERROR_INVALID_PARAMS,
} TVSTORM_Error;

typedef struct TVSTORM_FANCTRL_VERSION_S
{
    uint16_t    version;
    uint16_t    patchlevel;
    uint16_t    sublevel;
}TVSTORM_FANCTRL_VERSION_T;

typedef struct TVSTORM_FANCTRL_DATE_S
{
    uint16_t    year;
    uint16_t    month;
    uint16_t    day;
}TVSTORM_FANCTRL_DATE_T;

TVSTORM_Error TVSTORM_Fan_Pwm_Init(void);
TVSTORM_Error TVSTORM_Fan_Pwm_Uninit(void);
TVSTORM_Error TVSTORM_Fan_Pwm_Control(int state);

#ifdef SUPPORT_TVSTORM_TEMP_MONITOR
TVSTORM_Error TVSTORM_Temp_monitor_Init(void);
TVSTORM_Error TVSTORM_Temp_monitor_Uninit(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* FAN_PWM_CTRL_H */
