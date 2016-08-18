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

#ifndef DVB_SCRAMBLING_DESCRIPTOR_H_
#define DVB_SCRAMBLING_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class DVB_Scrambling_Descriptor : public Descriptor
{
    protected:
        unsigned m_scrambling_mode     : 8;

    public:
        DVB_Scrambling_Descriptor(const uint8_t* const buffer);
        virtual ~DVB_Scrambling_Descriptor();

        uint8_t getScramblingMode() const;
};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_SCRAMBLING_DESCRIPTOR_H_ */



