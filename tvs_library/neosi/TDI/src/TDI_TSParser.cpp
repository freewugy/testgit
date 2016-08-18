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
#include <list>
#include <queue>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "TDI.h"
#include "Logger.h"
#include "TDI_TSParser.h"

static uint8_t* gp_swbuff = NULL;
static int gp_swbuffidx=0;
	
TSDecoder *g_pSWTSDecoder=NULL;
typedef struct
{
	uint8_t* buff;
	uint16_t pid;
}tsQMsg;
extern std::queue<tsQMsg> tsQ;
extern std::list<cFilter*> SwFilterList;

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

uint32_t TSH_Base_DMXCRC32 (uint8_t *d, int len, uint32_t crc)
{
//   register int i;
	int i;
   uint8_t *u=d; // Saves '& 0xff'

   for (i=0; i<len; i++)
      crc = (crc << 8) ^ crc_table[((crc >> 24) ^ *u++)];

   return crc;
}

#define get_syntax_indicator(p)			((p[1]>>7)&0x01)
#define MAX_SECTION_SIZE	4096

int TSH_Base_DMXIsValidSection(uint8_t *d, int len)
{
	uint32_t CRCvalue;
	CRCvalue=0xFFFFFFFF;
	if( len > MAX_SECTION_SIZE ) {
		printf( "crc : Too big section !!! len=%d\n",len );
		return 0;
		//exit(-1);
	}

	if(get_syntax_indicator(d) == 0)	//if section systax indicator is 0 then the section has no CRC_32 field
		return 1;

	if (TSH_Base_DMXCRC32(d, len, CRCvalue) == 0)
		return 1;
	
	return 0;
	//return (TSH_Base_DMXCRC32(d, len, CRCvalue) == 0);
}

void initTSDecoder(TSDecoder *pTsDec)
{
	//if(pTsDec)
	{
		pTsDec->section_pos = 0;
		pTsDec->head = 0;
		pTsDec->section_len =0;
	}
}


uint16_t getPID_TSDecoder(uint8_t *pTsPacket)
{
	uint16_t pid = ((uint16_t)pTsPacket[1]&0x1f)<<8 | (uint16_t)pTsPacket[2] ;
	return pid;
}

TRESULT decodeTS (cFilter *filter,uint8_t* pPayload, uint8_t adaptation_field_control , bool payload_unit_start_indicator,int payload_size)
{
	TRESULT err = TOK;
	
//	uint8_t transport_scrambling_control,continuity_counter;

	TSDecoder *pTsDec=filter->tsDecoder;
	
	//calc more copy position if this packet is the first packet of a section
	if( payload_unit_start_indicator )
	{
		err=isMatchedSection( filter, pPayload, TDI_FILTER_MASK_LENGTH );
		if(err != TOK)
		{
			return err;
		}
		
		pTsDec->section_len = (((pPayload[1]&0x0F)<<8)|(pPayload[2]))+3;
		if(pTsDec->section_len>4096)
		{
			pTsDec->section_len = 0;
			return TERR_FAILURE;
		}

		if((payload_size)>=(pTsDec->section_len))
		{
			int len = pTsDec->section_len;

			memcpy(pTsDec->curSection,
					pPayload,
					len );
			pTsDec->head++;
		}
		else
		{
			memcpy(pTsDec->curSection,
					pPayload,
					payload_size );
			pTsDec->section_pos = payload_size;

		}

	}
	else if(pTsDec->section_pos != 0)
	{
		if((pTsDec->section_pos + payload_size)>=(pTsDec->section_len))
		{
			int len = pTsDec->section_len - pTsDec->section_pos;

			memcpy( &(pTsDec->curSection[pTsDec->section_pos]),
					pPayload,
					len );
			pTsDec->head++;
		}
		else
		{
			memcpy( &(pTsDec->curSection[pTsDec->section_pos]),
					pPayload,
					payload_size );
			pTsDec->section_pos += payload_size;

		}
	}

	return err;
}

#define TS_BUF_CNT 300;
#ifdef ANDROID
pthread_mutex_t mTsBuffQMutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
#else    
pthread_mutex_t mTsBuffQMutex = PTHREAD_MUTEX_INITIALIZER;
#endif

void findEmptyTsBuff()
{
	int totalsize = TS_BUF_CNT - 1;
	for(int i=gp_swbuffidx; i<totalsize; i++)
	{
		uint8_t* buff = gp_swbuff + 188*i ;
		if(buff[0]==0)
		{
			gp_swbuffidx = i;
			return ;
		}
	}
	gp_swbuffidx = 1;
}


void SetTSBufferEx(uint8_t* Buffer)
{
	static bool exhaustPackets = false;
	if(gp_swbuff && (Buffer[0] == TS_PACKET_ID) && !exhaustPackets)
	{
		uint16_t pid = getPID_TSDecoder(Buffer);
		
		if( pid!=0x1FFF )
		{
			findEmptyTsBuff();
			uint8_t* buff = gp_swbuff + 188*gp_swbuffidx;
			if(buff[0] != TS_PACKET_ID)
			{
				memcpy(buff,Buffer,188);
				tsQMsg msg;
				msg.buff = buff;
				msg.pid = pid;
				pthread_mutex_lock(&mTsBuffQMutex);
				tsQ.push(msg);
				pthread_mutex_unlock(&mTsBuffQMutex);
			}
			else
			{
				exhaustPackets = true;
//				L_INFO("exhaustPackets start\n");
			}
		}
	}
	else if(exhaustPackets)
	{
//		L_INFO("exhaustPackets\n");
		pthread_mutex_lock(&mTsBuffQMutex);
		if(tsQ.empty())
		{
			exhaustPackets = false;
			cFilter* copy;
			std::list<cFilter*>::iterator filterItr; 
			
			for( filterItr=SwFilterList.begin(); filterItr!=SwFilterList.end(); filterItr++ )
			{
				copy = *filterItr;
				initTSDecoder(copy->tsDecoder);
			}
		}
		pthread_mutex_unlock(&mTsBuffQMutex);
	}
	else
	{
	}
	return ;
}
extern int siUSleep(int usec);
void ReadTSBuffer()
{
	pthread_mutex_lock(&mTsBuffQMutex);
	if(tsQ.empty())
	{
		pthread_mutex_unlock(&mTsBuffQMutex);
		siUSleep(30000);
		return;
	}
	tsQMsg msg = tsQ.front();
	tsQ.pop();
	pthread_mutex_unlock(&mTsBuffQMutex);

	Demux_swFilterTSPacket(msg.buff, msg.pid);
	msg.buff[0] = 0;
}


void Demux_swFilterTSPacket( uint8_t *tsPacket , uint16_t Pid)
{
	TRESULT err = TOK;

	cFilter* copy;
	std::list<cFilter*>::iterator filterItr; 
	uint8_t adaptation_field_control = ((tsPacket[3]&0x30)>>4);
	//calc copy position to copy the packet
	if(adaptation_field_control == 0x2 || adaptation_field_control == 0x0) {		// 10 : adaptation only, no payload
		//--- no payload data---
		return ;
	}
	uint8_t* pPayload=NULL;
	bool payload_unit_start_indicator = 0;
	int payload_size = 0;
	
	
	for( filterItr=SwFilterList.begin(); filterItr!=SwFilterList.end(); filterItr++ )
	{
		copy = *filterItr;
		
		if(copy && copy->pid == Pid)
		{
			if(pPayload==NULL)
			{
				if( adaptation_field_control & 0x2 )
				{	// 11 : adaptation followed by payload
					ifTSPacket iftsPacket;
					SetAdaptationField(&(iftsPacket), tsPacket);
					pPayload = tsPacket + 5 + iftsPacket.adaptation.adaptation_field_length;
				}
				else
				{
					pPayload = tsPacket + 4;
				}
				
				payload_unit_start_indicator = ((tsPacket[1]&0x40)>>6);
				if(payload_unit_start_indicator)
				{
					int pointer_field = *pPayload;
					pPayload = pPayload + pointer_field + 1;	//+1 : pointer_field itself
				}
				payload_size = 188 - (int)(pPayload - tsPacket);
				//L_INFO("pid=0x%X con=0x%X\n",Pid,(tsPacket[3]&0xF));
			}
			if(payload_unit_start_indicator)
			{
				//new PES Packet or PSI Section
				initTSDecoder(copy->tsDecoder);
			}
			if(payload_unit_start_indicator || copy->tsDecoder->section_pos)
			{
				err=decodeTS( copy, pPayload ,adaptation_field_control ,payload_unit_start_indicator, payload_size );

				if(copy->tsDecoder->head != 0)
				{
					if(TSH_Base_DMXIsValidSection(copy->tsDecoder->curSection, copy->tsDecoder->section_len)==0)
					{
						printf("SW DEMUX : Section Error handle=%d tid=0x%X, len=%d\n",copy->hSF, copy->tsDecoder->curSection[0],copy->tsDecoder->section_len);
					}
					else
					{
						if(copy->callback)
							copy->callback(Pid,copy->tsDecoder->curSection, copy->userParam);
					}
					initTSDecoder(copy->tsDecoder);
				}
			}
		}
	}
}

void SetAdaptationField(pifTSPacket pTSPacket, uint8_t *packet)
{
	pTSPacket->adaptation.adaptation_field_length = packet[4];
	if(pTSPacket->adaptation.adaptation_field_length > 0)
	{
		pTSPacket->adaptation.discontinuity_indicator = (packet[5]&0x80)>>7;
	}
}

void initTsParser()
{
	if(gp_swbuff == NULL)
	{
		int bufsize = 188*TS_BUF_CNT;
		gp_swbuff = new uint8_t[bufsize];
	}
}
