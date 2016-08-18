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
#ifndef EEPROM_H
#define EEPROM_H

#ifdef SH960C_LN
#include "../../../lge/library/dloader/eeprom_common.h"
#else
#include "../../../lge/library/dloader/lge_st950i-ln_eeprom.h"
#endif

#if __cplusplus
extern "C" {
#endif

//#define SUPPORT_TVSTORM_EEPROM_DEBUG

#undef LOGE
#undef LOGW
#undef LOGI

#ifdef SUPPORT_TVSTORM_EEPROM_DEBUG
#define LOGE(...) fprintf(stdout, "E:" __VA_ARGS__)
#define LOGW(...) fprintf(stdout, "W:" __VA_ARGS__)
#define LOGI(...) fprintf(stdout, "I:" __VA_ARGS__)
#else
#define LOGE(...) do {} while (0)
#define LOGW(...) do {} while (0)
#define LOGI(...) do {} while (0)
#endif

int eeprom_init(void);
int eeprom_uninit(void);
int eeprom_read(unsigned int subAddr, unsigned char *data, unsigned int length);
int eeprom_write(unsigned int subAddr, unsigned char *data, unsigned int length);
TVSTORM_Error TVSTORM_Get_StbPowerStaus(STB_POWER_STATUS_T *ps);
TVSTORM_Error TVSTORM_Get_DownloadMode(SW_DOWNLOAD_MODE_T *dm);
TVSTORM_Error TVSTORM_Get_BootParam(SW_DOWNLOAD_MODE_T *dm, STB_POWER_STATUS_T *ps, FACTORY_RESET_MODE_T *fr, unsigned char *mfc, unsigned char *dfc);

#if __cplusplus
}
#endif

#endif
