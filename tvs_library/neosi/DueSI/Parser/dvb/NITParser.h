/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: loneowl $
 * $LastChangedDate: 2014-04-18 14:51:29 +0900 (금, 18 4월 2014) $
 * $LastChangedRevision: 647 $
 * Description:
 * Note:
 *****************************************************************************/

#ifndef _NIT_PARSER_H_
#define _NIT_PARSER_H_

#include "NITInfo.h"
#include "ServiceInfo.h"
#include "XML/XMLParser.h"
#include "./descriptor/DVB_Descriptor_Container.h"
#include "./descriptor/DVB_Service_List_Descriptor.h"

/*
 *
 */
class NITParser {
public:
    NITParser() {
        mUTCOffSet = 0;
        mVersion = 0;
        mItemList = NULL;
        init();
    };

	virtual ~NITParser();

    bool process(void* sectionData); /** @override BaseParser */

	list<NITInfo*> getNITList() {
		return m_NIT_List;
	};

    list<ProductInfo> getProductInfoList() {
        return m_ProductInfo_List;
    };

	uint16_t getDefaultChannelId() {
	    return barker_channel_id;
	}

	string getDefaultChannelUid() {
	    return barker_channel_uid;
	}

    void setUTCOffset(uint8_t offset) {
        mUTCOffSet = offset;
    };

    uint8_t getVersion() {
        return mVersion;
    };

private:
	XMLParser *xmlParser;
	uint8_t mUTCOffSet;
	uint8_t mVersion;

	list<NITInfo*> m_NIT_List;
	list<ProductInfo> m_ProductInfo_List;
	const ServiceListItemList* mItemList;
	bool bReceivedServiceList;

    tuner_delivery_parameter mTunerDeliveryParam;
    map<uint16_t, uint16_t> mMapServiceIDChannelNumber;
    uint16_t barker_channel_id;
    string barker_channel_uid;

    list<TiXmlNode*> m_additional_descriptor_xml_List;

    void init();

	void processNIT(uint8_t* ptr, uint32_t section_length);
	void processNITEntry(uint8_t* ptr, uint32_t entry_length, uint16_t network_id);

	TiXmlNode* processXMLDescriptor(BitStream* bs, int length);
	void processChannelDescriptor(TiXmlNode *n);
	void processLogicalChannelDescriptor(TiXmlNode *n);
    void processProductListDescriptor(TiXmlNode *n);
	void setBarkerChannelData(string barker_data);
	void processChildElements(TiXmlNode *n);
	void processDescriptors(const DescriptorList* descriptorList);

};

#endif /* _NIT_PARSER_H_ */
