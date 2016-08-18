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

#ifndef BITSTREAM_H_
#define BITSTREAM_H_

#include <stdint.h>

#include "Uncopyable.h"

class BitStream: private Uncopyable
{
public:
	bool readBits(uint32_t bitsNum, uint8_t* value, bool moveOffset = true);
	bool readBits(uint32_t bitsNum, uint16_t* value, bool moveOffset = true);
	bool readBits(uint32_t bitsNum, uint32_t* value, bool moveOffset = true);
	bool skipBits(uint32_t bitsNum);

	bool readByte(uint32_t byteNum, uint8_t* value, bool moveOffset = true);
	bool skipByte(uint32_t byteNum);
	uint32_t getRemainBytes();
	uint8_t* getPtr();

	bool checkCRC32();

private:
	uint8_t* m_buffer;
	uint32_t m_length;
	uint32_t m_offset; // bit offset.
	const bool m_copy;

public:
    BitStream(uint8_t* buffer, uint32_t length, bool copy = true);
    virtual ~BitStream(void);
};

#endif /* BITSTREAM_H_ */



