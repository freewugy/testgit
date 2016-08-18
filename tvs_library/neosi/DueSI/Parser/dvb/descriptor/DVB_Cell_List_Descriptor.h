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

#ifndef DVB_CELL_LIST_DESCRIPTOR_H_
#define DVB_CELL_LIST_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class Subcell {
protected:
    uint8_t mCellIDExtension;
    uint16_t mSubcellLatitude;
    uint16_t mSubcellLongitude;
    uint16_t mSubcellExtendOfLatitude;
    uint16_t mSubcellExtendOfLongitude;

public:
    Subcell(const uint8_t * const buffer);

    uint8_t getCellIdExtension(void) const
    {
        return mCellIDExtension;
    }

    uint16_t getSubcellLatitude(void) const
    {
        return mSubcellLatitude;
    }

    uint16_t getSubcellLongtitude(void) const
    {
        return mSubcellLongitude;
    }

    uint16_t getSubcellExtendOfLatitude(void) const
    {
        return mSubcellExtendOfLatitude;
    }

    uint16_t getSubcellExtendOfLongtitude(void) const
    {
        return mSubcellExtendOfLongitude;
    }

};

typedef list<Subcell *> SubcellList;
typedef SubcellList::iterator SubcellIterator;
typedef SubcellList::const_iterator SubcellConstIterator;

class Cell {
protected:
    uint16_t mCellID;
    uint16_t mCellLatitude;
    uint16_t mCellLongtitude;
    uint16_t mCellExtendOfLatitude;
    uint16_t mCellExtendOfLongtitude;
    uint8_t mSubcellInfoLoopLength;
    SubcellList mSubcells;

public:
    Cell(const uint8_t * const buffer);
    ~Cell(void);

    uint16_t getCellId(void) const
    {
        return mCellID;
    }

    uint16_t getCellLatitude(void) const
    {
        return mCellLatitude;
    }

    uint16_t getCellLongtitude(void) const
    {
        return mCellLongtitude;
    }

    uint16_t getCellExtendOfLatitude(void) const
    {
        return mCellExtendOfLatitude;
    }

    uint16_t getCellExtendOfLongtitude(void) const
    {
        return mCellExtendOfLongtitude;
    }

    const SubcellList *getSubcells(void) const
    {
        return &mSubcells;
    }


};

typedef list<Cell *> CellList;
typedef CellList::iterator CellIterator;
typedef CellList::const_iterator CellConstIterator;

class DVB_Cell_List_Descriptor: public Descriptor {
protected:
    CellList mListCell;

public:
    DVB_Cell_List_Descriptor(const uint8_t * const buffer);
    ~DVB_Cell_List_Descriptor(void);

    const CellList *getCells(void) const;
};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_CELL_LIST_DESCRIPTOR_H_ */

