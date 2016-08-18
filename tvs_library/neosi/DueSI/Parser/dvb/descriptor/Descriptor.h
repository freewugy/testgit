/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: loneowl $
 * $LastChangedDate: 2014-04-11 11:46:21 +0900 (금, 11 4월 2014) $
 * $LastChangedRevision: 583 $
 * Description:
 * Note:
 *****************************************************************************/

#ifndef __DESCRIPTOR_H__
#define __DESCRIPTOR_H__

#include "ByteStream.h"
#include "Logger.h"
#include "StringUtil.h"
#include <vector>
#include <list>
#include <string>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

using namespace std;

/* To be used only(!) in constructors of descendants of
 * class Descriptor. Might be enhanced to throw an
 * exception in the future.
 */
#define ASSERT_MIN_DLEN(len)			\
do {						\
	if (mLength < (len)) {	\
		mValid = false; return;			\
	}					\
} while (0)

class Descriptor {
protected:
    uint8_t mTag;
    uint8_t mLength;
    vector<uint8_t> mBytes;
    bool mValid;

public:
    Descriptor(const uint8_t * const buffer);
    virtual ~Descriptor()
    {
    }

    uint8_t getTag(void) const
    {
        return mTag;
    }

    uint8_t getLength(void) const
    {
        return mLength;
    }

    bool isValid(void) const
    {
        return mValid;
    }

    uint8_t writeToBytes(uint8_t * const buffer) const;
};

typedef std::list<Descriptor *> DescriptorList;
typedef DescriptorList::iterator DescriptorIterator;
typedef DescriptorList::const_iterator DescriptorConstIterator;

#endif /* __DESCRIPTOR_H__ */
