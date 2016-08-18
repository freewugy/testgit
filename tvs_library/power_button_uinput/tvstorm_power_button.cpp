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
#include "tvstorm_power_button.h"
#include "nxclient.h"

//#define SUPPORT_TVSTORM_PWBUTTON_DEBUG

#ifdef SUPPORT_TVSTORM_PWBUTTON_DEBUG
#define LOGE(...) fprintf(stdout, "E:" __VA_ARGS__)
#define LOGW(...) fprintf(stdout, "W:" __VA_ARGS__)
#define LOGI(...) fprintf(stdout, "I:" __VA_ARGS__)
#else
#define LOGE(...) do {} while (0)
#define LOGW(...) do {} while (0)
#define LOGI
#endif

// TVSTORM power button Version
#define TVSTORM_PWBUTTON_VERSION 1
#define TVSTORM_PWBUTTON_PATCHLEVEL 0
#define TVSTORM_PWBUTTON_SUBLEVEL 3

// TVSTORM power button date
#define TVSTORM_PWBUTTON_YEAR 2015
#define TVSTORM_PWBUTTON_MONTH 05
#define TVSTORM_PWBUTTON_DAY 14

#define PROPERTY_BOOTCOMPLETE		  "sys.boot_completed"
#define PROPERTY_NEXUS_PLATFORMINIT   "hw.nexus.platforminit"
#define POWER_DELAY_INTERVAL	500


bool g_initStatus = false;

NEXUS_GpioHandle pbtn_handle;

NexusIPCClientBase *g_ipcClient = NULL;
NexusClientContext *g_nexusClient = NULL;

int g_uinput_fd = -1;

char g_pkey_cnt = 0;

void* processPowKeyproc (void* param);


TVSTORM_Error TVSTORM_PWBUTTON_Init(void)
{
	TVSTORM_Error errCode = TVSTORM_OK;
	struct uinput_user_dev uinput;

	if (g_initStatus) {
		LOGE("tvstorm_FBUTTON already initialized.");
		return TVSTORM_ERROR;
	}

#ifdef NXCLIENT_BINDER
	b_refsw_client_client_configuration config;
	b_refsw_client_client_info client_info;
	g_ipcClient = NexusIPCClientFactory::getClient("TVSTORM_PWBUTTON");
	if (g_ipcClient == NULL) {
		LOGE("could not get NexusIPCClient!");
		return TVSTORM_ERROR;
	}

	BKNI_Memset(&config, 0, sizeof(config));
	BKNI_Snprintf(config.name.string,sizeof(config.name.string),"TVSTORM_PWBUTTON");
	g_nexusClient = g_ipcClient->createClientContext(&config);
#else
    NEXUS_Error rc;

    NxClient_JoinSettings joinSettings;

    NxClient_GetDefaultJoinSettings(&joinSettings);

    snprintf(joinSettings.name, NXCLIENT_MAX_NAME, "TVSTORM_PWBUTTON");
    rc = NxClient_Join(&joinSettings);
#endif

	g_uinput_fd = open(TVSTORM_UINPUT_DEVICE_PATH, O_WRONLY| O_NDELAY);
	if(g_uinput_fd < 0)
	{
		LOGE("Unable to open %s\n", TVSTORM_UINPUT_DEVICE_PATH);
		return TVSTORM_ERROR;
	}

	memset(&uinput,0, sizeof(uinput)); // Initialize the uInput device to NULL
	strncpy(uinput.name, "TvstormPwButton", UINPUT_MAX_NAME_SIZE);
	ioctl(g_uinput_fd, UI_SET_PHYS, "TvstormPwButton");
	ioctl(g_uinput_fd, UI_SET_EVBIT, EV_KEY);
	ioctl(g_uinput_fd, UI_SET_KEYBIT, KEY_POWER);
	write(g_uinput_fd, &uinput, sizeof(uinput));

	if(ioctl(g_uinput_fd, UI_DEV_CREATE))
	{
		LOGE("unable to create UINPUT device. \n");
		return TVSTORM_ERROR;
	}
	g_initStatus = true;

	pthread_t thread_id;
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&thread_id, &attr, processPowKeyproc, NULL);
	pthread_attr_destroy(&attr);

	return errCode;
}

TVSTORM_Error TVSTORM_PWBUTTON_Uninit(void)
{
	TVSTORM_Error errCode = TVSTORM_OK;

	ioctl(g_uinput_fd, UI_DEV_DESTROY);

	if(g_uinput_fd != -1)
	{
		close(g_uinput_fd);
	}
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
		LOGE("TVSTORM_PWBUTTON already deinitialized.");
		return TVSTORM_ERROR;
	}

	return errCode;
}

int TVSTORM_PWBUTTON_SendEvent(uint16_t type, uint16_t code, int32_t value)
{
	struct input_event event;
	struct timeval tv;
	int ret =0;

	memset(&event, 0, sizeof(event));

	gettimeofday(&tv, NULL);
	event.type = type;
	event.code = code;
	event.value = value;
	event.time = tv;

	ret = write(g_uinput_fd, &event, sizeof(event));
	LOGI("%s : type=%04x code=%04x, value=%04x, ret=%d\n", __FUNCTION__, type, code, value, ret);

	return ret;
}

/* power key interrupt */
void Stby_BUTTON_interrupt(void *context, int param)
{
	NEXUS_Error rc;
	NEXUS_GpioStatus gpioStatus;
	BSTD_UNUSED(param);
	BKNI_SetEvent((BKNI_EventHandle)context);
	static char old_value = 0;

	if(pbtn_handle != NULL)
	{
		rc = NEXUS_Gpio_GetStatus(pbtn_handle, &gpioStatus);
		LOGI("[power button interrupt AON GPIO000] POWER STBY: value %d\n", gpioStatus.value);
		BDBG_ASSERT(!rc);
		if( (gpioStatus.value == 1) && (old_value == 0) )
		{
			g_pkey_cnt++;
		}
		old_value = gpioStatus.value;
	}
}

void* processPowKeyproc(void* param)
{
	int recv_queue_cnt, recv_scancode;

	while(1)
	{
		if(g_pkey_cnt > 0)
		{
			TVSTORM_PWBUTTON_SendEvent(EV_KEY, KEY_POWER, 1);
			TVSTORM_PWBUTTON_SendEvent(EV_SYN, SYN_REPORT, 0);
			pbtn_msleep(150);
			TVSTORM_PWBUTTON_SendEvent(EV_KEY, KEY_POWER, 0);
			TVSTORM_PWBUTTON_SendEvent(EV_SYN, SYN_REPORT, 0);
			pbtn_msleep(POWER_DELAY_INTERVAL);
			g_pkey_cnt = 0;
		}
		pbtn_msleep(1);
	}
	return NULL;
}

TVSTORM_Error PBUTTON_set_initializing(NEXUS_GpioHandle* hPin_key, NEXUS_GpioSettings* gpio_key, BKNI_EventHandle event_key)
{
	/* AON GPIO */
	NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType_eAonStandard, gpio_key); /* eStandard/eAonStandard */
	gpio_key->mode  = NEXUS_GpioMode_eInput;  /* eInput/eOutputOpenDrain/eOutputPushPull */
	gpio_key->interruptMode = NEXUS_GpioInterrupt_eEdge;     /* eRisingEdge/eFallingEdge/eEdge/eLow/eHigh */
	gpio_key->interrupt.context = event_key;
	gpio_key->interrupt.callback = Stby_BUTTON_interrupt;
	*hPin_key = NEXUS_Gpio_Open(NEXUS_GpioType_eAonStandard, 0, gpio_key);
	return  TVSTORM_OK;
}


void TVSTORM_PWBUTTON_version(TVSTORM_PWBUTTON_VERSION_S *pwbutton_version)
{
    pwbutton_version->version = TVSTORM_PWBUTTON_VERSION;
    pwbutton_version->patchlevel = TVSTORM_PWBUTTON_PATCHLEVEL;
    pwbutton_version->sublevel = TVSTORM_PWBUTTON_SUBLEVEL;

    return;
}

void TVSTORM_PWBUTTON_last_date(TVSTORM_PWBUTTON_DATE_S *pwbutton_date)
{
    pwbutton_date->year = TVSTORM_PWBUTTON_YEAR;
    pwbutton_date->month = TVSTORM_PWBUTTON_MONTH;
    pwbutton_date->day = TVSTORM_PWBUTTON_DAY;

    return;
}

int main(int argc, char **argv)
{
	char property_nexus_platform_val[256] = {0, };
	char property_bootcomplete_val[256] = {0, };
	TVSTORM_Error    errCode = TVSTORM_OK;
	NEXUS_Error    rc;
	int i;
	TVSTORM_PWBUTTON_VERSION_T pb_version;
	TVSTORM_PWBUTTON_DATE_T pb_date;

	TVSTORM_PWBUTTON_version(&pb_version);
	LOGI("power button version : V%d.%d.%d\n", pb_version.version,  pb_version.patchlevel, pb_version.sublevel);
	TVSTORM_PWBUTTON_last_date(&pb_date);
	LOGI("power button last date : year = %d, month = %d, day = %d\n", pb_date.year,  pb_date.month, pb_date.day);

	while(1)
	{
		__system_property_get(PROPERTY_NEXUS_PLATFORMINIT,property_nexus_platform_val );
		__system_property_get(PROPERTY_BOOTCOMPLETE, property_bootcomplete_val);
		if(strcmp("on", property_nexus_platform_val) == 0 && strcmp("1", property_bootcomplete_val) == 0) {
			/* initializing */
			errCode = TVSTORM_PWBUTTON_Init();
			if(errCode == TVSTORM_ERROR)
			{
				LOGE("[TVSTORM_PWBUTTON_Init] error");
				return TVSTORM_ERROR;
			}

			LOGI("[TVSTORM_PWBUTTON_Init] start ");

			NEXUS_GpioSettings pbtn_gpio;
			BKNI_EventHandle pbtn_event;

			pbtn_handle = NULL;
			pbtn_event = NULL;
			BKNI_CreateEvent(&pbtn_event);
			errCode = PBUTTON_set_initializing(&pbtn_handle, &pbtn_gpio, pbtn_event);
			if(errCode == TVSTORM_ERROR)
			{
				LOGE("[PBUTTON_set_initializing] error");
				return TVSTORM_ERROR;
			}

			/* wait for interrupt */
			while(1)
			{
				rc = BKNI_WaitForEvent(pbtn_event, 0xFFFFFFFF);
				BDBG_ASSERT(!rc);
			}

			/* deinitializing */
			BKNI_DestroyEvent(pbtn_event);
			NEXUS_Gpio_Close(pbtn_handle);

			errCode = TVSTORM_PWBUTTON_Uninit();
			if(errCode == TVSTORM_ERROR)
			{
				LOGE("[TVSTORM_PWBUTTON_Uninit] error");
				return TVSTORM_ERROR;
			}
		}
		else{
			sleep(1);
		}
	}

	return 0;
}
