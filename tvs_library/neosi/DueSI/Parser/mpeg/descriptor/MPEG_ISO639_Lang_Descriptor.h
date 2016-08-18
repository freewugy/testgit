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

#ifndef MPEG_ISO639_LANG_DESCRIPTOR_H_
#define MPEG_ISO639_LANG_DESCRIPTOR_H_



////////////////////////////////////////////////////////////////////////////////
#include "BaseDescriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef struct _descISO639List {
    uint8_t ISO_639_language_code[3];
    uint8_t audio_type;
} descISO639List;

struct MPEG_ISO639_Lang_Descriptor: public BaseDescriptor
{
    MPEG_ISO639_Lang_Descriptor();
	virtual ~MPEG_ISO639_Lang_Descriptor();
	virtual bool parse(BitStream* bs);

	vector<descISO639List> m_ISO639_List;
};

////////////////////////////////////////////////////////////////////////////////



#endif /* MPEG_ISO639_LANG_DESCRIPTOR_H_ */



