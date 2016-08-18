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
 
#ifndef TVSTORM_EEPROM_H
#define TVSTORM_EEPROM_H

#include "cutils/log.h"
#include "../../../lge/library/dloader/eeprom_common.h"

#if __cplusplus
extern "C" {
#endif

#define SUPPORT_TVSTORM_EEPROM_DEBUG

#ifdef SUPPORT_TVSTORM_EEPROM_DEBUG
#define LOGE(...) fprintf(stdout, "E:" __VA_ARGS__)
#define LOGW(...) fprintf(stdout, "W:" __VA_ARGS__)
#define LOGI(...) fprintf(stdout, "I:" __VA_ARGS__)
#endif


// TVSTORM eeprom Version
#define TVSTORM_EEPROM_VERSION 1
#define TVSTORM_EEPROM_PATCHLEVEL 1
#define TVSTORM_EEPROM_SUBLEVEL 1

// TVSTORM eeprom date
#define TVSTORM_EEPROM_YEAR 2015
#define TVSTORM_EEPROM_MONTH 04
#define TVSTORM_EEPROM_DAY 01

typedef struct TVSTORM_EEPROM_VERSION_S
{
    uint16_t    version;
    uint16_t    patchlevel;
    uint16_t    sublevel;
}TVSTORM_EEPROM_VERSION_T;

typedef struct TVSTORM_EEPROM_DATE_S
{
    uint16_t    year;
    uint16_t    month;
    uint16_t    day;
}TVSTORM_EEPROM_DATE_T;


typedef enum
{
	TVSTORM_ERROR = -1,
	TVSTORM_OK = 0,
	TVSTORM_ERROR_NOT_IMPLEMENTED,
	TVSTORM_ERROR_INVALID_PARAMS,
} TVSTORM_Error;


void TVSTORM_eeprom_version(TVSTORM_EEPROM_VERSION_T *eeprom_version);
void TVSTORM_eeprom_last_date(TVSTORM_EEPROM_DATE_T *eeprom_date);

TVSTORM_Error TVSTORM_eeprom_read(unsigned int subAddr, unsigned char *data, unsigned int length);

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
TVSTORM_Error TVSTORM_eeprom_write(unsigned int subAddr, unsigned char *data, unsigned int length);

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
TVSTORM_Error TVSTORM_get_boot_param(SW_DOWNLOAD_MODE_T *dm, STB_POWER_STATUS_T *ps, FACTORY_RESET_MODE_T *fr, unsigned char *mfc, unsigned char *dfc);

/*
* Get software download mode
* input : 
*          dm: software download mode
*          tvs_dm: tvs download mode
* output :
* return : 
*       TVSTORM_ERROR is i2c error
*       TVSTORM_ERROR_NOT_IMPLEMENTED is empty i2c handler
*		TVSTORM_OK is success
*/
TVSTORM_Error TVSTORM_Get_DownloadMode(SW_DOWNLOAD_MODE_T *dm, TVS_DOWNLOAD_MODE_T *tvs_dm);

/*
* Get stb power statu
* input : 
*		stb power status : ps
* output :
*           return : TVSTORM_ERROR is i2c error
*           return : TVSTORM_ERROR_NOT_IMPLEMENTED is empty i2c handler
*		return : TVSTORM_OK is success
*/
TVSTORM_Error TVSTORM_Get_StbPowerStaus(STB_POWER_STATUS_T *ps);

/*
* Get factory reset mode
* input : 
*		factory reset mode : fr
* output :
*           return : TVSTORM_ERROR is i2c error
*           return : TVSTORM_ERROR_NOT_IMPLEMENTED is empty i2c handler
*		return : TVSTORM_OK is success
*/
TVSTORM_Error TVSTORM_Get_FactoryResetMode(FACTORY_RESET_MODE_T *fr);

/*
* Get ird sequence number
* input : 
*		ird sequence number : seq_no
* output :
*           return : TVSTORM_ERROR is i2c error
*           return : TVSTORM_ERROR_NOT_IMPLEMENTED is empty i2c handler
*		return : TVSTORM_OK is success
*/
TVSTORM_Error TVSTORM_Get_SequenceNumber(uint32_t *seq_no);

/*
* Set software download mode
* input : 
*        dm : SW_DOWNLOAD_MODE_ENABLED if download mode
*        tvs_dm : TVS_DOWNLOAD_MODE_SUS if sus download mode, TVS_DOWNLOAD_MODE_USB if usb downloade mode.
* return :
*          TVSTORM_ERROR is i2c error
*          TVSTORM_ERROR_NOT_IMPLEMENTED is empty i2c handler
*		   TVSTORM_OK is success
*/
TVSTORM_Error TVSTORM_Set_DownloadMode(SW_DOWNLOAD_MODE_T dm, TVS_DOWNLOAD_MODE_T tvs_dm);

/*
* Set stb power statu
* input : 
*		stb power status : ps
* output :
*           return : TVSTORM_ERROR is i2c error
*           return : TVSTORM_ERROR_NOT_IMPLEMENTED is empty i2c handler
*		return : TVSTORM_OK is success
*/
TVSTORM_Error TVSTORM_Set_StbPowerStaus(STB_POWER_STATUS_T ps);

/*
* Set factory reset mode
* input : 
*		factory reset mode : fr
* output :
*           return : TVSTORM_ERROR is i2c error
*           return : TVSTORM_ERROR_NOT_IMPLEMENTED is empty i2c handler
*		return : TVSTORM_OK is success
*/
TVSTORM_Error TVSTORM_Set_FactoryResetMode(FACTORY_RESET_MODE_T fr);

/*
* Set ird sequence number
* input : 
*		ird sequence number : seq_no
* output :
*           return : TVSTORM_ERROR is i2c error
*           return : TVSTORM_ERROR_NOT_IMPLEMENTED is empty i2c handler
*		return : TVSTORM_OK is success
*/
TVSTORM_Error TVSTORM_Set_SequenceNumber(uint32_t seq_no);

/*
* Get factory reset mode
* input : 
*           void
* output :
*           return : 0 is not set factory reset
*           return : 1 is set factory reset
*/
int TVSTORM_is_factory_reset_mode_enabled(void);

/*
* Set disable factory reset mode
* input : 
*           void
* output :
*           return : TVSTORM_ERROR is i2c error
*           return : TVSTORM_ERROR_NOT_IMPLEMENTED is empty i2c handler
*		return : TVSTORM_OK is success
*/
TVSTORM_Error TVSTORM_set_factory_reset_mode_disabled(void);

/*
* Set disable download mode
* input : 
*           void
* output :
*           return : TVSTORM_ERROR is i2c error
*           return : TVSTORM_ERROR_NOT_IMPLEMENTED is empty i2c handler
*		return : TVSTORM_OK is success
*/
TVSTORM_Error TVSTORM_set_download_mode_disabled(void);


TVSTORM_Error TVSTORM_get_last_ch(unsigned char* last_ch_uid);
TVSTORM_Error TVSTORM_set_last_ch(unsigned char* last_ch_uid);
#if __cplusplus
}
#endif

#endif /* #ifndef TVSTORM_EEPROM_H */
