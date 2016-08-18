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

#ifndef _TDT_PARSER_H_
#define _TDT_PARSER_H_

#include <stdint.h>
#include "Logger.h"
#include "util/TimeConvertor.h"

/*
 *
 */

////////////////////////////////////////////////////////////////////////////////
class TDTParser {
public:

    /**
     * @brief instance를 생성하여서 반환
     */
    static TDTParser& getInstance() {
        static TDTParser instance;

        return instance;
    }

	virtual ~TDTParser();

	void process(void* sectionData);

	void printCurrentTime();

	uint32_t getCurrentSeconds() {
	    return m_CurrentSeconds;
	}

    void setUTCOffset(uint8_t offset) {
        mUTCOffSet = offset;
    };

private:
    TDTParser() {
        mUTCOffSet = 0;
    };

    // Dont forget to declare these two. You want to make sure they
    // are unaccessable otherwise you may accidently get copies of
    // your singleton appearing.
    TDTParser(TDTParser const&);            // Don't Implement
    void operator=(TDTParser const&);               // Don't implement

	uint32_t m_CurrentSeconds;

    uint8_t mUTCOffSet;
};

#endif /* _TDT_PARSER_H_ */
