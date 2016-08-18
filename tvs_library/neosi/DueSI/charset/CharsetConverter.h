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

#ifndef CHARSETCONVERTER_H_
#define CHARSETCONVERTER_H_

/*
 *
Example:
	tvstring source("abcdefghijklmnopqrstuvwxyz. 가나다.", Charset::CHARSET_UTF8);
	Charset charset(Charset::CHARSET_UTF8, Charset::CHARSET_KSC);

	if(charset.convert(source.c_str(), source.length()), charset.targetType())
	{
		tvstring target(charset.getConvertResult(), charset.getConvertResultSize(), charset.getTargetType());
		printf("source:[%s]\ntarget:[%s]\n", source.c_str(), target.c_str());
	}
	else
	{
		L_ERROR(TAG, "FAIL to convert");
	}

NOTE:
	tvstring is NOT included in build system currently(2010-02-19).
 */

#include <stdio.h>
#include "Charset.h"
#include "tvstring.h"

////////////////////////////////////////////////////////////////////////////////
class ICharsetConverter
{
public:
	virtual ~ICharsetConverter(){}

	static ICharsetConverter * createInstance(Charset::Type sourceType, Charset::Type targetType);
	static void destroyInstance(ICharsetConverter * instance);

	virtual bool convert(const void * buffer, uint32_t bufferSize) = 0;

	virtual const uint8_t * getResult() const = 0;
	virtual uint32_t getResultSize() const = 0;

	virtual Charset::Type getSourceType() const = 0;
	virtual Charset::Type getTargetType() const = 0;
};



////////////////////////////////////////////////////////////////////////////////
class CharsetConverter
{
private:
	//	prevented.
	CharsetConverter() : m_impl(NULL) {}

public:
	CharsetConverter(Charset::Type sourceType, Charset::Type targetType)
		: m_impl(ICharsetConverter::createInstance(sourceType, targetType))
	{
		;
	}
	virtual ~CharsetConverter()
	{
		ICharsetConverter::destroyInstance(m_impl);
	}

	//
	bool convert(const void * buffer, uint32_t bufferSize)
	{
		return m_impl->convert(buffer, bufferSize);
	}
	bool convert(const tvstring & src)
	{
		return m_impl->convert(src.c_str(), src.getByteLength());
	}
	bool convert(const tvstring & src, tvstring & result)
	{
		bool err = m_impl->convert(src.c_str(), src.getByteLength());

		if (err != false)
		{
			result.setRawData(m_impl->getResult(), m_impl->getResultSize(), m_impl->getTargetType());
		}

		return err;
	}

	const uint8_t * result() const
	{
		return m_impl->getResult();
	}
	uint32_t resultLength() const
	{
		return m_impl->getResultSize();
	}

	const uint8_t * getResult() const
	{
		return m_impl->getResult();
	}
	uint32_t getResultSize() const
	{
		return m_impl->getResultSize();
	}

	//
	Charset::Type sourceType() const
	{
		return m_impl->getSourceType();
	}
	Charset::Type getSourceType() const
	{
		return m_impl->getSourceType();
	}
	const char * getSourceName() const
	{
		return Charset::getName(m_impl->getSourceType());
	}

	//
	Charset::Type targetType() const
	{
		return m_impl->getTargetType();
	}
	Charset::Type getTargetType() const
	{
		return m_impl->getTargetType();
	}
	const char * getTargetName() const
	{
		return Charset::getName(m_impl->getTargetType());
	}

private:
	ICharsetConverter * m_impl;
};

#endif /* CHARSETCONVERTER_H_ */
