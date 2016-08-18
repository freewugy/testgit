#include <unistd.h>

#include <sys/system_properties.h>
#include <cutils/properties.h>

#include "nexus_platform.h"
#include "nexus_ipc_client_factory.h"
#include "bkni.h"

#include "nexus_frontend.h"
#include "nexus_docsis.h"
#include "nexus_docsis_interface.h"
#include "nexus_docsis_data.h"

#include <CMHostDevice.h>

namespace android {


NexusIPCClientBase *g_ipcClient = NULL; 
NexusClientContext *g_nexusClient = NULL;  
bool mNexusInit = false;
static void notificationCallback(void *context, int param);
/* Only used to check booting state is or not because the timing of initialzing routing table of booting and updating cm bin are differenet. */
static bool isBooting = false;
/* Used to check that updating cm bin is going or not. */
static bool mSWDownload = false;
/* the flag about completion of cm registration */
static bool cmRegistration = false;


CMHostDevice::CMHostDevice()
{
	char buf[PROP_VALUE_MAX];
	int len, count = 0, maxCount = 5 ;

	CMLOG("Create CMHostDevice.. Version : %s", VERSION);

	/* Initialize NexusClient to join */
	mJoinThread = new CMHostJoinThread;
	mJoinThread->run();

	/* Monitoring for CM update */
	mUpdateCMThread = new UpdateCMThread;
	mUpdateCMThread->run();

	CMLOG("Complete to create CMHostDevice...");
}

CMHostDevice::~CMHostDevice(){
	CMLOG("Destory CMHostDevice...");
	if(mNexusInit){
		if(g_ipcClient){
			g_ipcClient->disconnectClientResources(g_nexusClient);  
			g_ipcClient->destroyClientContext(g_nexusClient); 
			g_nexusClient = NULL;
			delete g_ipcClient;
		}

		if(mJoinThread){
			delete mJoinThread;
			mJoinThread = NULL;
		}
		if(mUpdateCMThread){
			mUpdateCMThread->setThreadRunning(false);
			delete mUpdateCMThread;
			mUpdateCMThread = NULL;
		}
	}
}

static void notificationCallback(void *context, int param){
	BSTD_UNUSED(param);

#if NEXUS_FRONTEND_DOCSIS
	NEXUS_FrontendDeviceHandle handle = (NEXUS_FrontendDeviceHandle)NEXUS_Docsis_getFronendDevice(); 
	NEXUS_DocsisDeviceStatus status;
	NEXUS_Docsis_GetDeviceStatus(handle, &status);	

	CMLOG(" Current Device state : %d", (int)status.state);
	switch(status.state){
		case NEXUS_DocsisDeviceState_eOperational:{
			CMLOG("Current State is Operational...");
			sp<NotificationThread> notiThread = new NotificationThread;
			notiThread->setNotificationState((int)NEXUS_DocsisDeviceState_eOperational);
			notiThread->run();
				
		}
		break;

		case NEXUS_DocsisDeviceState_eReset:{
			CMLOG("Current State is Reset...");
			sp<NotificationThread> notiThread = new NotificationThread;
			notiThread->setNotificationState((int)NEXUS_DocsisDeviceState_eReset);
			notiThread->run();
		}
		break;

		case NEXUS_DocsisDeviceState_eFailed:{
			CMLOG("Current State is Failed...");
		}
		break;

		case NEXUS_DocsisDeviceState_eUninitialized:
		default:{
			CMLOG("Current State is uninitiailized...");
		}
		break;
	}
#endif
}

status_t CMHostDevice::getCMInfo(CmInfoStruct *info){
	status_t result = NO_ERROR;
	CMLOG("CMHostDevice::getCMInfo");

#if NEXUS_FRONTEND_DOCSIS 
	NEXUS_DocsisCMInfo cmInfo;
	BKNI_Memset(&cmInfo, 0x00, sizeof(cmInfo));

	NEXUS_FrontendDeviceHandle handle = (NEXUS_FrontendDeviceHandle)NEXUS_Docsis_getFronendDevice();
	result = NEXUS_Docsis_GetCMInfo(handle, &cmInfo);

	CMLOG("Get CM Info result - %d", result);
	CMLOG("CM Version	: %u.%u.%u", cmInfo.cmVersion[1], cmInfo.cmVersion[2], cmInfo.cmVersion[3]);
	CMLOG("CM MAC    	: %x:%x:%x:%x:%x:%x", cmInfo.cmMac[0], cmInfo.cmMac[1], cmInfo.cmMac[2],cmInfo.cmMac[3],
										cmInfo.cmMac[4], cmInfo.cmMac[5]);
	CMLOG("CM Status  	: %d", cmInfo.cmStatus);
	
	CMLOG("CM IP 		: %d.%d.%d.%d", cmInfo.ecmIPAddress[0],
										cmInfo.ecmIPAddress[1],
										cmInfo.ecmIPAddress[2],
										cmInfo.ecmIPAddress[3]);
	CMLOG("CMTS IP		: %d.%d.%d.%d", cmInfo.ecmtsIPAddress[0],
										cmInfo.ecmtsIPAddress[1],
										cmInfo.ecmtsIPAddress[2],
										cmInfo.ecmtsIPAddress[3]);
	CMLOG("CM DS Freq	: %d", cmInfo.DSRfFreq);
	CMLOG("CM US Freq	: %d", cmInfo.USRfFreq);
	CMLOG("CM QAMType	: %d", cmInfo.QAMType);
	CMLOG("CM Annex		: %d", cmInfo.Annex);
	CMLOG("CM MSE_SNR	: %d", cmInfo.MSE_SNR);
	CMLOG("CM DS Power	: %d", cmInfo.DSPower);
	CMLOG("CM US Power	: %d", cmInfo.USPower);

	if(result == NO_ERROR){
		int i;
		for(i=0;i<4;i++)
			info->cmVersion[i] = cmInfo.cmVersion[i];
		for(i=0;i<6;i++)
			info->ulCmMac[i] = cmInfo.cmMac[i];
	
		info->cmStatus = cmInfo.cmStatus;
	}
#else
	int cmVersion[]={0,5,0,0,1};
	int mac[]={0x10, 0x20,0x30,0xde,0xad,0x10};

	int i;
	for(i=0;i<4;i++)
		info->cmVersion[i] = cmVersion[i];
	for(i=0;i<6;i++)
		info->ulCmMac[i] = mac[i];

    info->cmStatus = 1;
    CMLOG("CMHost cmVersion for TEST:%s", info->cmVersion);
#endif

	return result;
}

status_t CMHostDevice::requestSWDL(){
	status_t result = NO_ERROR;
	CMLOG("CMHostDevice::requestSWDL");
	mUpdateCMThread->startSWDL();	
	return result;
}

status_t CMHostDevice::getTunerParam(TunerParams *param){
	status_t result = NO_ERROR;
#if NEXUS_FRONTEND_DOCSIS 
	NEXUS_DocsisCMInfo cmInfo;
	BKNI_Memset(&cmInfo, 0x00, sizeof(cmInfo));
	NEXUS_FrontendDeviceHandle handle = (NEXUS_FrontendDeviceHandle)NEXUS_Docsis_getFronendDevice();
	result = NEXUS_Docsis_GetCMInfo(handle, &cmInfo);

	CMLOG("CM get param - result : %d", result);
	CMLOG("CM DSRFfrequency : %d", cmInfo.DSRfFreq);
	CMLOG("CM USRFfrequency : %d", cmInfo.USRfFreq);
	CMLOG("CM QAMType 		: %d", cmInfo.QAMType);
	CMLOG("CM Annex 		: %d", cmInfo.Annex);
	CMLOG("CM MSE_SNR		: %d", cmInfo.MSE_SNR);
	CMLOG("CM DSPower		: %d", cmInfo.DSPower);
	CMLOG("CM USPower		: %d", cmInfo.USPower);
	
	if(result == NO_ERROR){
		param->Flag = 1;
		param->DSRFfrequency = (float)cmInfo.DSRfFreq;
		param->USRFfrequency = (float)cmInfo.USRfFreq;
		param->QAMType = (unsigned int)cmInfo.QAMType;
		param->Annex = cmInfo.Annex;
		param->MSE_SNR = cmInfo.MSE_SNR;
		param->DSPower = cmInfo.DSPower;
		param->BER = -1; // It doesn't be used.
		param->USPower = cmInfo.USPower;
	}
#else
    CMLOG("CMHostDevice::getTunerParam");
    param->Flag = 1;
    param->DSRFfrequency =100.0f;
    param->USRFfrequency = 200.0f;
    param->QAMType = 256;
    param->Annex=1;
    param->MSE_SNR=56.1f;
    param->DSPower=25.3f;
    param->BER=44;
    param->USPower=11.5f;
#endif
	
	return result;
}

status_t CMHostDevice::getIPAddr(IPAddress *addr){
	status_t result = NO_ERROR;
	CMLOG("CMHost getIPAddr >>> Begin");
#if NEXUS_FRONTEND_DOCSIS
	NEXUS_DocsisCMInfo cmInfo;
	BKNI_Memset(&cmInfo, 0x00, sizeof(cmInfo));
	NEXUS_FrontendDeviceHandle handle = (NEXUS_FrontendDeviceHandle)NEXUS_Docsis_getFronendDevice();
	result = NEXUS_Docsis_GetCMInfo(handle, &cmInfo);   

	CMLOG("CM get IP - result : %d", result);
	CMLOG("CM IP Address : %d.%d.%d.%d", cmInfo.ecmIPAddress[0],
										cmInfo.ecmIPAddress[1],
										cmInfo.ecmIPAddress[2],
										cmInfo.ecmIPAddress[3]);
	if(result == NO_ERROR){
		addr->ip1 = cmInfo.ecmIPAddress[0];
		addr->ip2 = cmInfo.ecmIPAddress[1];
		addr->ip3 = cmInfo.ecmIPAddress[2];
		addr->ip4 = cmInfo.ecmIPAddress[3];
	}
#else
    addr->ip1 = 192;
    addr->ip2 = 168;
    addr->ip3 = 200;
    addr->ip4 = 66;
    CMLOG("CMHostDevice::getIPAddr ip1 : %d, ip2 : %d, ip3: %d, ip4: %d",
        addr->ip1, addr->ip2, addr->ip3, addr->ip4);
#endif

	return result;
}

status_t CMHostDevice::setTuneFreq(uint32_t freq){
	status_t result = NO_ERROR;
	freq *= 10000;
	CMLOG("CMHostDevice::setTuneFreq - freq :%d", freq);

#if NEXUS_FRONTEND_DOCSIS
	NEXUS_FrontendDeviceHandle handle = (NEXUS_FrontendDeviceHandle)NEXUS_Docsis_getFronendDevice();
	result = NEXUS_Docsis_SetFreq(handle, freq);
#endif	
	CMLOG("CMHostDevice::setTuneFreq result : %d", result);
	return result;
}

status_t CMHostDevice::getCMTSIP(IPAddress *addr){
	status_t result = NO_ERROR;
#if NEXUS_FRONTEND_DOCSIS
	NEXUS_DocsisCMInfo cmInfo;
	BKNI_Memset(&cmInfo, 0x00, sizeof(cmInfo));
	NEXUS_FrontendDeviceHandle handle = (NEXUS_FrontendDeviceHandle)NEXUS_Docsis_getFronendDevice();
	result = NEXUS_Docsis_GetCMInfo(handle, &cmInfo);  

	CMLOG("CM Get CMTS IP - result : %d", result);
	CMLOG("CM CMTS IP Addres : %d.%d.%d.%d", cmInfo.ecmtsIPAddress[0],
												cmInfo.ecmtsIPAddress[1],
												cmInfo.ecmtsIPAddress[2],
												cmInfo.ecmtsIPAddress[3]);
	if(result == NO_ERROR){
		addr->ip1 = cmInfo.ecmtsIPAddress[0];
		addr->ip2 = cmInfo.ecmtsIPAddress[1];
		addr->ip3 = cmInfo.ecmtsIPAddress[2];
		addr->ip4 = cmInfo.ecmtsIPAddress[3];
	}
#else
    addr->ip1 = 192;
    addr->ip2 = 168;
    addr->ip3 = 200;
    addr->ip4 = 23;
#endif
	CMLOG("CMHostDevice::getCMTSIP");
	return result;
}

/* NotificationThread */
status_t NotificationThread::readyToRun(){
	return 0;
}

bool NotificationThread::threadLoop(){
	CMLOG("current CM State : %d", mState);
	switch(mState){
		case Noti_Operational:{
			if(!cmRegistration){
				cmRegistration = true;
				/* clear local gateway and route if new IP is allocated. */
				CMLOG("Prepare to refresh eth0 interface...");
				if(!isBooting){
					isBooting = true;
					resetEthState(5);
				}else{
					/* It need more time when refresh ip address after CM reset. */
					resetEthState(10);
				}
			}
			CMLOG("Clear old route and reset eth0 interface!!!");
		}
		break;

		case Noti_Reset:{
			CMLOG("Call API to complete for updating CM Bin.");
			completeTonUpdateCMBin();
		}
		break;

		case Noti_Failed:{
		}
		break;

		default:{
		}
		break;
	}

	return true;
}

void  NotificationThread::resetEthState(int sec){
	sleep(sec);
	CMLOG("Start to obtain ip address via dhcp.");
	system("netcfg eth0 dhcp");
	CMLOG("Lay down eth0 interface.");
	sleep(2);
	system("netcfg eth0 down");
	CMLOG("Raise up eth0 interface.");
	sleep(2);
	system("netcfg eth0 up");
}

void NotificationThread::completeTonUpdateCMBin(){
	char cmd[100];
	CMLOG("Current State is Reset...");

	if(property_set("ctl.stop", "tftpd_run") <0){
		CMLOG("tftpd stop is failed...");
	}

	memset(cmd, 0x00, sizeof(cmd));
	sprintf(cmd, "rm /data/%s", CM_BIN);
	CMLOG("Command : %s", cmd);

	int err = system(cmd);
	if(err == NO_ERROR){
		mSWDownload = false;
		cmRegistration = false;
	}
}
void NotificationThread::setNotificationState(int state){
	CMLOG("mState is %d:", state);
	mState = state;	
}

/* CMHostJoinThread */
status_t CMHostJoinThread::readyToRun(){
	return 0;
}

void CMHostJoinThread::initializeNexusContextClient(){
	b_refsw_client_client_configuration config; 
	b_refsw_client_client_info client_info;   

	if(mNexusInit){
		CMLOG("Nexus Client is already initialized...");
		return; 
	}   

	g_ipcClient = NexusIPCClientFactory::getClient("CMHostDevice"); 
    
	if(g_ipcClient){
		BKNI_Memset(&config, 0, sizeof(config));
		BKNI_Snprintf(config.name.string,sizeof(config.name.string),"TVSTORM_CMHOST");
		g_nexusClient = g_ipcClient->createClientContext(&config);
	}   

	if (g_nexusClient == NULL) {
		delete g_ipcClient;
	}else{
		NEXUS_Error rc; 
		mNexusInit = true;

#if NEXUS_FRONTEND_DOCSIS
		NEXUS_DocsisDeviceSettings deviceSettings;
		NEXUS_FrontendDeviceHandle handle = (NEXUS_FrontendDeviceHandle)NEXUS_Docsis_getFronendDevice(); 

		NEXUS_Docsis_GetDeviceSettings(handle, &deviceSettings);
		deviceSettings.stateChange.callback = notificationCallback; 
		deviceSettings.stateChange.context = NULL;
		rc = NEXUS_Docsis_SetDeviceSettings(handle, &deviceSettings);

		BDBG_ASSERT(!rc);
#endif    
	}
}

bool CMHostJoinThread::threadLoop(){
	initializeNexusContextClient();
	return true;
}

/* UpdateCmThread */

void UpdateCMThread::onFirstRef(){
	CMLOG("UpdateCMThread Start!! ");
	mRunning = true;
	mCopied = false;
}

status_t UpdateCMThread::readyToRun(){
	return 0;
}

bool UpdateCMThread::threadLoop(){
	char cmd[100];
	int err;
	CMLOG("Enter in loopling for checking update file.....");
	while(isThreadRunning()){
		if(!mSWDownload && haveCMBin()){
			mSWDownload = true;
			mCopied = true;

			memset(cmd, 0x00, sizeof(cmd));
			CMLOG("Start to copy cm bin.....");

			sprintf(cmd, "cp -f %s /data", CM_BIN_PATH);
			err = system(cmd);
			CMLOG("Copying cm bin is [%d]", err);
			sleep(1);

			memset(cmd, 0x00, sizeof(cmd));
			sprintf(cmd, "rm %s", CM_BIN_PATH);
			err = system(cmd);
			CMLOG("Deleting cm bin in USB is [%d]" , err);

			if(err == NO_ERROR){
				mCopied = false;
				startSWDL();
			}
		}

		sleep(5);
	}

	return true;
}

void UpdateCMThread::setThreadRunning(bool isRunning){
	mRunning = isRunning;
}

bool UpdateCMThread::isThreadRunning(){
	return mRunning;
}

bool UpdateCMThread::haveCMBin(){
	bool result = false;
	CMLOG("Does it have cm bin?");

	/* Don't check that it have cm bin in usb stick if cm bin is copied. */
	if(mCopied){
		return false;
	}

	if(access(CM_BIN_PATH, F_OK) >= 0){
		CMLOG("cm bin is exist!!!");
		result = true;
	}
	CMLOG("The result is %d",result);
	return result;
}

void UpdateCMThread::startSWDL(){
	CMLOG("startSWDL >>> Begin");
	status_t result = NO_ERROR;
	int cnt = 0, max = 3;

	while(cnt <max){
		if(property_set("ctl.start", "tftpd_run") < 0){
			CMLOG("tfptd start : failed...bit try again[max:%d] : %d",max, cnt);
			cnt++;
			sleep(1);
		}else{
			break;
		}
	}

	CMLOG("prepare to Start NEXUS_Docsis_SWDownload !!");
#if NEXUS_FRONTEND_DOCSIS
	NEXUS_FrontendDeviceHandle handle = (NEXUS_FrontendDeviceHandle)NEXUS_Docsis_getFronendDevice();
	result = NEXUS_Docsis_SWDownload(handle);
#endif
	CMLOG("UpdateCMThread::startSWDL result : %d", result);
	if(result != NO_ERROR){
		cnt = 0;
    	while(cnt <max){
	        if(property_set("ctl.stop", "tftpd_run") < 0){ 
	            CMLOG("tfptd start : failed...bit try again[max:%d] : %d",max, cnt);
	            cnt++;
	            sleep(1);
	        }else{
	            break;
	        } 
		}
	}
}


} //namespace android


