/******************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: $ cssong
 * $LastChangedDate: $ 2015.05.14
 * $LastChangedRevision: $ V1.0.2
 * Description: Power button uinput
 * Note:
 *****************************************************************************/

#ifndef TVSTORM_POWER_BUTTON_H
#define TVSTORM_POWER_BUTTON_H

#include "nexus_types.h"
#include "nexus_platform.h"
#include "nexus_ipc_client_factory.h"
#include "nexus_gpio.h"
#include "bkni.h"

#include <utils/Log.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#include <linux/input.h>
#include <linux/uinput.h>

#ifdef __cplusplus
extern "C"
{
#endif

//#define TVSTORM_POWER_BUTTON_TEST

#define TVSTORM_UINPUT_DEVICE_PATH		"/dev/uinput"
#define pbtn_msleep(time)	usleep(time * 1000)

typedef enum
{
	TVSTORM_ERROR = -1,
	TVSTORM_OK = 0,
	TVSTORM_ERROR_NOT_IMPLEMENTED,
	TVSTORM_ERROR_INVALID_PARAMS,
} TVSTORM_Error;

typedef struct TVSTORM_PWBUTTON_VERSION_S
{
    uint16_t    version;
    uint16_t    patchlevel;
    uint16_t    sublevel;
}TVSTORM_PWBUTTON_VERSION_T;

typedef struct TVSTORM_PWBUTTON_DATE_S
{
    uint16_t    year;
    uint16_t    month;
    uint16_t    day;
}TVSTORM_PWBUTTON_DATE_T;

enum
{
	AUD_MUTE_OFF =0,
	AUD_MUTE_ON,
};

TVSTORM_Error TVSTORM_PWBUTTON_Init(void);
TVSTORM_Error TVSTORM_PWBUTTON_Uninit(void);
int TVSTORM_PWBUTTON_SendEvent(uint16_t type, uint16_t code, int32_t value);
void TVSTORM_PWBUTTON_version(TVSTORM_PWBUTTON_VERSION_S *pwbutton_version);
void TVSTORM_PWBUTTON_last_date(TVSTORM_PWBUTTON_DATE_S *pwbutton_date);

#ifdef __cplusplus
}
#endif

#endif /* TVSTORM_POWER_BUTTON_H */

