#ifndef _I_NET_CONTROL_H_
#define _I_NET_CONTROL_H_

#include <binder/IInterface.h>
#include <binder/Parcel.h>

namespace android {

// ----------------------------------------------------------------------------

class INetControl : public IInterface
{
public:
	DECLARE_META_INTERFACE(NetControl);
	virtual bool refreshDhcpIp(const char *ifaceName) = 0;
};

// ----------------------------------------------------------------------------

class BnNetControl : public BnInterface<INetControl>
{
public:
	virtual status_t onTransact( uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags = 0);
};

// ----------------------------------------------------------------------------

}; // namespace android

#endif // _I_NET_CONTROL_H_
