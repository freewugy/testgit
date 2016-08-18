#include <CMHostServiceClient.h>

namespace android {

using android::Mutex;

namespace {
static Mutex sLock;
const int kCMHostServiceDelay = 100 * 1000; // 0.1s
sp<ICMHostService> mCMHost;

class DeathNotifier : public IBinder::DeathRecipient {
public:
	DeathNotifier(){}

	virtual void binderDied(const wp<IBinder>& who __unused){
		Mutex::Autolock lock(sLock);
		CMLOGE("Binder is died. so clear CMHostService!!!");
		mCMHost.clear();
	}
};

sp<DeathNotifier> gDeathNotifier;    
}

const sp<ICMHostService>& CMHostServiceClient::getService(){	
	CMLOGD("CMHost getService........");
	Mutex::Autolock lock(sLock);  

	if(mCMHost.get() == 0){
		sp<IServiceManager> sm = defaultServiceManager();
		sp<IBinder> binder;

		do{
			binder = sm->getService(String16("CMHostService"));

			if(binder != 0){
				CMLOGD("Quit in loop, because it takes binder.");
				break;
			}
			CMLOGI("CMHostService is not published, wating...");
			usleep(kCMHostServiceDelay);
		}while(true);

		if(gDeathNotifier == NULL){
			gDeathNotifier = new DeathNotifier();
		}

		binder->linkToDeath(gDeathNotifier);
		mCMHost = interface_cast<ICMHostService>(binder);
	}
	return mCMHost;
}

} //namespace android
