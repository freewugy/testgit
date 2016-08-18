#include <CMHostNative.h>
#include <CMHostServiceClient.h>
#include <stdio.h>

using namespace android;

extern "C" {
CmInfoStruct get_cm_info(){
	CMLOGT(); 

	CmInfoStruct info;

	memset(&info, 0x00, sizeof(info));
	CMHostServiceClient *cmHost = new CMHostServiceClient();
	const sp<ICMHostService>& service = cmHost->getService();

	if(service != 0){
		service->getCMInfo(&info);
	}
	else{
		CMLOGE("service is NULL");
	}
	return info;
}

int check_update_file(){
	CMLOGT(); 

	char buf[100];

	memset(buf, 0x00, sizeof(buf));
	sprintf(buf, "/data/%s", CM_BIN);

	if(access(buf, F_OK) < 0)
		return 0;
	else 
		return 1;
}

int request_swdl(){
	CMLOGT(); 

    CMHostServiceClient *cmHost = new CMHostServiceClient();
    const sp<ICMHostService>& service = cmHost->getService();

	if(service != 0){
		return service->requestSWDL();
	}else{
		CMLOGE("service is NULL");
	}	

	return -1;
}

TunerParams get_tuner_params(){
	CMLOGT(); 

	TunerParams param;

	memset(&param, 0x00, sizeof(param));
    CMHostServiceClient *cmHost = new CMHostServiceClient();
    const sp<ICMHostService>& service = cmHost->getService();

	if(service != 0){
		service->getTunerParam(&param);
	}else{
		CMLOGE("service is NULL");
	}

	return param;
}

IPAddress get_cm_ip_addr(){
	CMLOGT(); 

	IPAddress addr;
	memset(&addr, 0x00, sizeof(addr));

    CMHostServiceClient *cmHost = new CMHostServiceClient();
    const sp<ICMHostService>& service = cmHost->getService();

	if(service != 0){
		service->getIPAddr(&addr);
	}else{
		CMLOGE("service is NULL");
	}

	return addr;
}

int set_tune_frequency(uint32_t freq){
	CMLOGT(); 

    CMHostServiceClient *cmHost = new CMHostServiceClient();
    const sp<ICMHostService>& service = cmHost->getService();

	if(service != 0){
		return service->setTuneFreq(freq);	
	}else{
		CMLOGE("service is NULL");
	}

	return -1;
}

 IPAddress get_cmts_ip(){
 	CMLOGT(); 

 	IPAddress addr;

	memset(&addr, 0x00, sizeof(addr));
    CMHostServiceClient *cmHost = new CMHostServiceClient();
    const sp<ICMHostService>& service = cmHost->getService();

	if(service != 0){
		service->getCMTSIP(&addr);
	}else{
		CMLOGE("service is NULL");
	}

	return addr;
}

}
