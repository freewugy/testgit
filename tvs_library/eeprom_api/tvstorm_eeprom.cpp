 /******************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: $ cssong
 * $LastChangedDate: $ 2015.04.01
 * $LastChangedRevision: $ V1.1.1
 * Description: eeprom controler
 * Note:
 *****************************************************************************/
 
#include <stdio.h>
#include <string.h>
#include "bstd.h"
#include "nexus_platform.h"
#include "nexus_ipc_client_factory.h"
#include "bkni.h"
#include "tvstorm_eeprom.h"
#include "nxclient.h"

// eeprom address and i2c port number
#define CHIP_ADDR           0x50
#define EEPROM_I2C_CHANNEL  3
#define SUBADDR_BYTE        2

static NEXUS_I2cHandle hI2cHandle = NULL;

bool g_initStatus = false;
NexusIPCClientBase *g_ipcClient = NULL;
NexusClientContext *g_nexusClient = NULL;


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

/*
* eeprom nexus join
* input : 
*           void
* output :
*           return : TVSTORM_ERROR is can not join nexus client
*		return : TVSTORM_OK is join
*/
TVSTORM_Error eeprom_init(void)
{
    TVSTORM_Error errCode = TVSTORM_OK;
    
    if (g_initStatus) {
        LOGE("TVSTORM_EEPROM already initialized.\n");
        return TVSTORM_ERROR;
    }
    
#ifdef NXCLIENT_BINDER
    b_refsw_client_client_configuration config;
    b_refsw_client_client_info client_info;

    g_ipcClient = NexusIPCClientFactory::getClient("TVSTORM_EEPROM");
    if (g_ipcClient == NULL) {
        LOGE("could not get NexusIPCClient!\n");
        return TVSTORM_ERROR;
    }
    
    BKNI_Memset(&config, 0, sizeof(config));
    BKNI_Snprintf(config.name.string,sizeof(config.name.string),"TVSTORM_EEPROM");
    g_nexusClient = g_ipcClient->createClientContext(&config);
    if (g_nexusClient == NULL) {
        LOGE("%s: Could not create Nexus Client Context!!!", __FUNCTION__); 
        delete g_ipcClient;
    }
#else
    NEXUS_Error rc;

    NxClient_JoinSettings joinSettings;

    NxClient_GetDefaultJoinSettings(&joinSettings);

    snprintf(joinSettings.name, NXCLIENT_MAX_NAME, "TVSTORM_EEPROM");
    rc = NxClient_Join(&joinSettings);
#endif
    
    g_initStatus = true;

    return TVSTORM_OK;

}

/*
* eeprom nexus release
* input : 
*           void
* output :
*           return : TVSTORM_ERROR is can not release nexus client
*		return : TVSTORM_OK is release
*/
TVSTORM_Error eeprom_uninit(void)
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
        LOGE("TVSTORM_EEPROM already deinitialized.");
        return TVSTORM_ERROR;
    }
    
    return errCode;
}

TVSTORM_Error eeprom_read(uint32_t subAddr, uint8_t *data, uint32_t length)
{
    int i;
    BERR_Code           error = BERR_SUCCESS;
    TVSTORM_Error errCode = TVSTORM_OK;

    LOGI("libinit: eeprom_read(subAddr 0x%x, length %d) \n", subAddr, length);
    if(NULL == hI2cHandle){
        return TVSTORM_ERROR_NOT_IMPLEMENTED;
    }

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

    if(error == BERR_SUCCESS)
	errCode = TVSTORM_OK;
    else
	errCode = TVSTORM_ERROR;

    return errCode;
}

TVSTORM_Error eeprom_write(uint32_t subAddr, uint8_t *data, uint32_t length)
{
    BERR_Code           error = BERR_SUCCESS;
    TVSTORM_Error errCode = TVSTORM_OK;

    LOGI("libinit: eeprom_write(subAddr 0x%x, length %d) \n", subAddr, length);
    if(NULL == hI2cHandle){
        return TVSTORM_ERROR_NOT_IMPLEMENTED;
    }

    error = NEXUS_I2c_WriteA16 (hI2cHandle, CHIP_ADDR, subAddr, data, length);

    usleep(200*1000);

    if(error == BERR_SUCCESS)
	errCode = TVSTORM_OK;
    else
	errCode = TVSTORM_ERROR;
 
    return errCode;
}


TVSTORM_Error eeprom_i2c_Open(void)
{
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_I2cSettings settings;
    TVSTORM_Error errCode = TVSTORM_OK;

    LOGI("libinit: eeprom_init() \n");
    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_I2c_GetDefaultSettings(&settings);
    	
    hI2cHandle = platformConfig.i2c[EEPROM_I2C_CHANNEL] = NEXUS_I2c_Open(EEPROM_I2C_CHANNEL, &settings);

    if(NULL == hI2cHandle)
    {
        LOGE("Error, platformConfig.i2c[%d] handle is NULL \n", EEPROM_I2C_CHANNEL);
        errCode = TVSTORM_ERROR;
    }

    LOGI("1) channel     : %d\n", EEPROM_I2C_CHANNEL);
    LOGI("2) chip address: %02x\n", CHIP_ADDR);
    LOGI("3) hw/sw i2c   : %s\n", settings.softI2c?"soft i2c":"hard i2c");
    LOGI("4) clock speed : %d Hz\n", settings.clockRate);

    return errCode;
}


TVSTORM_Error eeprom_i2c_Close(void)
{
    NEXUS_I2c_Close(hI2cHandle);
    return TVSTORM_OK;
}


void TVSTORM_eeprom_version(TVSTORM_EEPROM_VERSION_T *eeprom_version)
{
    eeprom_version->version = TVSTORM_EEPROM_VERSION;
    eeprom_version->patchlevel = TVSTORM_EEPROM_PATCHLEVEL;
    eeprom_version->sublevel = TVSTORM_EEPROM_SUBLEVEL;

    return;
}

void TVSTORM_eeprom_last_date(TVSTORM_EEPROM_DATE_T *eeprom_date)
{
    eeprom_date->year = TVSTORM_EEPROM_YEAR;
    eeprom_date->month = TVSTORM_EEPROM_MONTH;
    eeprom_date->day = TVSTORM_EEPROM_DAY;

    return;
}

/*
* eeprom read
* input : 
*           start address : subAddr
*		read data : data
*		length of read data : length
* output :
*           return : TVSTORM_ERROR is i2c error
*           return : TVSTORM_ERROR_NOT_IMPLEMENTED is empty i2c handler
*		return : TVSTORM_OK is success
*/
TVSTORM_Error TVSTORM_eeprom_read(uint32_t subAddr, uint8_t *data, uint32_t length)
{
    int i;
    BERR_Code           error = BERR_SUCCESS;
    TVSTORM_Error errCode = TVSTORM_OK;

    if(eeprom_init() != TVSTORM_OK){
        return TVSTORM_ERROR;
    }

    if(eeprom_i2c_Open() != TVSTORM_OK){
        LOGE("Error: eeprom_init  !!\n");
        eeprom_uninit();
        return TVSTORM_ERROR;
    }

    LOGE("libinit: eeprom_read(subAddr 0x%x, length %d) \n", subAddr, length);
    if(NULL == hI2cHandle){
        eeprom_uninit();
        eeprom_i2c_Close();
        return TVSTORM_ERROR_NOT_IMPLEMENTED;
    }

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

    if(error == BERR_SUCCESS)
	errCode = TVSTORM_OK;
    else
	errCode = TVSTORM_ERROR;

    eeprom_i2c_Close();
    eeprom_uninit();

    return errCode;
}

/*
* eeprom write
* input : 
*           start address : subAddr
*		write data : data
*		length of write data : length
* output :
*           return : TVSTORM_ERROR is i2c error
*           return : TVSTORM_ERROR_NOT_IMPLEMENTED is empty i2c handler
*		return : TVSTORM_OK is success
*/
TVSTORM_Error TVSTORM_eeprom_write(uint32_t subAddr, uint8_t *data, uint32_t length)
{
    BERR_Code           error = BERR_SUCCESS;
    TVSTORM_Error errCode = TVSTORM_OK;

    if(eeprom_init() != TVSTORM_OK){
        return TVSTORM_ERROR;
    }

    if(eeprom_i2c_Open() != TVSTORM_OK){
        LOGE("Error: eeprom_init  !!\n");
        eeprom_uninit();
        return TVSTORM_ERROR;
    }

    LOGI("libinit: eeprom_write(subAddr 0x%x, length %d) \n", subAddr, length);
    if(NULL == hI2cHandle){
        eeprom_i2c_Close();
        eeprom_uninit();
        return TVSTORM_ERROR_NOT_IMPLEMENTED;
    }

    error = NEXUS_I2c_WriteA16 (hI2cHandle, CHIP_ADDR, subAddr, data, length);

    usleep(200*1000);

    if(error == BERR_SUCCESS)
	errCode = TVSTORM_OK;
    else
	errCode = TVSTORM_ERROR;

    eeprom_i2c_Close();
    eeprom_uninit();

    return errCode;
}


/*
* Get boot parameter
* input : 
*           software download mode : dm
*		stb power status : ps
*		factory reset mode : fr
*		main app lanuch fail counter : mfc
*		downloader launch fail counter : dfc
* output :
*           return : TVSTORM_ERROR is i2c error
*           return : TVSTORM_ERROR_NOT_IMPLEMENTED is empty i2c handler
*		return : TVSTORM_OK is success
*/
TVSTORM_Error TVSTORM_get_boot_param(SW_DOWNLOAD_MODE_T *dm, STB_POWER_STATUS_T *ps, FACTORY_RESET_MODE_T *fr, unsigned char *mfc, unsigned char *dfc)
{
    NVRAM_DATA_BOOTLOADER_T eeprom_bootldr;
    NVRAM_DATA_TVSTORM_T eeprom_tvstorm;
    TVSTORM_Error errCode = TVSTORM_OK;

    if(eeprom_init() != TVSTORM_OK){
        return TVSTORM_ERROR;
    }

    if(eeprom_i2c_Open() != TVSTORM_OK){
        LOGE("Error: eeprom_init  !!\n");
        eeprom_uninit();
        return TVSTORM_ERROR;
    }

    if(NULL == hI2cHandle){
        eeprom_i2c_Close();
        eeprom_uninit();
        return TVSTORM_ERROR_NOT_IMPLEMENTED;
    }

    if(eeprom_read(NVRAM_BASE_OFFSET_BOOTLOADER, (unsigned char*)&eeprom_bootldr, sizeof(NVRAM_DATA_BOOTLOADER_T)) == TVSTORM_OK)
    {
        *dm = eeprom_bootldr.downloadMode;
        *ps = eeprom_bootldr.stbPowerStatus;
        *mfc = eeprom_bootldr.appLaunchFailCnt;
        *dfc = eeprom_bootldr.downloadAppId;
        LOGI("download mode : 0x%08x\n", eeprom_bootldr.downloadMode);
        LOGI("power status  : 0%08x\n", eeprom_bootldr.stbPowerStatus);
        LOGI("main app launch faile cnt  : %0d\n", eeprom_bootldr.appLaunchFailCnt);
        LOGI("download launch faile cnt  : %0d\n", eeprom_bootldr.downloadAppId);
    }
    else{
        LOGE("Error: eeprom_read  !!\n");
        errCode = TVSTORM_ERROR;
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

    eeprom_i2c_Close();
    eeprom_uninit();

    return errCode;
}


/*
* Get software download mode
* input : 
*           software download mode : dm
* output :
*           return : TVSTORM_ERROR is i2c error
*           return : TVSTORM_ERROR_NOT_IMPLEMENTED is empty i2c handler
*		return : TVSTORM_OK is success
*/
TVSTORM_Error TVSTORM_Get_DownloadMode(SW_DOWNLOAD_MODE_T *dm, TVS_DOWNLOAD_MODE_T *tvs_dm)
{
    NVRAM_DATA_BOOTLOADER_T eeprom_bootldr;
    TVSTORM_Error errCode = TVSTORM_OK;

    if(eeprom_init() != TVSTORM_OK){
        return TVSTORM_ERROR;
    }

    if(eeprom_i2c_Open() != TVSTORM_OK){
        LOGE("Error: eeprom_init  !!\n");
        eeprom_uninit();
        return TVSTORM_ERROR;
    }

    if(NULL == hI2cHandle){
        eeprom_i2c_Close();
        eeprom_uninit();
        return TVSTORM_ERROR_NOT_IMPLEMENTED;
    }

    if(eeprom_read(GET_BLD_OFFSET(downloadMode), (unsigned char*)dm, sizeof(SW_DOWNLOAD_MODE_T)) == TVSTORM_OK)
    {
        LOGI("download mode : 0x%08x\n", dm);
    }
    else{
        LOGE("Error: eeprom_read  !!\n");
        errCode = TVSTORM_ERROR;
    }

    if(eeprom_read(GET_TVS_OFFSET(downloadMode), (unsigned char*)tvs_dm, sizeof(TVS_DOWNLOAD_MODE_T)) == TVSTORM_OK)
    {
        LOGI("download mode : 0x%08x\n", dm);
    }
    else{
        LOGE("Error: eeprom_read  !!\n");
        errCode = TVSTORM_ERROR;
    }

    eeprom_i2c_Close();
    eeprom_uninit();

    return errCode;
}

/*
* Get stb power statu
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

    if(eeprom_init() != TVSTORM_OK){
        return TVSTORM_ERROR;
    }

    if(eeprom_i2c_Open() != TVSTORM_OK){
        LOGE("Error: eeprom_init  !!\n");
        eeprom_uninit();
        return TVSTORM_ERROR;
    }

    if(NULL == hI2cHandle){
        eeprom_i2c_Close();
        eeprom_uninit();
        return TVSTORM_ERROR_NOT_IMPLEMENTED;
    }

    if(eeprom_read(NVRAM_BASE_OFFSET_BOOTLOADER, (unsigned char*)&eeprom_bootldr, sizeof(NVRAM_DATA_BOOTLOADER_T)) == TVSTORM_OK)
    {
        *ps = eeprom_bootldr.stbPowerStatus;
        LOGI("power status  : 0%08x\n", eeprom_bootldr.stbPowerStatus);
    }
    else{
        LOGE("Error: eeprom_read  !!\n");
        errCode = TVSTORM_ERROR;
    }

    eeprom_i2c_Close();
    eeprom_uninit();

    return errCode;
}

/*
* Get factory reset mode
* input : 
*		factory reset mode : fr
* output :
*           return : TVSTORM_ERROR is i2c error
*           return : TVSTORM_ERROR_NOT_IMPLEMENTED is empty i2c handler
*		return : TVSTORM_OK is success
*/
TVSTORM_Error TVSTORM_Get_FactoryResetMode(FACTORY_RESET_MODE_T *fr)
{
    NVRAM_DATA_TVSTORM_T eeprom_tvstorm;
    TVSTORM_Error errCode = TVSTORM_OK;

    if(eeprom_init() != TVSTORM_OK){
        return TVSTORM_ERROR;
    }

    if(eeprom_i2c_Open() != TVSTORM_OK){
        LOGE("Error: eeprom_init  !!\n");
        eeprom_uninit();
        return TVSTORM_ERROR;
    }

    if(NULL == hI2cHandle){
        eeprom_i2c_Close();
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

    eeprom_i2c_Close();
    eeprom_uninit();

    return errCode;
}

/*
* Get ird sequence number
* input : 
*		ird sequence number : seq_no
* output :
*           return : TVSTORM_ERROR is i2c error
*           return : TVSTORM_ERROR_NOT_IMPLEMENTED is empty i2c handler
*		return : TVSTORM_OK is success
*/
TVSTORM_Error TVSTORM_Get_SequenceNumber(uint32_t *seq_no)
{
    TVSTORM_Error errCode = TVSTORM_OK;

    if(eeprom_init() != TVSTORM_OK){
        return TVSTORM_ERROR;
    }

    if(eeprom_i2c_Open() != TVSTORM_OK){
        LOGE("Error: eeprom_init  !!\n");
        eeprom_uninit();
        return TVSTORM_ERROR;
    }

    if(NULL == hI2cHandle){
        eeprom_i2c_Close();
        eeprom_uninit();
        return TVSTORM_ERROR_NOT_IMPLEMENTED;
    }

    if(eeprom_read(GET_TVS_OFFSET(sequence_number), (unsigned char*)seq_no, 4) == TVSTORM_OK)
    {
        LOGI("seq_no   : 0x%08x\n", *seq_no);
    }
    else{
        LOGE("Error: eeprom_read  !!\n");
        errCode = TVSTORM_ERROR;
    }

    eeprom_i2c_Close();
    eeprom_uninit();

    return errCode;
}

/*
* Set software download mode
* input : 
*           software download mode : dm
* output :
*           return : TVSTORM_ERROR is i2c error
*           return : TVSTORM_ERROR_NOT_IMPLEMENTED is empty i2c handler
*		return : TVSTORM_OK is success
*/
TVSTORM_Error TVSTORM_Set_DownloadMode(SW_DOWNLOAD_MODE_T dm, TVS_DOWNLOAD_MODE_T tvs_dm)
{
    NVRAM_DATA_BOOTLOADER_T eeprom_bootldr;
    TVSTORM_Error errCode = TVSTORM_OK;

    if(eeprom_init() != TVSTORM_OK){
        return TVSTORM_ERROR;
    }

    if(eeprom_i2c_Open() != TVSTORM_OK){
        LOGE("Error: eeprom_init  !!\n");
        eeprom_uninit();
        return TVSTORM_ERROR;
    }

    if(NULL == hI2cHandle){
        eeprom_i2c_Close();
        eeprom_uninit();
        return TVSTORM_ERROR_NOT_IMPLEMENTED;
    }

    // write
    if(eeprom_write(GET_BLD_OFFSET(downloadMode), (unsigned char*)&dm, sizeof(SW_DOWNLOAD_MODE_T)) != TVSTORM_OK)
    {
        LOGE("Error: eeprom_write SW_DOWNLOAD_MODE_T !!\n");
        eeprom_i2c_Close();
        eeprom_uninit();

        return TVSTORM_ERROR;
    }

    // write
    if(eeprom_write(GET_TVS_OFFSET(downloadMode), (unsigned char*)&tvs_dm, sizeof(TVS_DOWNLOAD_MODE_T)) != TVSTORM_OK)
    {
        LOGE("Error: eeprom_write TVS_DOWNLOAD_MODE_T!!\n");
        eeprom_i2c_Close();
        eeprom_uninit();

        return TVSTORM_ERROR;
    }

    eeprom_i2c_Close();
    eeprom_uninit();

    return errCode;
}

/*
* Set stb power statu
* input : 
*		stb power status : ps
* output :
*           return : TVSTORM_ERROR is i2c error
*           return : TVSTORM_ERROR_NOT_IMPLEMENTED is empty i2c handler
*		return : TVSTORM_OK is success
*/
TVSTORM_Error TVSTORM_Set_StbPowerStaus(STB_POWER_STATUS_T ps)
{
    NVRAM_DATA_BOOTLOADER_T eeprom_bootldr;
    TVSTORM_Error errCode = TVSTORM_OK;

    if(eeprom_init() != TVSTORM_OK){
        return TVSTORM_ERROR;
    }

    if(eeprom_i2c_Open() != TVSTORM_OK){
        LOGE("Error: eeprom_init  !!\n");
        eeprom_uninit();
        return TVSTORM_ERROR;
    }

    if(NULL == hI2cHandle){
        eeprom_i2c_Close();
        eeprom_uninit();
        return TVSTORM_ERROR_NOT_IMPLEMENTED;
    }

    // read
    if(eeprom_read(NVRAM_BASE_OFFSET_BOOTLOADER, (unsigned char*)&eeprom_bootldr, sizeof(NVRAM_DATA_BOOTLOADER_T)) != TVSTORM_OK)
    {
        LOGE("Error: eeprom_read  !!\n");
        eeprom_i2c_Close();
        eeprom_uninit();
        return TVSTORM_ERROR;
    }

    // write
    eeprom_bootldr.stbPowerStatus = ps;
    if(eeprom_write(NVRAM_BASE_OFFSET_BOOTLOADER, (unsigned char*)&eeprom_bootldr, sizeof(NVRAM_DATA_BOOTLOADER_T)) != TVSTORM_OK)
    {
        LOGE("Error: eeprom_read  !!\n");
        errCode = TVSTORM_ERROR;
    }

    eeprom_i2c_Close();
    eeprom_uninit();

    return errCode;
}

/*
* Set factory reset mode
* input : 
*		factory reset mode : fr
* output :
*           return : TVSTORM_ERROR is i2c error
*           return : TVSTORM_ERROR_NOT_IMPLEMENTED is empty i2c handler
*		return : TVSTORM_OK is success
*/
TVSTORM_Error TVSTORM_Set_FactoryResetMode(FACTORY_RESET_MODE_T fr)
{
    NVRAM_DATA_TVSTORM_T eeprom_tvstorm;
    TVSTORM_Error errCode = TVSTORM_OK;

    if(eeprom_init() != TVSTORM_OK){
        return TVSTORM_ERROR;
    }

    if(eeprom_i2c_Open() != TVSTORM_OK){
        LOGE("Error: eeprom_init  !!\n");
        eeprom_uninit();
        return TVSTORM_ERROR;
    }

    if(NULL == hI2cHandle){
        eeprom_i2c_Close();
        eeprom_uninit();
        return TVSTORM_ERROR_NOT_IMPLEMENTED;
    }

    // read
    if(eeprom_read(NVRAM_BASE_OFFSET_TVSTORM, (unsigned char*)&eeprom_tvstorm, sizeof(FACTORY_RESET_MODE_T) + 4)  != TVSTORM_OK)
    {
        LOGE("Error: eeprom_read  !!\n");
        eeprom_i2c_Close();
        eeprom_uninit();

        return TVSTORM_ERROR;
    }

    // write
    eeprom_tvstorm.factoryResetMode = fr;
    if(eeprom_write(NVRAM_BASE_OFFSET_TVSTORM, (unsigned char*)&eeprom_tvstorm, sizeof(FACTORY_RESET_MODE_T) + 4)  != TVSTORM_OK)
    {
        LOGE("Error: eeprom_read  !!\n");
        errCode =  TVSTORM_ERROR;
    }

    eeprom_i2c_Close();
    eeprom_uninit();

    return errCode;
}

/*
* Set ird sequence number
* input : 
*		ird sequence number : seq_no
* output :
*           return : TVSTORM_ERROR is i2c error
*           return : TVSTORM_ERROR_NOT_IMPLEMENTED is empty i2c handler
*		return : TVSTORM_OK is success
*/
TVSTORM_Error TVSTORM_Set_SequenceNumber(uint32_t seq_no)
{
    TVSTORM_Error errCode = TVSTORM_OK;

    if(eeprom_init() != TVSTORM_OK){
        return TVSTORM_ERROR;
    }

    if(eeprom_i2c_Open() != TVSTORM_OK){
        LOGE("Error: eeprom_init  !!\n");
        eeprom_uninit();
        return TVSTORM_ERROR;
    }

    if(NULL == hI2cHandle){
        eeprom_i2c_Close();
        eeprom_uninit();
        return TVSTORM_ERROR_NOT_IMPLEMENTED;
    }

    if(eeprom_write(GET_TVS_OFFSET(sequence_number), (unsigned char*)&seq_no, 4) != TVSTORM_OK)
    {
        LOGE("Error: eeprom_write  !!\n");
        errCode =  TVSTORM_ERROR;
    }

    eeprom_i2c_Close();
    eeprom_uninit();

    return errCode;
}
/*
* Get factory reset mode
* input : 
*           void
* output :
*           return : 0 is not set factory reset
*           return : 1 is set factory reset
*/
int TVSTORM_is_factory_reset_mode_enabled(void)
{
    NVRAM_DATA_TVSTORM_T eeprom_tvstorm;

    if(eeprom_init() != TVSTORM_OK){
        return TVSTORM_ERROR;
    }

    if(eeprom_i2c_Open() != TVSTORM_OK){
        LOGE("Error: eeprom_init  !!\n");
        eeprom_uninit();
        return TVSTORM_ERROR;
    }

    if(NULL == hI2cHandle){
        eeprom_i2c_Close();
        eeprom_uninit();
        return TVSTORM_ERROR_NOT_IMPLEMENTED;
    }

    if(eeprom_read(NVRAM_BASE_OFFSET_TVSTORM, (unsigned char*)&eeprom_tvstorm, sizeof(FACTORY_RESET_MODE_T) + 4) == TVSTORM_OK)
    {
        LOGI("factory reset   : 0x%08x\n", eeprom_tvstorm.factoryResetMode);
        if(FACTORY_RESET_MODE_ENABLED == eeprom_tvstorm.factoryResetMode)
            return 1;
    }

    eeprom_i2c_Close();
    eeprom_uninit();

    return 0;
}


/*
* Set disable factory reset mode
* input : 
*           void
* output :
*           return : TVSTORM_ERROR is i2c error
*           return : TVSTORM_ERROR_NOT_IMPLEMENTED is empty i2c handler
*		return : TVSTORM_OK is success
*/
TVSTORM_Error TVSTORM_set_factory_reset_mode_disabled(void)
{
    NVRAM_DATA_TVSTORM_T eeprom_tvstorm;
    TVSTORM_Error errCode = TVSTORM_OK;

    if(eeprom_init() != TVSTORM_OK){
        return TVSTORM_ERROR;
    }

    if(eeprom_i2c_Open() != TVSTORM_OK){
        LOGE("Error: eeprom_init  !!\n");
        eeprom_uninit();
        return TVSTORM_ERROR;
    }

    if(NULL == hI2cHandle){
        eeprom_i2c_Close();
        eeprom_uninit();
        return TVSTORM_ERROR_NOT_IMPLEMENTED;
    }

    if(eeprom_read(NVRAM_BASE_OFFSET_TVSTORM, (unsigned char*)&eeprom_tvstorm, sizeof(FACTORY_RESET_MODE_T) + 4)  == TVSTORM_OK)
    {
        LOGI("factory reset   : 0x%08x\n", eeprom_tvstorm.factoryResetMode);

        eeprom_tvstorm.factoryResetMode = FACTORY_RESET_MODE_DISABLED;
        if(eeprom_write(NVRAM_BASE_OFFSET_TVSTORM, (unsigned char*)&eeprom_tvstorm, sizeof(FACTORY_RESET_MODE_T) + 4) != TVSTORM_OK)
        {
            LOGE("factory reset mode disabled fail!!! \n");
        }
    }
    else
    {
        LOGE("eeprom read fail! (NVMRAM_BASE_OFFSET_TVSTORM) \n");
    }

    eeprom_i2c_Close();
    eeprom_uninit();

    return errCode;
}


/*
* Set disable download mode
* input : 
*           void
* output :
*           return : TVSTORM_ERROR is i2c error
*           return : TVSTORM_ERROR_NOT_IMPLEMENTED is empty i2c handler
*		return : TVSTORM_OK is success
*/
TVSTORM_Error TVSTORM_set_download_mode_disabled(void)
{
    NVRAM_DATA_BOOTLOADER_T eeprom_bootldr;
    TVSTORM_Error errCode = TVSTORM_OK;

    if(eeprom_init() != TVSTORM_OK){
        return TVSTORM_ERROR;
    }

    if(eeprom_i2c_Open() != TVSTORM_OK){
        LOGE("Error: eeprom_init  !!\n");
        eeprom_uninit();
        return TVSTORM_ERROR;
    }

    if(NULL == hI2cHandle){
        eeprom_i2c_Close();
        eeprom_uninit();
        return TVSTORM_ERROR_NOT_IMPLEMENTED;
    }

    if(eeprom_read(NVRAM_BASE_OFFSET_BOOTLOADER, 
                (unsigned char*)&eeprom_bootldr, 
                sizeof(NVRAM_DATA_BOOTLOADER_T))  == TVSTORM_OK)
    {
        LOGI("download mode : 0x%08x\n", eeprom_bootldr.downloadMode);
        //LOGI("power status  : 0x%08x\n", eeprom_bootldr.stbPowerStatus);

        eeprom_bootldr.downloadMode = SW_DOWNLOAD_MODE_DISABLED;
        if(eeprom_write(NVRAM_BASE_OFFSET_BOOTLOADER, 
                    (unsigned char*)&eeprom_bootldr, 
                    sizeof(NVRAM_DATA_BOOTLOADER_T)) != TVSTORM_OK)
        {
            LOGE("download mode disabled fail!!! \n");
        }
    }
    else
    {
        LOGE("eeprom read fail! (NVMRAM_BASE_OFFSET_BOOTLOADER) \n");
    }

    eeprom_i2c_Close();
    eeprom_uninit();

    return errCode;
}

/*
 * Set disable download mode
 * input : 
 *           void
 * output :
 *           return : TVSTORM_ERROR is i2c error
 *           return : TVSTORM_ERROR_NOT_IMPLEMENTED is empty i2c handler
 *       return : TVSTORM_OK is success
 */
TVSTORM_Error TVSTORM_get_last_ch(unsigned char* last_ch_uid)
{
    TVSTORM_Error errCode = TVSTORM_OK;
    LAST_CH_INFO  last_ch[2];
    unsigned short csum0 = 0, csum1 = 0;
    int i, idx;

    if(NULL == last_ch_uid)
        return TVSTORM_ERROR;

    if(eeprom_init() != TVSTORM_OK){
        return TVSTORM_ERROR;
    }    

    if(eeprom_i2c_Open() != TVSTORM_OK){
        LOGE("Error: eeprom_init  !!\n");
        eeprom_uninit();
        return TVSTORM_ERROR;
    }    

    if(NULL == hI2cHandle){
        eeprom_i2c_Close();
        eeprom_uninit();
        return TVSTORM_ERROR_NOT_IMPLEMENTED;
    }    

    LOGI("offset last_ch : %d \n", GET_TVS_OFFSET(last_ch));
    LOGI("sizeof(last_ch): %d \n", sizeof(last_ch));

    memset(last_ch, 0, sizeof(last_ch));

    if(eeprom_read(GET_TVS_OFFSET(last_ch),
                (unsigned char*)last_ch,
                sizeof(last_ch))  == TVSTORM_OK)
    {    
        //LOGI("last ch uid : %s\n", last_ch);
    }    
    else 
    {    
        LOGE("eeprom read fail! (NVMRAM_BASE_OFFSET_BOOTLOADER) \n");
        return TVSTORM_ERROR;
    }    

    for(i=0; i<14; i++)
    {
        csum0 += last_ch[0].uid[i];
        csum1 += last_ch[1].uid[i];
    }

    LOGI("computed csum : 0x%04x, saved csum: 0x%04x \n", csum0, last_ch[0].csum);

    if(last_ch[0].csum == csum0)
    {
        idx = 0;
    }
    else if(last_ch[1].csum == csum1)
    {
        idx = 1;
    }
    else
    {
        LOGE("last ch info is all invalid! \n");
        idx = -1;
    }

    if(idx < 0)
    {
        errCode = TVSTORM_ERROR;
    }
    else
        strcpy(last_ch_uid, (char*)last_ch[idx].uid);

    eeprom_i2c_Close();
    eeprom_uninit();

    return errCode;
}

/*
 * Set disable download mode
 * input : 
 *           void
 * output :
 *           return : TVSTORM_ERROR is i2c error
 *           return : TVSTORM_ERROR_NOT_IMPLEMENTED is empty i2c handler
 *       return : TVSTORM_OK is success
 */
TVSTORM_Error TVSTORM_set_last_ch(unsigned char* last_ch_uid)
{
    TVSTORM_Error errCode = TVSTORM_OK;
    LAST_CH_INFO  last_ch[2];
    unsigned short int csum = 0;
    int i;

    if(NULL == last_ch_uid)
        return TVSTORM_ERROR;

    if(strlen(last_ch_uid) > 13)
    {
        LOGE("last ch length is too long! \n");
        return TVSTORM_ERROR;
    }

    memset(last_ch, 0, sizeof(last_ch));

    if(eeprom_init() != TVSTORM_OK){
        return TVSTORM_ERROR;
    }    

    if(eeprom_i2c_Open() != TVSTORM_OK){
        LOGE("Error: eeprom_init  !!\n");
        eeprom_uninit();
        return TVSTORM_ERROR;
    }    

    if(NULL == hI2cHandle){
        eeprom_i2c_Close();
        eeprom_uninit();
        return TVSTORM_ERROR_NOT_IMPLEMENTED;
    }    

    LOGI("last_ch_uid len %d \n", strlen(last_ch_uid));

    for(i=0; i<strlen(last_ch_uid); i++)
    {
        LOGI("csum 0x%04x last_ch_uid[%d] 0x%02x \n",
                csum, i, last_ch_uid[i]);
        csum += last_ch_uid[i];
    }

    LOGI("computed csum : 0x%04x \n", csum);

    for(i=0; i<2; i++)
    {
        memcpy(last_ch[i].uid, last_ch_uid, strlen(last_ch_uid));
        last_ch[i].csum = csum;
    }

    if(eeprom_write(GET_TVS_OFFSET(last_ch),
                (unsigned char*)last_ch,
                sizeof(last_ch))  == TVSTORM_OK)
    {    
        //LOGI("last ch uid : %s\n", last_ch);
    }    
    else 
    {    
        LOGE("eeprom read fail! (NVMRAM_BASE_OFFSET_BOOTLOADER) \n");
    }    

    eeprom_i2c_Close();
    eeprom_uninit();

    return errCode;
}

