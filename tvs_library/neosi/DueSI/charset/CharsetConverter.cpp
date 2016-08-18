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

#include "CharsetConverter.h"
#include "Logger.h"


#include <string>

#include <string.h>
#include <stdlib.h>
#include <iconv.h>
#include <map>

static const char* TAG = "CharsetConverter";
////////////////////////////////////////////////////////////////////////////////
class CharsetConverterImpl : public ICharsetConverter
{
public:
	CharsetConverterImpl(Charset::Type sourceType, Charset::Type targetType);
	virtual ~CharsetConverterImpl();

	bool convert(const void * buffer, uint32_t bufferSize);

	const uint8_t * getResult() const		{	return m_buffer;		}
	uint32_t getResultSize() const		{	return m_resultSize;	}

	Charset::Type getSourceType() const	{	return m_sourceType;	}
	Charset::Type getTargetType() const	{	return m_targetType;	}

protected:
	void expandBuffer(uint32_t growth);

private:
	void * m_iconvHandle;	//	== iconv_t
	uint8_t * m_buffer;
	uint32_t m_bufferSize;
	uint32_t m_resultSize;	// converted size.
	Charset::Type m_sourceType;
	Charset::Type m_targetType;
};



////////////////////////////////////////////////////////////////////////////////
ICharsetConverter * ICharsetConverter::createInstance(Charset::Type sourceType, Charset::Type targetType)
{
	return new CharsetConverterImpl(sourceType, targetType);
}

void ICharsetConverter::destroyInstance(ICharsetConverter * instance)
{
	delete instance;
}



////////////////////////////////////////////////////////////////////////////////
CharsetConverterImpl::CharsetConverterImpl(Charset::Type sourceType, Charset::Type targetType)
	: m_iconvHandle(NULL)
	, m_buffer(NULL)
	, m_bufferSize(0)
	, m_resultSize(0)
	, m_sourceType(sourceType)
	, m_targetType(targetType)
{
	if(sourceType == targetType)
	{
		L_WARN(TAG, "Conversion type is same.");
	}
	else
	{
		if(sourceType == Charset::UNKNOWN)
		{
			L_INFO(TAG, "sourceType was UNKNOWN assigned, cannot convert");
			return;
		}

		if(targetType == Charset::UNKNOWN)
		{
			L_INFO(TAG, "targetType was UNKNOWN assigned, cannot convert");
			return;
		}

#if __DEBUG_PRINT
		L_DEBUG(TAG, "%s(%d) -> %s(%d)",
			Charset::getName(sourceType), sourceType,
			Charset::getName(targetType), targetType);
#endif
		const char * srcName = Charset::getName(sourceType);
		const char * tarName = Charset::getName(targetType);

		if((srcName == NULL) || (tarName == NULL))
		{
			L_ERROR(TAG, "FAIL to initialize libiconv: charset was NOT linked. Need Full version of libiconv");
			return;
		}

		m_iconvHandle = iconv_open(tarName, srcName);
		if(m_iconvHandle == iconv_t(0xFFFFFFFF))
		{
			L_ERROR(TAG, "FAIL to create iconv handle : %s -> %s",
					Charset::getName(sourceType), Charset::getName(targetType));
		}
		else
		{
#if __DEBUG_PRINT
			L_DEBUG(TAG, "iconv handle created : %s -> %s",
					Charset::getName(sourceType), Charset::getName(targetType));
#endif
		}

		expandBuffer(256);
	}
}



////////////////////////////////////////////////////////////////////////////////
CharsetConverterImpl::~CharsetConverterImpl()
{
	if (m_buffer != NULL)	delete[] m_buffer;

	iconv_close(m_iconvHandle);
}



////////////////////////////////////////////////////////////////////////////////
bool CharsetConverterImpl::convert(const void * buffer, uint32_t bufferSize)
{
#if __DEBUG_PRINT
	L_DEBUG(TAG, "m_bufferSize = %d, bufferSize = %d, %s ==> %s",
			m_bufferSize, bufferSize,
			Charset::getName(getSourceType()), Charset::getName(getTargetType()));
#endif
	bool doConvert = true;

	//
	if(m_sourceType == m_targetType)
	{
		L_DEBUG(TAG, "source == target");
		doConvert = false;
	}

	if(m_sourceType == Charset::UNKNOWN)
	{
		L_DEBUG(TAG, "source == UNKNOWN");
		doConvert = false;
	}

	if(m_targetType == Charset::UNKNOWN)
	{
		L_DEBUG(TAG, "target == UNKNOWN");
		doConvert = false;
	}

	// if same type or unknown type, then just memory copy.
	if(!doConvert)
	{
		if(bufferSize > m_bufferSize)
			expandBuffer(bufferSize - m_bufferSize + 1);

		memcpy(m_buffer, buffer, bufferSize);
		m_resultSize = bufferSize;

		return true;
	}

	//
	if(m_iconvHandle == NULL)
	{
		L_ERROR(TAG, "ICONV library was NOT INITIALIZED properly");
		return false;
	}

	// simply makes 2-time more buffer.
	if (bufferSize > m_bufferSize)
	{
		expandBuffer(m_bufferSize);
	}

	memset(m_buffer, 0, m_bufferSize);

	char * p_in_buf = (char*)buffer;
	size_t in_left = bufferSize;

	char * p_out_buf = (char*)m_buffer;
	size_t out_left = m_bufferSize;

	/*
	 * iconv function do not works as man page says.
	 */
	iconv(m_iconvHandle, (char **) &p_in_buf, &in_left, &p_out_buf, &out_left);

#if __DEBUG_PRINT
	L_DEBUG(TAG, "in_buf(%p), in_left(%d), out_buf(%p), out_left(%d), result_size(%d), buf_size(%d)",
			p_in_buf, in_left, p_out_buf, out_left, m_bufferSize - out_left, m_bufferSize);
#endif
	// converted length.
	m_resultSize = m_bufferSize - out_left;

	/*
	 * If target and system endianess is different,
	 * iconv adds BOM (Byte order mark) in front of the result.
	 * --> remove it.
	 *
	 * BOM (FFFE, FEFF) : indicate Big/Little.
	 */
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
				m_resultSize -= sizeof(unsigned short);
				memcpy(m_buffer, m_buffer + sizeof(unsigned short), m_resultSize);
			}
		}
	}

	if (in_left == bufferSize)	//	fail to convert.
	{
		m_resultSize = 0;
		L_ERROR(TAG, "FAIL to convert");
		return false;
	}

	if (in_left == 0)		//	success
	{
		return true;
	}

	if (in_left > 0)	//	partial left.
	{
		L_WARN(TAG, "iconv conversion partially left");
		return true;
	}

	//	need more memory.
	expandBuffer(m_bufferSize);
	return convert(buffer, bufferSize);
}



////////////////////////////////////////////////////////////////////////////////
void CharsetConverterImpl::expandBuffer(uint32_t growth)
{
#if __DEBUG_PRINT
	L_DEBUG(TAG, "growth = %d", growth);
#endif

	uint8_t * temp = new uint8_t[m_bufferSize + growth];
	memset(temp, 0, m_bufferSize + growth);

	if(m_buffer != NULL)
	{
		memcpy(temp, m_buffer, m_bufferSize);
		delete[] m_buffer;
	}

	m_buffer = temp;
	m_bufferSize = m_bufferSize + growth;

#if __DEBUG_PRINT
	L_DEBUG(TAG, "new buffer size (%d)", m_bufferSize);
#endif
}

