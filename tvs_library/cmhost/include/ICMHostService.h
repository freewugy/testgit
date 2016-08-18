#ifndef _ICMHOST_SERVICE_H_
#define _ICMHOST_SERVICE_H_

#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <CMHostModule.h>

namespace android {

class ICMHostService : public IInterface {
public:
	enum {
		CM_GET_INFO = IBinder::FIRST_CALL_TRANSACTION,
		CM_SWDL_REQUEST,
		CM_GET_TUNNERPARAM,
		CM_GET_IPADDR,
		CM_SET_TUNE,
		CM_GET_TSIP,
	};

	DECLARE_META_INTERFACE(CMHostService);

	virtual status_t getCMInfo(CmInfoStruct *info) = 0;
	virtual status_t requestSWDL() = 0;
	virtual status_t getTunerParam(TunerParams *param) = 0;
	virtual status_t getIPAddr(IPAddress *addr) = 0;
	virtual status_t setTuneFreq(uint32_t freq) = 0;
	virtual status_t getCMTSIP(IPAddress *addr) = 0;
};

//------------------------------------------------------------------------------

class BnCMHostService : public BnInterface<ICMHostService> {
public:
	virtual status_t onTransact(uint32_t code, const Parcel &data, Parcel *reply, uint32_t flags = 0);
};

} // namspace android
#endif //_ICMHOST_SERVICE_H_


