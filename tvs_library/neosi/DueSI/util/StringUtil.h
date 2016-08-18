/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: loneowl $
 * $LastChangedDate: 2014-09-02 14:00:27 +0900 (화, 02 9월 2014) $
 * $LastChangedRevision: 1011 $
 * Description:
 * Note:
 *****************************************************************************/

#ifndef STRINGUTIL_H_
#define STRINGUTIL_H_

////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <stdbool.h>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cerrno>
#include <list>
#include <algorithm>
#include <iterator>
#include <vector>

using namespace std;

#ifndef IN
#define IN
#endif	/* IN */

#ifndef OUT
#define OUT
#endif	/* OUT */

#ifndef INOUT
#define INOUT
#endif	/* INOUT */

////////////////////////////////////////////////////////////////////////////////
#if 1 // STL

	bool keyValueTokenizer(
			IN const std::string str,
			IN const std::string delimiter,
			OUT std::string& key,
			OUT std::string& value);

	void trim(IN OUT std::string& str);
	void ltrim(IN OUT std::string& str);
	void rtrim(IN OUT std::string& str);

	std::string int2string(IN const int32_t number);
	int32_t string2int(IN const std::string str);

	enum UTF_ENCODING_TYPE
	{
		UTF_ENCODING_TYPE_UTF8, /** not implemented */
		UTF_ENCODING_TYPE_UTF8_NOBOM,

		UTF_ENCODING_TYPE_UTF16BE, /** not implemented */
		UTF_ENCODING_TYPE_UTF16BE_NOBOM,

		UTF_ENCODING_TYPE_UTF16LE, /** not implemented */
		UTF_ENCODING_TYPE_UTF16LE_NOBOM, /** not implemented */
	};

	/**
	 * @brief	unicode conversion function.
	 */
	void convertUTF(
			IN UTF_ENCODING_TYPE from, IN UTF_ENCODING_TYPE to,
			IN uint8_t* src, IN uint32_t src_length,
			OUT std::string& dest);

	string get_file_contents(IN const char *filename);
	string convertStrUTF(const string &text);
	string stripStrUTFHeader(const string &text);

	int splitStringByDelimiter(const string &token, const std::string &delimeter, vector<string> &elm);
	int getValueFromListString(IN list<string> listString, IN string strName, OUT string *strValue);

	int convertStringToInt(const string& text);
//	TBool unicodeConvert(
//			  IN const UNICODE_ENCODING_TYPE fromType
//			, IN const std::string from
//			, IN const UNICODE_ENCODING_TYPE toType
//			, OUT std::string& to
//		);

	bool hasPrefix(const std::string& prefix, const std::string& fullString);
	bool hasSuffix (std::string const &fullString, std::string const &suffix);
	bool is_number(const std::string& s);
	int hex2num(char c);
	int hexCharToInt(const char* ch);
	int hexCharToInt(const char* ch, int length);
	int convertStrToExtendedASCII(string in, string *out);
	void printHexa(string in);
	void printHexa(char* ch, int length);

	void convertSpecialChInXML(std::string& input);

    std::string string_to_xmlstring(const std::string& input);

    std::string string_to_hex(const std::string& input);
    std::string hex_to_string(const std::string& input);

    std::string convertUTFStringToHexaString(const string& utfString);

#else // C++

	void keyValueTokenizer(
			IN const char* str,
			IN const char* delimiter,
			OUT char** key,
			OUT char** value);

	void trim(IN OUT char* str);
	void ltrim(IN OUT char* str);
	void rtrim(IN OUT char* str);

#endif // STL



////////////////////////////////////////////////////////////////////////////////






/*
bool convert(const void * buffer, TUint32 bufferSize)
{
	printf("m_bufferSize = %d, bufferSize = %d, %s ==> %s",
			m_bufferSize, bufferSize,
			Charset::getName(getSourceType()), Charset::getName(getTargetType()));

	TChar * p_in_buf = (TChar*)buffer;
	size_t in_left = bufferSize;

	TChar * p_out_buf = (TChar*)m_buffer;
	size_t out_left = m_bufferSize;

	// iconv function do not works as man page says.
	iconv(m_iconvHandle, (char **) &p_in_buf, &in_left, &p_out_buf, &out_left);

	L_DEBUG("in_buf(%p), in_left(%d), out_buf(%p), out_left(%d), result_size(%d), buf_size(%d)",
			p_in_buf, in_left, p_out_buf, out_left, m_bufferSize - out_left, m_bufferSize);

	// converted length.
	m_resultSize = m_bufferSize - out_left;

//	 * If target and system endianess is different,
//	 * iconv adds BOM (Byte order mark) in front of the result.
//	 * --> remove it.
//	 *
//	 * BOM (FFFE, FEFF) : indicate Big/Little.

	if(Charset::isUnicode(getTargetType()))
	{
		//if(getTargetType() != Charset::getSystemUncodeEndian())	//	NOT WORK

		if(m_resultSize > 0)
		{
			if(
					((m_buffer[0] == 0xFF) && (m_buffer[1] == 0xFE))
					|| ((m_buffer[0] == 0xFE) && (m_buffer[1] == 0xFF))
					)
			{
				m_resultSize -= sizeof(TUniChar);
				std_memcpy(m_buffer, m_buffer + sizeof(TUniChar), m_resultSize);
			}
		}
	}

	if (in_left == bufferSize)	//	fail to convert.
	{
		m_resultSize = 0;
		L_ERROR("FAIL to convert");
		return false;
	}

	if (in_left == 0)		//	success
	{
		return true;
	}

	if (in_left > 0)	//	partial left.
	{
		L_WARN("iconv conversion partially left");
		return true;
	}

	//	need more memory.
	expandBuffer(m_bufferSize);
	return convert(buffer, bufferSize);
}
*/
////////////////////////////////////////////////////////////////////////////////



#endif /* STRINGUTIL_H_ */



