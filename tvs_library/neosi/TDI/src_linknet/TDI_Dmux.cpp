/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: ksh78 $
 * $LastChangedDate: 2014-07-07 15:08:00 +0900 (월, 07 7월 2014) $
 * $LastChangedRevision: 914 $
 * Description:
 * Note:
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>

#include "TDI.h"
#include "Logger.h"
#include "OhPlayerClientSIWrapper.h"
#include <pthread.h>
#include <list>
#include <queue>

static const uint32_t crc_table[256] = {
   0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9, 0x130476dc, 0x17c56b6b,
   0x1a864db2, 0x1e475005, 0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
   0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd, 0x4c11db70, 0x48d0c6c7,
   0x4593e01e, 0x4152fda9, 0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
   0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011, 0x791d4014, 0x7ddc5da3,
   0x709f7b7a, 0x745e66cd, 0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
   0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5, 0xbe2b5b58, 0xbaea46ef,
   0xb7a96036, 0xb3687d81, 0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
   0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49, 0xc7361b4c, 0xc3f706fb,
   0xceb42022, 0xca753d95, 0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
   0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d, 0x34867077, 0x30476dc0,
   0x3d044b19, 0x39c556ae, 0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
   0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16, 0x018aeb13, 0x054bf6a4,
   0x0808d07d, 0x0cc9cdca, 0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde,
   0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02, 0x5e9f46bf, 0x5a5e5b08,
   0x571d7dd1, 0x53dc6066, 0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
   0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e, 0xbfa1b04b, 0xbb60adfc,
   0xb6238b25, 0xb2e29692, 0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
   0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a, 0xe0b41de7, 0xe4750050,
   0xe9362689, 0xedf73b3e, 0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
   0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686, 0xd5b88683, 0xd1799b34,
   0xdc3abded, 0xd8fba05a, 0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637,
   0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb, 0x4f040d56, 0x4bc510e1,
   0x46863638, 0x42472b8f, 0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
   0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47, 0x36194d42, 0x32d850f5,
   0x3f9b762c, 0x3b5a6b9b, 0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
   0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623, 0xf12f560e, 0xf5ee4bb9,
   0xf8ad6d60, 0xfc6c70d7, 0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
   0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f, 0xc423cd6a, 0xc0e2d0dd,
   0xcda1f604, 0xc960ebb3, 0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
   0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b, 0x9b3660c6, 0x9ff77d71,
   0x92b45ba8, 0x9675461f, 0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
   0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640, 0x4e8ee645, 0x4a4ffbf2,
   0x470cdd2b, 0x43cdc09c, 0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8,
   0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24, 0x119b4be9, 0x155a565e,
   0x18197087, 0x1cd86d30, 0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
   0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088, 0x2497d08d, 0x2056cd3a,
   0x2d15ebe3, 0x29d4f654, 0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0,
   0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c, 0xe3a1cbc1, 0xe760d676,
   0xea23f0af, 0xeee2ed18, 0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
   0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0, 0x9abc8bd5, 0x9e7d9662,
   0x933eb0bb, 0x97ffad0c, 0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
   0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4};

uint32_t TSH_Base_DMXCRC32(uint8_t *d, int len, uint32_t crc)
{
//   register int i;
        int i;
        uint8_t *u = d; // Saves '& 0xff'

        for (i = 0; i < len; i++)
                crc = (crc << 8) ^ crc_table[((crc >> 24) ^ *u++)];

        return crc;
}

#define get_syntax_indicator(p)			((p[1]>>7)&0x01)
#define MAX_SECTION_SIZE	4096

bool hasCRC32(uint8_t* p)
{
        if(p[0] == 0x70)
        {
                return false;
        }
        else
        {
                return true;
        }
}

int TSH_Base_DMXIsValidSection(uint8_t *d, int len)
{
        uint32_t CRCvalue;
        CRCvalue = 0xFFFFFFFF;
        if(len > MAX_SECTION_SIZE) {
                printf("crc : Too big section !!! len=%d\n", len);
                return 0;
                //exit(-1);
        }

        if(hasCRC32(d) == false) //if section systax indicator is 0 then the section has no CRC_32 field
                return 1;

        if(TSH_Base_DMXCRC32(d, len, CRCvalue) == 0)
                return 1;

        return 0;
        //return (TSH_Base_DMXCRC32(d, len, CRCvalue) == 0);
}

using namespace tvstorm;

#define MIN_SECTION_LENGTH_BYTES 8
#define SectionLength(p) ((((p[1]&0x0F) << 8) |  p[2]) +3)

extern tvstorm::OhPlayerClientSIWrapper* mOhPlayerClient;
extern int getCurrentFreqKHz();
void playerLock();
void playerUnlock();

typedef struct __FilterWrapper {
        int tdi_filterId;
        int pid_filterId;
        TDI_FilterCallback tdi_cbfn;
        bool reserved;
        uint8_t tidmask;
        uint8_t snmask;
        uint8_t tidval;
        void* pFilter;
} FilterWrapper;

typedef struct __TDIPidFilter {
        int pid_filterId;
        int oh_filterId;
        bool reserved;
        int pid;
        bool enable;
        bool created;
        std::list<int> wrapfilterList;
} TDIPidFilter;

typedef struct __TDIPidFilterMsg {
        uint8_t *pBuff;
        int pid;
        void* pParam;
} TDIPidFilterMsg;

#define MAX_FILTER	60
#define MAX_PID_FILTER	16

FilterWrapper gFilters[MAX_FILTER];
TDIPidFilter gPidFilters[MAX_PID_FILTER];
std::queue<TDIPidFilterMsg> pidfilterQ;

pthread_mutex_t wrapfilterMutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
pthread_mutex_t pidQMutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;

static bool initWrapper = false;

bool checkPidFilter(int sectionFilterHandle, int pid, const unsigned char *sectionData, unsigned int length, void *pReceiver)
{
        TDIPidFilter* filter = (TDIPidFilter*) pReceiver;
        if(filter->enable && filter->oh_filterId == sectionFilterHandle && filter->pid == pid) {
                if(SectionLength(sectionData) == length) {
                        if(TSH_Base_DMXIsValidSection((uint8_t *) sectionData, SectionLength(sectionData))) {
                                return true;
                        } else {
                                //L_TEST("[ERROR] CRC Error\n");
                        }
                } else {
                        //L_TEST("[ERROR] SectionLength(sectionData)=%d length=%d\n", SectionLength(sectionData), length);
                }
        } else {
                //L_TEST("[ERROR] filter->enable=%d\n filter->oh_filterId=%d sectionFilterHandle=%d\n filter->pid=0x%X pid=0x%X\n",
                  //              filter->enable, filter->oh_filterId, sectionFilterHandle, filter->pid, pid);
        }
        return false;
}

int getFreeSectionHandle()
{
        static int freefilteridx = 0;

        for (int i = freefilteridx; i < MAX_FILTER; i++) {
                if(gFilters[i].reserved == false) {
                        freefilteridx = i + 1;
                        if(freefilteridx == MAX_FILTER) {
                                freefilteridx = 0;
                        }
                        return gFilters[i].tdi_filterId;
                }
        }
        for (int i = 0; i < MAX_FILTER; i++) {
                if(gFilters[i].reserved == false) {
                        freefilteridx = i + 1;
                        if(freefilteridx == MAX_FILTER) {
                                freefilteridx = 0;
                        }
                        return gFilters[i].tdi_filterId;
                }
        }
        return 0xffff;
}

int getFreePidHandle()
{
        static int freesearchIdx = 0;

        for (int i = freesearchIdx; i < MAX_PID_FILTER; i++) {
                if(gPidFilters[i].reserved == false) {
                        freesearchIdx = i + 1;
                        if(freesearchIdx == MAX_PID_FILTER) {
                                freesearchIdx = 0;
                        }
                        return gPidFilters[i].pid_filterId;
                }
        }
        for (int i = 0; i < MAX_PID_FILTER; i++) {
                if(gPidFilters[i].reserved == false) {
                        freesearchIdx = i + 1;
                        if(freesearchIdx == MAX_PID_FILTER) {
                                freesearchIdx = 0;
                        }
                        return gPidFilters[i].pid_filterId;
                }
        }
        return 0xffff;
}

int getpidHandleCountFromOhHandle(int ohHandle, int filterId)
{
        for (int i = 0; i < MAX_FILTER; i++) {
                if(ohHandle == gFilters[i].pid_filterId && (filterId != gFilters[i].tdi_filterId)) {
                        return 1;
                }
        }
        return 0;
}

int getOhHandleFromPid(int pid, TDIPidFilter** ppPidFilter)
{
        for (int i = 0; i < MAX_PID_FILTER; i++) {
                if(gPidFilters[i].pid == pid) {
                        *ppPidFilter = &gPidFilters[i];
                        return gPidFilters[i].oh_filterId;
                }
        }
        return -1;
}
#define TableIDExt(p) (((uint16_t)p[3] << 8) |((uint16_t)p[4]))
static void OhPlayerSectionCallback(const int demuxNumber, const int sectionFilterHandle, const int event_id, const int pid,
                const unsigned char *sectionData, const unsigned int length, void *pReceiver)
{
        if(length < MIN_SECTION_LENGTH_BYTES) {
                //L_WARN("invalid sectionData\n");
                return;
        }
        if(event_id == kSectionFilterSendData) {
#if 0
                if(sectionData[0] == 0x02) { // PMT TableId
                        L_TEST("handle=%d pid=0x%x prognum=0x%X\n", sectionFilterHandle, pid, TableIDExt(sectionData));
                }
                else if(sectionData[0] == 0x00) { // PAT TableId
                        L_TEST("handle=%d pid=0x%x tsid=0x%X\n", sectionFilterHandle, pid, TableIDExt(sectionData));
                }
#endif
                if(checkPidFilter(sectionFilterHandle, pid, sectionData, length, pReceiver)) {
                        TDIPidFilterMsg msg;
                        msg.pBuff = new uint8_t[length];
                        if(msg.pBuff) {
                                memcpy(msg.pBuff, sectionData, length);
                                msg.pParam = pReceiver;
                                msg.pid = pid;
                                pthread_mutex_lock(&pidQMutex);
                                pidfilterQ.push(msg);
                                pthread_mutex_unlock(&pidQMutex);
                        }
                }
        }
}

int siUSleep(int usec)
{
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = usec;
        
        return select(0, NULL, NULL, NULL, &tv);
}

void* tTDIDemux(void* param)
{
        int cnt = 0;

        while (1) {
                pthread_mutex_lock(&pidQMutex);
                if(pidfilterQ.empty()) {
                        pthread_mutex_unlock(&pidQMutex);
                        siUSleep(10000);
                } else {
                        TDIPidFilterMsg msg;
                        msg = pidfilterQ.front();
                        pidfilterQ.pop();
                        pthread_mutex_unlock(&pidQMutex);

                        pthread_mutex_lock(&wrapfilterMutex);
                        TDIPidFilter* pPidFilter = (TDIPidFilter*) msg.pParam;
                        if(pPidFilter->enable && pPidFilter->pid == msg.pid) {
                                std::list<int>::iterator loopitr;
                                for (loopitr = pPidFilter->wrapfilterList.begin(); loopitr != pPidFilter->wrapfilterList.end(); loopitr++) {
                                        int handle = *loopitr;
                                        FilterWrapper *filter = &gFilters[handle - 1];

                                        if((filter->tidval == (msg.pBuff[0] & filter->tidmask)) && ((msg.pBuff[6] & filter->snmask) == 0)) {
                                                TDI_FilterCallback tdifunc = filter->tdi_cbfn;
                                                if(tdifunc) {
                                                        tdifunc(msg.pid, (BYTE*) msg.pBuff, filter->pFilter);
                                                }
                                        }
                                }
                        }
                        delete[] msg.pBuff;
                        pthread_mutex_unlock(&wrapfilterMutex);
                }
        }

        L_INFO("TDI Demux thread close..\n\n");
        pthread_exit (NULL);
        return NULL;

}

TDI_Error TDI_Filter_Open(int demuxId, int* filterId)
{
        L_INFO("====IN===\n");
        TDI_Error err = TDI_SUCCESS;
        int handle;
        pthread_mutex_lock(&wrapfilterMutex);
        playerLock();
        if(mOhPlayerClient == NULL) {
                TDI_System_Open(false);
        }
        playerUnlock();

        handle = getFreeSectionHandle();
        if(handle != 0xffff) {
                *filterId = handle;
                gFilters[handle - 1].reserved = true;
        } else {
                pthread_mutex_unlock(&wrapfilterMutex);
                *filterId = 0xffff;
                return TDI_ERROR;
        }
        pthread_mutex_unlock(&wrapfilterMutex);

        return err;
}

TDI_Error TDI_Filter_Close(int filterId)
{
        L_INFO("====IN filterId=%d===\n",filterId);
        if(filterId <= 0 || filterId >= MAX_FILTER) {
                L_TEST("invalid filterId\n");
                return TDI_ERROR;
        }
        pthread_mutex_lock(&wrapfilterMutex);
        int pid_handle = gFilters[filterId - 1].pid_filterId;
        TDI_Error err = TDI_SUCCESS;

        if(pid_handle <= 0 || pid_handle > MAX_PID_FILTER) {
                L_TEST("invalid filterId\n");
                pthread_mutex_unlock(&wrapfilterMutex);
                return TDI_ERROR;
        }

        if(getpidHandleCountFromOhHandle(pid_handle, filterId) == 0) {
                L_INFO("pPidFilter->oh_filterId=%d pid=0x%X\n",gPidFilters[pid_handle-1].oh_filterId,gPidFilters[pid_handle-1].pid);
                int oh_handle = gPidFilters[pid_handle - 1].oh_filterId;
                if(oh_handle >= 0) {
                        playerLock();
                        if(mOhPlayerClient) {
                                L_TEST("begin_OhPlayerClient_enableSectionFilter(%d,false,%d) pid=0x%X\n",
                                                oh_handle, getCurrentFreqKHz(), gPidFilters[pid_handle-1].pid);
                                mOhPlayerClient->enableSectionFilter(oh_handle, false, getCurrentFreqKHz());
                                L_TEST("end_OhPlayerClient_enableSectionFilter(%d,false,%d) pid=0x%X\n",
                                                oh_handle, getCurrentFreqKHz(), gPidFilters[pid_handle-1].pid);
                                                
                                L_TEST("begin_OhPlayerClient_destroySectionFilter(%d,%d) pid=0x%X\n",
                                                oh_handle, getCurrentFreqKHz(), gPidFilters[pid_handle-1].pid);
                                mOhPlayerClient->destroySectionFilter(oh_handle, getCurrentFreqKHz());
                                L_TEST("end_OhPlayerClient_destroySectionFilter(%d,%d) pid=0x%X\n",
                                                oh_handle, getCurrentFreqKHz(), gPidFilters[pid_handle-1].pid);
                        }
                        playerUnlock();
                }

                gPidFilters[pid_handle - 1].oh_filterId = -1;
                gPidFilters[pid_handle - 1].pid = 0xffff;
                gPidFilters[pid_handle - 1].reserved = false;
                gPidFilters[pid_handle - 1].enable = false;
                gPidFilters[pid_handle - 1].created = false;
        }
        gFilters[filterId - 1].reserved = false;
        gFilters[filterId - 1].tdi_cbfn = NULL;
        gFilters[filterId - 1].pid_filterId = 0xffff;
        gFilters[filterId - 1].tidmask = 0xFF;
        gFilters[filterId - 1].snmask = 0xFF;
        gFilters[filterId - 1].tidval = 0xFF;
        pthread_mutex_unlock(&wrapfilterMutex);
        return err;
}

TDI_Error TDI_Filter_Start(int filterId, uint16_t pid, TDI_FILTER_MASK *pMask, TDI_FilterCallback cbFunc, void* pReceiver)
{
        L_INFO("====IN pid=0x%X tid=0x%X===\n",pid,pMask->comp[0]);
        if(filterId <= 0 || filterId >= MAX_FILTER) {
                L_TEST("invalid filterId\n");
                return TDI_ERROR;
        }

        TDI_Error err = TDI_SUCCESS;
        TDIPidFilter* pPidFilter = NULL;
        pthread_mutex_lock(&wrapfilterMutex);

        int oh_handle = getOhHandleFromPid(pid, &pPidFilter);

        if(oh_handle < 0) {
                int ohpid = (int) pid;
                unsigned int timeoutUsec = 50000 * 1000; // eschoi_20140111 | test 5s/section
                SectionFilterParameter_t aParam;

                int pidhandle = getFreePidHandle();
                if(pidhandle == 0xFFFF) {
                        L_TEST("no free pidhandle\n");
                        pthread_mutex_unlock(&wrapfilterMutex);
                        return TDI_ERROR;
                }
                gPidFilters[pidhandle - 1].reserved = true;
                pPidFilter = &gPidFilters[pidhandle - 1];

                aParam.pid = pid;
                aParam.filterType = DEMUX_FILTER_SECTION_TYPE;
                aParam.filterOption = DEMUX_FILTER_CONTINUOUS;

                memset(aParam.filterMatch, 0, 12);
                memset(aParam.filterMask, 0, 12);
                memset(aParam.negativeMatch, 0, 12);
#if 0
                // ksh_20140516 | knowhow to set tid's mask using the same pid
                aParam.filterMatch[0] = pMask->comp[0];
                aParam.filterMask[0] = 0xca;
#endif
                // ksh_20140506 | case PMT only, used version-change mode
                if(pMask->comp[0] == 0x02) { // PMT TableId
                        aParam.filterOption = DEMUX_FILTER_VERSION_CHANGE;
                        aParam.filterMatch[0] = 0x02;
                        aParam.filterMask[0] = 0xFF;
                }
                else if(pMask->comp[0] == 0x0)
                {
                        aParam.filterOption = DEMUX_FILTER_ONE_SHOT;
                        aParam.filterMatch[0] = 0x00;
                        aParam.filterMask[0] = 0xFF;
                }
#if 0
                // ksh_20140503 | case PMT only, used ohe-shot mode + negative mask setting, why? marvell issue
                if(pMask->comp[0] == 0x02 && pMask->mask[5]!=0) { // PMT TableId
                        aParam.filterOption = DEMUX_FILTER_ONE_SHOT;
                        aParam.filterMatch[0] = pMask->comp[0];
                        aParam.filterMask[0] = pMask->mask[0];

                        aParam.negativeMatch[5] = pMask->comp[5];
                        aParam.filterMask[5] = pMask->mask[5];
                }
#endif

                playerLock();
                if(mOhPlayerClient) {
                        L_TEST("begin_OhPlayerClient_createSectionFilter(pid=0x%X,tid=0x%X,freq=%d)\n", pid, pMask->comp[0], getCurrentFreqKHz());
                        oh_handle = mOhPlayerClient->createSectionFilter(&aParam, &OhPlayerSectionCallback, (void*) pPidFilter, timeoutUsec,
                                        getCurrentFreqKHz());
                        L_TEST("end_OhPlayerClient_createSectionFilter(pid=0x%X,tid=0x%X,freq=%d) oh_handle=%d\n",
                                        pid, pMask->comp[0], getCurrentFreqKHz(), oh_handle);
                }
                playerUnlock();

                if(oh_handle < 0) {
                        err = TDI_ERROR;
                } else {
                        err = TDI_SUCCESS;
                }

                pPidFilter->oh_filterId = oh_handle;
                pPidFilter->pid = pid;

                //L_INFO("create section filter pPidFilter->oh_filterId=%d pid=0x%X\n",pPidFilter->oh_filterId,pPidFilter->pid);
        }

        if(pPidFilter == NULL) {
                L_TEST("no free pidhandle\n");
                pthread_mutex_unlock(&wrapfilterMutex);
                return TDI_ERROR;
        }
        
        gFilters[filterId - 1].tdi_cbfn = cbFunc;
        gFilters[filterId - 1].pid_filterId = pPidFilter->pid_filterId;

        //work around 처리
        gFilters[filterId - 1].tidmask = pMask->mask[0];
        gFilters[filterId - 1].snmask = pMask->mask[6];
        gFilters[filterId - 1].tidval = (pMask->mask[0] & pMask->comp[0]);
        gFilters[filterId - 1].pFilter = pReceiver;
        
        pPidFilter->wrapfilterList.push_back(filterId);

        if(pPidFilter->enable == false && oh_handle >= 0) {
                pPidFilter->enable = true;
                if(pPidFilter->created == false)
                {
                        pPidFilter->created = true;
                        playerLock();
                        if(mOhPlayerClient) {
                                L_TEST("begin_OhPlayerClient_enableSectionFilter(%d,true,%d) pid=0x%X\n", oh_handle, getCurrentFreqKHz(), pPidFilter->pid);
                                mOhPlayerClient->enableSectionFilter(oh_handle, true, getCurrentFreqKHz());
                                L_TEST("end_OhPlayerClient_enableSectionFilter(%d,true,%d) pid=0x%X\n", oh_handle, getCurrentFreqKHz(), pPidFilter->pid);
                        }
                        playerUnlock();
                }
        }

        pthread_mutex_unlock(&wrapfilterMutex);

        return err;
}

TDI_Error TDI_Filter_Stop(int filterId)
{
        L_INFO("====IN filterId=%d===\n",filterId);
        if(filterId <= 0 || filterId >= MAX_FILTER) {
                L_TEST("invalid filterId\n");
                return TDI_ERROR;
        }
        pthread_mutex_lock(&wrapfilterMutex);
        int pid_handle = gFilters[filterId - 1].pid_filterId;
        if(pid_handle <= 0 || pid_handle > MAX_PID_FILTER) {
                L_TEST("invalid filterId\n");
                pthread_mutex_unlock(&wrapfilterMutex);
                return TDI_ERROR;
        }
        TDIPidFilter* pPidFilter = &gPidFilters[pid_handle - 1];
        TDI_Error err = TDI_SUCCESS;

        std::list<int>::iterator rmvitr; // wrapfilterList;
        std::list<int>::iterator loopitr;
        bool bFind = false;
        for (loopitr = pPidFilter->wrapfilterList.begin(); loopitr != pPidFilter->wrapfilterList.end(); loopitr++) {
                if(filterId == *loopitr) {
                        bFind = true;
                        rmvitr = loopitr;
                        break;
                }
        }
        if(bFind) {
                pPidFilter->wrapfilterList.erase(rmvitr);
                if(pPidFilter->wrapfilterList.size() == 0 && pPidFilter->enable == true) {
#if 0
                        //L_INFO("pPidFilter->oh_filterId=%d pid=0x%X\n",pPidFilter->oh_filterId,pPidFilter->pid);
                        playerLock();
                        if(mOhPlayerClient) {
                                L_TEST("begin_OhPlayerClient_enableSectionFilter(%d,false,%d) pid=0x%X\n",
                                                pPidFilter->oh_filterId, getCurrentFreqKHz(), pPidFilter->pid);
                                mOhPlayerClient->enableSectionFilter(pPidFilter->oh_filterId, false, getCurrentFreqKHz());
                                L_TEST("end_OhPlayerClient_enableSectionFilter(%d,false,%d) pid=0x%X\n",
                                                pPidFilter->oh_filterId, getCurrentFreqKHz(), pPidFilter->pid);
                        }
                        playerUnlock();
#endif
                        pPidFilter->enable = false;
                }
        }
        pthread_mutex_unlock(&wrapfilterMutex);
        return err;
}

TDI_Error TDI_Demux_Open(IN int demuxId, IN int tunerId)
{
        L_INFO("====IN===\n");
        TDI_Error err = TDI_SUCCESS;
        static bool createThread = false;

        if(initWrapper == false) {
                initWrapper = true;
                for (int i = 0; i < MAX_FILTER; i++) {
                        gFilters[i].tdi_filterId = i + 1;
                        gFilters[i].pid_filterId = 0xffff;
                        gFilters[i].tdi_cbfn = NULL;
                        gFilters[i].reserved = false;
                        gFilters[i].tidmask = 0xFF;
                        gFilters[i].snmask = 0xFF;
                        gFilters[i].tidval = 0xFF;
                }

                for (int i = 0; i < MAX_PID_FILTER; i++) {
                        gPidFilters[i].pid_filterId = i + 1;
                        gPidFilters[i].oh_filterId = -1;
                        gPidFilters[i].pid = 0xffff;
                        gPidFilters[i].reserved = false;
                        gPidFilters[i].enable = false;
                        gPidFilters[i].created = false;
                }
        }

        if(createThread == false) {
                createThread = true;
                pthread_t task;
                pthread_attr_t attr;
                pthread_attr_init(&attr);
                pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
                pthread_create(&task, &attr, tTDIDemux, NULL);
                pthread_attr_destroy(&attr);
        }

        return err;
}

TDI_Error TDI_Demux_Close(IN int demuxId)
{
        TDI_Error err = TDI_SUCCESS;

        return err;
}

TDI_Error TDI_Demux_Start(int demuxId)
{
        TDI_Error err = TDI_SUCCESS;

        return err;
}

TDI_Error TDI_Demux_Stop(int demuxId)
{
        TDI_Error err = TDI_SUCCESS;

        return err;
}

TDI_Error TDI_Demux_Reset()
{
        TDI_Error err = TDI_SUCCESS;

        pthread_mutex_lock(&wrapfilterMutex);

        for (int i = 0; i < MAX_PID_FILTER; i++) {
                if(gPidFilters[i].oh_filterId >= 0) {
                        playerLock();
                        if(mOhPlayerClient) {
                                L_TEST("begin_OhPlayerClient_enableSectionFilter(%d,false,%d) pid=0x%X\n",
                                                gPidFilters[i].oh_filterId, getCurrentFreqKHz(), gPidFilters[i].pid);
                                mOhPlayerClient->enableSectionFilter(gPidFilters[i].oh_filterId, false, getCurrentFreqKHz());
                                L_TEST("end_OhPlayerClient_enableSectionFilter(%d,false,%d) pid=0x%X\n",
                                                gPidFilters[i].oh_filterId, getCurrentFreqKHz(), gPidFilters[i].pid);
                                                
                                L_TEST("begin_OhPlayerClient_destroySectionFilter(%d,%d) pid=0x%X\n",
                                                gPidFilters[i].oh_filterId, getCurrentFreqKHz(), gPidFilters[i].pid);
                                mOhPlayerClient->destroySectionFilter(gPidFilters[i].oh_filterId, getCurrentFreqKHz());
                                L_TEST("end_OhPlayerClient_destroySectionFilter(%d,%d) pid=0x%X\n",
                                                gPidFilters[i].oh_filterId, getCurrentFreqKHz(), gPidFilters[i].pid);
                        }
                        playerUnlock();
                        gPidFilters[i].oh_filterId = -1;
                }
                if(gPidFilters[i].reserved) {
                        gPidFilters[i].pid = 0xffff;
                        gPidFilters[i].reserved = false;
                        gPidFilters[i].enable = false;
                        gPidFilters[i].created = false;
                        gPidFilters[i].wrapfilterList.clear();
                }
        }

        for (int i = 0; i < MAX_FILTER; i++) {
                if(gFilters[i].reserved) {
                        gFilters[i].pid_filterId = 0xffff;
                        gFilters[i].tdi_cbfn = NULL;
                        gFilters[i].reserved = false;
                        gFilters[i].tidmask = 0xFF;
                        gFilters[i].snmask = 0xFF;
                        gFilters[i].tidval = 0xFF;
                }
        }

        pthread_mutex_unlock(&wrapfilterMutex);

        pthread_mutex_lock(&pidQMutex);
        while (!pidfilterQ.empty()) {
                TDIPidFilterMsg msg;
                msg = pidfilterQ.front();
                pidfilterQ.pop();
                delete[] msg.pBuff;
        }
        pthread_mutex_unlock(&pidQMutex);

        return err;
}
