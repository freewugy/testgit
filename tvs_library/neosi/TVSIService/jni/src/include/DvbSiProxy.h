#ifndef __DVB_SI_PROXY_H__
#define __DVB_SI_PROXY_H__
/////////////////////////////////////////////////////////////////////////////////////////

#include "TVS_DVBSI_Interface.h"

typedef enum {
        DVBSI_STS_INACTIVE = 0, DVBSI_STS_ACTIVE
} DVBSI_STATUS;

extern JavaVM* gVM;
extern jobject gDvbSiMgrObj;

bool isDvbSiActive();
void setDvbSiStatus(DVBSI_STATUS sts);

/////////////////////////////////////////////////////////////////////////////////////////
#endif /* __DVB_SI_PROXY_H__ */
