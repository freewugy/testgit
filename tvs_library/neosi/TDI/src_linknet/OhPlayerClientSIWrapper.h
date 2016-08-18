/******************************************************************************
 *
 * Author : mcchoi
 * $LastChangedDate: 2014-05-30 22:21:57 +0900 (금, 30 5월 2014) $
 * $LastChangedRevision: 824 $
 * Description:
 * Note:
 *****************************************************************************/

#ifndef __OH_PLAYER_CLIENT_SI_WRAPPER_H__
#define __OH_PLAYER_CLIENT_SI_WRAPPER_H__

//#include "tvstorm/mediaservice/ITVSPlayer.h"
//#include "tvstorm/mediaservice/ITVSMediaService.h"
//#include "tvstorm/player/PlayerParameters.h"
#include "Parameters.h"

namespace tvstorm {

class IOhPlayerClient;

class OhPlayerClientSIWrapper {
public:
	OhPlayerClientSIWrapper();
	virtual ~OhPlayerClientSIWrapper();

	int startTune(const char *aUrl);
	int stopTune();
	int createSectionFilter(SectionFilterParameter_t *aParam, SectionCallback aCallback, void *aUserData, unsigned int aTimeoutUSec = 0, int aFrequencyKhz = -1);
	int destroySectionFilter(int aFilterId, int aFrequencyKhz);
	void enableSectionFilter(int aFilterId, bool aEnable, int aFrequencyKhz);
	void setMediaServerCallback(TVSMediaServerCallback aCallback);
private:
	void *mLibHandle;
	IOhPlayerClient *mCore;
};

} /* namespace TVSTORM */

#endif /* #ifndef __OH_PLAYER_CLIENT_WRAPPER_H__ */
