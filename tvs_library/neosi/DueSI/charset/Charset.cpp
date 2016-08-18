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

#include "Charset.h"
#include "Logger.h"

static const char* TAG = "CharSet";

const char * Charset::getName(Charset::Type charset)
{
	//	see DebugPrintSupportedCharacterSets() to add more character sets.

	switch(charset)
	{
		case ISO_8859_RESERVED_1:
		case ISO_8859_RESERVED_2:
		case ISO_8859_RESERVED_3:
		case ISO_8859_RESERVED_4:
		case ISO_8859:
			return "ISO-8859-1";	//	??

		case ISO_8859_5:			return "ISO-8859-5";
		case ISO_8859_6:			return "ISO-8859-6";
		case ISO_8859_7:			return "ISO-8859-7";
		case ISO_8859_8:			return "ISO-8859-8";
		case ISO_8859_9:			return "ISO-8859-9";
		case ISO_8859_10:			return "ISO-8859-10";
		case ISO_8859_11:			return "ISO-8859-11";
		case ISO_8859_12:			return "ISO-8859-12";
		case ISO_8859_13:			return "ISO-8859-13";
		case ISO_8859_14:			return "ISO-8859-14";
		case ISO_8859_15:			return "ISO-8859-15";
		case KSC_5601_1987:			return "CP949";			//	"KS_C_5601-1989";
		case GB_2312_1980:			return "GB_2312-80";

		case ISO_10646_1:			return "UTF-16";		//	UNICODE_BE
		case UNICODE_LE:			return "UTF-16LE";
		case ISO_10646_1_UTF8:		return "UTF-8";
		case ISO_10646_1_BIG5:		return "BIG-5";

		case TYPE_END:
		case UNKNOWN:
		{
			L_ERROR(TAG, "Unsupported code page: %d", charset);
		}
		break;
	}

	return NULL;
}

