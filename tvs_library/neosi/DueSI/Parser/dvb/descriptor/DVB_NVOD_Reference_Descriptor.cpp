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

#include "DVB_NVOD_Reference_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
NvodReference::NvodReference(const uint8_t * const buffer)
{
    m_transport_stream_id = r16(&buffer[0]);
    m_original_network_id = r16(&buffer[2]);
    m_service_id = r16(&buffer[4]);
}

uint16_t NvodReference::getTransportStreamId(void) const
{
    return m_transport_stream_id;
}

uint16_t NvodReference::getOriginalNetworkId(void) const
{
    return m_original_network_id;
}

uint16_t NvodReference::getServiceId(void) const
{
    return m_service_id;
}

DVB_NVOD_Reference_Descriptor::DVB_NVOD_Reference_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    for (size_t i = 0; i < mLength; i += 6) {
        ASSERT_MIN_DLEN(i + 6);
        m_nvod_references.push_back(new NvodReference(&buffer[i + 2]));
    }
}

DVB_NVOD_Reference_Descriptor::~DVB_NVOD_Reference_Descriptor(void)
{
    for (NvodReferenceIterator i = m_nvod_references.begin(); i != m_nvod_references.end(); ++i)
        delete *i;
}

const NvodReferenceList *DVB_NVOD_Reference_Descriptor::getNvodReferences(void) const
{
    return &m_nvod_references;
}

////////////////////////////////////////////////////////////////////////////////

