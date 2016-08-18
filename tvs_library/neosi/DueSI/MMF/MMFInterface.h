/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 * MMFInterface.h
 *
 * LastChangedBy: owl
 * LastChangedDate: 2015. 3. 3.
 * Description:
 * Note:
 *****************************************************************************/
#ifndef MMF_MMFINTERFACE_H_
#define MMF_MMFINTERFACE_H_

#include <stdint.h>

#include "TVSMMFClient.h"

using namespace tvstorm;

#define MMF_FILTER_MASK_LENGTH 12

typedef struct Section_Message_t {
    int sectionFilterHandle;
    int eventId;
    int pid;
    unsigned char *sectionData;
    unsigned int length;
    void *userParam;
} Section_Message;

bool MMF_Filter_Start(MMF_SectionFilterHandle* handle, uint8_t* uri, uint16_t pid,
        uint8_t* mask, uint8_t* mode, uint8_t* comp, SectionCallback sectionCallback, void* pReceiver, MMF_SectionFilterMonitoringMode filtermode);
bool MMF_Filter_Stop(MMF_SectionFilterHandle handle);

//bool MMF_Filter_Close(MMF_SectionFilterHandle filterId);
//
//bool MMF_Filter_Open( MMF_SectionFilterHandle* filterId);

bool MMF_Demux_Open();

bool MMF_Demux_Create(MMF_DemuxHandle* demux, uint8_t* uri);
bool MMF_Demux_Destroy(MMF_DemuxHandle demux);

#endif /* MMF_MMFINTERFACE_H_ */
