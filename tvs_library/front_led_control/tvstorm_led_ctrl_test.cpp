/******************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: $ cssong
 * $LastChangedDate: $ 2015.04.30
 * $LastChangedRevision: $ V1.0.0
 * Description: Front LED Controler test
 * Note:
 *****************************************************************************/
#include "tvstorm_led_ctrl_test.h"

#if 0
#define PRINT(fmt, args...)    printf(fmt, ##args)
#else
#define PRINT(fmt, args...)
#endif

#define PROPERTY_NEXUS_PLATFORMINIT   "hw.nexus.platforminit"
#define PROPERTY_BOOTCOMPLETE		  "sys.boot_completed"

char PromptChar( void )
{
    char buf[256];
    printf("\nChoice: ");
    fgets(buf, 256, stdin);
    return buf[0];
}

int main(int argc, char **argv)
{
	char choice;

	if (TVSTORM_Front_Led_Init() != TVSTORM_OK) {
		printf("TVSTORM_Front_Led_Init error\n");
		return -1;
	}

	while(1)
	{
	  printf("\n\n");
        printf("===================================\n");
        printf("  Front LED test\n");
        printf("===================================\n");
        printf("    0) Exit\n");
        printf("    1) Power Green LED On\n");
        printf("    2) Power Green LED Off\n");
        printf("    3) Power Red LED On\n");
        printf("    4) Power Red LED Off\n");		
        printf("    5) Ethernet Green LED On\n");
        printf("    6) Ethernet Green LED Off\n");
        printf("    7) Ethernet Red LED On\n");
        printf("    8) Ethernet Red LED Off\n");		
        printf("    9) Wi-Fi Green LED On\n");
        printf("    a) Wi-Fi Green LED Off\n");
        printf("    b) Wi-Fi Red LED On\n");
        printf("    c) Wi-Fi Red LED Off\n");

        choice = PromptChar();

        switch (choice)
        {
            case '0':
                TVSTORM_Front_Led_Uninit();
                return 0;
            case '1':
                TVSTORM_Front_Led_Control(LED_POWER_GREEN, TVS_LED_ON);
                break;
            case '2':
                TVSTORM_Front_Led_Control(LED_POWER_GREEN, TVS_LED_OFF);
                break;
            case '3':
                TVSTORM_Front_Led_Control(LED_POWER_RED, TVS_LED_ON);
                break;
            case '4':
                TVSTORM_Front_Led_Control(LED_POWER_RED, TVS_LED_OFF);
                break;
            case '5':
                TVSTORM_Front_Led_Control(LED_ETHERNET_GREEN, TVS_LED_ON);
                break;
            case '6':
                TVSTORM_Front_Led_Control(LED_ETHERNET_GREEN, TVS_LED_OFF);
                break;
            case '7':
                TVSTORM_Front_Led_Control(LED_ETHERNET_RED, TVS_LED_ON);
                break;
            case '8':
                TVSTORM_Front_Led_Control(LED_ETHERNET_RED, TVS_LED_OFF);
                break;
            case '9':
                TVSTORM_Front_Led_Control(LED_WIFI_GREEN, TVS_LED_ON);
                break;
            case 'a':
                TVSTORM_Front_Led_Control(LED_WIFI_GREEN, TVS_LED_OFF);
                break;
            case 'b':
                TVSTORM_Front_Led_Control(LED_WIFI_RED, TVS_LED_ON);
                break;
            case 'c':
                TVSTORM_Front_Led_Control(LED_WIFI_RED, TVS_LED_OFF);
                break;
            default:
                printf("\nInvalid Choice!\n\n");
                break;
        }
    }

    return 0;
}
