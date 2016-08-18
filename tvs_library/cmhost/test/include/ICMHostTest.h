#ifndef _ICMHOST_TEST_H_
#define _ICMHOST_TEST_H_

namespace android {

class ICMHostTest {
public:
	enum COMMAND {
		CMD_UNKNOWN = 0,
		CMD_QUIT,
		CMD_HELP,

		GET_INFO,
		SWDL_REQUEST,
		GET_PARAM,
		GET_IPADDR,
		SET_TUNE,
		GET_TSIP,
		TESTAPP_VER,
	};

	ICMHostTest(){}
	virtual ~ICMHostTest(){}

	virtual void PrintMenu() = 0;
	virtual bool AddCMDList(const char *key, COMMAND cmd) = 0;
	virtual void RunCMD(const char *key) = 0;

protected:
	virtual COMMAND ExtractCMD(const char *key) = 0;
};

} //namespace android

#endif // _ICMHOST_TEST_H_
