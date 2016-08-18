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

#ifndef DVB_SERVICE_IDENTIFIER_DESCRIPTOR_H_
#define DVB_SERVICE_IDENTIFIER_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class DVB_Service_Identifier_Descriptor : public Descriptor
{
    protected:
        std::string m_textual_service_identifier;

    public:
        DVB_Service_Identifier_Descriptor(const uint8_t * const buffer);

        const std::string &getTextualServiceIdentifier(void) const;
};

////////////////////////////////////////////////////////////////////////////////


#endif /* DVB_SERVICE_IDENTIFIER_DESCRIPTOR_H_ */



