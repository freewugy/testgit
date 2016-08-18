#include <unistd.h>

#ifndef DLOADER
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <ICMHostService.h>
#endif

#include <cutils/properties.h>
#include <sys/system_properties.h>   
#include <CMHostService.h> 

#include "nexus_platform.h"
#include "nexus_ipc_client_factory.h"
#include "bkni.h"

#include "nexus_frontend.h"
#include "nexus_docsis.h"
#include "nexus_docsis_interface.h"
#include "nexus_docsis_data.h"

#include <netutils/ifc.h>

namespace android {

NexusIPCClientBase *g_ipcClient = NULL; 
NexusClientContext *g_nexusClient = NULL;  
sp<CMMessageHandler> CMHostService::mHandler = new CMMessageHandler(); 
sp<Looper> CMHostService::mLooper = new Looper(true);

BKNI_EventHandle mUpdateEvent = NULL;

const int kCMHostSleep = 500 * 1000; // 0.5s

static void notificationCallback(void *context, int param);

static const char PROPERTY_TVSTORM_ETH_VALID[]="tvstorm.eth.valid";
static const char PROPERTY_TVSTORM_CM_UPDATE_STATE[]="tvstorm.cmusb.state";

static const char SVC_TVSTORM_TFTP[]="init.svc.tftpd_run";

#ifndef DLOADER
void CMHostService::instantiate(){
	defaultServiceManager()->addService(String16(getServiceName()), new CMHostService());
}
#endif

CMHostService::CMHostService()
{
	CMLOGT();
	char buf[PROP_VALUE_MAX];
	int len, count = 0, maxCount = 5 ;

	CMLOGI("Create CMHostService.. Version : %s",VERSION);

	mMonitorThread = new MonitorThread(mLooper, mHandler);
	mMonitorThread->run();

	registerNotificationCallback();	
}

CMHostService::~CMHostService(){
	CMLOGT(); 

	if(g_ipcClient){
		g_ipcClient->disconnectClientResources(g_nexusClient);  
		g_ipcClient->destroyClientContext(g_nexusClient); 
		g_nexusClient = NULL;
		delete g_ipcClient;
		g_ipcClient = NULL;
	}

	if(mMonitorThread){
		mMonitorThread->setThreadRunning(false);

		delete mMonitorThread;
		mMonitorThread = NULL;
	}
}

static void notificationCallback(void *context, int param){
	CMLOGT(); 

	BSTD_UNUSED(param);

#if NEXUS_FRONTEND_DOCSIS
	NEXUS_FrontendDeviceHandle handle = (NEXUS_FrontendDeviceHandle)NEXUS_Docsis_getFronendDevice(); 
	NEXUS_DocsisDeviceStatus status;
	NEXUS_Docsis_GetDeviceStatus(handle, &status);	

	CMLOGD(" Current Device state : %d", (int)status.state);
	switch(status.state){
		case NEXUS_DocsisDeviceState_eOperational:{
			CMLOGI("Current State is Operational...");
			CMHostService::sendStaticMessage(CM_STATE_OPERATIONAL);
		};
		break;

		case NEXUS_DocsisDeviceState_eCMRegistration:{
			CMLOGI("Current State is CM Registration completed");
			CMHostService::sendStaticMessage(CM_COMPLETE_REGISTRATION);
		}
		break;

		case NEXUS_DocsisDeviceState_eReset:{
			CMLOGI("Current State is Reset...");
			CMHostService::sendStaticMessage(CM_STATE_RESET);
		}
		break;

		case NEXUS_DocsisDeviceState_eFailed:{
			CMLOGI("Current State is Failed...");
		}
		break;

		case NEXUS_DocsisDeviceState_eUpdateFailed:{
			CMLOGI("Current State is that updating CM bin is failed.");
			CMHostService::sendStaticMessage(CM_STATE_UPDATE_FAILED);
		}
		break;

		case NEXUS_DocsisDeviceState_eUninitialized:
		default:{
			CMLOGI("Current State is uninitiailized...");
		}
		break;
	}
#endif
}

void CMHostService::registerNotificationCallback(){
	CMLOGD("Registration Notification Callback...");
	sendMessage(CM_REGISTER_NOTIFICATION);
}

void CMHostService::sendStaticMessage(const int message){
	if(mHandler.get() != 0){
		if(mLooper.get() != 0){
			mLooper->sendMessage(mHandler, message);
		}
	}
}

void CMHostService::sendMessage(const int message){
	if(mHandler.get() == 0){
		CMLOGE(" CM Message Handler is NULL");
		return;
	}
	
	sendMessage(mHandler, Message(message));
}

void CMHostService::sendMessage(const sp<MessageHandler>& handler, const Message& message){
	if(mLooper.get() == 0){
		CMLOGE("CM looper is NULL");
		return;
	}
	
	mLooper->sendMessage(handler, message);
}

int CMHostService::convertChtoDec(int ch){
	if(ch >= '0' && ch <= '9'){
		return ch - '0';
	}else if(ch >= 'a' && ch <= 'f'){
		return ch - 'a' + 10;
	}else if(ch >= 'A' && ch <= 'F'){
		return ch - 'A' + 10;
	}

	return -1;
}

status_t CMHostService::getCMMacAddress(CmInfoStruct *info){
	FILE *in;
    int ch[2],i=0;
	status_t result = -1;

    if((in = fopen("factory_settings/CM_MAC_ADDR", "rb")) != NULL){
    	fseek(in, 0, SEEK_SET);

        while((ch[0] = fgetc(in)) != EOF){
	        if(ch[0] != ':'){
   		    	ch[1] = fgetc(in);
				ch[0] = convertChtoDec(ch[0]);
				ch[1] = convertChtoDec(ch[1]);
                info->ulCmMac[i] =  ch[0]*16 + ch[1];
                CMLOGI("MAC[%d] = %x[%d] by CM_MAC_ADDR", i, info->ulCmMac[i], info->ulCmMac[i]);
                i++;
			}				

			if(i > 5){
				result = NO_ERROR;
			    break;
			}
		}
		fclose(in);
	}

	if(result == -1)
		CMLOGD("getCMMacAddress - Failed to get CM MAC because there is no CM_MAC_ADDR file...");

	return result;
}

status_t CMHostService::getCMInfo(CmInfoStruct *info){
	CMLOGT(); 

	status_t result = NO_ERROR;
	int i=0;
	
#if NEXUS_FRONTEND_DOCSIS 
	NEXUS_DocsisCMInfo cmInfo;
	BKNI_Memset(&cmInfo, 0x00, sizeof(cmInfo));

	NEXUS_FrontendDeviceHandle handle = (NEXUS_FrontendDeviceHandle)NEXUS_Docsis_getFronendDevice();
	result = NEXUS_Docsis_GetCMInfo(handle, &cmInfo);

	CMLOGD("Get CM Info result - %d", result);
	CMLOGD("CM Version	: %u.%u.%u", cmInfo.cmVersion[1], cmInfo.cmVersion[2], cmInfo.cmVersion[3]);
	CMLOGD("CM MAC    	: %x:%x:%x:%x:%x:%x", cmInfo.cmMac[0], cmInfo.cmMac[1], cmInfo.cmMac[2],cmInfo.cmMac[3],
										cmInfo.cmMac[4], cmInfo.cmMac[5]);
	CMLOGD("CM Status  	: %d", cmInfo.cmStatus);
	
	CMLOGD("CM IP 		: %d.%d.%d.%d", cmInfo.ecmIPAddress[0],
										cmInfo.ecmIPAddress[1],
										cmInfo.ecmIPAddress[2],
										cmInfo.ecmIPAddress[3]);
	CMLOGD("CMTS IP		: %d.%d.%d.%d", cmInfo.ecmtsIPAddress[0],
										cmInfo.ecmtsIPAddress[1],
										cmInfo.ecmtsIPAddress[2],
										cmInfo.ecmtsIPAddress[3]);
	CMLOGD("CM DS Freq	: %d", cmInfo.DSRfFreq);
	CMLOGD("CM US Freq	: %d", cmInfo.USRfFreq);
	CMLOGD("CM QAMType	: %d", cmInfo.QAMType);
	CMLOGD("CM Annex		: %d", cmInfo.Annex);
	CMLOGD("CM MSE_SNR	: %d", cmInfo.MSE_SNR);
	CMLOGD("CM DS Power	: %d", cmInfo.DSPower);
	CMLOGD("CM US Power	: %d", cmInfo.USPower);

	if(result == NO_ERROR){
		 for(i=0;i<6;i++)
		 	info->ulCmMac[i] = cmInfo.cmMac[i];
	}else{
		result = getCMMacAddress(info);
	}

	for(i=0;i<4;i++)
		info->cmVersion[i] = cmInfo.cmVersion[i];
	
	info->cmStatus = cmInfo.cmStatus;
		
#else
	int cmVersion[]={0,5,0,0,1};
	int mac[]={0x10, 0x20,0x30,0xde,0xad,0x10};

	for(i=0;i<4;i++)
		info->cmVersion[i] = cmVersion[i];
	for(i=0;i<6;i++)
		info->ulCmMac[i] = mac[i];

    info->cmStatus = 1;
    CMLOGD("CMHost cmVersion for TEST:%s", info->cmVersion);
#endif

	return result;
}

status_t CMHostService::requestSWDL(){
	CMLOGT(); 

	status_t result = NO_ERROR;
	
	CMLOGD(" *^^*^^*^^*^^*^^*^^*^^*^^*^^*^^*^^*^^*^^*^^*^^*^^*^^*^^*^^*^^*^^*^^*^^*^^");
	CMLOGD(" *^^*^^*^^*^^*^^*^^* CMHostService::requestSWDL *^^*^^*^^*^^*^^*^^*^^*");
	CMLOGD(" *^^*^^*^^*^^*^^*^^*^^*^^*^^*^^*^^*^^*^^*^^*^^*^^*^^*^^*^^*^^*^^*^^*^^*^^");

	mMonitorThread->initUpdateEvent();

	sendMessage(CM_START_SWDL);

	if(mMonitorThread->waitUpdateEvent())
		result = NO_ERROR;
	else
		result = -1;

	mMonitorThread->destroyUpdateEvent();	
	CMLOGD("*^^*^^*^^* Complete.............................................result : %d", result);
	return result;
}

status_t CMHostService::getTunerParam(TunerParams *param){
	CMLOGT(); 

	status_t result = NO_ERROR;
#if NEXUS_FRONTEND_DOCSIS 
	NEXUS_DocsisCMInfo cmInfo;
	BKNI_Memset(&cmInfo, 0x00, sizeof(cmInfo));
	NEXUS_FrontendDeviceHandle handle = (NEXUS_FrontendDeviceHandle)NEXUS_Docsis_getFronendDevice();
	result = NEXUS_Docsis_GetCMInfo(handle, &cmInfo);

	CMLOGD("CM get param - result : %d", result);
	CMLOGD("CM DSRFfrequency : %d", cmInfo.DSRfFreq);
	CMLOGD("CM USRFfrequency : %d", cmInfo.USRfFreq);
	CMLOGD("CM QAMType 		: %d", cmInfo.QAMType);
	CMLOGD("CM Annex 		: %d", cmInfo.Annex);
	CMLOGD("CM MSE_SNR		: %d", (int)cmInfo.MSE_SNR);
	CMLOGD("CM DSPower		: %d", (short)cmInfo.DSPower);
	CMLOGD("CM USPower		: %d", (short)cmInfo.USPower);
	
	if(result == NO_ERROR){
		param->Flag = 1;
		param->DSRFfrequency = (float)cmInfo.DSRfFreq;
		param->USRFfrequency = (float)cmInfo.USRfFreq;
		param->QAMType = (unsigned int)cmInfo.QAMType;
		param->Annex = cmInfo.Annex;
		param->MSE_SNR = cmInfo.MSE_SNR;
		param->DSPower = (short)cmInfo.DSPower;
		param->BER = -1; // It doesn't be used.
		param->USPower = (short)cmInfo.USPower;
	}
#else
    CMLOGD("CMHostService::getTunerParam");
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

status_t CMHostService::getIPAddr(IPAddress *addr){
	CMLOGT(); 

	status_t result = NO_ERROR;
#if NEXUS_FRONTEND_DOCSIS
	NEXUS_DocsisCMInfo cmInfo;
	BKNI_Memset(&cmInfo, 0x00, sizeof(cmInfo));
	NEXUS_FrontendDeviceHandle handle = (NEXUS_FrontendDeviceHandle)NEXUS_Docsis_getFronendDevice();
	result = NEXUS_Docsis_GetCMInfo(handle, &cmInfo);   

	CMLOGD("CM get IP - result : %d", result);
	CMLOGD("CM IP Address : %d.%d.%d.%d", cmInfo.ecmIPAddress[0],
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
    CMLOGD("CMHostService::getIPAddr ip1 : %d, ip2 : %d, ip3: %d, ip4: %d",
        addr->ip1, addr->ip2, addr->ip3, addr->ip4);
#endif

	return result;
}

status_t CMHostService::setTuneFreq(uint32_t freq){
	CMLOGT(); 

	status_t result = NO_ERROR;
	freq *= 10 * 10000;
	CMLOGD("Freq to set :%d", freq);

#if NEXUS_FRONTEND_DOCSIS
	NEXUS_FrontendDeviceHandle handle = (NEXUS_FrontendDeviceHandle)NEXUS_Docsis_getFronendDevice();
	result = NEXUS_Docsis_SetFreq(handle, freq);
#endif	
	CMLOGD("CMHostService::setTuneFreq result : %d", result);
	return result;
}

status_t CMHostService::getCMTSIP(IPAddress *addr){
	CMLOGT(); 

	status_t result = NO_ERROR;
#if NEXUS_FRONTEND_DOCSIS
	NEXUS_DocsisCMInfo cmInfo;
	BKNI_Memset(&cmInfo, 0x00, sizeof(cmInfo));
	NEXUS_FrontendDeviceHandle handle = (NEXUS_FrontendDeviceHandle)NEXUS_Docsis_getFronendDevice();
	result = NEXUS_Docsis_GetCMInfo(handle, &cmInfo);  

	CMLOGD("CM Get CMTS IP - result : %d", result);
	CMLOGD("CM CMTS IP Addres : %d.%d.%d.%d", cmInfo.ecmtsIPAddress[0],
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
	return result;
}

#ifndef DLOADER
status_t CMHostService::onTransact(uint32_t code, const Parcel &data, Parcel *reply, uint32_t flags){
	return BnCMHostService::onTransact(code, data, reply, flags);
}
#endif

/* Monitoring Thread */

MonitorThread::MonitorThread(const sp<Looper>& looper, const sp<CMMessageHandler>& handler){
	mLooper = looper;
	mHandler = handler;
}

MonitorThread::~MonitorThread(){
}

status_t MonitorThread::readyToRun(){
	mRunning = true;
	updateMode = NONE_UPDATE;
	mCMInitState = kCMUinit;
	mInvalidCount = 0;
	mRegistrationCount = 0;
	mUpdateCount = 0;

	return 0;
}

bool MonitorThread::threadLoop(){
	int result, i, size;
	CMLOGD("Start treadloop of Monitor Thread!!");
	while(isRunning()){
#ifdef DLOADER
		/* wait until event is received. */
		result = waitOnEvent();
#else
		/* wait until event is received or timer is expired. */
		result = waitOnEvent(10*1000); //10sec
#endif
		if(result == Looper::POLL_CALLBACK){
			CMLOGI("Looper type is POLL_CALLBACK");
			size = getEventMessageSize();
			CMLOGI(" Message size : %d", size);
			for(i = 0;i<size ; i++){
				switch(getEventMessage()){
					case CM_REGISTER_NOTIFICATION:{
						CMLOGI("CM_REGISTER_NOTIFICATION message is received.");
						initializeNexusContextClient();
					}
					break;
				
					case CM_START_SWDL:{
						CMLOGI("CM_START_SWDL message is received.");
						setUpdateMode(ONLINE_UPDATE);
						startSWDL();
					}
					break;

					case CM_STATE_OPERATIONAL:{
						CMLOGI("CM_STATE_OPERATIONAL message is received.");
					}
					break;

					case CM_COMPLETE_REGISTRATION:{
						CMLOGI("CM_COMPLETE_REGISTRATION message is received.");
						allocateIPAddress();
					}
					break;
	
					case CM_STATE_RESET:{
						CMLOGI("CM_STATE_RESET message is received.");
						completeOnUpdateCMBin();
					}
					break;

					case CM_STATE_FAILED:{
					}
					break;

					case CM_STATE_UPDATE_FAILED:{
						CMLOGI("CM_STATE_UPDATE_FAILED message is recevied.");
						cancelToUpdateCMBin();
					}
					break;

					case CM_STATE_UNINIT:{
					}
					break;
				}
			}
		}else if(result == Looper::POLL_TIMEOUT){

			CMLOGI("Looper type is POLL_TIMEOUT");
			if(getCMInitState() < kCMAddRoute){
				addCMRoute();
			}
			/* Is there available CM Bin to update? */
			prepareToUpdateCMBin();

			/* Refresh ethernet state if ethernet state is not vaild. */
			checkAndRefreshEthState();
		}
	}

	return true;
}

int MonitorThread::waitOnEvent(){
    return waitOnEvent(-1); 
}                                                                                   
int MonitorThread::waitOnEvent(int timeoutMillis){
    if(mLooper.get() == 0){ 
	    CMLOGE("CM looper is NULL"); 
        return -1; 
    } 

    return mLooper->pollOnce(timeoutMillis); 
}      

void MonitorThread::setThreadRunning(bool isRunning){
	mRunning = isRunning;
}

bool MonitorThread::isRunning(){
	return mRunning;
}

int MonitorThread::getEventMessage(){
	int cmMessage = CM_EVENT_UNKNOWN;
	CMLOGI("Message size of handler : %d", mHandler->getMessageSize());

	if(mHandler.get() != 0){
		if(mHandler->getMessageSize() != 0){
			cmMessage = mHandler->getCurrentMessage();
			mHandler->popMessage();
		}
	}
	return cmMessage;
}

int MonitorThread::getEventMessageSize(){
	if(mHandler.get() != 0)	
		return mHandler->getMessageSize();
	else
		return 0;
}

void MonitorThread::allocateIPAddress(){
	CMLOGT(); 

    CMLOGD("Allocate IP Address for eth0 interface!!!");
	resetEthState();
	setCMInitState(kCMRegistration);	
}

void MonitorThread::resetEthState(bool stateChange){
	CMLOGT(); 

	int result;
    CMLOGD("stateChange : %d", stateChange); 

	result = ifc_disable("eth0"); 
	CMLOGI("Lay down eth0 interface.[result : %d]", result); 
    usleep(kCMHostSleep);
	result = ifc_enable("eth0");
	CMLOGI("Raise up eth0 interface.[result : %d]", result);

	/* Route in CM routing table is disappear after down eth0, so we need to add route to CM routing table after up again. */
	addCMRoute();
	if(stateChange)
		setCMInitState(kCMAddRoute);
}

void MonitorThread::completeOnUpdateCMBin(){
	CMLOGT(); 

	char cmd[100];
    
	if(property_set("ctl.stop", "tftpd_run") <0){
		CMLOGE("tftpd stop is failed...");
	}

    if(property_set(PROPERTY_TVSTORM_CM_UPDATE_STATE, "2") <0){
		CMLOGE("Led off is failed...");
	} 

	memset(cmd, 0x00, sizeof(cmd));
	sprintf(cmd, "rm /data/%s", CM_BIN);
	CMLOGD("Command : %s", cmd);

	system(cmd);

	/* Change from update to opertional and enter not registration state. */
	setUpdateMode(NONE_UPDATE);
	resetEthState();

	/* Set true as update event to notify the success of updating CM bin via online. */ 
	setUpdateEvent(true);
}

bool MonitorThread::prepareToUpdateCMBin(){
	CMLOGT(); 

	char cmd[100];
	int err;

	CMLOGI("Update State : %d", isUpdate());

	if((isUpdate() == NONE_UPDATE) && haveCMBin()){
		/* prepare to start updating new CM bin */
		setUpdateMode(USB_UPDATE);

		memset(cmd, 0x00, sizeof(cmd));
		CMLOGD("Start to copy cm bin.....");

		sprintf(cmd, "cp -f %s /data", CM_BIN_PATH);
		err = system(cmd);
		CMLOGD("Copying cm bin is [%d]", err);
		usleep(kCMHostSleep);

		startSWDL();
		CMLOGD("Start To update CM bin.");
		return true;
	}
	CMLOGD("There is no CM bin file to update or it is updating state.");
	return false;
}

void MonitorThread::checkAndRefreshEthState(){
	CMLOGT(); 

	CMLOGD("Update State : %d", isUpdate());

	if(isUpdate() == NONE_UPDATE){
		char value[PROPERTY_VALUE_MAX] = {'\0'};
		mUpdateCount = 0;

		property_get(PROPERTY_TVSTORM_ETH_VALID, value, "0");

		CMLOGD("Current ETH state : %s", value);

		if(!strcmp(value, "0")){
			mInvalidCount++;
		}else{
			mInvalidCount = 0;
		}

		mRegistrationCount++;
		CMLOGI("getCMInitState : %d[wan error count : %d / Waiting count for checking CM registration : %d]", getCMInitState(), mInvalidCount, mRegistrationCount);

		/* WAN check is failed 3 times(30 sec) in row although IP address is vaild. This can occur when Cable is removed after registering CMTS. */
		if((mInvalidCount > 2) && isValidEthernetIP()){
			mInvalidCount = 0;	
			resetEthState();
		}

		/* If eth0 has not valid IP address despite CM registration is already completed. */
		if((getCMInitState() == kCMRegistration) && !isValidEthernetIP()){
            mInvalidCount = 0;
	        resetEthState();
			setCMInitState(kCMRegistration);
	    }

		/*  check cm status if current CMInitState is not registration state every 1 min. */
		if(mRegistrationCount > 5){
			status_t result = NO_ERROR;
			mRegistrationCount = 0;

			if(getCMInitState() != kCMRegistration){
#if NEXUS_FRONTEND_DOCSIS 
	    		NEXUS_DocsisCMInfo cmInfo;
		    	BKNI_Memset(&cmInfo, 0x00, sizeof(cmInfo));

			    NEXUS_FrontendDeviceHandle handle = (NEXUS_FrontendDeviceHandle)NEXUS_Docsis_getFronendDevice();
			    result = NEXUS_Docsis_GetCMInfo(handle, &cmInfo);

				if(result == NO_ERROR){
					CMLOGI("The real state of CM : %d[kOperational : 12",cmInfo.cmStatus);
					if(cmInfo.cmStatus == kOperational){
						resetEthState();
						setCMInitState(kCMRegistration);
					}
				}else{
					CMLOGI("eth interface state is invaild. So reset eth0 state.");
					resetEthState(); 
					setCMInitState(kCMRegistration); 
				}
#endif
			}
		}
	}else{
		/* Cancel update if updating CM bin is not complete in 2 min. */
		if(mUpdateCount > 11){
			mUpdateCount = 0;
			cancelToUpdateCMBin();	
		}else{
			mUpdateCount++;
		}
	}
}

bool MonitorThread::isValidEthernetIP(){
	CMLOGT(); 

	if(ifc_init() == 0){
		unsigned addr = 0;

		ifc_get_addr("eth0", &addr);
		ifc_close();

		if( addr <= 0){
			CMLOGD("isValidEthernetIP - eth0 ip address is invalid...");
			return false;
		}
	}
	CMLOGD("isValidEthernetIP - eth0 ip address is valid...");
	return true;
}

bool MonitorThread::haveCMBin(){
	bool result = false;

	if(access(CM_BIN_PATH, F_OK) >= 0){
		CMLOGI("cm bin is exist!!!");
		result = true;
	}
	return result;
}

void MonitorThread::startSWDL(){
	CMLOGT(); 

	status_t result = NO_ERROR;
	int cnt = 0, max = 3, isCMBinver;

	isCMBinver = checkCMBinVer();

	CMLOGD("result of checking CM bin version : %d", isCMBinver);

	if(isCMBinver == cmBin_NoError){
		while(cnt < max){
			if(property_set("ctl.start", "tftpd_run") < 0){
				CMLOGE("tfptd start : failed...retry again[max:%d] : %d",max, cnt);
				cnt++;
				usleep(kCMHostSleep);
			}else{
				break;
			}
		}

		CMLOGD("prepare to Start NEXUS_Docsis_SWDownload !!");
#if NEXUS_FRONTEND_DOCSIS
		NEXUS_FrontendDeviceHandle handle = (NEXUS_FrontendDeviceHandle)NEXUS_Docsis_getFronendDevice();
		result = NEXUS_Docsis_SWDownload(handle);
#endif
		CMLOGD("UpdateCMThread::startSWDL result : %d", result);

		if(result == NO_ERROR){
			if(property_set(PROPERTY_TVSTORM_CM_UPDATE_STATE,"1")<0){
				CMLOGE("Led Green blicking is failed...");
			}
		}else{
			/* Cancel CM bin update mode */
			cancelToUpdateCMBin();
		}
	}else if(isCMBinver == cmBin_NoAvailableVersion){
		/* No led red blinking because led green is blinking one time when CM bin version is not available version. */
		cancelToUpdateCMBin(false);
	}else{
		/* Led red blinking when CM bin file open error */
		cancelToUpdateCMBin();
	}
}

int MonitorThread::checkCMBinVer(){
	CMLOGT(); 

	FILE *in;
	int ch,i=0;
	int version[3];
	CmInfoStruct info;
	status_t result = NO_ERROR;

	if((in = fopen("data/ecram_sto.bin", "rb")) != NULL){
		fseek(in, 0, SEEK_SET);
		fseek(in, 5, SEEK_CUR);

		while(i<3){
			if((ch = fgetc(in)) != EOF){
				version[i] = ch;
			}
			i++;
		}
	
		fclose(in);

		/* Get CM info */
#if NEXUS_FRONTEND_DOCSIS 
		NEXUS_DocsisCMInfo cmInfo;
		BKNI_Memset(&cmInfo, 0x00, sizeof(cmInfo));
		NEXUS_FrontendDeviceHandle handle = (NEXUS_FrontendDeviceHandle)NEXUS_Docsis_getFronendDevice();
		result = NEXUS_Docsis_GetCMInfo(handle, &cmInfo);

		/* Compare version */
		i = 0;
	
		while(i<3){
			CMLOGD("new CM bin version[%d] : %d", i, version[i]);
			CMLOGD("current CM bin version[%d] : %d", i+1, cmInfo.cmVersion[i+1]);
		
			/* Dicards the first number of CM version because it's a CM model number not binary version. */
			/* Only allow upgrade in case of online upgrade but allow both upgrade and downgrade in case of usb. */
			if(isUpdate() == USB_UPDATE){
				if(version[i] != cmInfo.cmVersion[i+1]){
					CMLOGI("New CM Bin is not same as current in USB update.");
					return cmBin_NoError;
				}
			//	i++;
			}else if(isUpdate() == ONLINE_UPDATE){
				if(version[i] > cmInfo.cmVersion[i+1]){
					CMLOGI("New CM bin is higher than current in Online update.");
					return cmBin_NoError;
				}
			//	i++;
			}
			i++;
		}

		CMLOGI("New CM Bin's version is lower or equal than current. So cancel it.");
		/* Led green blicking twice when cm version is the same */
		if(property_set(PROPERTY_TVSTORM_CM_UPDATE_STATE, "3")<0){
			CMLOGE("Led green blicking is failed...");
		}
#endif
		return cmBin_NoAvailableVersion;
	}

	CMLOGE("CM Bin File Open Error");
	return cmBin_FileOpenErrror;
}

void MonitorThread::initializeNexusContextClient(){
	CMLOGT(); 

	b_refsw_client_client_configuration config; 
	b_refsw_client_client_info client_info;   

	if(getCMInitState() > kCMUinit){
		CMLOGI("Nexus Client is already initialized...");
		return; 
	}   

	g_ipcClient = NexusIPCClientFactory::getClient("CMHostService"); 
    
	if(g_ipcClient){
		BKNI_Memset(&config, 0, sizeof(config));
		BKNI_Snprintf(config.name.string,sizeof(config.name.string),"TVSTORM_CMHOST");
		g_nexusClient = g_ipcClient->createClientContext(&config);
	}   

	if (g_nexusClient == NULL) {
		delete g_ipcClient;
		g_ipcClient = NULL;
	}else{
		NEXUS_Error rc; 
		setCMInitState(kCMNexusInit);

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

void MonitorThread::addCMRoute(){
	CMLOGT(); 

	if(ifc_init() == 0){
		unsigned flags = 0;
		ifc_get_info("eth0", NULL, NULL, &flags);

		CMLOGD("eth0 state : %d", flags);
		if(flags & 1){
			CMLOGI("ip route add table cm_network");

			int err = system("ip route add table cm_network from all 192.168.17.0/24 dev eth0");
			if(err == 0){
				CMLOGI("Adding CM internal address to CM routing table is success...");
				setCMInitState(kCMAddRoute);
			}
		}
		ifc_close();
	}
}

int MonitorThread::getCMInitState(){
	return mCMInitState;
}

void MonitorThread::setCMInitState(int state){
	mCMInitState = state;
}

int MonitorThread::isUpdate(){
	return updateMode;
}

void MonitorThread::setUpdateMode(int state){
	updateMode = state;
}

void MonitorThread::cancelToUpdateCMBin(bool led){
	CMLOGT(); 

	int cnt = 0, max = 3;
	char tftp_status[PROPERTY_VALUE_MAX] = {'\0'};
	
	/* Stop tftpd_run */
	if(property_get(SVC_TVSTORM_TFTP, tftp_status, NULL) && strcmp(tftp_status, "running") == 0){
		CMLOGD("Stop tftpd_run because tftpd_run is running!");
		while(cnt < max){
			if(property_set("ctl.stop", "tftpd_run") <0){
				CMLOGE("tfptd stop : failed...retry again[max:%d] : %d",max,cnt);
				cnt++;
				usleep(kCMHostSleep);
			}else{
				break;
			}
		}
	}

	if(led){
		/* Led red on twice when cm update is failed. */
		if(property_set(PROPERTY_TVSTORM_CM_UPDATE_STATE, "4")<0){
			CMLOGE("Led Red blicking is failed...");
		}
	}

	reInitToNoUpdateMode();
}

void MonitorThread::reInitToNoUpdateMode(){
    CMLOGT();

    char cmd[100];

    memset(cmd, 0x00, sizeof(cmd));
    sprintf(cmd, "rm /data/%s", CM_BIN);
    CMLOGD("Command : %s", cmd);

    system(cmd);

    /* change download state from updating to operational */
    setUpdateMode(NONE_UPDATE);

    /* Set false as update event to notify the fail of updating CM bin via online. */
    setUpdateEvent(false);
}

void MonitorThread::initUpdateEvent(){
	CMLOGT(); 

	mOnlineUpdate = false;
	if(mUpdateEvent)
		BKNI_DestroyEvent(mUpdateEvent);

	BKNI_CreateEvent(&mUpdateEvent);
	BKNI_ResetEvent(mUpdateEvent);
}

void MonitorThread::setUpdateEvent(bool success){
	CMLOGT(); 

	if(mUpdateEvent){
		mOnlineUpdate = success;
		CMLOGD("BKNI_SetEvent for mUpdateEvent - (%d)", mOnlineUpdate);

		/* guard time(5sec) to reset CM after updating CM bin */
		if(mOnlineUpdate)
			usleep(1000*1000*5);

		BKNI_SetEvent(mUpdateEvent);
	}
}

void MonitorThread::destroyUpdateEvent(){
	CMLOGT(); 

	if(mUpdateEvent){
		CMLOGD("BKNI_DestroyEvent for mUpdateEvent");
		BKNI_DestroyEvent(mUpdateEvent);
		mUpdateEvent = NULL;
	}
}

bool MonitorThread::waitUpdateEvent(){	
	CMLOGT(); 

	if(mUpdateEvent){
		CMLOGD("BKNI_WaitForEvent for mUpdateEvent in 2000ms.");
		BKNI_WaitForEvent(mUpdateEvent, 2000*60);
	}
	return mOnlineUpdate;
}

} //namespace android


