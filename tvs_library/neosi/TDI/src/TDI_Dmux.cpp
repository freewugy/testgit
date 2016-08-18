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
#include <memory.h>

#include "TDI_Dmux.h"
#include "TDI_SWSectionFilter.h"

uint32_t TDI_Dmux_Get_Available_Count()
{
	fprintf(stdout,"TDI_Dmux_Get_Available_Count called \n");
	return 1;
}
TDI_DMUX_RESULT TDI_Dmux_Reserve(int *hdl, int tuner_handle)
{
	*hdl = 0;
	fprintf(stdout,"TDI_Dmux_Reserve called , tuner handle[%d], handle[%d]\n",tuner_handle, *hdl);

	InitSWDemux();

	return (TDI_DMUX_RESULT)0;
}	
TDI_DMUX_RESULT TDI_Dmux_Start(int hdl)
{
	fprintf(stdout,"TDI_Dmux_Start called ,handle[%d]\n",hdl);
	StartSWDemux();
	return (TDI_DMUX_RESULT)0;
}	
TDI_DMUX_RESULT TDI_Dmux_Stop(int hdl)
{
	fprintf(stdout,"TDI_Dmux_Stop called ,handle[%d]\n",hdl);
	StopSWDemux();
	return (TDI_DMUX_RESULT)0;
}	
TDI_DMUX_RESULT TDI_Dmux_Status(int hdl, TDI_DMUX_STATUS *status)
{
	fprintf(stdout,"TDI_Dmux_Status called handle[%d], status[%d] \n",hdl, *status);
	return (TDI_DMUX_RESULT)0;
}	
TDI_DMUX_RESULT TDI_Dmux_Release(int hdl)
{
	fprintf(stdout,"TDI_Dmux_Release called ,handle[%d]\n",hdl);
	CloseSWDemux();
	return (TDI_DMUX_RESULT)0;
}	
int32_t TDI_Filter_Get_Available_Count(int hdl)
{
	fprintf(stdout,"TDI_Filter_Get_Available_Count called handle[%d]\n", hdl);
	return 0;
}	

TDI_FILTER_RESULT  TDI_Filter_Status(int hdl, TDI_FILTER_STATUS *status)
{
	fprintf(stdout,"TDI_Filter_Status called  handle[%d], status[%d] \n",hdl, *status);
	return (TDI_FILTER_RESULT)0;
}	


TDI_Error TDI_Filter_Start(int filterId, uint16_t pid,
		TDI_FILTER_MASK *pMask, TDI_FilterCallback cbFunc, void* pReceiver)
{
	TRESULT err;
	err = SetPIDSWSectionFilter(filterId,pid);
	if(err != TOK)
	{
		return TDI_ERROR;
	}
	
	err = SetSWSectionFilter(filterId,pid, pMask,(void*) cbFunc,pReceiver);
	if(err != TOK)
	{
		return TDI_ERROR;
	}
	err = EnableSWSectionFilter(filterId);
	if(err == TOK)
	{
		return TDI_SUCCESS;
	}
	else
	{
		return TDI_ERROR;
	}
}

TDI_Error TDI_Filter_Stop(int filterId)
{
	DisableSWSectionFilter(filterId);
	return (TDI_Error) 0;
}

TDI_Error TDI_Filter_Open(int demuxId, int* filterId)
{
	TRESULT err;
	err = OpenSWSectionFilter((uint32_t*)filterId, 0);
	if (err == TOK)
	{
		return TDI_SUCCESS;
	}
	return TDI_ERROR;
}

TDI_Error TDI_Filter_Close(int filterId)
{
	CloseSWSectionFilter(filterId);
	return TDI_SUCCESS;
}

TDI_Error TDI_Demux_Open(IN int demuxId, IN int tunerId)
{
	int hdl=0;
	TDI_Dmux_Reserve(&hdl, tunerId);
	return (TDI_Error)0;
}

TDI_Error TDI_Demux_Close(IN int demuxId)
{
	int hdl=0;
	TDI_Dmux_Release(hdl);
	return (TDI_Error)0;
}

TDI_Error TDI_Demux_Start(int demuxId)
{
	int hdl=0;
	TDI_Dmux_Start(hdl);
	return (TDI_Error)0;
}

TDI_Error TDI_Demux_Stop(int demuxId)
{
	int hdl=0;
	TDI_Dmux_Stop(hdl);
	return (TDI_Error)0;
}
