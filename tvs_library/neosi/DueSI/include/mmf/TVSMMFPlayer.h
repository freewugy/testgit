/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: skkim $
 * $LastChangedDate: 2015. 1. 28. $
 * $LastChangedRevision: 80 $
 * Description:
 * Note:
 *****************************************************************************/

#ifndef INCLUDE_TVSTORM_MMF_INTF_TVSMMFPLAYER_H_
#define INCLUDE_TVSTORM_MMF_INTF_TVSMMFPLAYER_H_

#include "TVSMMFTypes.h"

namespace tvstorm
{

class TVSMMFPlayer
{
public:
	class Listener
	{
	protected:
		Listener();
	public:
		virtual ~Listener();

		virtual void onPlayerError(int32_t errorCode) = 0;
		virtual void onPlayerInfo(PlayInfoCode infoCode, int32_t extra) = 0;
		virtual void onEndOfStream() = 0;

		/*
		 * TODO: hide TCaptionData type and Add TVSCCRender.
		 */
		//virtual void onCaptionData(const TCaptionData & caption) = 0;
	};

public:
	TVSMMFPlayer(Listener & listener);
	TVSMMFPlayer(Listener * listener);
	virtual ~TVSMMFPlayer();

    bool setDataSource(const char * uri);
    bool prepare();
    bool reset();

    /*
     * Play control
     */
    bool start();
    bool stop();
    bool pause();
    bool resume();
    bool isPlaying() const;

    /*
     * Seek & Position
     */
    bool seekTo(int32_t msec);
    bool getCurrentPosition(int32_t & msec) const;
    bool getDuration(int32_t & msec) const;

    /*
     * Trick control
     */

    bool trick(float speed);

    /*
     * resize
     */
#ifndef ANDROID
    bool setVideoSize(int32_t x, int32_t y, int32_t w, int32_t h);
#endif

private:
    friend class TVSPlayerListenerProxy;
    Listener * mListener;
	void * mData1;
	void * mData2;
};

} /* namespace tvstorm */

#endif /* INCLUDE_TVSTORM_MMF_INTF_TVSMMFPLAYER_H_ */
