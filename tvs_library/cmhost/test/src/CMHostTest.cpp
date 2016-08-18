#include <utils/Log.h>

#include <CMHostService.h>
#include <CMHostServiceClient.h>
#include <ICMHostTest.h>
#include <CMHostTest.h>

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

using namespace android;

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	const char *key;
	ICMHostTest::COMMAND cmd;
}CMDGroup;

static const char VER[] = "0.4";

CMDGroup cmdGroup[] ={
	{"quit", ICMHostTest::CMD_QUIT},
	{"help", ICMHostTest::CMD_HELP},
	{"getinfo", ICMHostTest::GET_INFO},
	{"swdl", ICMHostTest::SWDL_REQUEST},
	{"getparam", ICMHostTest::GET_PARAM},
	{"getaddr", ICMHostTest::GET_IPADDR},
	{"settune", ICMHostTest::SET_TUNE},
	{"gettsip", ICMHostTest::GET_TSIP},
	{"ver",ICMHostTest::TESTAPP_VER}, 
	{"unknown", ICMHostTest::CMD_UNKNOWN}
};

CMHostTest::CMHostTest()
: mQuit(false) {
	int i =0;

	while(cmdGroup[i].cmd != ICMHostTest::CMD_UNKNOWN){
		AddCMDList(cmdGroup[i].key, cmdGroup[i].cmd);
		i++;
	}
	loadAPI();
}

CMHostTest::~CMHostTest(){
	mCMDList.clear();
}

void CMHostTest::initHandle(){
	handle = NULL;
	getcminfo_func = NULL;
	checkupdatefile_func = NULL;
	requestswdl_func = NULL;
	gettunerparams_func = NULL;
	getcmipaddr_func = NULL;
	settunefrequency_func = NULL;
	getcmtsip_func = NULL;
}

void CMHostTest::loadAPI(){
	CMLOGD("CMHost - Load API!!!");
	char *error;
	handle = dlopen("libtvs_cmhostjni.so", RTLD_LAZY);
	
	if(handle != NULL){
		CMLOGD("CMHost - Set Callback Func!!!!!!!!!!!!!!!!!!!!!");
		dlerror();
		getcminfo_func = (getCMInfoCallBack)dlsym(handle, "get_cm_info");
		if((error = (char*)dlerror()) != NULL){
			CMLOGE("CMHost Error : the Symbol was not found.");
			printf("Error : the Symbol was not found.\n");
		}else{
			CMLOGE("CMHosy OK, the Symbol was found");
			printf("OK, the Symbol was found.\n");
		}
		checkupdatefile_func = (checkUpdateFileCallBack)dlsym(handle, "check_update_file");
		requestswdl_func = (requestSWDLCallBack)dlsym(handle, "request_swdl");
		gettunerparams_func = (getTunerParamCallBack)dlsym(handle, "get_tuner_params");
		getcmipaddr_func = (getIPAddrCallBack)dlsym(handle, "get_cm_ip_addr" );
		settunefrequency_func = (setTuneFreqCallBack)dlsym(handle, "set_tune_frequency");
		getcmtsip_func = (getCMTSIPCallBack)dlsym(handle, "get_cmts_ip" );
	}
}

void CMHostTest::PrintMenu(){
	printf("\n\n");
	printf("===========================================================\n");
	printf("				TVSTORM CM Host Test 					   \n");
	printf("===========================================================\n");
	printf("  Command : 	Information\n");
	printf("  quit    :		quit test program.\n");
	printf("  help    :     display command information.\n");
	printf("===========================================================\n\n");
	helpMessage();
}

bool CMHostTest::AddCMDList(const char *key, COMMAND cmd){
	map<string, COMMAND>::iterator it;
	int len = strlen(key)-1;
	CMLOGD("CMHost - key : %s, cmd : %d\n", key, cmd);
	for(it = mCMDList.begin(); it != mCMDList.end(); it++){
		if(strncmp((*it).first.c_str(), key, len) == 0)
			break;
	}

	if(it == mCMDList.end()){
		CMLOGD("CMHost- Insert key : %s\n", key);
		mCMDList.insert(pair<string, COMMAND>(key, cmd));
		return true;
	}

	return false;
}

void CMHostTest::RunCMD(const char *key){
	COMMAND cmd = ExtractCMD(key);
	CMLOGD("CMHost RunCMD - key : %s, cmd : %d\n", key, cmd);
	switch(cmd){
		case CMD_QUIT:{
			mQuit = true;
		}
		break;

		case CMD_HELP:{
			helpMessage();
		}
		break;

		case GET_INFO:{
			getInfo();
		}
		break;

		case SWDL_REQUEST:{
			requestSWDL();
		}
		break;

		case GET_PARAM:{
			getTunerParam();
		}
		break;

		case GET_IPADDR:{
			getIPAddr();
		}

		break;

		case SET_TUNE:{
			setTuneFreq();
		}
		break;

		case GET_TSIP:{
			getCMTSIP();
		}
		break;

		case TESTAPP_VER:{
			showTestAppVer();	
		}
		break;

		default:
		break;
	}


}

ICMHostTest::COMMAND CMHostTest::ExtractCMD(const char *key){
	map<string, COMMAND>::iterator it;
	COMMAND cmd = CMD_UNKNOWN;
	int len = strlen(key)-1;

	for(it = mCMDList.begin(); it != mCMDList.end();it++){
		if(strncmp((*it).first.c_str(), key, len) == 0){
			cmd = (*it).second;
			break;
		}
	}

	return cmd;
}


void CMHostTest::helpMessage(){
	printf("\n\n");
	printf("===================================================\n");
	printf(" quit 		: Quit test program.			\n");
	printf(" help 		: Show help message.			\n");
	printf(" getinfo 	: Get CM info.					\n");
	printf(" swdl 		: Requset software download.	\n");
	printf(" getparam 	: Get Tuner parameters.			\n");
	printf(" getaddr	: Get IP address.				\n");
	printf(" settune	: Set Tune.						\n");
	printf(" gettsip	: Get TS IP address.			\n");
	printf(" ver		: Print test app ver.			\n");
	printf("===================================================\n");
}

bool CMHostTest::isQuit(){
	return mQuit;
}

void CMHostTest::getInfo(){
	CmInfoStruct info;

	if(getcminfo_func!= NULL){
		printf("Call getCMInfo\n");
		info = getcminfo_func();
	}

	printf("\n===================================\n");
	printf(" Version	: %x.%x.%x.%x\n", info.cmVersion[0],info.cmVersion[1], info.cmVersion[2], info.cmVersion[3]);
	printf(" MAC		: %x:%x:%x:%x:%x:%x\n", info.ulCmMac[0], info.ulCmMac[1],info.ulCmMac[2], info.ulCmMac[3], info.ulCmMac[4], info.ulCmMac[5]);
	printf(" Status		: %d\n", info.cmStatus);
	printf("===================================\n\n"); 
}

void CMHostTest::requestSWDL(){
	if(requestswdl_func != NULL){
		status_t status = requestswdl_func();
		printf("Call requestswdl\n");
		CMLOGD("CMHost - status[%d]\n", status);
		printf("\n===================================\n");
		printf(" result : %d\n", status);
		printf("===================================\n\n");
	}
}

void CMHostTest::getTunerParam(){
	TunerParams param;

	if(gettunerparams_func != NULL){
		param = gettunerparams_func();

		printf("\n===================================\n");
		printf(" Flag		: %c\n", param.Flag);
		printf(" DSR Freq	: %f\n", param.DSRFfrequency);
		printf(" USR Freq	: %f\n", param.USRFfrequency);
		printf(" QAMType	: %zu\n", param.QAMType);
		printf(" Annex		: %d\n", param.Annex);
		printf(" MSE_ANR	: %f\n", param.MSE_SNR);
		printf(" DSPower	: %f\n", param.DSPower);
		printf(" BER		: %d\n", param.BER);
		printf(" USPower	: %f\n", param.USPower);
		printf("==================================\n\n");
	}
}

void CMHostTest::getIPAddr(){
	IPAddress addr;

	if(getcmipaddr_func != NULL){
		ALOGD("CMHost Call getcmipaddr_func().");
		addr = getcmipaddr_func();
		printf("\n===================================\n");
		printf(" IP Address :%d.%d.%d.%d\n", addr.ip1, addr.ip2, addr.ip3, addr.ip4);
		printf("===================================\n");
	}
}

#define FREQ_MAXLEN 10
void CMHostTest::setTuneFreq(){
	uint32_t freq = 597000000;
	char  *key = new char[FREQ_MAXLEN];
	
	printf("\n\n Input Freq ## ");
	memset(key, 0x00, sizeof(char)*FREQ_MAXLEN);
	fgets(key, FREQ_MAXLEN, stdin);
	freq = atoi(key);

	CMLOGD("CMHost set Freq : %d",freq);
	
	if(settunefrequency_func != NULL){
		printf("\n selected frequency : %d", freq);
		status_t status = settunefrequency_func(freq);
		printf("\n===================================\n");
		printf(" result : %d\n", status);
		printf("==================================\n");
	}

	if(key){
		delete[] key;
		key = NULL;
	}
}

void CMHostTest::getCMTSIP(){
	IPAddress addr;
	if(getcmtsip_func != NULL){
		addr = getcmtsip_func();
	    printf("\n===================================\n");
	    printf(" IP Address :%d.%d.%d.%d\n", addr.ip1, addr.ip2, addr.ip3, addr.ip4);
	    printf("===================================\n");
	}
}

void CMHostTest::showTestAppVer(){
    FILE *in;
    int ch,i = 0;
    int version[3];

    if((in = fopen("data/ecram_sto.bin", "rb")) == NULL){
        CMLOGE("File Open Error.");
        return;
    }

	fseek(in, 0, SEEK_CUR);
	fseek(in, 5, SEEK_CUR);

    while(i<3){
        if((ch = fgetc(in)) != EOF){
        	version[i] = ch;
			printf("version!![%d] : %2x\n", i, version[i]);
            i++;
        }
    }
																							
	printf("\n===================================\n");
	printf(" CM Bin Version : %x.%x.%x\n", version[0],version[1], version[2]);
	printf("====================================\n");
}

#ifdef __cplusplus
}
#endif
