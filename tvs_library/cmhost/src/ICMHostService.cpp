#include <ICMHostService.h>
#include <utils/Log.h>

namespace android {

class BpCMHostService : public BpInterface<ICMHostService> {
public:
	BpCMHostService(const sp<IBinder>& impl)
	: BpInterface<ICMHostService>(impl){
	}

	virtual status_t getCMInfo(CmInfoStruct *info){
		Parcel data, reply;

		data.writeInterfaceToken(ICMHostService::getInterfaceDescriptor());
		remote()->transact(BnCMHostService::CM_GET_INFO, data, &reply);

		status_t status = reply.readInt32();
		if(status == NO_ERROR){
			reply.read(info, sizeof(CmInfoStruct));
		}
		return status;
	}

	virtual status_t requestSWDL(){
		Parcel data, reply;

		data.writeInterfaceToken(ICMHostService::getInterfaceDescriptor());
		remote()->transact(BnCMHostService::CM_SWDL_REQUEST, data, &reply);
		
		status_t status = reply.readInt32();
		return status;
	}

	virtual status_t getTunerParam(TunerParams *param){
		Parcel data, reply;

		data.writeInterfaceToken(ICMHostService::getInterfaceDescriptor()); 
		remote()->transact(BnCMHostService::CM_GET_TUNNERPARAM, data, &reply);

		status_t status = reply.readInt32();	
		if(status == NO_ERROR){
			reply.read(param, sizeof(TunerParams));
		}
		return status;
	}

	virtual status_t getIPAddr(IPAddress *addr){
		Parcel data, reply;

		data.writeInterfaceToken(ICMHostService::getInterfaceDescriptor()); 
		remote()->transact(BnCMHostService::CM_GET_IPADDR, data, &reply);

		status_t status = reply.readInt32();
		if(status == NO_ERROR){
			reply.read(addr, sizeof(IPAddress));
		}
		return status;
	}

	virtual status_t setTuneFreq(uint32_t freq){
		Parcel data, reply;

		data.writeInterfaceToken(ICMHostService::getInterfaceDescriptor());
		data.writeInt32(freq);
		remote()->transact(BnCMHostService::CM_SET_TUNE, data, &reply);

		status_t status = reply.readInt32();

		return status;
	}
	
	virtual status_t getCMTSIP(IPAddress *addr){
		Parcel data, reply;

		data.writeInterfaceToken(ICMHostService::getInterfaceDescriptor());
		remote()->transact(BnCMHostService::CM_GET_TSIP, data, &reply);

		status_t status = reply.readInt32();
		if(status == NO_ERROR){
			reply.read(addr, sizeof(IPAddress));
		}

		return status;
	}
};

IMPLEMENT_META_INTERFACE(CMHostService, "CMHostService");

//------------------------------------------------------------------------

status_t BnCMHostService::onTransact(uint32_t code, const Parcel &data, Parcel *reply, uint32_t flags){
	switch(code){
		case CM_GET_INFO:{
			CHECK_INTERFACE(ICMHostService, data, reply);
			CmInfoStruct info;

			status_t status = getCMInfo(&info);
			reply->writeInt32(status);	
			if(status == NO_ERROR){
				reply->write(&info, sizeof(CmInfoStruct));
			}
			return NO_ERROR;
		}
		break;

		case CM_SWDL_REQUEST:{
			CHECK_INTERFACE(ICMHostService, data, reply);
			reply->writeInt32(requestSWDL());

			return NO_ERROR;
		}
		break;

		case CM_GET_TUNNERPARAM:{
			CHECK_INTERFACE(ICMHostService, data, reply);
			TunerParams param;

			status_t status = getTunerParam(&param);
			reply->writeInt32(status);
			if(status == NO_ERROR){
				reply->write(&param, sizeof(TunerParams));
			}
			return NO_ERROR;
		}
		break;

		case CM_GET_IPADDR:{
			CHECK_INTERFACE(ICMHostService, data, reply);
			IPAddress addr;

			status_t status = getIPAddr(&addr);
			reply->writeInt32(status);
			if(status == NO_ERROR){
				reply->write(&addr, sizeof(IPAddress));
			}
			return NO_ERROR;
		}
		break;

		case CM_SET_TUNE:{
			CHECK_INTERFACE(ICMHostService, data, reply);
			uint32_t freq = (uint32_t)data.readInt32();

			reply->writeInt32(setTuneFreq(freq));
			return NO_ERROR;
		}
		break;

		case CM_GET_TSIP:{
			CHECK_INTERFACE(ICMHostService, data, reply);
			IPAddress addr;

			status_t status = getCMTSIP(&addr);
			reply->writeInt32(status);

			if(status == NO_ERROR){
				reply->write(&addr, sizeof(IPAddress));
			}

			return NO_ERROR;
		}
		break;

		default:
			return BBinder::onTransact(code, data, reply, flags);
	}
}

} //namespace android

