/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: eschoi $
 * $LastChangedDate: 2014-06-03 13:58:35 +0900 (화, 03 6월 2014) $
 * $LastChangedRevision: 837 $
 * Description:
 * Note:
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include <pthread.h>
#include <list>
#include <queue>
#include <unistd.h>

#include "Logger.h"
#include "TDI_SWSectionFilter.h"
#include "TDI_TSParser.h"

#define SWDMX_HANDLE_BASE		32

static bool bSWDmxInit = false;
static bool bSetFilePath=false;
static bool bSWTaskClose=false;
static bool bUseSWDemux=true;

typedef struct
{
	int cmd;
	uint32_t handle;
}SwFilterQMsg;

std::queue<SwFilterQMsg> SwFilterQ;
std::queue<uint8_t*> tsQ;
std::list<cFilter*> SwFilterList;

#ifdef ANDROID
pthread_mutex_t SwFilterLockID = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
#else    
pthread_mutex_t SwFilterLockID = PTHREAD_MUTEX_INITIALIZER;
#endif

static cFilter *pSWSectionFilter[MAX_FILTER_LIST]={NULL,};

cFilter* GetSWSectionFilter(uint32_t hSF);

pthread_t CreatSWSFTask(void *(taskfn)(void*), uint32_t priority)
{
	static bool created = false;
	if(created) return NULL;
	created = true;
	
	pthread_t task;
#if 0	
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create (&task, &attr, taskfn, NULL);
	pthread_attr_destroy(&attr);
#else
	pthread_create (&task, NULL, taskfn, NULL);
#endif	
	return task;
}
void DeleteSWSFTask()
{
	
}

void* tSWDemux(void* param)
{
	int cnt = 0;

	TRESULT err;
	uint32_t hSF;
	uint32_t dummy;
	
	initTsParser();

	while(1)
	{
		pthread_mutex_lock(&SwFilterLockID);
		bool swfQ = !SwFilterQ.empty();
		pthread_mutex_unlock(&SwFilterLockID);
		while(swfQ)
		{
			SwFilterQMsg msg;
			pthread_mutex_lock(&SwFilterLockID);
			msg = SwFilterQ.front();
			SwFilterQ.pop();
			swfQ = !SwFilterQ.empty();
			pthread_mutex_unlock(&SwFilterLockID);
			hSF = msg.handle;
			if(msg.cmd == 0)
			{
				cFilter* filter = GetSWSectionFilter(hSF);
				if(filter)
				{
					std::list<cFilter*>::iterator itr,finditr; 
					for(itr=SwFilterList.begin(); itr!=SwFilterList.end(); itr++)
					{
						if(filter == *itr)
						{
							SwFilterList.erase(itr);
							break;
						}
					}
					filter->bEnable = false;
					filter->bUsed = false;
					filter->callback = NULL;
					//CloseSWSectionFilterEx(hSF);
				}
			}
			else if(msg.cmd == 1)
			{
				cFilter* filter = GetSWSectionFilter(hSF);
				if(filter)
					SwFilterList.push_back(filter);
			}
			else if(msg.cmd == 2)
			{
				cFilter* filter = GetSWSectionFilter(hSF);
				if(filter)
				{
					std::list<cFilter*>::iterator itr,finditr; 
					for(itr=SwFilterList.begin(); itr!=SwFilterList.end(); itr++)
					{
						if(filter == *itr)
						{
							SwFilterList.erase(itr);
							break;
						}
					}
				}
			}
		}		
		//pthread_mutex_lock(&SwFilterLockID);
		ReadTSBuffer();
		//pthread_mutex_unlock(&SwFilterLockID);
	}
	pthread_exit(NULL);

	printf("SWDemux thread close..\n\n");

	return NULL;

}

void initSwFilter(cFilter* filter)
{
	
}

void InitSWDemux()
{
	int i;
	bSWTaskClose=false;
	
	if(g_pSWTSDecoder == NULL)
		g_pSWTSDecoder = (pTSDecoder)malloc(sizeof(TSDecoder) * MAX_FILTER_LIST);

	for(i=0;i<MAX_FILTER_LIST;i++)
	{
		pSWSectionFilter[i] =(cFilter*)malloc(sizeof(cFilter));
		pSWSectionFilter[i]->bUsed = false;
		pSWSectionFilter[i]->bEnable = false;
		pSWSectionFilter[i]->callback = NULL;
		pSWSectionFilter[i]->hSF = TINVALID_HANDLE;
		pSWSectionFilter[i]->pid = 0;
		memset(&(pSWSectionFilter[i]->param),0,sizeof(TDI_FILTER_MASK));
		pSWSectionFilter[i]->tsDecoder = (TSDecoder*)(&g_pSWTSDecoder[i]);
		initTSDecoder(&g_pSWTSDecoder[i]);
	}

	CreatSWSFTask(tSWDemux,NULL);

	bSWDmxInit = true;
	bSetFilePath=false;

}
void CloseSWDemux()
{
	DeleteSWSFTask();

	bSWDmxInit = false;
}
void StartSWDemux()
{
	if(bSWDmxInit==false)
		InitSWDemux();

}
void StopSWDemux()
{
	if(bSWDmxInit==false)
		InitSWDemux();

}

TRESULT isMatchedSection( cFilter *thiz, uint8_t *isection, int sectionlen )
{
	
#if 0
	cFilter *filter = (cFilter*) thiz;
	int i;
	uint8_t *pBuffer1 = (uint8_t*)isection;
	for(i=0;i<DTVMW_MAX_SWDMX_FILTER_MASK_LEN;i++)
	{
		if(filter->param.mask[i] == 0x00) continue;
		
		if((filter->param.comp[i]) != (filter->param.mask[i] & pBuffer1[i]))
		{
			return TERR_FAILURE;
		}
	}
#else
	if((thiz->param.comp[0]) != (thiz->param.mask[0] & isection[0]))
	{
		return TERR_FAILURE;
	}
	if(thiz->param.mask[6])
	{
		if((thiz->param.comp[6]) != (thiz->param.mask[6] & isection[6]))
		{
			return TERR_FAILURE;
		}
	}
#endif	
	return TOK;
}

TRESULT OpenSWSectionFilter(uint32_t *phSF, uint16_t PID)
{
	if(bSWDmxInit==false)
		InitSWDemux();
	TRESULT err=TERR_NORESOURCE;
	
	for(int i=0;i<MAX_FILTER_LIST;i++)
	{
		if(pSWSectionFilter[i]!=NULL && pSWSectionFilter[i]->bUsed == false)
		{
			pSWSectionFilter[i]->hSF = SWDMX_HANDLE_BASE+i;
			pSWSectionFilter[i]->bUsed = true;
			pSWSectionFilter[i]->pid = PID;

			*phSF = pSWSectionFilter[i]->hSF;
			return TOK;
		}
	}
	
	return err;
}


cFilter* GetSWSectionFilter(uint32_t hSF)
{
	int i= hSF-SWDMX_HANDLE_BASE;
	
	if(i<0 || i>=MAX_FILTER_LIST)
	{
		printf("[GetSWSectionFilter] invalid handle i=%d\n",i);
		return NULL;
	}
	
	if(pSWSectionFilter[i]!=NULL && pSWSectionFilter[i]->bUsed == true)
	{
		if(pSWSectionFilter[i]->hSF == hSF)
		{
			return pSWSectionFilter[i];
		}
	}
	
	return NULL;
}
TRESULT SetPIDSWSectionFilter(uint32_t hSF,uint16_t PID)
{
	if(bSWDmxInit==false)
		InitSWDemux();

	TRESULT err=TOK;

	cFilter *filter = GetSWSectionFilter(hSF);
	if(filter == NULL)
	{
		err = TERR_INVHANDLE;
		return err;
	}
	filter->pid = PID;
	return err;
}

TRESULT SetSWSectionFilter(uint32_t hSF,uint16_t pid, TDI_FILTER_MASK *psfentry,void* cbfn,void* userparam)
{
	
	int i;
	if(bSWDmxInit==false)
		InitSWDemux();

	TRESULT err=TOK;

	cFilter *filter = GetSWSectionFilter(hSF);
	if(filter == NULL)
	{
		err = TERR_INVHANDLE;
		return err;
	}
	filter->callback = (TDI_FilterCallback)cbfn;

	filter->param.mask[0] = psfentry->mask[0];
	filter->param.comp[0] = (psfentry->mask[0]&psfentry->comp[0]);
	
	filter->param.mask[6] = psfentry->mask[6];
	filter->param.comp[6] = (psfentry->mask[6]&psfentry->comp[6]);
	filter->userParam = userparam;
	return err;
}

TRESULT EnableSWSectionFilter(uint32_t hSF)
{
	if(bSWDmxInit==false)
		InitSWDemux();

	TRESULT err = TOK;
	cFilter *filter = GetSWSectionFilter(hSF);
	if(filter == NULL)
	{
		err = TERR_INVHANDLE;
		return err;
	}

	filter->bEnable = true;

	initTSDecoder(((TSDecoder*)(filter->tsDecoder)));
	
	SwFilterQMsg msg;
	msg.cmd = 1;
	msg.handle = hSF;
	pthread_mutex_lock(&SwFilterLockID);
	SwFilterQ.push(msg);
	pthread_mutex_unlock(&SwFilterLockID);
		
	return err;
}

TRESULT DisableSWSectionFilter(uint32_t hSF)
{
	if(bSWDmxInit==false)
		InitSWDemux();
	TRESULT err = TOK;
	cFilter *filter = GetSWSectionFilter(hSF);

	if(filter == NULL)
	{
		err = TERR_INVHANDLE;
		return err;
	}

	filter->bEnable = false;
	
	SwFilterQMsg msg;
	msg.cmd = 2;
	msg.handle = hSF;
	pthread_mutex_lock(&SwFilterLockID);
	SwFilterQ.push(msg);
	pthread_mutex_unlock(&SwFilterLockID);

	return err;
}

TRESULT CloseSWSectionFilter(uint32_t hSF)
{
	TRESULT err = TOK;
	cFilter *filter = GetSWSectionFilter(hSF);

	if(filter == NULL)
	{
		err = TERR_INVHANDLE;
		return err;
	}
	
	SwFilterQMsg msg;
	msg.cmd = 0;
	msg.handle = hSF;
	pthread_mutex_lock(&SwFilterLockID);
	SwFilterQ.push(msg);
	pthread_mutex_unlock(&SwFilterLockID);

	return err;
}

TRESULT CloseSWSectionFilterEx(uint32_t hSF)
{
	TRESULT err = TOK;

	cFilter *filter = GetSWSectionFilter(hSF);

	if(filter == NULL)
	{
		err = TERR_INVHANDLE;
		return err;
	}

	filter->bEnable = false;
	filter->bUsed = false;
	filter->callback = NULL;
	//filter->hSF = TINVALID_HANDLE;
	//filter->pid = 0;
	//memset(&(filter->param),0,sizeof(TDI_FILTER_MASK));
	//initTSDecoder(((TSDecoder*)(filter->tsDecoder)));
	return err;
}

