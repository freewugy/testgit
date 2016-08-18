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

#ifndef DVB_CELL_FREQUENCY_LINK_DESCRIPTOR_H_
#define DVB_CELL_FREQUENCY_LINK_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class SubcellInfo {
protected:
    uint8_t mCellIDExtension;
    uint32_t mTransposerFrequency;

public:
    SubcellInfo(const uint8_t * const buffer);

    uint8_t getCellIdExtension(void) const
    {
        return mCellIDExtension;
    }

    uint32_t getTransposerFrequency(void) const
    {
        return mTransposerFrequency;
    }

};

typedef list<SubcellInfo *> SubcellInfoList;
typedef SubcellInfoList::iterator SubcellInfoIterator;
typedef SubcellInfoList::const_iterator SubcellInfoConstIterator;

class CellFrequencyLink {
protected:
    uint16_t mCellID;
    uint32_t mFrequency;
    uint8_t mSubcellInfoLoopLen;
    SubcellInfoList mSubcells;

public:
    CellFrequencyLink(const uint8_t * const buffer);
    ~CellFrequencyLink(void);

    uint16_t getCellID(void) const
    {
        return mCellID;
    }

    uint32_t getFrequency(void) const
    {
        return mFrequency;
    }

    const SubcellInfoList *getSubcells(void) const;
};

typedef list<CellFrequencyLink *> CellFrequencyLinkList;
typedef CellFrequencyLinkList::iterator CellFrequencyLinkIterator;
typedef CellFrequencyLinkList::const_iterator CellFrequencyLinkConstIterator;

class DVB_Cell_Frequency_Link_Descriptor: public Descriptor {
protected:
    CellFrequencyLinkList m_cell_frequency_links;

public:
    DVB_Cell_Frequency_Link_Descriptor(const uint8_t * const buffer);
    ~DVB_Cell_Frequency_Link_Descriptor(void);

    const CellFrequencyLinkList *getCellFrequencyLinks(void) const
    {
        return &m_cell_frequency_links;
    }
};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_CELL_FREQUENCY_LINK_DESCRIPTOR_H_ */

