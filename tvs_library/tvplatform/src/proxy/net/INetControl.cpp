#include "net/INetControl.h"

#define LOG_TAG "INetControl"
#include <android/log.h>
#include <stdint.h>
#include <sys/types.h>

namespace android {

enum {
	REFRESH_DHCP_IP = IBinder::FIRST_CALL_TRANSACTION
};

class BpNetControl : public BpInterface<INetControl>
{
public:
	BpNetControl(const sp<IBinder>& impl)
		: BpInterface<INetControl>(impl)
	{
	}

	virtual bool refreshDhcpIp(const char *ifaceName)
	{
		bool result = false;
		Parcel data, reply;
		data.writeInterfaceToken(INetControl::getInterfaceDescriptor());
		data.writeCString(ifaceName);
		if(remote()->transact(REFRESH_DHCP_IP, data, &reply) == NO_ERROR)
		{
			result = (reply.readInt32() == OK);
		}
		return result;
	}
};

IMPLEMENT_META_INTERFACE(NetControl, "com.tvstorm.net.INetControl");

// ----------------------------------------------------------------------

status_t BnNetControl::onTransact( uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
	switch (code)
	{
		case REFRESH_DHCP_IP:
		{
			CHECK_INTERFACE(INetControl, data, reply);
			const char *ifName = data.readCString();
			reply->writeInt32(refreshDhcpIp(ifName) == OK ? OK : UNKNOWN_ERROR);
			return NO_ERROR;
		}
		default:
			return BBinder::onTransact(code, data, reply, flags);
	}
}

// ----------------------------------------------------------------------------

};
