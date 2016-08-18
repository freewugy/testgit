#define LOG_TAG "native_tvplatorm_service"

#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <cutils/properties.h>
#include <utils/Log.h>

#include "NetControl.h"

using namespace android;

int main(int argc __unused, char** argv)
{
	ALOGD("native tvplatform service start!!!");
	NetControl::instantiate();
	ProcessState::self()->startThreadPool();
	IPCThreadState::self()->joinThreadPool();
	return 0;
}
