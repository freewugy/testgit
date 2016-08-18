/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: loneowl $
 * $LastChangedDate: 2014-04-18 10:44:38 +0900 (금, 18 4월 2014) $
 * $LastChangedRevision: 643 $
 * Description:
 * Note:
 *****************************************************************************/

#ifndef _EIT_PARSER_H_
#define _EIT_PARSER_H_

#include <list>
#include "EventInfo.h"
#include "XML/XMLParser.h"
#include "Table.h"
#include "./descriptor/DVB_Descriptor_Container.h"

/*
 *
 */
class EITParser {
public:
    EITParser() {
        mTableID = 0;
        mVersion = 0;
        mServiceID = 0;
        mTransportStreamID = 0;
        mOriginalNetworkID = 0;
        mServiceUID = "";

        mUTCOffSet = 0;
        bFirst = true;
        xmlParser = new XMLParser();
        mBXmlUse = false;
    };

	virtual ~EITParser();

    virtual bool init();
    virtual bool parse(BitStream* bs); /** @override BaseParser */
    virtual bool process(void* sectionData); /** @override BaseParser */

	vector<EventInfo*> getEventList() {
		return mListProgramInfo;
	};

	uint8_t getTableID() {
	    return mTableID;
	};

	uint8_t getVersion() {
	    return mVersion;
	};

	uint32_t getServiceID() {
	    return mServiceID;
	};

	string getServiceUID() {
	    return mServiceUID;
	};

	void clear();

	void setUTCOffset(uint8_t offset) {
		mUTCOffSet = offset;
	};

	void setXMLUse(bool bXmlUse) {
		mBXmlUse = bXmlUse;
	};

private:
	XMLParser *xmlParser;
	vector<EventInfo *> mListProgramInfo;
	uint8_t mTableID;
	uint8_t mVersion;
	uint32_t mServiceID;
    uint32_t mTransportStreamID;
    uint32_t mOriginalNetworkID;
    string mServiceUID;
    bool bFirst;
	bool mBXmlUse;
	uint8_t mUTCOffSet;

    void processEIT(uint8_t* ptr, uint32_t section_length);
    void processEITEntry(const uint8_t* buffer, EventInfo* eventInfo);

	TiXmlNode* processXMLDescriptor(BitStream* bs, int length);
	void processChildElements(TiXmlNode *n, EventInfo *eventInfo);
	void processExtendedEventDescriptor(TiXmlNode *n, EventInfo *eventInfo);
	void processShortEventDescriptor(TiXmlNode *n, EventInfo *eventInfo);
	void processContentDescriptor(TiXmlNode *n, EventInfo *eventInfo);
	void processParentalRatingDescriptor(TiXmlNode *n, EventInfo *eventInfo);

	void processDescriptors(EventInfo *eventInfo, const DescriptorList* descriptorList);
};

#endif /* _EIT_PARSER_H_ */
