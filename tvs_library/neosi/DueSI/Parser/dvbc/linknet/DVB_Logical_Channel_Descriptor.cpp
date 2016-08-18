/*
 * DVB_Logical_Channel_Descriptor.cpp
 *
 *  Created on: Jan 20, 2015
 *      Author: wjpark
 */

#include "DVB_Logical_Channel_Descriptor.h"
static const char* TAG = "local channel";
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
LogicalChannelItem::LogicalChannelItem(const uint8_t * const buffer)
{
    m_service_id = r16(&buffer[0]);
    m_channel_number = r16(&buffer[2]);
}

uint16_t LogicalChannelItem::getServiceId(void) const
{
    return m_service_id;
}

uint16_t LogicalChannelItem::getChannelNumber(void) const
{
    return m_channel_number;
}

DVB_Logical_Channel_Descriptor::DVB_Logical_Channel_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    for (size_t i = 0; i < mLength; i += 4) {
        ASSERT_MIN_DLEN(i + 4);
        m_channel_list.push_back(new LogicalChannelItem(&buffer[i + 2]));
    }
}

DVB_Logical_Channel_Descriptor::~DVB_Logical_Channel_Descriptor(void)
{
    for (LogicalChannelItemIterator i = m_channel_list.begin(); i != m_channel_list.end(); ++i)
        delete *i;
}

const LogicalChannelItemList *DVB_Logical_Channel_Descriptor::getChannelList(void) const
{
    return &m_channel_list;
}

////////////////////////////////////////////////////////////////////////////////


