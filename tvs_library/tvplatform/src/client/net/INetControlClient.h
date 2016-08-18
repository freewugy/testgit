#ifndef _I_NET_CONTROL_CLIENT_H_
#define _I_NET_CONTROL_CLIENT_H_

namespace TVS {

class INetControlClient {
public:
	INetControlClient()
	{
	}

	virtual ~INetControlClient()
	{
	}

	virtual bool refreshDhcpIp(const char *ifaceName) = 0;
};

typedef INetControlClient* (*OPEN)();
typedef void (*CLOSE)(INetControlClient* Client);

} /*namespace TVS */

#endif /*_I_NET_CONTROL_CLIENT_H_*/
