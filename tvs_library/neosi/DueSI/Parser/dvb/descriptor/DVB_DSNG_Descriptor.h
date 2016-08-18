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

#ifndef DVB_DSNG_DESCRIPTOR_H_
#define DVB_DSNG_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef std::vector<uint8_t> PrivateDataByteVector;
typedef PrivateDataByteVector::iterator PrivateDataByteIterator;
typedef PrivateDataByteVector::const_iterator PrivateDataByteConstIterator;

class DVB_DSNG_Descriptor : public Descriptor
{
    protected:
        PrivateDataByteVector m_private_data_bytes;

    public:
        DVB_DSNG_Descriptor(const uint8_t* const buffer);
        virtual ~DVB_DSNG_Descriptor();

        const PrivateDataByteVector* getPrivateDataBytes(void) const;
};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_DSNG_DESCRIPTOR_H_ */



