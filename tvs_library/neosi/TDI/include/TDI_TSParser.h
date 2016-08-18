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

#ifndef __TS_PARSER_H
#define __TS_PARSER_H

#include <stdio.h>
#include <stdlib.h>
////////////////////////////////////////////////////////////////////////////////
#include "TST_Error.h"


////////////////////////////////////////////////////////////////////////////////
#include "TDI_SWSectionFilter.h"


#define TS_PACKET_ID 			0x47

#if 0
#if  TDH_DEVICE_ID==TDH_DID_DST_7405B0
#define MAX_TS_BUFFER_SIZE  188*100+1
#else
#define MAX_TS_BUFFER_SIZE  188*300+1
#endif

typedef struct TsBuff_t
{
	TUint8 buffer[MAX_TS_BUFFER_SIZE];
	int offset;
	int readPtr;
}TsBuffer,*pTsBuffer;
#endif


#ifdef __cplusplus
extern "C" {
#endif




void initTSDecoder(TSDecoder *pTsDec);
void ResetTSDecoders();

void ReadTSBuffer();
void Demux_swFilterTSPacket( uint8_t *tsPacket ,uint16_t Pid);
void SetTSPacketHeader(pifTSPacketHeader pTSHeader, uint8_t *Packet);
void SetAdaptationField(pifTSPacket pTSPacket, uint8_t *packet);
void initTsParser();

#ifdef __cplusplus
}
#endif

extern TSDecoder *g_pSWTSDecoder;

#endif
