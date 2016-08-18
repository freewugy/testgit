/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: eschoi $
 * $LastChangedDate: 2014-06-03 13:58:35 +0900 (화, 03 6월 2014) $
 * $LastChangedRevision: 837 $
 * Description:
 * Note:
 *****************************************************************************/

#ifndef SERVICEINFOHANDLER_H_
#define SERVICEINFOHANDLER_H_

#include <list>
#include "ServiceInfo.h"
#include "XML/XMLParser.h"
#include "Table.h"
#include "./descriptor/DVB_Descriptor_Container.h"

/*
 *
 */

////////////////////////////////////////////////////////////////////////////////
struct SDT_Entry
{
    SDT_Entry();
    virtual ~SDT_Entry();
    bool process(const uint8_t* buffer);

    TiXmlNode* processXMLDescriptor(BitStream* bs, int length);

    uint16_t m_service_id;
    uint8_t m_running_status;
    uint8_t m_free_CA_mode;

    uint32_t transport_stream_id;
    uint32_t original_network_id;

    vector<TiXmlNode*> m_desc_xml_list;
    DVB_Descriptor_Container* dc;

    void setXMLUse(bool b) {
        mBXmlUse = b;
    };
private:
    XMLParser *xmlParser;
    bool mBXmlUse;
};

////////////////////////////////////////////////////////////////////////////////
struct SDT_Section
{
    SDT_Section();
    virtual ~SDT_Section();
    bool process(const uint8_t* buffer, uint32_t sectionLength);

    uint8_t mVersion;

    std::vector<SDT_Entry*> m_channel_List;
    void setXMLUse(bool b) {
        mBXmlUse = b;
    };
private:
    bool mBXmlUse;
};

class SDTParser {
public:
	SDTParser() {
	    mVersion = 0;
		mBXmlUse = false;
	};

	virtual ~SDTParser();

	list<ServiceInfo *> getServiceList() {
		return m_ServiceInfo_List;
	};

	void setXMLUse(bool b) {
	    mBXmlUse = b;
	};

	void process(void* sectionData);

	uint8_t getVersion() {
	    return mVersion;
	};
private:
	uint8_t mVersion;
	list<ServiceInfo *> m_ServiceInfo_List;
	bool mBXmlUse;

	void processChildElements(TiXmlNode *n, ServiceInfo *serviceInfo);
	void processDescriptors(ServiceInfo *serviceInfo, const DescriptorList* descriptorList);
	void processIPDeliveryDescriptor(TiXmlNode *n, ServiceInfo *serviceInfo);

	void makeServiceInfo(ServiceInfo* serviceInfo, SDT_Entry* entry);

};

#endif /* SERVICEINFOHANDLER_H_ */
