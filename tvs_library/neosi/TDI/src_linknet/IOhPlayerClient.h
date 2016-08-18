/******************************************************************************
 *
 * Author : mcchoi
 * $LastChangedDate: 2014-05-30 18:53:14 +0900 (금, 30 5월 2014) $
 * $LastChangedRevision: 822 $
 * Description:
 * Note:
 *****************************************************************************/

#ifndef __IOH_PLAYER_CLIENT_H__
#define __IOH_PLAYER_CLIENT_H__

//#include "tvstorm/mediaservice/ITVSPlayer.h"
//#include "tvstorm/mediaservice/ITVSMediaService.h"
//#include "tvstorm/player/PlayerParameters.h"
// ksh_20140530 | modify include path for si
//#include "tvstorm/export/Parameters.h"
#include "Parameters.h"

namespace tvstorm {

typedef void (*SectionCallback)(const int demuxNumber, const int sectionFilterHandle,
const int eventId, const int pid, const unsigned char *sectionData,
const unsigned int length, void *userParam);

typedef void (*TVSMediaServerCallback)(int aMsg, int aArg1, int aArg2, const char* aData);

class IOhPlayerClient {
public:
    IOhPlayerClient()
    {
    }
    ;
    virtual ~IOhPlayerClient()
    {
    }
    ;

    virtual int tuneStart(const char *aUrl) = 0;
    virtual int tuneStop() = 0;

    virtual int createSectionFilter(SectionFilterParameter_t *aParam,
            SectionCallback aCallback, void *aUserData, unsigned int aTimeoutUSec, int aFrequencyKhz) = 0;
    virtual int setEnableSectionFilter(int aFilterId, bool aFilterEnable, int aFrequencyKhz) = 0;
    virtual int destroySectionFilter(int aFilterId, int aFrequencyKhz) = 0;

    virtual int setCallback(TVSMediaServerCallback aCallback) = 0;
/*
    virtual int createSectionFilterByUrl(SectionFilterParameter_t *aParam,
            SectionCallback aCallback, void *aUserData, unsigned int aTimeoutUSec, const char* aUrl) = 0;
    virtual int setEnableSectionFilterByUrl(int aFilterId, bool aFilterEnable, const char* aUrl) = 0;
    virtual int destroySectionFilterByUrl(int aFilterId, const char* aUrl) = 0;

    virtual int createSectionFilterByTunerId(SectionFilterParameter_t *aParam,
            SectionCallback aCallback, void *aUserData, unsigned int aTimeoutUSec, int aTunerId) = 0;
    virtual int setEnableSectionFilterByTunerId(int aFilterId, bool aFilterEnable, int aTunerId) = 0;
    virtual int destroySectionFilterByTunerId(int aFilterId, int aTunerId) = 0;
*/
    virtual int teletextInit() = 0;
    virtual int teletextStart(int aPid) = 0;
    virtual int teletextStop() = 0;

    /*
     *settings
     */
    virtual bool setVideoSize(int32_t aX, int32_t aY, int32_t aW, int32_t aH) = 0;
    virtual bool getVideoSize(int32_t& aX, int32_t& aY, int32_t& aW, int32_t& aH) = 0;

    virtual bool setVideoScalingMode(VideoScalingMode aMode) = 0;
    virtual bool getVideoScalingMode(VideoScalingMode& aMode) = 0;

    virtual bool setVideoAspectRatio(VideoAspectRatio aAspectRatio) = 0;
    virtual bool getVideoAspectRatio(VideoAspectRatio& aAspectRatio) = 0;

    virtual bool getDecSerialNumber(int& aNumber) = 0;

    virtual bool getTunerInfo(TunerInfo& aInfo) = 0;
    virtual bool enableTunerLog(bool aEnable) = 0;

    virtual bool getMetaDataInfo(const char* aFilePath, MetaDataInfo& aInfo) = 0;

    virtual bool changeAudioStream(int aPid, int aCodec) = 0;
    virtual bool changeAudioStream(const char* aUrl) = 0;

    virtual bool setDescrambleKey(const DescramblerKey_t& aKey, const char* aUrl, int aTunerNumber) = 0;

    /*
     * Using url, tuner number.
     */
    virtual int createSectionFilter(SectionFilterParameter_t *aParam, SectionCallback aCallback,
            void *aUserData, unsigned int aTimeoutUSec, int aFrequencyKhz, const char* aUrl, int aTunerId) = 0;
    virtual int setEnableSectionFilter(int aFilterId, bool aFilterEnable, int aFrequencyKhz, const char* aUrl, int aTunerId) = 0;
    virtual int destroySectionFilter(int aFilterId, int aFrequencyKhz, const char* aUrl, int aTunerId) = 0;

    virtual int getCaSystemInfo(CasSystemInfo& info) = 0;			/* 0 for okay, others for error state */
    virtual int getCaSmartcardInfo(CasSmartcardInfo& info) = 0;	/* 0 for okay, others for error */
    virtual bool setBlankScreen(const char* aPlayerUrl, bool aBlankScreen) = 0;

    virtual int notifyCasEvent(int aEvent, const char* aString) = 0;

    // Downmix Test
    virtual void changeSoundConfig(int mode) = 0;// mode : 0(surround), mode : 1(stereo)
private:

};

class IOhPlayerClient;
typedef IOhPlayerClient* (*OPEN)(int aClientType);
typedef void (*CLOSE)(IOhPlayerClient* aClient);

} /*namespace TVSTORM {*/

#endif /*__IOH_PLAYER_CLIENT_H__*/
