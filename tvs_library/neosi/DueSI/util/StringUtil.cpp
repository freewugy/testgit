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

#include "StringUtil.h"
#ifdef __CONVERT_UTF__
#include "charset/tvstring.h"
#include <iconv.h>
#endif
#include <stdio.h>
#include <string.h>
#include <math.h>

////////////////////////////////////////////////////////////////////////////////
#if 1 // STL
bool keyValueTokenizer( IN const std::string str,
		IN const std::string delimiter, OUT std::string& key,
		OUT std::string& value) {
	uint32_t index = str.find(delimiter);
	if (std::string::npos == index) {
		return false;
	}
	key = str.substr(0, index);
	value = str.substr(index + 1);
	return true;
}

void trim(IN OUT std::string& str) {
	ltrim(str);
	rtrim(str);
}

void ltrim(IN OUT std::string& str) {
	str.erase(0, str.find_first_not_of(" "));
	str.erase(0, str.find_first_not_of("\t"));
}

void rtrim(IN OUT std::string& str) {
	str.erase(str.find_last_not_of(" ") + 1);
	str.erase(str.find_last_not_of("\t") + 1);
	str.erase(str.find_last_not_of("\r") + 1);
}

std::string int2string(IN const int32_t number) {
	std::stringstream ss;
	ss << number;
	return ss.str();
}

int32_t string2int(IN const std::string str) {
	std::stringstream ss(str);
	int32_t value;
	ss >> value;
	return value;
}

void convertUTF( IN UTF_ENCODING_TYPE from, IN UTF_ENCODING_TYPE to,
		IN uint8_t* src, IN uint32_t src_length, OUT std::string& dest) {
	uint32_t offset = 0;

	// 1. offset by from.
	if (UTF_ENCODING_TYPE_UTF8 == from) {
		// 0xEF 0xBB 0xBF
		offset = 3;
	} else if ((UTF_ENCODING_TYPE_UTF16BE == from)
			|| (UTF_ENCODING_TYPE_UTF16LE == from)) {
		// 0xFE 0xFF (BE), 0xFF 0xFE (LE)
		offset = 2;
	}

	// 2. BOM by to.
	if (UTF_ENCODING_TYPE_UTF8 == to) {
		// 0xEF 0xBB 0xBF
		uint8_t a;
		a = 0xEF;
		dest.push_back(a);
		a = 0xBB;
		dest.push_back(a);
		a = 0xBF;
		dest.push_back(a);
	} else if (UTF_ENCODING_TYPE_UTF16BE == to) {
		// 0xFE 0xFF (BE)
		uint8_t a;
		a = 0xFE;
		dest.push_back(a);
		a = 0xFF;
		dest.push_back(a);
	} else if (UTF_ENCODING_TYPE_UTF16LE == to) {
		// 0xFF 0xFE (LE)
		uint8_t a;
		a = 0xFF;
		dest.push_back(a);
		a = 0xFE;
		dest.push_back(a);
	}

	if (((UTF_ENCODING_TYPE_UTF16BE == from)
			|| (UTF_ENCODING_TYPE_UTF16BE_NOBOM == from))
			&& ((UTF_ENCODING_TYPE_UTF8 == to)
					|| (UTF_ENCODING_TYPE_UTF8_NOBOM == to))) {
		// 3. convert(1), UTF16 BE -> UTF8.

		uint8_t a, b, c;
		uint16_t temp;
		std::string utf8;

		for (; offset < src_length; offset += 2) {
//				printf("[0x%2X][0x%2X]\t", src[offset], src[offset+1]);
			temp = (src[offset] << 8) | src[offset + 1];

			if (0x7F >= temp) {
				a = 0x7F & temp;
//					printf("[0x%2X]\n", a);
				utf8.push_back(a);
			} else if (0x7FF >= temp) {
				a = 0xC0 | (temp >> 6);
				b = 0x80 | (0x3F & temp);
//					printf("[0x%2X][0x%2X]\n", a,b);

				utf8.push_back(a);
				utf8.push_back(b);
			} else {
				a = 0xE0 | (temp >> 12);
				b = 0x80 | (0x3F & (temp >> 6));
				c = 0x80 | (0x3F & temp);
//					printf("[0x%2X][0x%2X][0x%2X]\n", a,b,c);

				utf8.push_back(a);
				utf8.push_back(b);
				utf8.push_back(c);
			}
		}

//			printf("name[%s]\n", utf8.c_str());
//			for (unsigned int i = 0; i < utf8.size(); i++) {	printf("[0x%2X]", (TUint8) utf8[i]);	}
//			printf("\n");
		dest.append(utf8.begin(), utf8.end());
	} else if (((UTF_ENCODING_TYPE_UTF16LE == from)
			|| (UTF_ENCODING_TYPE_UTF16LE_NOBOM == from))
			&& ((UTF_ENCODING_TYPE_UTF8 == to)
					|| (UTF_ENCODING_TYPE_UTF8_NOBOM == to))) {
		// TODO : 4. convert(2), UTF16 LE -> UTF8.
	} else if (((UTF_ENCODING_TYPE_UTF8 == from)
			|| (UTF_ENCODING_TYPE_UTF8_NOBOM == from))
			&& ((UTF_ENCODING_TYPE_UTF16BE == to)
					|| (UTF_ENCODING_TYPE_UTF16BE_NOBOM == to))) {
		// TODO : 5. convert(3), UTF8 -> UTF16 BE.
	} else if (((UTF_ENCODING_TYPE_UTF8 == from)
			|| (UTF_ENCODING_TYPE_UTF8_NOBOM == from))
			&& ((UTF_ENCODING_TYPE_UTF16LE == to)
					|| (UTF_ENCODING_TYPE_UTF16LE_NOBOM == to))) {
		// TODO : 6. convert(4), UTF8 -> UTF16 LE.
	} else if (((UTF_ENCODING_TYPE_UTF16BE == from)
			|| (UTF_ENCODING_TYPE_UTF16BE_NOBOM == from))
			&& ((UTF_ENCODING_TYPE_UTF16LE == to)
					|| (UTF_ENCODING_TYPE_UTF16LE_NOBOM == to))) {
		// TODO : 7. convert(5), UTF16 BE -> UTF16 LE.
	} else if (((UTF_ENCODING_TYPE_UTF16LE == from)
			|| (UTF_ENCODING_TYPE_UTF16LE_NOBOM == from))
			&& ((UTF_ENCODING_TYPE_UTF16BE == to)
					|| (UTF_ENCODING_TYPE_UTF16BE_NOBOM == to))) {
		// TODO : 8. convert(6), UTF16 LE -> UTF16 BE.
	}
}

string get_file_contents(IN const char *filename) {
	ifstream in(filename, ios::in | ios::binary);
	if (in) {
		string contents="";
		in.seekg(0, ios::end);
		contents.resize(in.tellg());
		in.seekg(0, ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return (contents);
	}
	throw(errno);
}

string stripStrUTFHeader(const string &text) {
    if(text.length() == 0) return "";

    string convertedValue="";
    const char* src = text.c_str();
    int length = text.length();
    uint8_t dst[length];
    memset(dst, '\0', length);

    uint8_t charSet = src[0];

    if (charSet == 0x12) {
        if(length == 1) {
            return "";
        } else {
            memcpy(dst, &src[1], length - 1);
            convertedValue.assign((const char*)dst, length - 1);
        }
    } else {
        return text;
    }

    return convertedValue;
}

string convertStrUTF(const string &text) {
	if(text.length() == 0) return "";

#ifdef __CONVERT_UTF__
	string convertedValue="";
	const char* src = text.c_str();
	int length = text.length();
	uint8_t dst[length];
	memset(dst, '\0', length);

	uint8_t charSet = src[0];

	if (charSet == 0x12) {
		if(length == 1) {
			return "";
		} else {
			memcpy(dst, &src[1], length - 1);
			tvstring tvsString;
			tvsString.setRawData((const void *) dst, length - 1, Charset::KSC);

			convertedValue = tvsString.getStringUTF8();
		}
	} else {
		return text;
	}

	return convertedValue;
#else
	return text;
#endif
}

int splitStringByDelimiter(const string &token, const std::string &delimiter, vector<string> &elm)
{
    int count = 0;
    size_t pos = 0;
    string e="";
    string str = token;

    while ((pos = str.find(delimiter)) != std::string::npos) {
        e = str.substr(0, pos);
        str.erase(0, pos + delimiter.length());
        count++;
        elm.push_back(e);
    }

    elm.push_back(str);

    return count;
}

int getValueFromListString(IN list<string> listString, IN string strName, OUT string *strValue) {
	list<string>::iterator itr;
	string delimiterSpace = " ";
	string delimiterEqual = "=";
	string name="", value="";
	string sName="", sValue="";

	for(itr = listString.begin(); itr != listString.end(); itr++) {
		string s = *itr, a, b;
		remove_copy(s.begin(), s.end(), std::back_inserter(a), '"');
		remove_copy(a.begin(), a.end(), std::back_inserter(b), '<');

		s = b;
		size_t pos = 0;
		std::string token="";

		bool bFirst = true;
		while ((pos = s.find(delimiterSpace)) != std::string::npos) {
		    token = s.substr(0, pos);

		    if(bFirst) {
		    	bFirst = false;
		    	continue;
		    }

		    name = token.substr(0, token.find(delimiterEqual));
		    value = token.substr(token.find(delimiterEqual) + 1, token.length());

		    if(name.compare("name") == 0) {
		    	sName = value;
		    } else if(name.compare("value") == 0) {
		    	sValue = value;
		    }

		    s.erase(0, pos + delimiterSpace.length());
		}

		if(sName.compare(strName) == 0) {
			*strValue = sValue;

			return 1;
		}
	}

	return 0;

}

bool hasPrefix(const std::string& prefix, const std::string& fullString) {
    return fullString.substr(0,prefix.size()) == prefix;
}

bool hasSuffix (std::string const &fullString, std::string const &suffix)
{
    if (fullString.length() >= suffix.length()) {
        return (0 == fullString.compare (fullString.length() - suffix.length(), suffix.length(), suffix));
    } else {
        return false;
    }
}

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

int hex2num(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return -1;
}

int hexCharToInt(const char* ch) {
	int id = hex2num(ch[2]) * 16 + hex2num(ch[3]);

	return id;
}

int hexCharToInt(const char* ch, int length) {
    int id = 0;
    for(int i = 0; i < length; i++) {
        id += hex2num(ch[i]) * (int)pow(16, length - i - 1);
    }

    return id;
}

int convertStringToInt(const string& text) {
    if(is_number(text)) {
        return atoi(text.c_str());
    }
    return 0;
}

void printHexa(string in) {
    char *ch = (char *) in.c_str();
    for(int i = 0; i < in.length(); i++) {
        if(i > 0 && i % 16 == 0) printf("\n");
        printf("%02x ", ch[i]);
    }
    printf("\n");
}

void printHexa(char* ch, int length) {
    if(length <= 0) return;
    char chr[length + 1];
    memset(chr, 0, length + 1);
    memcpy(chr, ch, length);
    printf("%s\n", chr);
    for(int i = 0; i < length; i++) {
        if(i > 0 && i % 16 == 0) printf("\n");
        printf("%02x ", ch[i]);
    }
    printf("\n");
}

int convertStrToExtendedASCII(string in, string *out) {
#ifdef __CONVERT_UTF__
    // create transcoder from iso-8859-1 to utf8
    iconv_t cp1250 = iconv_open("UTF-8", "CP1250");
    size_t len = in.length() * 2;

    // the converted string can be four times larger
    // then the original, as the largest known char width is 4 bytes.
    char chConverted[len];
    memset(chConverted, 0, len);

    // we need to store an additional pointer that targets the
    // start of converted. (iconv modifies the original 'converted')
    char *pIn = (char *) in.c_str();
    char *pOut = (char *) chConverted;

    size_t ibl = in.length(); // len of iso
    size_t obl = len; // len of converted

    // do it!
    int ret = iconv(cp1250, &pIn, &ibl, &pOut, &obl);

    // if iconv fails it returns -1
    if(ret < 0) {
        iconv_close(cp1250);
        *out = in;
        return ret;
    } else {
        iconv_close(cp1250);
        out->assign(chConverted, len);
        return ret;
    }

    return 0;
#else
    *out = in;
    return 0;
#endif
}

/**
 * & -> &amp;
< -> &lt;
> -> &gt;
' -> &apos;
" -> &quot;
 */

void ReplaceStringInPlace(std::string& subject, const std::string& search,
                          const std::string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
}

void convertSpecialChInXML(std::string& input) {
    ReplaceStringInPlace(input, "&amp;", "&");
    ReplaceStringInPlace(input, "&lt;", "<");
    ReplaceStringInPlace(input, "&gt;", ">");
    ReplaceStringInPlace(input, "&apos;", "'");
    ReplaceStringInPlace(input, "&quot;", "\"");
    ReplaceStringInPlace(input, "&nbsp;", " ");
}

bool IsXMLSpecialCharactor(char c)
{
    switch (c) {
        case '&':
        case '<':
        case '>':
        case '\'':
        case '\"':
        return true;
        default:
        return false;
    }
}

std::string string_to_xmlstring(const std::string& input)
{
    std::string output = input;

    output.erase(std::remove_if(output.begin(), output.end(), &IsXMLSpecialCharactor), output.end());

    return output;
}

std::string string_to_hex(const std::string& input)
{
    static const char* const lut = "0123456789ABCDEF";
    size_t len = input.length();

    std::string output;
    output.reserve(2 * len);
    for (size_t i = 0; i < len; ++i) {
        const unsigned char c = input[i];
        output.push_back(lut[c >> 4]);
        output.push_back(lut[c & 15]);
    }
    return output;
}

std::string hex_to_string(const std::string& input)
{
#ifdef __CONVERT_UTF__
    return input;
#else
    static const char* const lut = "0123456789ABCDEF";
    size_t len = input.length();
    if(len & 1) {
//        throw std::invalid_argument("odd length");
        return "";
    }

    std::string output;
    output.reserve(len / 2);
    for (size_t i = 0; i < len; i += 2) {
        char a = input[i];
        const char* p = std::lower_bound(lut, lut + 16, a);
        if(*p != a) {
//            throw std::invalid_argument("not a hex digit");
            continue;
        }

        char b = input[i + 1];
        const char* q = std::lower_bound(lut, lut + 16, b);
        if(*q != b) {
//            throw std::invalid_argument("not a hex digit");
            continue;
        }

        output.push_back(((p - lut) << 4) | (q - lut));
    }
    return output;
#endif
}

std::string convertUTFStringToHexaString(const string& utfString)
{
#ifdef __CONVERT_UTF__
    return string_to_xmlstring(utfString);
#else
    return string_to_hex(utfString);
#endif
}

#else // C++
/**
 * @NOTE: caller must free the key and value argument.
 */
bool keyValueTokenizer(
		IN const char* str,
		IN const char* delimiter,
		OUT char** key,
		OUT char** value)
{
	char* temp;

	char* keyStr;
	int32_t keyLen;

	char* valueStr;
	int32_t valueLen;

	// ex) "[space] key [space] = [space] value [space]"
	temp = strstr(str, delimiter);
	if (NULL == temp) {
		return false;
	}

	keyLen = strlen(str) - strlen(temp);
	keyStr = (TChar*) calloc(keyLen + 1, sizeof(TChar));
	strncpy(keyStr, str, keyLen);
	*key = keyStr;

	temp++;
	valueLen = strlen(temp);
	valueStr = (TChar*) calloc(valueLen + 1, sizeof(TChar));
	strncpy(valueStr, temp, valueLen);
	*value = valueStr;

	return true;
}

void trim(IN OUT char* str)
{
	ltrim(str);
	rtrim(str);
}

void ltrim(IN OUT char* str)
{
	// 1. find, first non-space character position.
	char* temp = str;
	while ('\0' != *temp)
	{
		if (isspace(*temp))
		{
			*temp = '\0';
			temp++;
		}
		else {
			break;
		}
	}

	// 2. exist space.
	if (temp != str)
	{
		TUint32 i = 0;
		while ('\0' != *temp)
		{
			str[i] = *temp;
			*temp = '\0';
			temp++;
		}
	}
}

void rtrim(IN OUT char* str)
{
	int32_t index = strlen(str) - 1;

	while (0 != index)
	{
		if (isspace(str[index]))
		{
			str[index] = '\0';
			index--;
		}
		else {
			break;
		}
	}
}

#endif // STL

////////////////////////////////////////////////////////////////////////////////

