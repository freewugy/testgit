/******************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: $ cssong
 * $LastChangedDate: $ 2015.05.19
 * $LastChangedRevision: $ V1.1.0
 * Description: Front LED Controler Library
 * Note:
 *         V1.1.0 
 *                  add TVS_NETWORK_STATUS
 *****************************************************************************/
#include <stdio.h>
#include <string.h>
#include "bstd.h"
#include "nexus_platform.h"
#include "front_led_ctrl.h"
#include "eeprom.h"

#define CHIP_ADDR           0x50
#define EEPROM_I2C_CHANNEL  3
#define SUBADDR_BYTE        2

static NEXUS_I2cHandle hI2cHandle = NULL;

void printf_buf(unsigned char* buf, int len)
{
    int i;

    for(i=0; i<len; i++)
    {
        if(0 != i)
            if(!(i%16)) LOGI("\n");
        LOGI("%02x ", buf[i]);
    }
    LOGI("\n");
}

int eeprom_init(void)
{
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_I2cSettings settings;
    NEXUS_Error rc;

    LOGI("libinit: eeprom_init() \n");
    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_I2c_GetDefaultSettings(&settings);
    	
    hI2cHandle = platformConfig.i2c[EEPROM_I2C_CHANNEL] = NEXUS_I2c_Open(EEPROM_I2C_CHANNEL, &settings);

    if(NULL == hI2cHandle)
    {
        LOGE("Error, platformConfig.i2c[%d] handle is NULL \n", EEPROM_I2C_CHANNEL);
        return -1;
    }

    LOGI("1) channel     : %d\n", EEPROM_I2C_CHANNEL);
    LOGI("2) chip address: %02x\n", CHIP_ADDR);
    LOGI("3) hw/sw i2c   : %s\n", settings.softI2c?"soft i2c":"hard i2c");
    LOGI("4) clock speed : %d Hz\n", settings.clockRate);

    return 0;
}


int eeprom_uninit(void)
{
    NEXUS_I2c_Close(hI2cHandle);
    return 0;
}
int eeprom_read(uint32_t subAddr, uint8_t *data, uint32_t length)
{
    int i;
    BERR_Code           error = BERR_SUCCESS;

    LOGE("libinit: eeprom_read(subAddr 0x%x, length %d) \n", subAddr, length);
    if(NULL == hI2cHandle) return 0;

    error = NEXUS_I2c_ReadA16 (hI2cHandle, CHIP_ADDR, subAddr, data, length);
    if(BERR_SUCCESS != error)
    {
        LOGE("Error: NEXUS_I2c_ReadA16 return %d \n", error);
    }
    else
    {
        LOGI("data: \n");
        printf_buf(data, length);
    }

    usleep(1000);
    return error == BERR_SUCCESS;
}

int eeprom_write(uint32_t subAddr, uint8_t *data, uint32_t length)
{
    BERR_Code           error = BERR_SUCCESS;

    LOGI("libinit: eeprom_write(subAddr 0x%x, length %d) \n", subAddr, length);
    if(NULL == hI2cHandle) return 0;

    error = NEXUS_I2c_WriteA16 (hI2cHandle, CHIP_ADDR, subAddr, data, length);

    usleep(200*1000);

    return error == BERR_SUCCESS;
}

/*
* Get stb power status from boot parameter
* input : 
*		stb power status : ps
* output :
*           return : TVSTORM_ERROR is i2c error
*           return : TVSTORM_ERROR_NOT_IMPLEMENTED is empty i2c handler
*		return : TVSTORM_OK is success
*/
TVSTORM_Error TVSTORM_Get_StbPowerStaus(STB_POWER_STATUS_T *ps)
{
    NVRAM_DATA_BOOTLOADER_T eeprom_bootldr;
    TVSTORM_Error errCode = TVSTORM_OK;
    if(eeprom_init() == -1){
        LOGE("Error: eeprom_init  !!\n");
        return TVSTORM_ERROR;
    }

    if(eeprom_read(NVRAM_BASE_OFFSET_BOOTLOADER, (unsigned char*)&eeprom_bootldr, sizeof(NVRAM_DATA_BOOTLOADER_T)))
    {
        *ps = eeprom_bootldr.stbPowerStatus;
        LOGI("power status  : 0x%08x\n", eeprom_bootldr.stbPowerStatus);
    }
    else{
        LOGE("Error: eeprom_read  !!\n");
        errCode = TVSTORM_ERROR_NOT_IMPLEMENTED;
    }

    eeprom_uninit();

    return errCode;
}

/*
* Get software download mode from boot parameter
* input : 
*           software download mode : dm
* output :
*           return : TVSTORM_ERROR is i2c error
*           return : TVSTORM_ERROR_NOT_IMPLEMENTED is empty i2c handler
*		return : TVSTORM_OK is success
*/
TVSTORM_Error TVSTORM_Get_DownloadMode(SW_DOWNLOAD_MODE_T *dm)
{
    NVRAM_DATA_BOOTLOADER_T eeprom_bootldr;
    TVSTORM_Error errCode = TVSTORM_OK;
    if(eeprom_init() == -1){
        LOGE("Error: eeprom_init  !!\n");
        return TVSTORM_ERROR;
    }

    if(eeprom_read(NVRAM_BASE_OFFSET_BOOTLOADER, (unsigned char*)&eeprom_bootldr, sizeof(NVRAM_DATA_BOOTLOADER_T)))
    {
        *dm = eeprom_bootldr.downloadMode;
        LOGI("download mode : 0x%08x\n", eeprom_bootldr.downloadMode);
    }
    else{
        LOGE("Error: eeprom_read  !!\n");
        errCode = TVSTORM_ERROR_NOT_IMPLEMENTED;
    }

    eeprom_uninit();

    return errCode;
}


TVSTORM_Error TVSTORM_Get_BootParam(SW_DOWNLOAD_MODE_T *dm, STB_POWER_STATUS_T *ps, FACTORY_RESET_MODE_T *fr, unsigned char *mfc, unsigned char *dfc)
{
    NVRAM_DATA_BOOTLOADER_T eeprom_bootldr;
    NVRAM_DATA_TVSTORM_T eeprom_tvstorm;
    TVSTORM_Error errCode = TVSTORM_OK;

    if(eeprom_init() == -1){
        LOGE("Error: eeprom_init  !!\n");
        return TVSTORM_ERROR;
    }

    if(eeprom_read(NVRAM_BASE_OFFSET_BOOTLOADER, (unsigned char*)&eeprom_bootldr, sizeof(NVRAM_DATA_BOOTLOADER_T)))
    {
        *dm = eeprom_bootldr.downloadMode;
        *ps = eeprom_bootldr.stbPowerStatus;
        *mfc = eeprom_bootldr.appLaunchFailCnt;
        *dfc = eeprom_bootldr.downloadAppId;
        LOGI("download mode : 0x%08x\n", eeprom_bootldr.downloadMode);
        LOGI("power status  : 0%08x\n", eeprom_bootldr.stbPowerStatus);
        LOGI("app launch faile cnt  : %0d\n", eeprom_bootldr.appLaunchFailCnt);
        LOGI("download launch faile cnt  : %0d\n", eeprom_bootldr.downloadAppId);
    }
    else{
        LOGE("Error: eeprom_read  !!\n");
        eeprom_uninit();
        return TVSTORM_ERROR_NOT_IMPLEMENTED;
    }

    if(eeprom_read(NVRAM_BASE_OFFSET_TVSTORM, (unsigned char*)&eeprom_tvstorm, sizeof(FACTORY_RESET_MODE_T) + 4) == TVSTORM_OK)
    {
        *fr = eeprom_tvstorm.factoryResetMode;
        LOGI("factory reset   : 0x%08x\n", eeprom_tvstorm.factoryResetMode);
    }
    else{
        LOGE("Error: eeprom_read  !!\n");
        errCode = TVSTORM_ERROR;
    }

    eeprom_uninit();

    return errCode;
}
