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

#include "DVB_VBI_Data_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
VbiDataLine::VbiDataLine(const uint8_t * const buffer)
{
    mFieldParity = (buffer[0] >> 5) & 0x01;
    mLineOffset = buffer[0] & 0x1F;
}

VbiDataService::VbiDataService(const uint8_t * const buffer)
{
    uint16_t i;

    mDataServiceID = buffer[0];
    mDataServiceDescriptorLength = buffer[1];

    switch (mDataServiceID) {
    case 0x01:
    case 0x02:
    case 0x04:
    case 0x05:
    case 0x06:
    case 0x07:
        for (i = 0; i < mDataServiceDescriptorLength; ++i)
            mVbiDataLines.push_back(new VbiDataLine(&buffer[i + 2]));
        break;

    default:
        break;
    }
}

VbiDataService::~VbiDataService(void)
{
    for (VbiDataLineIterator i = mVbiDataLines.begin(); i != mVbiDataLines.end(); ++i)
        delete *i;
}

DVB_VBI_Data_Descriptor::DVB_VBI_Data_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    for (size_t i = 0; i < mLength; i += buffer[i + 3] + 2)
        mVbiDataServiceList.push_back(new VbiDataService(&buffer[i + 2]));
}

DVB_VBI_Data_Descriptor::~DVB_VBI_Data_Descriptor(void)
{
    for (VbiDataServiceIterator i = mVbiDataServiceList.begin(); i != mVbiDataServiceList.end(); ++i)
        delete *i;
}

const VbiDataServiceList *DVB_VBI_Data_Descriptor::getVbiDataServices(void) const
{
    return &mVbiDataServiceList;
}

////////////////////////////////////////////////////////////////////////////////

