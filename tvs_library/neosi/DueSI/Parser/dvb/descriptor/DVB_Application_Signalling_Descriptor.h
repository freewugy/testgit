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

#ifndef DVB_APPLICATION_SIGNALLING_DESCRIPTOR_H_
#define DVB_APPLICATION_SIGNALLING_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class ApplicationSignalling
{
    protected:
        unsigned m_application_type            : 16;
        unsigned m_ait_version_number           : 5;

    public:
        ApplicationSignalling(const uint8_t * const buffer);

        uint16_t getApplicationType(void) const;
        uint8_t getAitVersionNumber(void) const;
};

typedef std::list<ApplicationSignalling *> ApplicationSignallingList;
typedef ApplicationSignallingList::iterator ApplicationSignallingIterator;
typedef ApplicationSignallingList::const_iterator ApplicationSignallingConstIterator;

class DVB_Application_Signalling_Descriptor : public Descriptor
{
    protected:
        ApplicationSignallingList m_application_signallings;

    public:
        DVB_Application_Signalling_Descriptor(const uint8_t * const buffer);
        ~DVB_Application_Signalling_Descriptor(void);

        const ApplicationSignallingList *getApplicationSignallings(void) const;
};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_APPLICATION_SIGNALLING_DESCRIPTOR_H_ */



