/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: mcchoi $
 * $LastChangedDate: 2014-12-2 22:28:48 +0900 (Mon, 17 Dec 2012) $
 * $LastChangedRevision: $
 * Description:
 * Note:
 *****************************************************************************/

#ifndef __TVSMMFCLIENT_H__
#define __TVSMMFCLIENT_H__

#include "TVSMMFTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int64_t MMF_RecordingHandle;
typedef int64_t MMF_SectionFilterHandle;
typedef int64_t MMF_DemuxHandle;

typedef void (*SectionCallback)(
		const int sectionFilterHandle,
		const int eventId, const int pid, const unsigned char *sectionData,
		const unsigned int length, void *userParam);


/*
 * Need to define events.
 */

typedef void (*TVSMediaCallback)(int what, int extra, const uint8_t* data, size_t size);
typedef TVSMediaCallback TVSPvrCallback;

typedef enum
{
	MMF_ERR_OK,
	MMF_ERR_UNKNOWN,
	MMF_ERR_INVALID_PARAM,
	MMF_ERR_HW,
	MMF_ERR_NOT_INITIALIZED,
	MMF_ERR_NOT_SUPPORTED,
	MMF_ERR_NULL_PTR
}
MMF_Error;

typedef enum
{
	SECTION_MODE_ONE_SHOT,
	SECTION_MODE_VERSION_CHANGE,
	SECTION_MODE_CONTINOUS
}
MMF_SectionFilterMonitoringMode;

typedef struct
{
	uint8_t* uri;
	uint32_t pid;
	uint32_t options;

	MMF_SectionFilterMonitoringMode monitormode;

	uint8_t mask[12];
	uint8_t mode[12];
	uint8_t comp[12];
	uint32_t masksize;
	uint32_t modesize;
	uint32_t compsize;

	SectionCallback callback;
	void* userParam;
}
MMF_SectionFilter;

/*
 * Global callback.
 */
MMF_Error MMF_SetCallback(TVSMediaCallback callback);

/*
 * Inband demux interface.
 */

/*******************************************************

 * Caution !!!!

 * - MMF_Error MMF_DemuxCreate(MMF_DemuxHandle * demux, const char* uri)
 * - MMF_Error MMF_DemuxDestroy(MMF_DemuxHandle demux)

 *  Interface SHOULD be only used as Channel Scan.
 * In case of using section filter on current play
 * channel(etc DSM-CC, PSI monitoring), SHOULD NOT USE it!!!!

 *******************************************************/
MMF_Error MMF_DemuxCreate(MMF_DemuxHandle * demux, const char* uri);
MMF_Error MMF_DemuxDestroy(MMF_DemuxHandle demux);

MMF_Error MMF_DemuxAddSectionFilter(MMF_SectionFilterHandle* handle, const MMF_SectionFilter& mmfSectionFilter);
MMF_Error MMF_DemuxRemoveSectionFilter(MMF_SectionFilterHandle handle);
MMF_Error MMF_DemuxControlSectionFilter(MMF_SectionFilterHandle handle, bool enable);

/*
 * Recording interface
 */
MMF_Error MMF_PvrStartRecording(const char* uri, MMF_RecordingHandle* handle, TVSPvrCallback callback);
MMF_Error MMF_PvrStopRecording(MMF_RecordingHandle handle);

/*
 * General command interface : see "tvs_control help"
 */

typedef void * 	MMF_Command;

MMF_Command 	MMF_CommandCreate();
MMF_Error 		MMF_CommandAddLine(MMF_Command command, const char * line);
int32_t 		MMF_CommandResultCount(MMF_Command command);
const char * 	MMF_CommandResultLine(MMF_Command command, int32_t index);
void 			MMF_CommandDestroy(MMF_Command command);

MMF_Error 		MMF_CommandExecute(MMF_Command command);

#ifdef __cplusplus
}
#endif

#endif /* __MMF_CLIENT_H__ */
