/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: loneowl $
 * $LastChangedDate: 2014-04-10 15:55:50 +0900 (목, 10 4월 2014) $
 * $LastChangedRevision: 580 $
 * Description:
 * Note:
 *****************************************************************************/

#include "DVB_Cell_Frequency_Link_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
SubcellInfo::SubcellInfo(const uint8_t * const buffer)
{
    mCellIDExtension = buffer[0];
    mTransposerFrequency = r32(&buffer[1]);
}

CellFrequencyLink::CellFrequencyLink(const uint8_t * const buffer)
{
    mCellID = r16(&buffer[0]);
    mFrequency = r32(&buffer[2]);
    mSubcellInfoLoopLen = buffer[6];

    for (size_t i = 0; i < mSubcellInfoLoopLen; i += 5)
        mSubcells.push_back(new SubcellInfo(&buffer[i + 7]));
}

CellFrequencyLink::~CellFrequencyLink(void)
{
    for (SubcellInfoIterator i = mSubcells.begin(); i != mSubcells.end(); ++i)
        delete *i;
}

const SubcellInfoList *CellFrequencyLink::getSubcells(void) const
{
    return &mSubcells;
}

DVB_Cell_Frequency_Link_Descriptor::DVB_Cell_Frequency_Link_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    for (size_t i = 0; i < mLength; i += buffer[i + 10] + 6) {
        ASSERT_MIN_DLEN(i + buffer[i + 10] + 6);
        m_cell_frequency_links.push_back(new CellFrequencyLink(&buffer[i + 2]));
    }
}

DVB_Cell_Frequency_Link_Descriptor::~DVB_Cell_Frequency_Link_Descriptor(void)
{
    for (CellFrequencyLinkIterator i = m_cell_frequency_links.begin(); i != m_cell_frequency_links.end(); ++i)
        delete *i;
}

////////////////////////////////////////////////////////////////////////////////

