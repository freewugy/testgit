#ifndef _CMHOST_NATIVE_H_
#define _CMHOST_NATIVE_H_

#include <CMHostModule.h>

extern "C" {
CmInfoStruct get_cm_info();
int check_update_file();
int request_swdl();
TunerParams get_tuner_params();
IPAddress get_cm_ip_addr();
IPAddress get_cmts_ip();
}
#endif //_CMHOST_NATIVE_H_


