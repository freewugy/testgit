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

#include "tvstring.h"

#include "CharsetConverter.h"
#include "Logger.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static const char* TAG = "tvstring";

tvstring::tvstring()
	: m_charsetType(Charset::UNKNOWN)
{
}
tvstring::tvstring(const tvstring & tvstr)
{
	m_charsetType = tvstr.m_charsetType;
	m_string = tvstr.m_string;
}
tvstring::tvstring(const void * str, Charset::Type charsetType)
	: m_charsetType(charsetType)
{
	setString(str, charsetType);
}
tvstring::~tvstring()
{
}


bool tvstring::operator==(const tvstring rhs) const
{
	bool result = (m_charsetType == rhs.m_charsetType)
			&& (m_string == rhs.m_string);

#if __DEBUG_PRINT
	L_DEBUG(TAG, "%d == %d, %d == %d, %s, %s, %s",
			m_charsetType, rhs.m_charsetType,
			m_string.size(), rhs.m_string.size(),
			m_string == rhs.m_string ? "equal" : "NOT equal",
			memcmp(m_string.c_str(), rhs.m_string.c_str(), length()) == 0 ? "equal" : "NOT equal",
			result ? "true" : "false");
#endif
	return result;
}

bool tvstring::operator!=(const tvstring rhs) const
{
	return !(*this == rhs);
}

uint32_t tvstring::getLength() const
{
	if(m_string.c_str() == NULL || m_string.empty())
	{
		return 0;
	}
	else
	{
		if(Charset::isUnicode(m_charsetType))
		{
			return m_string.size() / 2 - 1;	//	NULL
		}
		else
		{
			return m_string.size() - 1;		//	NULL
		}
	}
}

uint32_t tvstring::getByteLength() const
{
	if(m_string.size() > 0)
	{
		return Charset::isUnicode(m_charsetType)
				? getBufferLength() - sizeof(unsigned short)
				: getBufferLength() - sizeof(char);
	}

	return 0;
}


uint32_t tvstring::getStrLength(const char * str)
{
	if(str == NULL)
		return 0;

	return strlen(str);
}

uint32_t tvstring::getStrLength(const unsigned short * str)
{
	if(str == NULL)
		return 0;

	//return wcslen((const wchar_t *)str);		//	NOT WORKING???

	uint32_t length = 0;
	while(*str != L'\0')
	{
		length++;
		str++;
	}

	return length;
}

void tvstring::setRawData(const void * buffer, uint32_t bufferSize,
		Charset::Type type)
{
	if(buffer == NULL)
	{
		m_charsetType = Charset::UNKNOWN;
		m_string.clear();
	}
	else
	{
		m_charsetType = type;

		uint32_t bytes = (Charset::isUnicode(type))
				? bufferSize + sizeof(unsigned short)
				: bufferSize + sizeof(char);

		m_string.resize(bytes);

		memset(&m_string[0], 0, bytes);
		memcpy(&m_string[0], buffer, bufferSize);
	}
}



////////////////////////////////////////////////////////////////////////////////
std::string tvstring::getString(Charset::Type toCharset) const
{
	// 1. character set check (UNKNOWN)
	if (Charset::UNKNOWN == getCharset())
	{
		L_WARN(TAG, "Charset::UNKNOWN");
		return m_string;
	}

	// 2. character set check (UTF8)
	//if (Charset::UTF8 == getCharset())
	if (toCharset == getCharset())
	{
		return m_string;
	}

	// 3. length check.
	if (1 > getLength())
	{
		L_WARN(TAG, "1 > getLength()");
		return m_string;
	}

	// 4. iconv convert or nothing.
	tvstring tempString;
	CharsetConverter converter(getCharset(), toCharset);
	converter.convert(*this, tempString);

	return tempString.m_string;
}



////////////////////////////////////////////////////////////////////////////////
std::string tvstring::getStringUTF8() const
{
	return getString(Charset::UTF8);
}



////////////////////////////////////////////////////////////////////////////////
std::string tvstring::getStringUTF16() const
{
	return getString(Charset::UTF_16);
}


