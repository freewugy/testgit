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

#ifndef __SW_FILTER_H
#define __SW_FILTER_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
////////////////////////////////////////////////////////////////////////////////
#include "TST_Error.h"

////////////////////////////////////////////////////////////////////////////////
#include "TDI.h"
#include "TDI_Dmux.h"

#define TINVALID_HANDLE 0xFFFF

typedef struct _stAdaptationField {
	uint8_t adaptation_field_length	;
	uint8_t discontinuity_indicator;
	uint8_t data[183];							//-4 : 4Bytes TS header, -1 : adaptation_field_length, discontinuity_indicator
} ifAdaptationField_t;

typedef struct {
	//unsigned char data[188];

	uint8_t payload_unit_start_indicator;	// '1' - start of PES Packet
													//  or PSI Section including 'pointer_field'
	uint8_t transport_scrambling_control;	//00-clear, //01-reserved,
													//10-even word scrambling,
													//11-odd word scrambling
	uint8_t adaptation_field_control;	//0x10 || 0x11
	uint8_t continuity_counter;
} ifTSPacketHeader,*pifTSPacketHeader;

typedef struct {
	ifTSPacketHeader Header;
	ifAdaptationField_t adaptation;
	uint8_t payload[184];						//-4 : 4Bytes TS header
} ifTSPacket,*pifTSPacket;


typedef struct _cTSDecoder {
	int section_pos;
	int head;
	int section_len;
	uint8_t curSection[4096];
}TSDecoder, *pTSDecoder;

typedef struct _cFilter {
	uint32_t hSF;
	bool bUsed;
	bool bEnable;
	uint16_t pid;
	TDI_FILTER_MASK param;
	TDI_FilterCallback callback;
	TSDecoder *tsDecoder;
	void* userParam;
}cFilter;

#define MAX_FILTER_LIST 14

#ifdef __cplusplus
extern "C" {
#endif

void InitSWDemux();
void CloseSWDemux();
void StartSWDemux();
void StopSWDemux();

bool checkFilePath(char *path1, char *path2);
long TSFileLength(FILE *fp);



void SetTsFilePath(uint8_t *path);
TRESULT isMatchedSection( cFilter *thiz, uint8_t *isection, int sectionlen );
TRESULT OpenSWSectionFilter(uint32_t *phSF, uint16_t PID);
TRESULT IsSWSectionFilterValidPID(uint16_t PID);
TRESULT SetPIDSWSectionFilter(uint32_t hSF,uint16_t PID);
TRESULT SetSWSectionFilter(uint32_t hSF,uint16_t pid, TDI_FILTER_MASK *pMask,void* cbfn,void* userparam);
TRESULT EnableSWSectionFilter(uint32_t hSF);
TRESULT DisableSWSectionFilter(uint32_t hSF);
TRESULT CloseSWSectionFilter(uint32_t hSF);
TRESULT CloseSWSectionFilterEx(uint32_t hSF);
bool GetSWSectionfilterState();
void SetUseSWDemux(bool bUse);
void SetTSBufferEx(uint8_t* Buffer);
#ifdef __cplusplus
}
#endif

#endif /*__SW_FILTER_H*/
