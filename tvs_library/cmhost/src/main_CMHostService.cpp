#include <stdio.h>

#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <utils/Log.h>
#include <CMHostService.h>

using namespace android;

int main(int argc,char *argv[]){
	CMLOGI("Starting CMHostService....");

	sp<IServiceManager> sm = defaultServiceManager();

	CMHostService::instantiate();

	ProcessState::self()->startThreadPool();
	IPCThreadState::self()->joinThreadPool();

	return 0;
}
