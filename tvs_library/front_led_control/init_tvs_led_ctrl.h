/******************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: $ cssong
 * $LastChangedDate: $ 2015.07.14
 * $LastChangedRevision: $ V1.3.2
 * Description: Init Front LED Controler
 * Note:
 *         V1.1.0 
 *                   Change Scenarios
 *                   remove Network valid status 
 *                   add eeprom property of passive standby mode
 *                   STB is not provisioned mode
 *         V1.1.1
 *                   Fix sleep
 *         V1.1.2
 *                   demo Scenarios
 *                   wifi on/off
 *         V1.2.0
 *                   wifi on/off
 *                   wifi wpa_supplicant.conf
 *         V1.2.1
 *                   fix deleted wifi wpa_supplicant.conf
 *         V1.2.2
 *                   fix arp to ifc_get_info
 *         V1.3.0
 *                   add rcu led control
 *         V1.3.1
 *                   add voice property - Voice search status is tvstorm.bt.rcu.voice.status
 *         V1.3.2
 *                   fix - property init
 *****************************************************************************/
#ifndef INIT_TVSTORM_LED_CONTROL_H
#define INIT_TVSTORM_LED_CONTROL_H

#include "front_led_ctrl.h"

#ifdef __cplusplus
extern "C"
{
#endif

// TVSTORM init tvs led ctrl Version
#define TVSTORM_INITTVSLEDCTRL_VERSION 1
#define TVSTORM_INITTVSLEDCTRL_PATCHLEVEL 5
#define TVSTORM_INITTVSLEDCTRL_SUBLEVEL 3

// TVSTORM init tvs led ctrl date
#define TVSTORM_INITTVSLEDCTRL_YEAR 2016
#define TVSTORM_INITTVSLEDCTRL_MONTH 7
#define TVSTORM_INITTVSLEDCTRL_DAY 25

typedef struct TVSTORM_INITTVSLEDCTRL_VERSION_S
{
    uint16_t    version;
    uint16_t    patchlevel;
    uint16_t    sublevel;
}TVSTORM_INITTVSLEDCTRL_VERSION_T;

typedef struct TVSTORM_INITTVSLEDCTRL_DATE_S
{
    uint16_t    year;
    uint16_t    month;
    uint16_t    day;
}TVSTORM_INITTVSLEDCTRL_DATE_T;

void TVSTORM_InitTvsLedCtrl_version(TVSTORM_INITTVSLEDCTRL_VERSION_T *inittvsled_version);
void TVSTORM_InitTvsLedCtrl_last_date(TVSTORM_INITTVSLEDCTRL_DATE_T *inittvsled_date);

void initProperty();
void initLedVersion();
void LEDStateInit();

void changePowerLEDStatus(bool standby);
void changeEthernetLEDStatus(int led_blinking);
void changeWifiLEDStatus(int led_blinking);
void changeCMUpdateStatus(int led_blinking);
int getEthernetStatus();

#ifdef __cplusplus
}
#endif

#endif /* INIT_TVSTORM_LED_CONTROL_H */
