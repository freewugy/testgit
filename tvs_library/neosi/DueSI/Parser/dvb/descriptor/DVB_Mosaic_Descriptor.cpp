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

#include "DVB_Mosaic_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
ElementaryCellField::ElementaryCellField (const uint8_t * const buffer)
{
    mElementaryCellID = buffer[0] & 0x3F;
}

MosaicCell::MosaicCell(const uint8_t * const buffer)
{
    mLogicalCellID = (buffer[0] >> 2) & 0x3F;
    mLogicalCellPresentationInfo = buffer[1] & 0x07;
    mElementaryCellFieldLength = buffer[2];

    for (size_t i = 0; i < mElementaryCellFieldLength; ++i)
        mElementaryCellFields.push_back(new ElementaryCellField(&buffer[i + 3]));

    mCellLinkageInfo = buffer[mElementaryCellFieldLength + 3];

    switch (mCellLinkageInfo) {
    case 0x01:
        mBouquetID = r16(&buffer[mElementaryCellFieldLength + 4]);
        break;
    case 0x04:
        mEventID = r16(&buffer[mElementaryCellFieldLength + 10]);
        /* fall through */
    case 0x02:
    case 0x03:
        mOriginalNetworkID = r16(&buffer[mElementaryCellFieldLength + 4]);
        mTransportStreamID = r16(&buffer[mElementaryCellFieldLength + 6]);
        mServiceID = r16(&buffer[mElementaryCellFieldLength + 8]);
        break;
    default:
        break;
    }
}

MosaicCell::~MosaicCell(void)
{
    for (ElementaryCellFieldIterator i = mElementaryCellFields.begin(); i != mElementaryCellFields.end(); ++i)
        delete *i;
}

DVB_Mosaic_Descriptor::DVB_Mosaic_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    mMosaicEntryPoint = (buffer[2] >> 7) & 0x01;
    mNumberOfHorizontalElementaryCells = (buffer[2] >> 4) & 0x07;
    mNumberOfVerticalElementaryCells = buffer[2] & 0x07;
    uint8_t elementaryCellFieldLength = 0;

    for (size_t i = 0; i < mLength - 1; i += buffer[i + 5] + 4 + elementaryCellFieldLength) {
        mListMosaicCell.push_back(new MosaicCell(&buffer[i + 3]));
        switch (buffer[i + 5 + buffer[i + 5] + 1]) {
        case 0x01:
            elementaryCellFieldLength = 2;
            break;
        case 0x02:
        case 0x03:
            elementaryCellFieldLength = 6;
            break;
        case 0x04:
            elementaryCellFieldLength = 8;
            break;
        default:
            elementaryCellFieldLength = 0;
            break;
        }
    }
}

DVB_Mosaic_Descriptor::~DVB_Mosaic_Descriptor(void)
{
    for (MosaicCellIterator i = mListMosaicCell.begin(); i != mListMosaicCell.end(); ++i)
        delete *i;
}
////////////////////////////////////////////////////////////////////////////////

