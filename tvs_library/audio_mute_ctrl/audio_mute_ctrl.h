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

#ifndef AUDIO_MUTE_CTRL_H
#define AUDIO_MUTE_CTRL_H

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

typedef struct TVSTORM_AUDIOMUTE_VERSION_S
{
    uint16_t    version;
    uint16_t    patchlevel;
    uint16_t    sublevel;
}TVSTORM_AUDIOMUTE_VERSION_T;

typedef struct TVSTORM_AUDIOMUTE_DATE_S
{
    uint16_t    year;
    uint16_t    month;
    uint16_t    day;
}TVSTORM_AUDIOMUTE_DATE_T;

enum
{
	AUD_MUTE_OFF =0,
	AUD_MUTE_ON,
};

TVSTORM_Error TVSTORM_AUDIO_MUTE_Init(void);
TVSTORM_Error TVSTORM_AUDIO_MUTE_Uninit(void);
TVSTORM_Error TVSTORM_AUDIO_MUTE_Control(int state);
void TVSTORM_AUDIO_MUTE_version(TVSTORM_AUDIOMUTE_VERSION_S *audiomute_version);
void TVSTORM_AUDIO_MUTE_last_date(TVSTORM_AUDIOMUTE_DATE_S *audiomute_date);

#ifdef __cplusplus
}
#endif

#endif /* AUDIO_MUTE_CTRL_H */
