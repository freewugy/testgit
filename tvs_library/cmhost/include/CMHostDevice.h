#ifndef _CMHOST_SERVICE_H_
#define _CMHOST_SERVICE_H_

#include <utils/Thread.h>
#include <CMHostModule.h>

namespace android {

class UpdateCMThread;
class CMHostJoinThread;

class CMHostDevice {
public:
	status_t getCMInfo(CmInfoStruct *info);
	status_t requestSWDL();
	status_t getTunerParam(TunerParams *param);
	status_t getIPAddr(IPAddress *addr);
	status_t setTuneFreq(uint32_t freq);
	status_t getCMTSIP(IPAddress *addr);

private:
	CMHostDevice();
	virtual ~CMHostDevice();

	UpdateCMThread *mUpdateCMThread;
	CMHostJoinThread *mJoinThread;
};

const char CM_BIN_PATH[]="/mnt/media_rw/sdcard1/cmupdate/ecram_sto.bin";
const char CM_BIN[] = "ecram_sto.bin";

class NotificationThread : public Thread {
enum {
	Noti_UnInit = 0,
	Noti_Operational,
	Noti_Reset,
	Noti_Failed,
	NOti_Max
};
public:
	virtual status_t readyToRun();
	virtual bool threadLoop();
	void setNotificationState(int state);

private:
	int mState;

	void resetEthState(int sec);
	void completeTonUpdateCMBin();
};

class CMHostJoinThread : public Thread {
public:
	virtual status_t readyToRun();
	virtual bool threadLoop();

private:
	void initializeNexusContextClient();
};

class UpdateCMThread : public Thread{
public:
	void onFirstRef();
	virtual status_t readyToRun();
	virtual bool threadLoop();
	void setThreadRunning(bool isRunning);
	void startSWDL();

private:
	bool mRunning;
	bool mCopied;

	bool isThreadRunning();
	bool isUpdateDir();
	bool haveCMBin();
};

} // namespace android
#endif // _CMHOST_SERVICE_H_
