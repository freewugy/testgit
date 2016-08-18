#ifndef _CMHOST_TEST_H_
#define _CMHOST_TEST_H_

#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <map>

#include <ICMHostTest.h>
#include <CMHostServiceClient.h>
#include <CMHostModule.h>

using namespace std;
using namespace android;

#ifdef __cplusplus
extern "C" {
#endif

typedef CmInfoStruct (*getCMInfoCallBack)();
typedef uint8_t (*checkUpdateFileCallBack)();
typedef uint8_t (*requestSWDLCallBack)();
typedef TunerParams (*getTunerParamCallBack)(); 
typedef IPAddress (*getIPAddrCallBack)();   
typedef int (*setTuneFreqCallBack)(uint32_t tuneFreq);
typedef IPAddress (*getCMTSIPCallBack)();
class CMHostTest : public ICMHostTest {
public:
	CMHostTest();
	virtual ~CMHostTest();

	virtual void PrintMenu();
	virtual bool AddCMDList(const char *key, COMMAND cmd);
	virtual void RunCMD(const char *key);
	bool isQuit();

protected:
	void helpMessage();
	void getInfo();
	void requestSWDL();
	void getTunerParam();
	void getIPAddr();
	void setTuneFreq();
	void getCMTSIP();
	void showTestAppVer();

	virtual COMMAND ExtractCMD(const char *key);
	void initHandle();
	void loadAPI();
private:
	map<string, COMMAND> mCMDList;
	bool mQuit;
	void *handle;

	getCMInfoCallBack getcminfo_func;
	checkUpdateFileCallBack checkupdatefile_func;
	requestSWDLCallBack requestswdl_func;
	getTunerParamCallBack gettunerparams_func;
	getIPAddrCallBack getcmipaddr_func;
	setTuneFreqCallBack settunefrequency_func;
	getCMTSIPCallBack getcmtsip_func;
};

#ifdef __cplusplus
}
#endif

#endif // _CMHOST_TEST_H_
