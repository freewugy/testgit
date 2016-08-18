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

#include "DVB_Application_Signalling_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
ApplicationSignalling::ApplicationSignalling(const uint8_t * const buffer)
{
    m_application_type = r16(&buffer[0]);
    m_ait_version_number = buffer[2] & 0x1f;
}

uint16_t ApplicationSignalling::getApplicationType(void) const
{
    return m_application_type;
}

uint8_t ApplicationSignalling::getAitVersionNumber(void) const
{
    return m_ait_version_number;
}

DVB_Application_Signalling_Descriptor::DVB_Application_Signalling_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    for (size_t i = 0; i < mLength; i += 3) {
        ASSERT_MIN_DLEN(i + 3);
        m_application_signallings.push_back(new ApplicationSignalling(&buffer[i + 2]));
    }
}

DVB_Application_Signalling_Descriptor::~DVB_Application_Signalling_Descriptor(void)
{
    for (ApplicationSignallingIterator i = m_application_signallings.begin(); i != m_application_signallings.end(); ++i)
        delete *i;
}

const ApplicationSignallingList *DVB_Application_Signalling_Descriptor::getApplicationSignallings(void) const
{
    return &m_application_signallings;
}

////////////////////////////////////////////////////////////////////////////////

