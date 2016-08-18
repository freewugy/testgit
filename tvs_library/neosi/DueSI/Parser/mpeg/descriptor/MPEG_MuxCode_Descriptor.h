/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: loneowl $
 * $LastChangedDate: 2014-04-10 15:55:50 +0900 (목, 10 4월 2014) $
 * $LastChangedRevision: 580 $
 * Description:
 * Note:
 *****************************************************************************/

#ifndef MPEG_MUXCODE_DESCRIPTOR_H_
#define MPEG_MUXCODE_DESCRIPTOR_H_



////////////////////////////////////////////////////////////////////////////////
#include "BaseDescriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef struct _MuxCode {
    uint8_t m_flexMuxChannel;
    uint8_t m_numberOfBytes;
} MuxCode;

typedef struct _substructure {
    uint8_t m_slotCount;
    vector<MuxCode> m_MuxCode_List;
} substructure;

typedef struct _MuxCodeTable {
    uint8_t m_length;
    uint8_t m_Mux_code;
    uint8_t m_version;
    vector<substructure> m_substructure_List;
} MuxCodeTable;

struct MPEG_MuxCode_Descriptor: public BaseDescriptor
{
    MPEG_MuxCode_Descriptor();
	virtual ~MPEG_MuxCode_Descriptor();
	virtual bool parse(BitStream* bs);

	vector<MuxCodeTable> m_MuxCodeTable_List;
};

////////////////////////////////////////////////////////////////////////////////



#endif /* MPEG_MUXCODE_DESCRIPTOR_H_ */



