#ifndef _CMHOST_SERVICE_CLIENT_H_
#define _CMHOST_SERVICE_CLIENT_H_

#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <CMHostService.h>

namespace android {

class CMHostServiceClient {
public:
	CMHostServiceClient(){ }
	~CMHostServiceClient(){ }

	const sp<ICMHostService>& getService();
};

} //namespace android
#endif // _CMHOST_SERVICE_CLIENT_H_

