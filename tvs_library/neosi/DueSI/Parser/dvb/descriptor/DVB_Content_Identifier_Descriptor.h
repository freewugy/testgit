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

#ifndef DVB_CONTENT_IDENTIFIER_DESCRIPTOR_H_
#define DVB_CONTENT_IDENTIFIER_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef std::vector<uint8_t> ContentReferenceIdentifierByteVector;
typedef ContentReferenceIdentifierByteVector::iterator ContentReferenceIdentifierByteIterator;
typedef ContentReferenceIdentifierByteVector::const_iterator ContentReferenceIdentifierByteConstIterator;

class ContentReferenceIdentifier
{
    protected:
        unsigned m_type               : 6;
        unsigned m_location           : 2;

        // case location == 0x00
        unsigned m_length             : 8;
        ContentReferenceIdentifierByteVector m_crid_bytes;

        // case location == 0x01
        unsigned m_reference          :16;

    public:
        ContentReferenceIdentifier(const uint8_t* const buffer);
        ~ContentReferenceIdentifier();

        uint8_t getType() const;
        uint8_t getLocation() const;
        uint8_t getLength() const;
        const ContentReferenceIdentifierByteVector* getBytes() const;
        uint16_t getReference() const;

        friend class ContentIdentifierDescriptor;
};

typedef std::list<ContentReferenceIdentifier*> ContentReferenceIdentifierList;
typedef ContentReferenceIdentifierList::iterator ContentReferenceIdentifierIterator;
typedef ContentReferenceIdentifierList::const_iterator ContentReferenceIdentifierConstIterator;

class DVB_Content_Identifier_Descriptor : public Descriptor
{
    protected:
        ContentReferenceIdentifierList m_identifiers;

    public:
        DVB_Content_Identifier_Descriptor(const uint8_t* const buffer);
        virtual ~DVB_Content_Identifier_Descriptor();

        const ContentReferenceIdentifierList* getIdentifier() const;
};

////////////////////////////////////////////////////////////////////////////////


#endif /* DVB_CONTENT_IDENTIFIER_DESCRIPTOR_H_ */



