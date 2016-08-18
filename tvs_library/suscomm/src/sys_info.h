/*
 * sys_info.h
 *
 *  Created on: Jan 28, 2015
 *      Author: jhseo
 */

#ifndef SYS_INFO_H_
#define SYS_INFO_H_

typedef struct {
    char * pStbSerialNum;
    char * pMacAddress;
    char * pFwVersion;
    char * pModelName;
    int susFullVer;
    int susPartialVer;
} SYS_INFO_T;

typedef enum {
    RET_FAIL = -1, RET_SUCCESS
} SYS_INFO_RET_T;

//////////////////////////////////////////////////
// API for System Information
//////////////////////////////////////////////////
char * getStbSerialNum();

char * getMacAddress();

char * getModelName();

char * getSusFullVer();
int setSusFullVer(int ver);

char * getSusPartialVer();
int setSusPartialVer(int ver);

char * getGroupId();
int saveGroupId(char * pGroupId);
#endif /* SYS_INFO_H_ */
