/*
 * DVB_Logical_Channel_Descriptor.h
 *
 *  Created on: Jan 20, 2015
 *      Author: wjpark
 */

#ifndef DVB_LOGICAL_CHANNEL_DESCRIPTOR_H_
#define DVB_LOGICAL_CHANNEL_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "../../dvb/descriptor/Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class LogicalChannelItem
{
    protected:
        unsigned m_service_id              : 16;
        unsigned m_channel_number                : 16;

    public:
        LogicalChannelItem(const uint8_t * const buffer);

        uint16_t getServiceId(void) const;
        uint16_t getChannelNumber(void) const;
};

typedef std::list<LogicalChannelItem *> LogicalChannelItemList;
typedef LogicalChannelItemList::iterator LogicalChannelItemIterator;
typedef LogicalChannelItemList::const_iterator LogicalChannelItemConstIterator;

class DVB_Logical_Channel_Descriptor : public Descriptor
{
    protected:
	LogicalChannelItemList m_channel_list;

    public:
	DVB_Logical_Channel_Descriptor(const uint8_t * const buffer);
        ~DVB_Logical_Channel_Descriptor(void);

        const LogicalChannelItemList *getChannelList(void) const;
};

////////////////////////////////////////////////////////////////////////////////



#endif /* DVB_LOGICAL_CHANNEL_DESCRIPTOR_H_ */
