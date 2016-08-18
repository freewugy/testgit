/******************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: $ cssong
 * $LastChangedDate: $ 2015.04.01
 * $LastChangedRevision: $ V1.1.1
 * Description: eeprom controler test
 * Note:
 *****************************************************************************/
#include <stdio.h>
#include <string.h>
#include "tvstorm_eeprom.h"

char PromptChar( void )
{
    char buf[256];
    printf("\nChoice: ");
    fgets(buf, 256, stdin);
    return buf[0];
}

int main(int argc, char **argv)
{

	TVSTORM_Error errCode = TVSTORM_OK;
	NVRAM_DATA_BOOTLOADER_T eeprom_bootldr;
	NVRAM_DATA_TVSTORM_T eeprom_tvstorm;
	SW_DOWNLOAD_MODE_T dm;
	TVS_DOWNLOAD_MODE_T tvs_dm;
	STB_POWER_STATUS_T ps;
	FACTORY_RESET_MODE_T fr;	
	uint32_t seq_no;
	char str[20], last_ch[16];
	int lval, download_mode;
	char choice;

	while(1)
	{
	  printf("\n\n");
        printf("===================================\n");
        printf("  eeprom read/write test\n");
        printf("===================================\n");
        printf("    0) Exit\n");
        printf("    1) Read Download mode\n");
        printf("    2) Write Download mode\n");
        printf("    3) Read STB power status\n");
        printf("    4) Write STB power status\n");
        printf("    5) Read Factory reset mode\n");
        printf("    6) Write  Factory reset mode\n");
        printf("    7) Read Boot PARAMETER\n");
        printf("    8) Version & date\n");
        printf("    a) Read ird sequence number\n");
        printf("    b) Write ird sequence number\n");
        printf("    c) Read last ch number\n");
        printf("    d) Write last ch number\n");

        choice = PromptChar();

        switch (choice)
        {
            case '0':
                return 0;
            case '1':
                errCode = TVSTORM_Get_DownloadMode(&dm, &tvs_dm);
                if(errCode != TVSTORM_OK){
                    printf("errCode %d\n", errCode);
                }
                else{
                    printf("download mode : 0x%08x, tvs_download_mode : 0x%08x\n", dm, tvs_dm);
                }
                break;
            case '2':
                printf("Enter the download mode (1 is enable or 2 is disable): \n");
                fgets(str, 20, stdin);
                sscanf (str, "%d", &lval);
                if (lval > 2 || lval == 0)
                {
                    printf("Invalid download mode, enter only 1 or 2\n");
                    break;
                }

                printf("Enter the tvs download mode (1 is SUS or 2 is USB or 3 is Disabled): \n");
                fgets(str, 20, stdin);
                sscanf (str, "%d", &download_mode);
                if (download_mode > 3 || download_mode < 1)
                {
                    printf("Invalid download mode, enter only 1 ~ 3\n");
                    break;
                }


                if(lval == 1)
                	dm = SW_DOWNLOAD_MODE_ENABLED;
                else
                	dm = SW_DOWNLOAD_MODE_DISABLED;

                if(1 == download_mode)
                    tvs_dm = TVS_DOWNLOAD_MODE_SUS;
                else if(2 == download_mode)
                    tvs_dm = TVS_DOWNLOAD_MODE_USB;
                if(1 == download_mode)
                    tvs_dm = TVS_DOWNLOAD_MODE_DISABLED;
				
                errCode = TVSTORM_Set_DownloadMode(dm, tvs_dm);

                if(errCode != TVSTORM_OK){
                    printf("errCode %d\n", errCode);
                }
                break;
            case '3':
                errCode = TVSTORM_Get_StbPowerStaus(&ps);
		   if(errCode != TVSTORM_OK){
		       printf("errCode %d\n", errCode);
		   }
		   else{
		   	 printf("STB power status  : 0x%08x\n", ps);
		   }
                break;
            case '4':
                printf("Enter the STB power status (1 is normal booting or 2 is Stand-by mode booting): \n");
                fgets(str, 20, stdin);
                sscanf (str, "%d", &lval);
                if (lval > 2 || lval == 0)
                {
                    printf("Invalid STB power status, enter only 1 or 2\n");
                    break;
                }

                if(lval == 1)
                	ps = STB_POWER_STATUS_ON;
                else
                	ps = STB_POWER_STATUS_STANDBY;

                errCode = TVSTORM_Set_StbPowerStaus(ps);
		   if(errCode != TVSTORM_OK){
		       printf("errCode %d\n", errCode);
		   }
                break;
            case '5':
                errCode = TVSTORM_Get_FactoryResetMode(&fr);
		   if(errCode != TVSTORM_OK){
		       printf("errCode %d\n", errCode);
		   }
		   else{
		   	 printf("factory reset   : 0x%08x\n", fr);
		   }
                break;
            case '6':
                printf("Enter the factory reset (1 is enable or 2 is disable): \n");
                fgets(str, 20, stdin);
                sscanf (str, "%d", &lval);
                if (lval > 2 || lval == 0)
                {
                    printf("Invalid STB power status, enter only 1 or 2\n");
                    break;
                }

                if(lval == 1)
                    fr = FACTORY_RESET_MODE_ENABLED;
                else
                    fr = FACTORY_RESET_MODE_DISABLED;

                errCode = TVSTORM_Set_FactoryResetMode(fr);
		   if(errCode != TVSTORM_OK){
		       printf("errCode %d\n", errCode);
		   }
                break;
            case '7':
		   unsigned char mfc;
		   unsigned char dfc;
                errCode = TVSTORM_get_boot_param(&dm, &ps, &fr, &mfc, &dfc);
		   if(errCode != TVSTORM_OK){
		       printf("errCode %d\n", errCode);
		   }
		   else{
		   	 printf("download mode : 0x%08x\n", dm );
		   	 printf("power status  : 0x%08x\n", ps);
		   	 printf("factory reset   : 0x%08x\n", fr);
			 printf("main app launch faile cnt  : %0d\n", mfc);
		   	 printf("download launch faile cnt  : %0d\n", dfc);
		   }
                break;
            case '8':
                TVSTORM_EEPROM_VERSION_T eeprom_version;
                TVSTORM_EEPROM_DATE_T eeprom_date;

                TVSTORM_eeprom_version(&eeprom_version);
                printf("eeprom libary version : V%d.%d.%d\n", eeprom_version.version,  eeprom_version.patchlevel, eeprom_version.sublevel);
                TVSTORM_eeprom_last_date(&eeprom_date);
                printf("eeprom libary date : year = %d, month = %d, day = %d\n", eeprom_date.year,  eeprom_date.month, eeprom_date.day);
                break;
            case 'a':
                errCode = TVSTORM_Get_SequenceNumber(&seq_no);
                if(errCode != TVSTORM_OK){
                    printf("errCode %d\n", errCode);
                }
                else{
                    printf("sequence number : %d\n", seq_no);
                }
                break;
            case 'b':
                printf("Enter sequence : \n");
                fgets(str, 20, stdin);
                sscanf (str, "%d", &lval);

                errCode = TVSTORM_Set_SequenceNumber(lval);
			   if(errCode != TVSTORM_OK){
				   printf("errCode %d\n", errCode);
			   }
                break;
            case 'c':
                errCode = TVSTORM_get_last_ch(last_ch);
                if(errCode != TVSTORM_OK){
                    printf("errCode %d\n", errCode);
                }
                else{
                    printf("last ch UID: %s\n", last_ch);
                }
                break;
            case 'd':
                printf("Enter last ch UID: \n");
                fgets(last_ch, 16, stdin);

                errCode = TVSTORM_set_last_ch(last_ch);
                if(errCode != TVSTORM_OK){
                    printf("errCode %d\n", errCode);
                }
                break;

            default:
                printf("\nInvalid Choice!\n\n");
                break;
        }
    }

    return 0;
}
