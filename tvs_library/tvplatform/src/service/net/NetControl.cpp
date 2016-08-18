#include "NetControl.h"

#define LOG_TAG "NetControl"
#include <utils/Log.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <stdlib.h>

#include "util/TVPlatformConst.h"

namespace android {

void NetControl::instantiate() {
	defaultServiceManager()->addService(String16(NET_CONTROL_SERVICE_NAME), new NetControl());
}

NetControl::NetControl()
{
	ALOGD("NetControl Created");
}

NetControl::~NetControl()
{
	ALOGD("NetControl Deleted");
}

bool NetControl::refreshDhcpIp(const char *ifaceName)
{
	ALOGD("refreshDhcpIp(%s)", ifaceName);
	char command[64];
	memset(command,0x00,64);
	sprintf(command,"netcfg %s dhcp", ifaceName);
	ALOGD("executing (%s)", command);
	system(command);
	return true;
}

}  // namespace android

