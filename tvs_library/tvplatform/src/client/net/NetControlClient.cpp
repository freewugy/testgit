#include "INetControlClient.h"

#define LOG_TAG "NetControl"
#include <utils/Log.h>
#include <binder/IServiceManager.h>

#include "util/TVPlatformConst.h"
#include "net/INetControl.h"

using namespace android;
using namespace TVS;

class NetControlClient : public INetControlClient
{
public:
	NetControlClient();
	virtual ~NetControlClient();
	virtual bool refreshDhcpIp(const char *ifaceName);

private:
	sp<INetControl> netControlService;
};

NetControlClient::NetControlClient()
: netControlService(NULL)
{
	sp<IServiceManager> sm = defaultServiceManager();
	sp<IBinder> binder = sm->getService(String16(NET_CONTROL_SERVICE_NAME));
	if(binder.get() != NULL)
	{
		netControlService = interface_cast<INetControl>(binder);
	}
}

NetControlClient::~NetControlClient()
{
	if (netControlService.get() != NULL) {
		netControlService.clear();
		netControlService = NULL;
	}
}

bool NetControlClient::refreshDhcpIp(const char *ifaceName)
{
	ALOGD("refreshDhcpIp(%s)",ifaceName);
	return netControlService->refreshDhcpIp(ifaceName);
}

// using dlsym by NDK
#ifdef __cplusplus
extern "C" {
#endif

INetControlClient* open()
{
	return new NetControlClient();
}

void closeNetControl(TVS::INetControlClient* client)
{
	if(client != NULL)
	{
		delete client;
	}
}

#ifdef __cplusplus 
}
#endif
