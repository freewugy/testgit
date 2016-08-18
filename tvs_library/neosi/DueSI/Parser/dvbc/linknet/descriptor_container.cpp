/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * Description:
 * Note:
 *****************************************************************************/

#include "DVB_Logical_Channel_Descriptor.h"
#include "descriptor_container.h"
#include "descriptor_tag_linknet.h"

DescriptorContainerLinknet::~DescriptorContainerLinknet(void)
{
    for (DescriptorIterator i = descriptorList.begin(); i != descriptorList.end(); ++i)
        delete *i;
}

void DescriptorContainerLinknet::descriptor(const uint8_t * const buffer)
{
    Descriptor *d;

    d = descriptorSi(buffer);

    if(!d->isValid())
        delete d;

    descriptorList.push_back(d);
}

Descriptor *DescriptorContainerLinknet::descriptorSi(const uint8_t * const buffer)
{
    switch (buffer[0]) {
        case LOGICAL_CHANNEL_DESCRIPTOR:
        	printf("LOGICAL_CHANNEL_DESCRIPTOR\n");
        return new DVB_Logical_Channel_Descriptor(buffer);
        default:
        return new Descriptor(buffer);
    }
}

const DescriptorList *DescriptorContainerLinknet::getDescriptors(void) const
{
    return &descriptorList;
}
