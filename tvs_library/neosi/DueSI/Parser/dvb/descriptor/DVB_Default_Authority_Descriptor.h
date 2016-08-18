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

#ifndef DVB_DEFAULT_AUTHORITY_DESCRIPTOR_H_
#define DVB_DEFAULT_AUTHORITY_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef std::vector<uint8_t> DefaultAuthorityByteVector;
typedef DefaultAuthorityByteVector::iterator DefaultAuthorityByteIterator;
typedef DefaultAuthorityByteVector::const_iterator DefaultAuthorityByteConstIterator;

class DVB_Default_Authority_Descriptor : public Descriptor
{
    protected:
        DefaultAuthorityByteVector m_bytes;

    public:
        DVB_Default_Authority_Descriptor(const uint8_t* const buffer);
        virtual ~DVB_Default_Authority_Descriptor();

        const DefaultAuthorityByteVector* getAuthorityBytes(void) const;
};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_DEFAULT_AUTHORITY_DESCRIPTOR_H_ */



