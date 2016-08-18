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

#ifndef HDD_CTRL_WIFISTATUSTHREAD_H_
#define HDD_CTRL_WIFISTATUSTHREAD_H_

#include "hdd_ctrl.h"

class WifiStatusThread : public Thread
{
	bool	bBlinkingFlag;
	bool	bToggleBlinking;

	void	*handle;
	int		(*power_led_onoff)(int);
	int		(*cablemodem_led_onoff)(int);
	int		(*wifi_led_onoff)(int);
public:
	enum {
		ETH_RENEW					= 115
	};

	enum {
		WIFI_GET_WIFI_STATUS_LED	= 125,
		WIFI_SET_WIFI_STATUS_LED	= 126,
		WIFI_WIFI_STATUS			= 127
	};

	enum {
		ECM_SET_DOCSIS_STATUS_LED   = 252
	};

	enum {
		SET_POWER_LED           	= 253
	};

	uint8_t	CurWifiStatus;	// 0(OFF) / 1(BLINKING) / 2(ON)

	~WifiStatusThread();

    void onFirstRef();
    virtual status_t readyToRun();
    virtual bool threadLoop();

	void WIFI_WifiDriverStatus(uint8_t wifi_status);
	uint8_t WIFI_getWifiStatusLed();
	bool WIFI_setWifiStatusLed(uint8_t wifi_status);

	bool setDocsisStatusLed(uint8_t docsis_status);

	bool setPowerLed(uint8_t power_status);
};

extern sp<WifiStatusThread> pWifiStatus;

#endif /* HDD_CTRL_WIFISTATUSTHREAD_H_ */
