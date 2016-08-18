/******************************************************************************
 *    (c) 2015 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 *****************************************************************************/
#define LOG_TAG "nuid_crypt"

#include <stdio.h>
#include "bstd.h"
#include "nexus_platform.h"
#include "nexus_ipc_client_factory.h"
#include "bkni.h"
#include "nuid_crypt.h"

#define NEXUS_CLI_NAME      "tvstorm_nuidcrypt_keygen"

NexusIPCClientBase *g_ipcClient = NULL;
NexusClientContext *g_nexusClient = NULL;
/*
* nexus join
* input : 
*           void
* output :
*           return : TVSTORM_ERROR is can not join nexus client
*		return : TVSTORM_OK is join
*/
int nexus_init(void)
{
    b_refsw_client_client_configuration config;
    b_refsw_client_client_info client_info;
    
    g_ipcClient = NexusIPCClientFactory::getClient(NEXUS_CLI_NAME);
    if (g_ipcClient == NULL) {
        printf("could not get NexusIPCClient!\n");
        return -1;
    }
    
    BKNI_Memset(&config, 0, sizeof(config));
    BKNI_Snprintf(config.name.string,sizeof(config.name.string), NEXUS_CLI_NAME);
    g_nexusClient = g_ipcClient->createClientContext(&config);
    if (g_nexusClient == NULL) {
        printf("%s: Could not create Nexus Client Context!!!", __FUNCTION__); 
        delete g_ipcClient;
        return -2;
    }
    
    return 0;

}

/*
* eeprom nexus release
* input : 
*           void
* output :
*           return : TVSTORM_ERROR is can not release nexus client
*		return : TVSTORM_OK is release
*/
int nexus_uninit(void)
{
    g_ipcClient->disconnectClientResources(g_nexusClient);
    g_ipcClient->destroyClientContext(g_nexusClient);
    g_nexusClient = NULL;
    delete g_ipcClient;

    return 0;
}

int main(int argc, char **argv)
{
    int ret = 0;

    printf("NUID crypt key generator v1.1 2015.10.26\n\n");

    if(nexus_init()) return -1;

    ret = generate_encrypt_tvs_sys_key();
    if(ret)
    {
        printf("tvs_sys.key generation failed(%d)! \n", ret);
    }
    else
    {
        printf("tvs_sys.key created successfully! \n");
    }

    nexus_uninit();

    return 0;
}
