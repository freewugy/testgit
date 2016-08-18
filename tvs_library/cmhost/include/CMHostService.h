#ifndef _CMHOST_SERVICE_H_
#define _CMHOST_SERVICE_H_

#ifdef DLOADER
#include <CMHostModule.h>
#else
#include <ICMHostService.h>
#endif

#include <utils/Looper.h>
#include <utils/Thread.h>

namespace android {

class MonitorThread;
class CMMessageHandler;

enum {
	CM_EVENT_UNKNOWN = -1,
	CM_REGISTER_NOTIFICATION,
	CM_COMPLETE_REGISTRATION,
	CM_START_SWDL,
	CM_STATE_OPERATIONAL,
	CM_STATE_RESET,
	CM_STATE_FAILED,
	CM_STATE_UPDATE_FAILED,
	CM_STATE_UNINIT,
	CM_EVENT_MAX,
};
#ifdef DLOADER
class CMHostService {
public:

    status_t getCMInfo(CmInfoStruct *info);
    status_t requestSWDL();
    status_t getTunerParam(TunerParams *param);
    status_t getIPAddr(IPAddress *addr);
    status_t setTuneFreq(uint32_t freq);
    status_t getCMTSIP(IPAddress *addr);

    void sendMessage(const sp<MessageHandler>& handler, const Message& message);
    void sendMessage(const int message);
    static void sendStaticMessage(const int message);

private:
    MonitorThread *mMonitorThread;
    static sp<CMMessageHandler> mHandler;
    static sp<Looper> mLooper;

    CMHostService();
    virtual ~CMHostService();
    void registerNotificationCallback();
};

#else
class CMHostService : public BnCMHostService {
public:
	static void instantiate();
	static const char* getServiceName() { return "CMHostService"; }

	virtual status_t getCMInfo(CmInfoStruct *info);
	virtual status_t requestSWDL();
	virtual status_t getTunerParam(TunerParams *param);
	virtual status_t getIPAddr(IPAddress *addr);
	virtual status_t setTuneFreq(uint32_t freq);
	virtual status_t getCMTSIP(IPAddress *addr);

	virtual status_t onTransact(uint32_t code, const Parcel &data, Parcel *reply, uint32_t flags);

	void sendMessage(const sp<MessageHandler>& handler, const Message& message);
	void sendMessage(const int message);
	static void sendStaticMessage(const int message);
private:
	MonitorThread *mMonitorThread;
	static sp<CMMessageHandler> mHandler;
	static sp<Looper> mLooper;

	CMHostService();
	virtual ~CMHostService(); 
	void registerNotificationCallback();
	status_t getCMMacAddress(CmInfoStruct *info);
	int convertChtoDec(int ch);
};
#endif //DLOADER

const char CM_BIN_PATH[]="/mnt/media_rw/sdcard1/cmupdate/ecram_sto.bin";
const char CM_BIN[] = "ecram_sto.bin";

class CMMessageHandler : public MessageHandler {
public:
	virtual void handleMessage(const Message& message){
			messages.push(message);
	}

	Vector<Message> getMessageList(){
		return messages;
	}

	int getMessageSize(){
		return messages.size();
	}

	void popMessage(){
		messages.pop();
	}

	int getCurrentMessage(){
		return messages[0].what;
	}

private:
	Vector<Message> messages;
};

class MonitorThread : public Thread {
/* update state */
enum {
	NONE_UPDATE = 0,
	ONLINE_UPDATE,
	USB_UPDATE,
};

/* CM initializing state */
enum {
	kCMUinit = 0,
	kCMNexusInit,
	kCMAddRoute,
	kCMRegistration,
	kCMMax,
};

/* about checking ver of CM binary.*/
enum {
	cmBin_NoError = 0,
	cmBin_NoAvailableVersion,
	cmBin_FileOpenErrror,
	cmBin_MAX
};

public:
	MonitorThread(const sp<Looper>& looper, const sp<CMMessageHandler>& handler);
	~MonitorThread();

	virtual status_t readyToRun();
	virtual bool threadLoop();

	int waitOnEvent();
	int waitOnEvent(int timeoutMillis);  
	void setThreadRunning(bool isRunning);

	/* Online updtate API */
	void initUpdateEvent();
	void setUpdateEvent(bool success);
	void destroyUpdateEvent();
	bool waitUpdateEvent();

private:
	bool mRunning;
	bool mOnlineUpdate;
	int  updateMode;
	int	 mCMInitState;
	int  mRegistrationCount;
	int  mInvalidCount;
	int  mUpdateCount;
		
	sp<Looper> mLooper;
	sp<CMMessageHandler> mHandler;

	int getEventMessage();
	int getEventMessageSize();
	bool isRunning();
	
	/* Initialze API */
	void initializeNexusContextClient();
	void addCMRoute();
	void allocateIPAddress();
	int getCMInitState();
	void setCMInitState(int state);

	/* Refresh ethernet API */
	void resetEthState(bool stateChange = true);
	void checkAndRefreshEthState();
	bool isValidEthernetIP();

	/* Update API */
	void completeOnUpdateCMBin();
	bool prepareToUpdateCMBin();
	bool haveCMBin();
	int  checkCMBinVer();
	void startSWDL();
	void setUpdateMode(int state); 
	int  isUpdate();
	void cancelToUpdateCMBin(bool led = true);
	void reInitToNoUpdateMode();
};

} // namespace android
#endif // _CMHOST_SERVICE_H_
