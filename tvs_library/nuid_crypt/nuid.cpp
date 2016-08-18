
#include <stdio.h>
#include <cutils/log.h>
#include "bstd.h"
#include "nexus_platform.h"
#include "nexus_otpmsp.h"
#include "bkni.h"
#include "nuid.h"
#include "common.h"

#define SEC_NUID_LENGTH_32  32
int secGetNuid(unsigned char* pxNuid)
{
    BERR_Code errCode = BERR_SUCCESS;
    NEXUS_ReadOtpIO otpIo;
    unsigned int i;

    ALOGI("Enter %s \n", __FUNCTION__);

    if(pxNuid ==NULL) {
        ALOGE("%s pxNuid ==NULL \n", __FUNCTION__);
        return -1;
    }

    /* Read the ID from OTP. */
    errCode = NEXUS_Security_ReadOTP (
            NEXUS_OtpCmdReadRegister_eKeyID,
            NEXUS_OtpKeyType_eA,
            &otpIo);

    if (errCode != BERR_SUCCESS) {
        ALOGE("%s NEXUS_Security_ReadOTP Error \n", __FUNCTION__);
        return -1;
    }
    /* NUID is the second Dword */
    BKNI_Memcpy((void *)pxNuid, (void *)&(otpIo.otpKeyIdBuf[0+SEC_NUID_LENGTH_32/8]), SEC_NUID_LENGTH_32/8 );
    //ALOGI("NUID: %02x %02x %02x %02x \n", pxNuid[0], pxNuid[1], pxNuid[2], pxNuid[3]);
    ALOGI("Leave %s\n", __FUNCTION__);

    return 0;
}

