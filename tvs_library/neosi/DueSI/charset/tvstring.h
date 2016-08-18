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

#ifndef TVSTRING_H_
#define TVSTRING_H_

/*
 * NOTE:
 * 		wchar_t's size can be variable for compilers...
 * 		so, we should review to using 'wstring' type..
 */

#include <stdint.h>
#include "Charset.h"
#include <string>

using namespace std;

class tvstring
{
public:
	tvstring();
	tvstring(const tvstring & tvstr);
	tvstring(const void * str, Charset::Type charsetType);

	virtual ~tvstring();

	void clear()
	{
		m_charsetType = Charset::UNKNOWN;
		m_string.clear();
	}


	////////////////////////////////////////////////////////////////////////////
	/**
	 * NOTE:
	 * 		w_str() **DO NOT** convert internal data as UNICODE.
	 *
	 * 		It just return, internal data's pointer as const TUniChar *,
	 * 		it only meaningful which internal data IS UNICODE.
	 *
	 * 		if you want to convert to UNICODE internal data which is not ,
	 * 		use CharsetConverter.
	 */
	const unsigned short * w_str() const
	{
		return (const unsigned short *)m_string.c_str();
	}
	const char * c_str() const
	{
		return m_string.c_str();
	}
	operator const unsigned short * ()	//	type cast operator.
	{
		return (const unsigned short *)m_string.c_str();
	}
	////////////////////////////////////////////////////////////////////////////


	/*
	 * operators : ==, =, (const TUniChar *)
	 */
	bool operator==(const tvstring rhs) const;
	bool operator!=(const tvstring rhs) const;

	const tvstring & operator=(const tvstring & rhs)
	{
		m_charsetType = rhs.m_charsetType;
		m_string = rhs.m_string;

		return (*this);
	}

	/*
	 * getters: charset, length, byte length, buffer length.
	 *
	 * length : char count.
	 * byte length : byte length that needed to save characters.
	 * buffer length : byte length that needed to save characters with last NULL.
	 */
	Charset::Type getCharset() const	{	return m_charsetType;		}
	uint32_t getLength() const;
	uint32_t getByteLength() const;
	uint32_t getBufferLength() const		{	return m_string.size();		}

	Charset::Type charset() const		{	return getCharset();		}
	uint32_t length() const				{	return getLength();			}
	uint32_t byteLength() const			{	return getByteLength();		}
	uint32_t bufferLength() const		{	return getBufferLength();	}

	/*
	 * setters.
	 */
	void setString(const void * str, Charset::Type type)
	{
		if(Charset::isUnicode(type))
		{
			setRawData(str, getStrLength((const unsigned short *)str) * sizeof(unsigned short), type);
		}
		else
		{
			setRawData(str, getStrLength((const char *)str) * sizeof(char), type);
		}
	}
	void setRawData(const void * buffer, uint32_t bufferSize, Charset::Type type);

	/**
	 * @brief   convert current character set string to UTF-8 character set string.
	 * @return  converted UTF-8 std::string.
	 */
	string getStringUTF8() const;
	string getStringUTF16() const;

	/*
	 * Utility functions.
	 */
	static uint32_t getStrLength(const char * str);
	static uint32_t getStrLength(const unsigned short * str);

private:
	std::string getString(Charset::Type toCharset) const;

	Charset::Type m_charsetType;
	string m_string;
};

#endif /* TVSTRING_H_ */
