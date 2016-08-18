/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: loneowl $
 * $LastChangedDate: 2014-04-15 13:01:53 +0900 (화, 15 4월 2014) $
 * $LastChangedRevision: 613 $
 * Description:
 * Note:
 *****************************************************************************/

#include "MPEG_MuxCode_Descriptor.h"

// util.
#include "BitStream.h"
#include "Logger.h"

static const char* TAG = "MPEG_MuxCode_Desc";

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MPEG_MuxCode_Descriptor::MPEG_MuxCode_Descriptor()
{
}

MPEG_MuxCode_Descriptor::~MPEG_MuxCode_Descriptor()
{
}

bool MPEG_MuxCode_Descriptor::parse(BitStream* bs)
{
    // 1. get {descriptor_tag, descriptor_length}.
    if(false == BaseDescriptor::parse(bs)) {
        return false;
    }

    for(int i = 0; i < m_descriptor_length;) {
        MuxCodeTable table;
        uint8_t count;
        if(false == bs->readBits(8, &table.m_length)
                || false == bs->readBits(4, &table.m_Mux_code)
                || false == bs->readBits(4, &table.m_version)
                || false == bs->readBits(8, &count)) {
            L_ERROR(TAG, "parse() failed\n");
            return false;
        }

        i += 3;
        for(int j = 0; j < count; j++) {
            substructure substruct;
            uint8_t repetition_count;
            if(false == bs->readBits(5, &substruct.m_slotCount)
                    || false == bs->readBits(3, &repetition_count)) {
                L_ERROR(TAG, "parse() failed\n");
                return false;
            }

            i += 1 + repetition_count * 2;
            for(int k = 0; k < repetition_count; k++) {
                MuxCode muxCode;
                if(false == bs->readBits(8, &muxCode.m_flexMuxChannel)
                        || false == bs->readBits(8, &muxCode.m_numberOfBytes)) {
                    L_ERROR(TAG, "parse() failed\n");
                    return false;
                }

                substruct.m_MuxCode_List.push_back(muxCode);
            }

            table.m_substructure_List.push_back(substruct);
        }

        m_MuxCodeTable_List.push_back(table);
    }

    return true;
}



////////////////////////////////////////////////////////////////////////////////

