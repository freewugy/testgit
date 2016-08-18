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

#ifndef DVB_RELATED_CONTENT_DESCRIPTOR_H_
#define DVB_RELATED_CONTENT_DESCRIPTOR_H_



////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef std::vector<uint8_t> RelatedContentByteVector;
typedef RelatedContentByteVector::iterator RelatedContentByteIterator;
typedef RelatedContentByteVector::const_iterator RelatedContentByteConstIterator;

class DVB_Related_Content_Descriptor : public Descriptor
{
    protected:
        RelatedContentByteVector m_bytes;

    public:
        DVB_Related_Content_Descriptor(const uint8_t* const buffer);
        virtual ~DVB_Related_Content_Descriptor();

        const RelatedContentByteVector* getRelatedContentBytes(void) const;
};

////////////////////////////////////////////////////////////////////////////////



#endif /* DVB_RELATED_CONTENT_DESCRIPTOR_H_ */



