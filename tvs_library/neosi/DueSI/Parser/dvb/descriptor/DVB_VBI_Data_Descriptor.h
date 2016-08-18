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

#ifndef DVB_VBI_DATA_DESCRIPTOR_H_
#define DVB_VBI_DATA_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class VbiDataLine {
protected:
    uint8_t mFieldParity;
    uint8_t mLineOffset;

public:
    VbiDataLine(const uint8_t * const buffer);

    uint8_t getFieldParity(void) const
    {
        return mFieldParity;
    }
    uint8_t getLineOffset(void) const
    {
        return mLineOffset;
    }
};

typedef list<VbiDataLine *> VbiDataLineList;
typedef VbiDataLineList::iterator VbiDataLineIterator;
typedef VbiDataLineList::const_iterator VbiDataLineConstIterator;

class VbiDataService {
protected:
    uint8_t mDataServiceID;
    uint8_t mDataServiceDescriptorLength;
    VbiDataLineList mVbiDataLines;

public:
    VbiDataService(const uint8_t * const buffer);
    ~VbiDataService(void);

    uint8_t getDataServiceId(void) const
    {
        return mDataServiceID;
    }

    const VbiDataLineList *getVbiDataLines(void) const
    {
        return &mVbiDataLines;
    }

};

typedef list<VbiDataService *> VbiDataServiceList;
typedef VbiDataServiceList::iterator VbiDataServiceIterator;
typedef VbiDataServiceList::const_iterator VbiDataServiceConstIterator;

class DVB_VBI_Data_Descriptor: public Descriptor {
protected:
    VbiDataServiceList mVbiDataServiceList;

public:
    DVB_VBI_Data_Descriptor(const uint8_t * const buffer);
    ~DVB_VBI_Data_Descriptor(void);

    const VbiDataServiceList *getVbiDataServices(void) const;
};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_VBI_DATA_DESCRIPTOR_H_ */
