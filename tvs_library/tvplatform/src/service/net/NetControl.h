#ifndef _NET_CONTROL_H_
#define _NET_CONTROL_H_

#include "net/INetControl.h"
#include <utils/threads.h>
#include <utils/KeyedVector.h>

namespace android {

class NetControl : public BnNetControl
{
public:
	NetControl();
	virtual ~NetControl();
	static void instantiate();
	virtual bool refreshDhcpIp(const char *ifaceName);
};

}  // namespace android

#endif  // _NET_CONTROL_H_
