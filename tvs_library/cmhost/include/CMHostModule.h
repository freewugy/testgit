#ifndef _CMHOST_MODULE_H_
#define _CMHOST_MODULE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>

#include <utils/Log.h>
#include <android/log.h>

#define CM_TAG "CMHostService"

#define VERSION "1.0.1"


#define DEBUG_LEVEL 2


#if DEBUG_LEVEL >= 2
#define CMLOGD(...) __android_log_print(ANDROID_LOG_DEBUG, CM_TAG, __VA_ARGS__)  
#else
#define CMLOGD(...)
#endif

#if DEBUG_LEVEL >= 1
#define CMLOGI(...) __android_log_print(ANDROID_LOG_INFO, CM_TAG, __VA_ARGS__)
#else
#define CMLOGI(...)
#endif

#define CMLOGE(...) __android_log_print(ANDROID_LOG_ERROR, CM_TAG, __VA_ARGS__) 

#define CMLOGT()  PrintTraceLog __print(__FUNCTION__ , __LINE__)

class PrintTraceLog {
public:
	PrintTraceLog(const char *function, const int line) :
	mFunc(function), mLine(line)
	{
		CMLOGI("%s(%d) >>> Begin", mFunc, mLine);	
	}

	~PrintTraceLog(){
		CMLOGI("%s(%d) <<< End", mFunc, mLine); 
	}

private:
	const char *mFunc;
	const int mLine;
};
enum {
	PHY_DS_NO_SPECIFIC_MODULATION = 0,
	PHY_DS_64QAM_MODULATION,
	PHY_DS_256QAM_MODULATION
};

enum {
	CABLE_OTHERS = 0,
	CABLE_ANNEX_A,
	CABLE_ANNEX_B,
	CABLE_ANNEX_C
};

enum {
kOther = 1,
kNotReady,
kNotSynchronized,
kPhySynchronized,
kUsParametersAcquired,
kRangingComplete,
kDhcpv4Complete,
kTodEstablished,
kSecurityEstablished,
kConfigFileDownloadComplete,
kRegistrationComplete,
kOperational,
kAccessDenied,
kEaeInProgress,
kDhcpv4InProgress,
kDhcpv6InProgress,
kDhcpv6Complete,
kRegistrationInProgress,
kBpiInit,
kForwardingDisabled,
kDsTopologyResolutionInProgress,
kRangingInProgress,
kRfMuteAll,
kStateEnd
};

typedef struct {
	int cmState;
	int operationState;
} CMStatusSet;

#define CM_VERSION_LEN 4
#define MAC_ADDR_LEN 6

typedef struct
{
	int	cmVersion[CM_VERSION_LEN];
	int	ulCmMac[MAC_ADDR_LEN];
	int	cmStatus;
} CmInfoStruct;

typedef struct
{
	int				Flag;	// 1: Valid, 0: Invalid
	float			DSRFfrequency;
	float			USRFfrequency;
	unsigned int	QAMType;
	int				Annex;
	float			MSE_SNR;
	float			DSPower;
	int				BER;
	float			USPower;
} TunerParams;

typedef struct
{
	int		ip1;
	int		ip2;
	int		ip3;
	int		ip4;
} IPAddress;

#endif

