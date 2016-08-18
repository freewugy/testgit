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

#include "fan_pwm_ctrl.h"
#include "nxclient.h"

//#define SUPPORT_TVSTORM_FANCTRL_DEBUG

#ifdef SUPPORT_TVSTORM_FANCTRL_DEBUG
#define LOGE(...) fprintf(stdout, "E:" __VA_ARGS__)
#define LOGW(...) fprintf(stdout, "W:" __VA_ARGS__)
#define LOGI(...) fprintf(stdout, "I:" __VA_ARGS__)
#else
#define LOGE(...) do {} while (0)
#define LOGW(...) do {} while (0)
#define LOGI
#endif

// TVSTORM fan ctrl Version
#define TVSTORM_FANCTRL_VERSION 1
#define TVSTORM_FANCTRL_PATCHLEVEL 0
#define TVSTORM_FANCTRL_SUBLEVEL 2

// TVSTORM fan ctrl date
#define TVSTORM_FANCTRL_YEAR 2015
#define TVSTORM_FANCTRL_MONTH 04
#define TVSTORM_FANCTRL_DAY 21


#define PROPERTY_NEXUS_PLATFORMINIT   "hw.nexus.platforminit"

#ifdef CONSTANT_FREQUENCY_PWM_MODE  /*  constant-frequency PWM mode */ 
#define PWM2_MAX_WORD 	0x2f9f
#define PWM2_MIN_WORD		0x0
#define PWM1_UNIT_WORD	0x1
#else  /*  variable-frequency PWM mode */
#define PWM2_MAX_WORD 	0x6fb8
#define PWM2_MIN_WORD		0x0
#define PWM2_UNIT_WORD	0x11e
#endif


bool g_initStatus = false;

NexusIPCClientBase *g_ipcClient = NULL;
NexusClientContext *g_nexusClient = NULL;

TVSTORM_Error TVSTORM_Fan_Pwm_Init(void)
{
    TVSTORM_Error errCode = TVSTORM_OK;
   
    if (g_initStatus) {
        LOGE("TVSTORM_FAN_CONTROL already initialized.\n");
        return TVSTORM_ERROR;
    }
    
#ifdef NXCLIENT_BINDER
    b_refsw_client_client_configuration config;
    b_refsw_client_client_info client_info;
 
    g_ipcClient = NexusIPCClientFactory::getClient("TVSTORM_FAN_CONTROL");
    if (g_ipcClient == NULL) {
        LOGE("could not get NexusIPCClient!\n");
        return TVSTORM_ERROR;
    }
    
    BKNI_Memset(&config, 0, sizeof(config));
    BKNI_Snprintf(config.name.string,sizeof(config.name.string),"TVSTORM_FAN_CONTROL");
    g_nexusClient = g_ipcClient->createClientContext(&config);
    if (g_nexusClient == NULL) {
        LOGE("%s: Could not create Nexus Client Context!!!", __FUNCTION__); 
        delete g_ipcClient;
    }
#else
    NEXUS_Error rc;

    NxClient_JoinSettings joinSettings;

    NxClient_GetDefaultJoinSettings(&joinSettings);

    snprintf(joinSettings.name, NXCLIENT_MAX_NAME, "TVSTORM_FAN_CONTROL");
    rc = NxClient_Join(&joinSettings);
#endif
    
    g_initStatus = true;

    return TVSTORM_OK;

}

TVSTORM_Error TVSTORM_Fan_Pwm_Uninit(void)
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
        LOGE("TVSTORM_FAN_CONTROL already deinitialized.");
        return TVSTORM_ERROR;
    }
    
    return errCode;
}


TVSTORM_Error TVSTORM_Fan_Pwm_Control(uint16_t cWord, uint16_t OnInterval, uint16_t PeriodInterval)
{
	TVSTORM_Error errCode = TVSTORM_OK;

	NEXUS_PwmChannelHandle pwm;
	NEXUS_PwmChannelSettings pwmSettings;
	NEXUS_PwmFreqModeType frequencyMode;

	NEXUS_Pwm_GetDefaultChannelSettings(&pwmSettings);
	pwmSettings.openDrain = false;

#ifdef CONSTANT_FREQUENCY_PWM_MODE
	LOGI("Fan_Pwm_Control Constant : cWord = 0x%x, OnInterval = 0x%x, PeriodInterval = 0x%x \n", cWord, OnInterval, PeriodInterval);
	pwmSettings.eFreqMode = NEXUS_PwmFreqModeType_eConstant;
	pwm = NEXUS_Pwm_OpenChannel(2, &pwmSettings);

	NEXUS_Pwm_SetControlWord(pwm, cWord);
	NEXUS_Pwm_SetOnAndPeriodInterval(pwm, OnInterval, PeriodInterval);
	NEXUS_Pwm_Start(pwm);
#else
	LOGI("Fan_Pwm_Control Variable : cWord = 0x%x, OnInterval = 0x%x, PeriodInterval = 0x%x \n", cWord, OnInterval, PeriodInterval);
	pwmSettings.eFreqMode = NEXUS_PwmFreqModeType_eVariable;
	pwm = NEXUS_Pwm_OpenChannel(2, &pwmSettings);

	NEXUS_Pwm_SetControlWord(pwm, cWord);
	NEXUS_Pwm_Start(pwm);
#endif

	NEXUS_Pwm_CloseChannel(pwm);

	return TVSTORM_OK;
}



#ifndef NEXUS_NUM_TEMP_MONITORS
#define NEXUS_NUM_TEMP_MONITORS 2
#endif

static void TVSTORM_data_ready(void *context, int param)
{
    int rc;
    NEXUS_TempMonitorStatus status;
    NEXUS_TempMonitorHandle tempMonitor = (NEXUS_TempMonitorHandle)context;

    rc = NEXUS_TempMonitor_GetStatus(tempMonitor, &status);
     LOGI("NEXUS_TempMonitor_GetStatus = %d\n" ,rc);
    if (!status.valid) {
        LOGE("temp_monitor %d: status not valid\n", param);
    }
    else {
        LOGI("temp_monitor %d: %#x,%#x,%#x; %d degrees C\n",
            param,
            status.top,
            status.middle,
            status.bottom,
            status.onChipTemperature);
    }
}

#ifdef SUPPORT_TVSTORM_TEMP_MONITOR
NEXUS_TempMonitorHandle gtempMonitor[NEXUS_NUM_TEMP_MONITORS];
TVSTORM_Error TVSTORM_Temp_monitor_Init(void)
{
	unsigned i;
	NEXUS_Error rc;

	for (i=0;i<NEXUS_NUM_TEMP_MONITORS;i++) {
		NEXUS_TempMonitorSettings settings;
		gtempMonitor[i] = NEXUS_TempMonitor_Open(i, NULL);
		 LOGI("NEXUS_TempMonitor_Open [%d] is %s, rc = %d\n" ,i, (gtempMonitor[i] == NULL)?"NULL":"not NULL", rc);

		NEXUS_TempMonitor_GetSettings(gtempMonitor[i], &settings);
		settings.enabled = true;
		settings.dataReady.callback = TVSTORM_data_ready;
		settings.dataReady.context = gtempMonitor[i];
		settings.dataReady.param = i;
		settings.measureInterval = 2000; /* 2 seconds */
		rc = NEXUS_TempMonitor_SetSettings(gtempMonitor[i], &settings);
		LOGI("NEXUS_TempMonitor_SetSettings = %d\n" ,rc);
	}
	return TVSTORM_OK;
}

TVSTORM_Error TVSTORM_Temp_monitor_Uninit(void)
{
    unsigned i;
    for (i=0;i<NEXUS_NUM_TEMP_MONITORS;i++) {
        NEXUS_TempMonitor_Close(gtempMonitor[i]);
    }

    return TVSTORM_OK;
}
#endif


void TVSTORM_FanCtrl_version(TVSTORM_FANCTRL_VERSION_S *fancrtl_version)
{
    fancrtl_version->version = TVSTORM_FANCTRL_VERSION;
    fancrtl_version->patchlevel = TVSTORM_FANCTRL_PATCHLEVEL;
    fancrtl_version->sublevel = TVSTORM_FANCTRL_SUBLEVEL;

    return;
}

void TVSTORM_FanCtrl_last_date(TVSTORM_FANCTRL_DATE_S *fancrtl_date)
{
    fancrtl_date->year = TVSTORM_FANCTRL_YEAR;
    fancrtl_date->month = TVSTORM_FANCTRL_MONTH;
    fancrtl_date->day = TVSTORM_FANCTRL_DAY;

    return;
}

#ifndef TVSTORM_PWM_TEST
int main(int argc, char **argv)
{
	char property_nexus_platform_val[256] = {0, };
	uint16_t acWord, aOnInterval, aPeriodInterval;
	char   *ptr;
	TVSTORM_FANCTRL_VERSION_T fanctrl_version;
	TVSTORM_FANCTRL_DATE_T fanctrl_date;

	if (argc <= 3) {
		LOGI("TVSTORM_FAN_CONTROL parameter error\n");
		return -1;
	}

	acWord = (uint16_t)strtol(argv[1], &ptr, 16);
	aOnInterval = (uint16_t)strtol(argv[2], &ptr, 16);
	aPeriodInterval = (uint16_t)strtol(argv[3], &ptr, 16);

	while(1)
	{
		__system_property_get(PROPERTY_NEXUS_PLATFORMINIT,property_nexus_platform_val );

		if(!strcmp("on", property_nexus_platform_val))
		{
			if (TVSTORM_Fan_Pwm_Init() != TVSTORM_OK) {
				LOGE("TVSTORM_FAN_CONTROL error\n");
				return -1;
			}

			TVSTORM_Fan_Pwm_Control(acWord, aOnInterval, aPeriodInterval);
			LOGI("TVSTORM_FAN_CONTROL Setting : cWord = 0x%x, OnInterval = 0x%x, PeriodInterval = 0x%x, dute cyle is %%%2f \n", 
				acWord, 
				aOnInterval, 
				aPeriodInterval, 
				(((double)aOnInterval/(double)aPeriodInterval)*100));

#ifdef SUPPORT_TVSTORM_TEMP_MONITOR
			TVSTORM_Temp_monitor_Init();
			LOGI("press ENTER to exit\n");
			getchar();
			TVSTORM_Temp_monitor_Uninit();
#endif

			TVSTORM_Fan_Pwm_Uninit();
			return 0;
		}
		else	{
			sleep(1);
		}
	}

	TVSTORM_FanCtrl_version(&fanctrl_version);
	LOGI("fanctrl version : V%d.%d.%d\n", fanctrl_version.version,  fanctrl_version.patchlevel, fanctrl_version.sublevel);
	TVSTORM_FanCtrl_last_date(&fanctrl_date);
	LOGI("fanctrl last date : year = %d, month = %d, day = %d\n", fanctrl_date.year,  fanctrl_date.month, fanctrl_date.day);
				
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#else
char PromptChar( void )
{
    char buf[256];
    printf("\nChoice: ");
    fgets(buf, 256, stdin);
    return buf[0];
}

int main(int argc, char **argv)
{

    NEXUS_PwmChannelHandle pwm;
    NEXUS_PwmChannelSettings pwmSettings;
    char str[256];
    char choice;
    int lval,lval2;
    uint16_t lva3;	
    NEXUS_PwmFreqModeType frequencyMode;
    char comma;
    NEXUS_Error rc;
	
    Fan_Pwm_Init();

    while (1)
    {
        printf("\n\n");
        printf("===================================\n");
        printf("  PWM test\n");
        printf("===================================\n");
        printf("    0) Exit\n");
        printf("    1) Select PWM channel\n");
        printf("    2) Write PWM control word\n");
        printf("    3) Write PWM Freq Mode\n");
        printf("    4) Write PWM On Interval\n");
        printf("    5) Write PWM Period Interval\n");
        printf("    6) Write PWM On and Period Interval\n");
        printf("    7) Read PWM  Freq mode \n");
        printf("    8) Read PWM On and Period Interval\n");
        printf("    9) Read PWM  control word \n");
        printf("    a) Start PWM generation\n");
        printf("    b) Stop PWM generation\n");

        choice = PromptChar();

        switch (choice)
        {
            case '0':
                Fan_Pwm_Uninit();
                return 0;

            case '1':
                printf("Enter the PWM channel (1 or 2): \n");
                fgets(str, 256, stdin);
                sscanf (str, "%d", &lval);
                if (lval > 2 || lval == 0)
                {
                    printf("Invalid PWM channel, enter only 1 or 2\n");
                    break;
                }
                NEXUS_Pwm_GetDefaultChannelSettings(&pwmSettings);
                pwm = NEXUS_Pwm_OpenChannel(lval, &pwmSettings);
                break;

            case '2':
                printf("Enter control word value (in hex):\n");
                fgets(str, 256, stdin);
                sscanf(str, "%x", (unsigned int *)&lval);
                NEXUS_Pwm_SetControlWord(pwm, lval);
                break;

            case '3':
                printf("Enter the desired Freq Mode : 0=Variable, 1=Constant\n");
                fgets(str, 256, stdin);
                sscanf (str, "%d", (int *)&frequencyMode);
                if (frequencyMode > 2 )
                {
                    printf("Invalid Fre mode, enter only 0 or 1\n");
                    break;
                }
                NEXUS_Pwm_SetFreqMode(pwm, frequencyMode);
                break;

            case '4':
                printf("Enter On Interval value (in hex):\n");
                fgets(str, 256, stdin);
                sscanf(str, "%x", (unsigned int *)&lval);
                NEXUS_Pwm_SetOnInterval(pwm, lval);
                break;

            case '5':
                printf("Enter Period Interval value (in hex):\n");
                fgets(str, 256, stdin);
                sscanf(str, "%x", (unsigned int *)&lval);
                NEXUS_Pwm_SetPeriodInterval(pwm, lval);
                break;

            case '6':
                printf("Enter On and period intervals seperated by comma (in hex), like this: 40,80\n");
                fgets(str, 256, stdin);
                sscanf(str, "%x %c %x", (unsigned int *)&lval,&comma, (unsigned int *)&lval2);
                NEXUS_Pwm_SetOnAndPeriodInterval(pwm, lval, lval2);
                break;

            case '7':
                NEXUS_Pwm_GetFreqMode(pwm, &frequencyMode);
                if (frequencyMode == 0)
                    printf("The mode is Variable Freq Mode\n");
                else
                    printf("The mode is Contant Freq Mode\n");
                break;

            case '8':
                NEXUS_Pwm_GetOnAndPeriodInterval(pwm, (uint16_t *)&lval, (uint16_t *)&lval2);
                printf("The On Interval is : %x and Period interval is: %x\n", lval, lval2);
                break;

            case '9':
                NEXUS_Pwm_GetControlWord(pwm, &lva3);
                printf("The Control Word is : %x, rc = %d \n", lval, rc);
                break;

            case 'a':
                NEXUS_Pwm_Start(pwm);
                break;

            case 'b':
                NEXUS_Pwm_Stop(pwm);
                break;
				
            default:
                printf("\nInvalid Choice!\n\n");
                break;
        }
    }

    return 0;
}
#endif
