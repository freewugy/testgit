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

#ifndef DVB_MOSAIC_DESCRIPTOR_H_
#define DVB_MOSAIC_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class ElementaryCellField {
protected:
    uint8_t mElementaryCellID;

public:
    ElementaryCellField(const uint8_t * const buffer);

    uint8_t getElementaryCellId(void) const
    {
        return mElementaryCellID;
    }

};

typedef list<ElementaryCellField *> ElementaryCellFieldList;
typedef ElementaryCellFieldList::iterator ElementaryCellFieldIterator;
typedef ElementaryCellFieldList::const_iterator ElementaryCellFieldConstIterator;

class MosaicCell {
protected:
    uint8_t mLogicalCellID;
    uint8_t mLogicalCellPresentationInfo;
    uint8_t mElementaryCellFieldLength;
    ElementaryCellFieldList mElementaryCellFields;
    uint8_t mCellLinkageInfo;
    uint16_t mBouquetID;
    uint16_t mOriginalNetworkID;
    uint16_t mTransportStreamID;
    uint16_t mServiceID;
    uint16_t mEventID;

public:
    MosaicCell(const uint8_t * const buffer);
    ~MosaicCell(void);

    uint8_t getLogicalCellId(void) const
    {
        return mLogicalCellID;
    }

    uint8_t getLogicalCellPresentationInfo(void) const
    {
        return mLogicalCellPresentationInfo;
    }

    const ElementaryCellFieldList *getElementaryCellFields(void) const
    {
        return &mElementaryCellFields;
    }

    uint8_t getCellLinkageInfo(void) const
    {
        return mCellLinkageInfo;
    }

    uint16_t getBouquetId(void) const
    {
        return mBouquetID;
    }

    uint16_t getOriginalNetworkId(void) const
    {
        return mOriginalNetworkID;
    }

    uint16_t getTransportStreamId(void) const
    {
        return mTransportStreamID;
    }

    uint16_t getServiceId(void) const
    {
        return mServiceID;
    }

    uint16_t getEventId(void) const
    {
        return mEventID;
    }

};

typedef list<MosaicCell *> MosaicCellList;
typedef MosaicCellList::iterator MosaicCellIterator;
typedef MosaicCellList::const_iterator MosaicCellConstIterator;

class DVB_Mosaic_Descriptor: public Descriptor {
protected:
    uint8_t mMosaicEntryPoint;
    uint8_t mNumberOfHorizontalElementaryCells;
    uint8_t mNumberOfVerticalElementaryCells;
    MosaicCellList mListMosaicCell;

public:
    DVB_Mosaic_Descriptor(const uint8_t * const buffer);
    ~DVB_Mosaic_Descriptor(void);

    uint8_t getMosaicEntryPoint(void) const
    {
        return mMosaicEntryPoint;
    }

    uint8_t getNumberOfHorizontalElementaryCells(void) const
    {
        return mNumberOfHorizontalElementaryCells;
    }

    uint8_t getNumberOfVerticalElementaryCells(void) const
    {
        return mNumberOfVerticalElementaryCells;
    }

    const MosaicCellList *getMosaicCells(void) const
    {
        return &mListMosaicCell;
    }
};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_MOSAIC_DESCRIPTOR_H_ */

