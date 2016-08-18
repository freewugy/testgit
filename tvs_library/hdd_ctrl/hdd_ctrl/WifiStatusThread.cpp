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

#define TVS_LOG_ENABLED
#define LOG_TAG "WifiStatusThread"

#include "WifiStatusThread.h"
#include "WaitUserThread.h"

sp<WifiStatusThread> pWifiStatus;

WifiStatusThread::~WifiStatusThread()
{
	dlclose(handle);
}

void WifiStatusThread::onFirstRef()
{
	//ALOGD("WifiStatusThread::onFirstRef()\n");
	handle = dlopen("/system/lib/libledjni.so", RTLD_NOW);
	if (handle != NULL)
	{
		power_led_onoff = (int(*)(int)) dlsym( handle, "power_led_onoff" );
		cablemodem_led_onoff = (int(*)(int)) dlsym( handle, "cablemodem_led_onoff" );
		wifi_led_onoff = (int(*)(int)) dlsym( handle, "wifi_led_onoff" );
	}
	else
	{
		power_led_onoff = NULL;
		cablemodem_led_onoff = NULL;
		wifi_led_onoff = NULL;
	}
}

status_t WifiStatusThread::readyToRun()
{
	//ALOGD("WifiStatusThread::readyToRun()\n");
	CurWifiStatus = 0;	// OFF
	bBlinkingFlag = false;
	bToggleBlinking = false;
	return 0;
}

bool WifiStatusThread::threadLoop()
{
	//ALOGD("WifiStatusThread::threadLoop()\n");

	char wlan_driver_status_prop[PROPERTY_VALUE_MAX] = {'\0'};
	if (property_get("wlan.driver.status", wlan_driver_status_prop, NULL)
		&& !strcmp(wlan_driver_status_prop, "ok"))
	{
		char dhcp_wlan0_result_prop[PROPERTY_VALUE_MAX] = {'\0'};
		if (property_get("dhcp.wlan0.result", dhcp_wlan0_result_prop, NULL)
			&& !strcmp(dhcp_wlan0_result_prop, "ok"))
		{
			if (CurWifiStatus != 2)
			{
				CurWifiStatus = 2;
				WIFI_WifiDriverStatus(2);	//ON(2)
			}
		}
		else    // "failed"
		{
			if (CurWifiStatus != 1)
			{
				CurWifiStatus = 1;
				WIFI_WifiDriverStatus(1);	//BLINKING(1)
			}
		}
	}
	else    // "unloaded"
	{
		if (CurWifiStatus != 0)
		{
			CurWifiStatus = 0;
			WIFI_WifiDriverStatus(0);	//OFF(0)
		}
	}
	usleep(1000000); // 1000ms

	if (bBlinkingFlag)
	{
		if (bToggleBlinking)
		{
			bToggleBlinking = false;
			wifi_led_onoff(0);
		}
		else
		{
			bToggleBlinking = true;
			wifi_led_onoff(1);
		}
	}

	return true;
}

bool WifiStatusThread::setPowerLed(uint8_t power_status)
{
	if (power_led_onoff != NULL && power_status <= 2)
	{
		power_led_onoff(power_status);
		return true;
	}
	else
	{
		return false;
	}
}

void WifiStatusThread::WIFI_WifiDriverStatus(uint8_t wifi_status)
{
	if (wifi_status == 0)
		ALOGD("===============> WIFI_WifiDriverStatus: wifi_status [OFF(0)]\n");
	else if (wifi_status == 1)
		ALOGD("===============> WIFI_WifiDriverStatus: wifi_status [BLINKING(1)]\n");
	else if (wifi_status == 2)
		ALOGD("===============> WIFI_WifiDriverStatus: wifi_status [ON(2)]\n");

	uint8_t sndMsgConfig[64];
	memset(sndMsgConfig, 0, 64);
	sndMsgConfig[0] = 1;
	sndMsgConfig[1] = WIFI_WIFI_STATUS;
	sndMsgConfig[2] = 1;
	sndMsgConfig[3] = wifi_status;
	pWaitUser->sendToThread(sndMsgConfig, sizeof(sndMsgConfig));
}

uint8_t WifiStatusThread::WIFI_getWifiStatusLed()
{
	return CurWifiStatus;	// 0(OFF) / 1(BLINKING) / 2(ON)
}

bool WifiStatusThread::WIFI_setWifiStatusLed(uint8_t wifi_status)
{
	if (wifi_led_onoff != NULL && wifi_status <= 2)
	{
		bBlinkingFlag = false;
		switch (wifi_status)
		{
			case 0:	// OFF
				wifi_led_onoff(0);
				break;
			case 1:	// BLINKING
				bBlinkingFlag = true;
				bToggleBlinking = false;
				break;
			case 2:	// ON
				wifi_led_onoff(1);
				break;
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool WifiStatusThread::setDocsisStatusLed(uint8_t docsis_status)
{
	if (cablemodem_led_onoff != NULL && docsis_status <= 3)
	{
		//pWaitUser->bBlinkingFlag = false;
		switch (docsis_status)
		{
			case 0: // OFF
				cablemodem_led_onoff(0);
				break;
			case 1: // BLINKING
				//pWaitUser->bBlinkingFlag = true;
				//pWaitUser->bToggleBlinking = false;
				break;
			case 2: // BLINKING
				//pWaitUser->bBlinkingFlag = true;
				//pWaitUser->bToggleBlinking = false;
				break;
			case 3: // ON
				cablemodem_led_onoff(1);
				break;
		}
		return true;
	}
	else
	{
		return false;
	}
}
