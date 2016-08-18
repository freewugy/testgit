/*
 * BATParser.h
 *
 *  Created on: Jan 15, 2015
 *      Author: wjpark
 */

#ifndef BATPARSER_H_
#define BATPARSER_H_

#include <stdint.h>
#include "Logger.h"
#include "ServiceInfo.h"
#include "BATInfo.h"
#include "./descriptor/DVB_Descriptor_Container.h"
#include "./descriptor/DVB_Service_List_Descriptor.h"

#include "../dvbc/linknet/descriptor_container.h"
#include "../dvbc/linknet/descriptor_tag_linknet.h"
#include "../dvbc/linknet/DVB_Logical_Channel_Descriptor.h"
#include "./descriptor/DVB_Descriptor_Container.h"
#include "./descriptor/DVB_Service_List_Descriptor.h"
////////////////////////////////////////////////////////////////////////////////
class BATParser {
public:
	BATParser() {
    mUTCOffSet = 0;
    mVersion = 0;
    init();
	};

	virtual ~BATParser();

    bool process(void* sectionData); /** @override BaseParser */
	list<BATInfo*> getBATList() {
		return m_BAT_List;
	};

	uint8_t getVersion() {
		return mVersion;
	};
private:
	void processBAT(uint8_t* ptr, uint32_t section_length);
	void processBATEntry(uint8_t* ptr, uint32_t entry_length, uint16_t bouquet_id);
    void processLinknetDescriptors(const DescriptorList* descriptorList);


protected:

    void processDescriptors(const DescriptorList* descriptorList);

	uint8_t mVersion;
	uint8_t mUTCOffSet;

	list<BATInfo*> m_BAT_List;
	const LogicalChannelItemList* mItemList;
	bool bReceivedLogicalChannel;
	void init();

};



#endif /* BATPARSER_H_ */
