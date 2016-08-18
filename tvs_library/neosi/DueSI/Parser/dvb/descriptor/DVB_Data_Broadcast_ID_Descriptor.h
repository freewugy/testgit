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

#ifndef DVB_DATA_BROADCAST_ID_DESCRIPTOR_H_
#define DVB_DATA_BROADCAST_ID_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef std::list<uint8_t> IdSelectorByteList;
typedef IdSelectorByteList::iterator IdSelectorByteIterator;
typedef IdSelectorByteList::const_iterator IdSelectorByteConstIterator;

class DVB_Data_Broadcast_ID_Descriptor : public Descriptor
{
    protected:
        unsigned m_data_broadcast_id            : 16;
        IdSelectorByteList m_id_selector_bytes;

    public:
        DVB_Data_Broadcast_ID_Descriptor(const uint8_t * const buffer);

        uint16_t getDataBroadcastId(void) const;
        const IdSelectorByteList *getIdSelectorBytes(void) const;
};

////////////////////////////////////////////////////////////////////////////////



#endif /* DVB_DATA_BROADCAST_ID_DESCRIPTOR_H_ */



