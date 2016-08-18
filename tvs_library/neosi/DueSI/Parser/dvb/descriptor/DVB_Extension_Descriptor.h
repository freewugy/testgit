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

#ifndef DVB_EXTENSION_DESCRIPTOR_H_
#define DVB_EXTENSION_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef std::vector<uint8_t> SelectorByteVector;
typedef SelectorByteVector::iterator SelectorByteByteIterator;
typedef SelectorByteVector::const_iterator SelectorByteConstIterator;

class DVB_Extension_Descriptor : public Descriptor
{
    protected:
        unsigned m_extension_tag       : 8;

        SelectorByteVector m_selector_bytes;

    public:
        DVB_Extension_Descriptor(const uint8_t* const buffer);
        virtual ~DVB_Extension_Descriptor();

        uint8_t getExtensionTag() const;
        const SelectorByteVector* getSelectorBytes() const;
};

////////////////////////////////////////////////////////////////////////////////



#endif /* DVB_EXTENSION_DESCRIPTOR_H_ */



