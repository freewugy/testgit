/******************************************************************************
 *
 * Author : mcchoi
 * $LastChangedDate: 2014-06-03 13:58:35 +0900 (화, 03 6월 2014) $
 * $LastChangedRevision: 837 $
 * Description:
 * Note:
 *****************************************************************************/

#ifndef __CLIENT_PARAMETER_H__
#define __CLIENT_PARAMETER_H__

#include <sys/types.h>

namespace tvstorm {

typedef void (*SectionCallback)(const int demuxNumber, const int sectionFilterHandle,
        const int eventId, const int pid, const unsigned char *sectionData,
        const unsigned int length, void *userParam);

typedef void (*TVSMediaServerCallback)(int aMsg, int aArg1, int aArg2, const char* aData);

#define TVSMEDIA_CLIENT_TYPE_SI 0
#define TVSMEDIA_CLIENT_TYPE_PVR 1
#define TVSMEDIA_CLIENT_TYPE_SETTING 2
#define TVSMEDIA_CLIENT_TYPE_CAS 3
#define TVSMEDIA_CLIENT_TYPE_ALL 4

enum TVSMediaEventType {
    MEDIA_PLAY_STOPPED = 300,                            // Media Info
    MEDIA_PLAY_STARTED = 301,                             // Media Info
    MEDIA_PLAY_TRICKED = 302,                              // Media Info
    MEDIA_PLAY_SEEKED = 303,                               // Not used.
    MEDIA_PLAY_START_OF_STREAM = 304,           // Need Start player.
    MEDIA_PLAY_END_OF_STREAM = 305,             // Need Stop and Release(Only for PVR play, except TimeShift
    MEDIA_PLAY_FIRST_FRAME_DISPLAYED = 306,

    MEDIA_RECORD_STOPPED = 320,                 // Not used,
    MEDIA_RECORD_STARTED = 321,                 // Not used,
    MEDIA_RECORD_FAILED = 322,                  // Need stop recorder
    MEDIA_RECORD_STORAGE_SPACE_LEFT = 323,      // 10 means capacity is 10% left
    MEDIA_RECORD_NO_STREAM = 324,               // Need stop recorder
    MEDIA_RECORD_LOW_STORAGE_LEFT = 325,      // means under 15GByte
    MEDIA_RECORD_STORAGE_FULL = 326,            //means under 100MByte

    MEDIA_TUNER_LOCKED = 330,                   // used for MMF Client(SI Service)
    MEDIA_TUNER_LOCK_FAILED = 331,              //  used for MMF Client(SI Service)
    MEDIA_TUNER_SIGNAL_LOST = 332,              //  used for MMF Client(SI Service)
    MEDIA_TUNER_SIGNAL_ACQUIRE = 333,              //  used for MMF Client(SI Service)

    MEDIA_LOW_BUFFER = 340,                     // Need pause
    MEDIA_MID_BUFFER = 341,                     // Need resume
    MEDIA_HIGT_BUFFER = 342,                    //Currently Not used.

    MEDIA_SERVICE_DIED = 400,

};

enum VideoScalingMode {
    PILLAR_BOX = 0,
    STRETCHED = 1,
    LETTER_BOX = 2,
    SQUEEZED = 3,
    NONE = 4,
};

enum VideoAspectRatio {
    ASPECT_RATIO_16_9 = 0,
    ASPECT_RATIO_4_3 = 1,
};

struct TunerInfo {
    bool mLocked;
    int mStrength;
    int mQuality;
    int mPowerLevel;
    int mPacketErr;
    double mBER;
    int mAGC;
    int mSNR;
};

struct MetaDataInfo {
    unsigned long long mDurationmsec;
};

/* Macro Definition */
#ifndef MAX_LENGTH_OF_IRD_SERIAL_NUMBER
#define MAX_LENGTH_OF_IRD_SERIAL_NUMBER                        15
#endif /* MAX_LENGTH_OF_IRD_SERIAL_NUMBER */
#ifndef MAX_LENGTH_OF_CAK_VERSION
#define MAX_LENGTH_OF_CAK_VERSION                              19
#endif /* MAX_LENGTH_OF_CAK_VERSION */
#ifndef MAX_LENGTH_OF_PROJECT_INFORMATION
#define MAX_LENGTH_OF_PROJECT_INFORMATION                      11
#endif /* MAX_LENGTH_OF_PROJECT_INFORMATION */
#ifndef MAX_LENGTH_OF_SMARTCARD_SERIAL_NUMBER
#define MAX_LENGTH_OF_SMARTCARD_SERIAL_NUMBER                  15
#endif /* MAX_LENGTH_OF_SMARTCARD_SERIAL_NUMBER */
#ifndef MAX_LENGTH_OF_SMARTCARD_VERSION
#define MAX_LENGTH_OF_SMARTCARD_VERSION                        16
#endif /* MAX_LENGTH_OF_SMARTCARD_VERSION */


struct CasSystemInfo{
	/* all strings are null - terminated */
	char irdSerialNumber[MAX_LENGTH_OF_IRD_SERIAL_NUMBER+1];		/* "{XX XXXX XXXX XX}" */
	char cakVersion[MAX_LENGTH_OF_CAK_VERSION+1];					/* "D-{ZZZZZ-ZZZZZ}-NAK/TBK{ZZ}" */
	char projectInformation[MAX_LENGTH_OF_PROJECT_INFORMATION+1];	/* "{XXXXYY-ZZZZ}" : XXXX - Operator ID, YY - Operator SW Version*/
};

struct CasSmartcardInfo{
	/* all strings are null - terminated */
	char serialNumber[MAX_LENGTH_OF_SMARTCARD_SERIAL_NUMBER+1];		/* "{XX XXXX XXXX XX}" */
	char version[MAX_LENGTH_OF_SMARTCARD_VERSION+1];				/* "DNASP{XXX}-RevR{YY}" */
	unsigned short emmCaSystemId;
};

enum SectionFilterEvent_t {
    kSectionFilterCmdCreate = 300,
    kSectionFilterCmdCreateDone,
    kSectionFilterCmdSetEnable,
    kSectionFilterCmdDestroy,
    kSectionFilterCmdDestroyDone,
    kSectionFilterSendData,
    kSectionFilterTimeOut,
    kWhatSetTeletext,
};

typedef enum {
    DEMUX_FILTER_TS_TYPE, /* Transport stream filter */
    DEMUX_FILTER_SECTION_TYPE, /* Section information filter */
    DEMUX_FILTER_APG_TYPE,
    DEMUX_FILTER_MPT_TYPE,
    DEMUX_FILTER_PIP_TYPE,
    DEMUX_FILTER_ECM_TYPE, /* ECM filter */
    DEMUX_FILTER_EMM_TYPE, /* EMM filter */
    DEMUX_FILTER_PES_TYPE
} DEMUX_FILTER_TYPE;

typedef enum {
    DEMUX_FILTER_VERSION_CHANGE = 0,
    DEMUX_FILTER_ONE_SHOT,
    DEMUX_FILTER_CONTINUOUS,
} DEMUX_FILTER_OPTION;

#pragma pack(push, 1)
struct SectionFilterParameter_t {
    int pid;
    int filterType;
    int filterOption;
    unsigned char filterMatch[12];
    unsigned char filterMask[12];
    unsigned char negativeMatch[12];
};
struct SectionFilterData_t {
	bool fromCache;
	int pid;
	unsigned int filterId;
	int filterOption;
	char *data;
	unsigned int dataLen;
};


/*
 * pvr
 */
enum {
    kPvrDataTypeTS = 0,
    kPvrDataTypeCtrl = 1,
    kPvrDataTypeIndex = 2,

    kPvrDataTypeUnknown = 100
};
struct PvrData {
    char *mData;
    unsigned int mDataLen;
    int mDataType;
};
struct DescramblerKey_t {
	DescramblerKey_t() : evenKeyLength(16), oddKeyLength(16) 
        {
            for(int i = 0; i < 16; i++)
            {
                 evenKey[i] = oddKey[i] = '\0';
            }
        }
	unsigned int evenKeyLength;
	unsigned char evenKey[16];
	unsigned int oddKeyLength;
	unsigned char oddKey[16];
};


#pragma pack(pop)


}

#endif /*__IOH_PLAYER_CLIENT_H__*/






