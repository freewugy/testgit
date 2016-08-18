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

#include "DVB_Cell_List_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
Subcell::Subcell(const uint8_t * const buffer)
{
    mCellIDExtension = buffer[0];
    mSubcellLatitude = r16(&buffer[1]);
    mSubcellLongitude = r16(&buffer[3]);
    mSubcellExtendOfLatitude = r16(&buffer[5]) >> 4;
    mSubcellExtendOfLongitude = r16(&buffer[6]) & 0x0fff;
}

Cell::Cell(const uint8_t * const buffer)
{
    mCellID = r16(&buffer[0]);
    mCellLatitude = r16(&buffer[2]);
    mCellLongtitude = r16(&buffer[4]);
    mCellExtendOfLatitude = r16(&buffer[6]) >> 4;
    mCellExtendOfLongtitude = r16(&buffer[7]) & 0x0fff;
    mSubcellInfoLoopLength = buffer[9];

    for (size_t i = 0; i < mSubcellInfoLoopLength; i += 8)
        mSubcells.push_back(new Subcell(&buffer[i + 10]));
}

Cell::~Cell(void)
{
    for (SubcellIterator i = mSubcells.begin(); i != mSubcells.end(); ++i)
        delete *i;
}

DVB_Cell_List_Descriptor::DVB_Cell_List_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    for (size_t i = 0; i < mLength; i += buffer[i + 11] + 10)
        mListCell.push_back(new Cell(&buffer[i + 2]));
}

DVB_Cell_List_Descriptor::~DVB_Cell_List_Descriptor(void)
{
    for (CellIterator i = mListCell.begin(); i != mListCell.end(); ++i)
        delete *i;
}

const CellList *DVB_Cell_List_Descriptor::getCells(void) const
{
    return &mListCell;
}

////////////////////////////////////////////////////////////////////////////////

