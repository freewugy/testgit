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

#ifndef __descriptor_container_linknet_h__
#define __descriptor_container_linknet_h__

#include "../../dvb/descriptor/Descriptor.h"

class DescriptorContainerLinknet
{
	private:
		Descriptor *descriptorSi(const uint8_t * const buffer);

	protected:
		DescriptorList descriptorList;

	public:
		~DescriptorContainerLinknet(void);

		void descriptor(const uint8_t * const buffer);
		const DescriptorList *getDescriptors(void) const;
};

#endif /* __descriptor_container_linknet_h__ */
