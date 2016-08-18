/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: loneowl $
 * $LastChangedDate: 2014-04-21 17:32:52 +0900 (월, 21 4월 2014) $
 * $LastChangedRevision: 665 $
 * Description:
 * Note:
 *****************************************************************************/

#ifndef CHARSET_H_
#define CHARSET_H_

#define __DEBUG_PRINT 0

namespace Charset
{
	enum Type
	{
		UNKNOWN,

		ISO_8859_5  			= 0x01,
		ISO_8859_6  			= 0x02,
		ISO_8859_7  			= 0x03,
		ISO_8859_8  			= 0x04,
		ISO_8859_9  			= 0x05,
		ISO_8859_10 			= 0x06,
		ISO_8859_11 			= 0x07,
		ISO_8859_12 			= 0x08,
		ISO_8859_13 			= 0x09,
		ISO_8859_14 			= 0x0A,
		ISO_8859_15 			= 0x0B,
		ISO_8859_RESERVED_1 	= 0x0C,
		ISO_8859_RESERVED_2 	= 0x0D,
		ISO_8859_RESERVED_3 	= 0x0E,
		ISO_8859_RESERVED_4 	= 0x0F,
		ISO_8859 				= 0x10,
		ISO_10646_1 			= 0x11,
		KSC_5601_1987 			= 0x12,
		GB_2312_1980 			= 0x13,
		ISO_10646_1_BIG5 		= 0x14,
		ISO_10646_1_UTF8 		= 0x15,

		//	0x16~0x1f are reserved for future use.

		UNICODE_LITTLE_ENDIAN	= 0x20,

		TYPE_END,

		//	DO NOT MOVE followings up.

		KSC 					= Charset::KSC_5601_1987,
		UTF8 					= Charset::ISO_10646_1_UTF8,
		UTF_16					= Charset::ISO_10646_1,		//	Big Endian
		UTF_16LE				= UNICODE_LITTLE_ENDIAN,

		UNICODE_BE 				= UTF_16,
		UNICODE_LE				= UTF_16LE
	};

	const char * getName(Type type);

	inline bool isUnicode(Type type)
	{
		return (type == UNICODE_LE) || (type == UNICODE_BE);
	}

	inline Type getSystemUncodeEndian()
	{
#ifdef CPU_ENDIAN_BIG
		return UNICODE_BE;
#else
		return UNICODE_LE;
#endif
	}
};

#endif /* CHARSET_H_ */
