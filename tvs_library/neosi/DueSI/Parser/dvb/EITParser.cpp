/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: loneowl $
 * $LastChangedDate: 2014-04-22 17:09:22 +0900 (화, 22 4월 2014) $
 * $LastChangedRevision: 673 $
 * Description:
 * Note:
 *****************************************************************************/

#include <unistd.h>
#include <algorithm>
#include "EITParser.h"
#include "Logger.h"
#include "Element.h"
#include "util/TimeConvertor.h"
#include "StringUtil.h"
#include "ByteStream.h"

#include "./descriptor/DVB_Short_Event_Descriptor.h"
#include "./descriptor/DVB_Extended_Event_Descriptor.h"
#include "./descriptor/DVB_Parental_Rating_Descriptor.h"
#include "./descriptor/DVB_Content_Descriptor.h"
#include "./descriptor/DVB_Component_Descriptor.h"
#include "./descriptor/DVB_CA_Identifier_Descriptor.h"
#include "./descriptor/DVB_Multilingual_Component_Name_Descriptor.h"

#define EIT_HEADER_LENGTH 14
#define EIT_ENTRY_LENGTH 12
#define EIT_DESC_LENGTH_POS 10
#define CRC_LENGTH 4

static const char* TAG = "EITParser";

EITParser::~EITParser() {
	mListProgramInfo.clear();
}

void EITParser::clear() {
	mListProgramInfo.clear();
}

TiXmlNode* EITParser::processXMLDescriptor(BitStream* bs, int length) {
	TiXmlNode* xmlNode = xmlParser->parseDescriptor(bs, length);

    return xmlNode;
}

void EITParser::processChildElements(TiXmlNode *n, EventInfo *eventInfo) {

	const char *name = n->Value();

	if (strcmp(name, "elm") == 0) {
		string sName = n->ToElement()->Attribute("name");
		if (hasPrefix("length_of_item", sName)
				|| hasSuffix(sName, "_length") || hasSuffix(sName, "descriptor_number")
				|| hasSuffix(sName, "_count")
				|| sName.compare("descriptor_tag_extension") == 0) {

		} else {
			Element elm(sName, n->ToElement()->Attribute("value"));

			eventInfo->saveData(elm);
		}
	}

	for (TiXmlNode* child = n->FirstChild(); child; child =	child->NextSibling()) {
		processChildElements(child, eventInfo);
	}

	return;
}

void EITParser::processDescriptors(EventInfo *eventInfo, const DescriptorList* descriptorList) {
	for(DescriptorConstIterator i = descriptorList->begin(); i != descriptorList->end(); i++) {
		switch((*i)->getTag()) {
		/* 0x40 - 0x7F: ETSI EN 300 468 V1.5.1 (2003-05) */
		case DVB_SHORT_EVENT_DESCRIPTOR: {
			DVB_Short_Event_Descriptor* desc = (DVB_Short_Event_Descriptor*)(*i);

            convertStrToExtendedASCII(desc->getEventName(), &(eventInfo->event_name));
            convertStrToExtendedASCII(desc->getText(), &(eventInfo->event_short_description));
			break;
		}
		case DVB_EXTENDED_EVENT_DESCRIPTOR: {
			DVB_Extended_Event_Descriptor* desc = (DVB_Extended_Event_Descriptor*) (*i);

			convertStrToExtendedASCII(desc->getText(), &(eventInfo->event_description));

			const ExtendedEventList* itemList = desc->getItems();

			for(ExtendedEventConstIterator itr = itemList->begin(); itr != itemList->end(); itr++) {
				ExtendedEvent * item = (ExtendedEvent *) (*itr);
				string description = stripStrUTFHeader(item->getItemDescription());
				// eschoi_20140108 | 명시하거나, 변환하지말것!!! 대문자를 소문자로 교환(Ste->ste, Rsl->rsl)
				std::transform(description.begin(), description.end(), description.begin(), ::tolower);
				string value = "";
				convertStrToExtendedASCII(item->getItem(), &value);

				if (value.length() > 0) {
					Element elm(description.c_str(), value.c_str());
					eventInfo->saveData(elm);
				}
			}
			break;
		}
		case DVB_COMPONENT_DESCRIPTOR: {
#ifdef __DEBUG_LOG_PSR__
		    DVB_Component_Descriptor* desc = (DVB_Component_Descriptor*)(*i);
	        uint8_t streamContent = desc->getStreamContent();
	        uint8_t componentType = desc->getComponentType();
	        uint8_t componentTag = desc->getComponentTag();
	        std::string iso639LanguageCode = desc->getIso639LanguageCode();
	        std::string text = desc->getText();

	        L_DEBUG(TAG, "COMPONENT_DESCRIPTOR : streamContent[%u], componentType[%u], componentTag[%u], iso639LanguageCode[%s], text[%s], \n",
	                streamContent, componentType, componentTag, iso639LanguageCode.c_str(), text.c_str());
#endif
			break;
		}
		case DVB_CA_IDENTIFIER_DESCRIPTOR: {
			DVB_CA_Identifier_Descriptor* desc = (DVB_CA_Identifier_Descriptor*)(*i);
			const CaSystemIdList* caSystemIdList = desc->getCaSystemIds();

			for(CaSystemIdConstIterator itr = caSystemIdList->begin(); itr != caSystemIdList->end(); itr++) {
			    uint16_t caSystemId = (uint16_t) (*itr);
			    L_INFO(TAG, "CaSystemId : caSystemId[0x%04x]\n", caSystemId);
			}
			break;
		}
		case DVB_CONTENT_DESCRIPTOR: {
			DVB_Content_Descriptor* desc = (DVB_Content_Descriptor*)(*i);
			const ContentClassificationList* contentList = desc->getClassifications();

			for(ContentClassificationConstIterator itr = contentList->begin(); itr != contentList->end(); itr++) {
				ContentClassification* content = (ContentClassification*) (*itr);

				eventInfo->content_nibble_level_1 = content->getContentNibbleLevel1();
				eventInfo->content_nibble_level_2 = content->getContentNibbleLevel2();

				eventInfo->user_nibble_1 = content->getUserNibble1();
				eventInfo->user_nibble_2 = content->getUserNibble2();
			}
			break;
		}
		case DVB_PARENTAL_RATING_DESCRIPTOR: {
			DVB_Parental_Rating_Descriptor* desc = (DVB_Parental_Rating_Descriptor *)(*i);
			const ParentalRatingList* ratingList = desc->getParentalRatings();

			for(ParentalRatingConstIterator itr = ratingList->begin(); itr != ratingList->end(); itr++) {
				ParentalRating * rating = (ParentalRating *) (*itr);
				string countryCode = rating->getCountryCode();
				eventInfo->rating = rating->getRating()==0?0:rating->getRating()+3;
			}
			break;
		}
        case DVB_MULTILINGUAL_COMPONENT_DESCRIPTOR: {
            DVB_Multilingual_Component_Name_Descriptor* desc = (DVB_Multilingual_Component_Name_Descriptor *)(*i);

            const MultilingualComponentList* compList = desc->getMultilingualComponents();
            for(MultilingualComponentConstIterator itr = compList->begin(); itr != compList->end(); itr++) {
                MultilingualComponent* comp = (MultilingualComponent*) (*itr);
                string iso639LanguageCode = comp->getIso639LanguageCode();
                string text = comp->getText();

                L_INFO(TAG, "MultilingualComponent iso639LanguageCode[%s] text[%s]\n", iso639LanguageCode.c_str(), text.c_str());
            }
            break;
        }
        case DVB_TIME_SHIFTED_EVENT_DESCRIPTOR:
            L_INFO(TAG, "TIME_SHIFTED_EVENT_DESCRIPTOR\n");
            break;
        case DVB_MOSAIC_DESCRIPTOR:
            L_INFO(TAG, "MOSAIC_DESCRIPTOR\n");
            break;
        case DVB_STREAM_IDENTIFIER_DESCRIPTOR:
            L_INFO(TAG, "STREAM_IDENTIFIER_DESCRIPTOR\n");
            break;
		case DVB_TELETEXT_DESCRIPTOR:
			L_INFO(TAG, "TELETEXT_DESCRIPTOR\n");
			break;
		case DVB_TELEPHONE_DESCRIPTOR:
			L_INFO(TAG, "TELEPHONE_DESCRIPTOR\n");
			break;
		case DVB_LOCAL_TIME_OFFSET_DESCRIPTOR:
			L_INFO(TAG, "LOCAL_TIME_OFFSET_DESCRIPTOR\n");
			break;
		case DVB_SUBTITLING_DESCRIPTOR:
			L_INFO(TAG, "SUBTITLING_DESCRIPTOR\n");
			break;
		case DVB_TERRESTRIAL_DELIVERY_SYSTEM_DESCRIPTOR:
			L_INFO(TAG, "TERRESTRIAL_DELIVERY_SYSTEM_DESCRIPTOR\n");
			break;
		case DVB_MULTILINGUAL_NETWORK_NAME_DESCRIPTOR:
			L_INFO(TAG, "MULTILINGUAL_NETWORK_NAME_DESCRIPTOR\n");
			break;
		case DVB_MULTILINGUAL_BOUQUET_NAME_DESCRIPTOR:
			L_INFO(TAG, "MULTILINGUAL_BOUQUET_NAME_DESCRIPTOR\n");
			break;
		case DVB_MULTILINGUAL_SERVICE_NAME_DESCRIPTOR:
			L_INFO(TAG, "MULTILINGUAL_SERVICE_NAME_DESCRIPTOR\n");
			break;
        case DVB_NETWORK_NAME_DESCRIPTOR:
            L_INFO(TAG, "NETWORK_NAME_DESCRIPTOR\n");
            break;
        case DVB_SERVICE_LIST_DESCRIPTOR:
            L_INFO(TAG, "SERVICE_LIST_DESCRIPTOR\n");
            break;
        case DVB_STUFFING_DESCRIPTOR:
            L_INFO(TAG, "STUFFING_DESCRIPTOR\n");
            break;
        case DVB_SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR:
            L_INFO(TAG, "SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR\n");
            break;
        case DVB_CABLE_DELIVERY_SYSTEM_DESCRIPTOR:
            L_INFO(TAG, "CABLE_DELIVERY_SYSTEM_DESCRIPTOR\n");
            break;
        case DVB_VBI_DATA_DESCRIPTOR:
            L_INFO(TAG, "VBI_DATA_DESCRIPTOR\n");
            break;
        case DVB_VBI_TELETEXT_DESCRIPTOR:
            L_INFO(TAG, "VBI_TELETEXT_DESCRIPTOR\n");
            break;
        case DVB_BOUQUET_NAME_DESCRIPTOR:
            L_INFO(TAG, "BOUQUET_NAME_DESCRIPTOR\n");
            break;
        case DVB_SERVICE_DESCRIPTOR:
            L_INFO(TAG, "SERVICE_DESCRIPTOR\n");
            break;
        case DVB_COUNTRY_AVAILABILITY_DESCRIPTOR:
            L_INFO(TAG, "COUNTRY_AVAILABILITY_DESCRIPTOR\n");
            break;
        case DVB_LINKAGE_DESCRIPTOR:
            L_INFO(TAG, "LINKAGE_DESCRIPTOR\n");
            break;
        case DVB_NVOD_REFERENCE_DESCRIPTOR:
            L_INFO(TAG, "NVOD_REFERENCE_DESCRIPTOR\n");
            break;
        case DVB_TIME_SHIFTED_SERVICE_DESCRIPTOR:
            L_INFO(TAG, "TIME_SHIFTED_SERVICE_DESCRIPTOR\n");
            break;

		case DVB_PRIVATE_DATA_SPECIFIER_DESCRIPTOR:
			L_INFO(TAG, "PRIVATE_DATA_SPECIFIER_DESCRIPTOR\n");
			break;
		case DVB_SERVICE_MOVE_DESCRIPTOR:
			L_INFO(TAG, "SERVICE_MOVE_DESCRIPTOR\n");
			break;
		case DVB_SHORT_SMOOTHING_BUFFER_DESCRIPTOR:
			L_INFO(TAG, "SHORT_SMOOTHING_BUFFER_DESCRIPTOR\n");
			break;
		case DVB_FREQUENCY_LIST_DESCRIPTOR:
			L_INFO(TAG, "FREQUENCY_LIST_DESCRIPTOR\n");
			break;
		case DVB_PARTIAL_TRANSPORT_STREAM_DESCRIPTOR:
			L_INFO(TAG, "PARTIAL_TRANSPORT_STREAM_DESCRIPTOR\n");
			break;
		case DVB_DATA_BROADCAST_DESCRIPTOR:
			L_INFO(TAG, "DATA_BROADCAST_DESCRIPTOR\n");
			break;
		case DVB_SCRAMBLING_DESCRIPTOR:
			L_INFO(TAG, "SCRAMBLING_DESCRIPTOR\n");
			break;
		case DVB_DATA_BROADCAST_ID_DESCRIPTOR:
			L_INFO(TAG, "DATA_BROADCAST_ID_DESCRIPTOR\n");
			break;
		case DVB_TRANSPORT_STREAM_DESCRIPTOR:
			L_INFO(TAG, "TRANSPORT_STREAM_DESCRIPTOR\n");
			break;
		case DVB_DSNG_DESCRIPTOR:
			L_INFO(TAG, "DSNG_DESCRIPTOR\n");
			break;
		case DVB_PDC_DESCRIPTOR:
			L_INFO(TAG, "PDC_DESCRIPTOR\n");
			break;
		case DVB_AC3_DESCRIPTOR:
			L_INFO(TAG, "AC3_DESCRIPTOR\n");
			break;
		case DVB_ANCILLARY_DATA_DESCRIPTOR:
			L_INFO(TAG, "ANCILLARY_DATA_DESCRIPTOR\n");
			break;
		case DVB_CELL_LIST_DESCRIPTOR:
			L_INFO(TAG, "CELL_LIST_DESCRIPTOR\n");
			break;
		case DVB_CELL_FREQUENCY_LINK_DESCRIPTOR:
			L_INFO(TAG, "CELL_FREQUENCY_LINK_DESCRIPTOR\n");
			break;
		case DVB_ANNOUNCEMENT_SUPPORT_DESCRIPTOR:
			L_INFO(TAG, "ANNOUNCEMENT_SUPPORT_DESCRIPTOR\n");
			break;
		case DVB_APPLICATION_SIGNALLING_DESCRIPTOR:
			L_INFO(TAG, "APPLICATION_SIGNALLING_DESCRIPTOR\n");
			break;
		case DVB_ADAPTATION_FIELD_DATA_DESCRIPTOR:
			L_INFO(TAG, "ADAPTATION_FIELD_DATA_DESCRIPTOR\n");
			break;
		case DVB_SERVICE_IDENTIFIER_DESCRIPTOR:
			L_INFO(TAG, "SERVICE_IDENTIFIER_DESCRIPTOR\n");
			break;
		case DVB_SERVICE_AVAILABILITY_DESCRIPTOR:
			L_INFO(TAG, "SERVICE_AVAILABILITY_DESCRIPTOR\n");
			break;
		// TS 102 323
		case DVB_DEFAULT_AUTHORITY_DESCRIPTOR:
			L_INFO(TAG, "DEFAULT_AUTHORITY_DESCRIPTOR\n");
			break;
		case DVB_RELATED_CONTENT_DESCRIPTOR:
			L_INFO(TAG, "RELATED_CONTENT_DESCRIPTOR\n");
			break;
		case DVB_TVA_ID_DESCRIPTOR:
			L_INFO(TAG, "TVA_ID_DESCRIPTOR\n");
			break;
		case DVB_CONTENT_IDENTIFIER_DESCRIPTOR: {
			L_INFO(TAG, "CONTENT_IDENTIFIER_DESCRIPTOR\n");
			break;
		}
		// EN 301 192
		case DVB_TIME_SLICE_FEC_IDENTIFIER_DESCRIPTOR:
			L_INFO(TAG, "TIME_SLICE_FEC_IDENTIFIER_DESCRIPTOR\n");
			break;
		case DVB_ECM_REPETITION_RATE_DESCRIPTOR:
			L_INFO(TAG, "ECM_REPETITION_RATE_DESCRIPTOR\n");
			break;
		// EN 300 468 1.7.1 defines 0x79 - 0x7C / 0x7F
		case DVB_S2_SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR:
			L_INFO(TAG, "S2_SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR\n");
			break;
		case DVB_ENHANCED_AC3_DESCRIPTOR:
			L_INFO(TAG, "ENHANCED_AC3_DESCRIPTOR\n");
			break;
		case DVB_DTS_DESCRIPTOR:
			L_INFO(TAG, "DTS_DESCRIPTOR\n");
			break;
		case DVB_AAC_DESCRIPTOR:
			L_INFO(TAG, "AAC_DESCRIPTOR\n");
			break;
		// EN 300 468 1.7.1 defines 0x7D - 0x7E reserved
		case DVB_EXTENSION_DESCRIPTOR:
			L_INFO(TAG, "EXTENSION_DESCRIPTOR\n");
			break;
		default:
			L_INFO(TAG, "OOPS : Unknown : 0x%02x\n", (*i)->getTag());
			break;
		}

	}
}

void EITParser::processExtendedEventDescriptor(TiXmlNode *n, EventInfo *eventInfo) {
    for (TiXmlNode* child = n->FirstChild(); child; child = child->NextSibling()) {
        const char *name = child->ToElement()->Attribute("name");
        if(strcmp(name, "text") == 0) {
            eventInfo->event_description = child->ToElement()->Attribute("value");
        }
    }

	for(TiXmlNode* entry = n->FirstChild("entry"); entry; entry = entry->NextSibling("entry")) {
		Element elm;
		elm.name = "";
		elm.value = "";
		for (TiXmlNode* child = entry->FirstChild(); child;	child = child->NextSibling()) {
			string sName(child->ToElement()->Attribute("name"));
			if (sName.compare("item_description") == 0) {
				elm.name = child->ToElement()->Attribute("value");
			} else if (sName.compare("item") == 0) {
				elm.value = child->ToElement()->Attribute("value");
			}
		}

		if (elm.name.length() > 0) {
#ifdef __DEBUG_LOG_PSR__
			L_DEBUG(TAG, "name : %s, value : %s\n", elm.name.c_str(), elm.value.c_str());
#endif
			eventInfo->saveData(elm);
		}
	}

	return;
}

void EITParser::processShortEventDescriptor(TiXmlNode *n, EventInfo *eventInfo) {
	for (TiXmlNode* child = n->FirstChild(); child; child = child->NextSibling()) {
		const char *name = child->ToElement()->Attribute("name");
		if (strcmp(name, "short_event_name") == 0) {
			eventInfo->event_name = child->ToElement()->Attribute("value");
		} else if (strcmp(name, "short_event_description") == 0) {
			eventInfo->event_short_description = child->ToElement()->Attribute("value");
		}
	}

	return;
}

void EITParser::processContentDescriptor(TiXmlNode *n, EventInfo *eventInfo) {
	for(TiXmlNode* entry = n->FirstChild("entry"); entry; entry = entry->NextSibling("entry")) {
		for (TiXmlNode* child = entry->FirstChild(); child; child = child->NextSibling()) {
			const char *name = child->ToElement()->Attribute("name");
			if (strcmp(name, "content_nibble_level_1") == 0) {
				eventInfo->content_nibble_level_1 = (uint8_t) atoi(child->ToElement()->Attribute("value"));
			} else if (strcmp(name, "content_nibble_level_2") == 0) {
				eventInfo->content_nibble_level_2 = (uint8_t) atoi(child->ToElement()->Attribute("value"));
			}
		}
	}

	return;
}

void EITParser::processParentalRatingDescriptor(TiXmlNode *n, EventInfo *eventInfo) {
	for(TiXmlNode* entry = n->FirstChild("entry"); entry; entry = entry->NextSibling("entry")) {
		for (TiXmlNode* child = entry->FirstChild(); child; child = child->NextSibling()) {
			const char *name = child->ToElement()->Attribute("name");
			if (strcmp(name, "country_code") == 0) {
//				eventInfo->content_nibble_level_1 = (uint8_t) atoi(child->ToElement()->Attribute("value"));
			} else if (strcmp(name, "rating") == 0) {
				eventInfo->rating = (uint8_t) atoi(child->ToElement()->Attribute("value"));
			}
		}
	}

	return;
}

void EITParser::processEITEntry(const uint8_t* buffer, EventInfo* eventInfo) {

	uint32_t start_time_MJD, time_BCD;
	int descleng;
	list<TiXmlNode *> desc_xml_list;

	eventInfo->event_id = r16(&buffer[0]);
	start_time_MJD = r16(&buffer[2]);
	time_BCD = (buffer[4] << 16) | r16(&buffer[5]);

	eventInfo->start_time = TimeConvertor::getInstance().convertTimeMJDBCD2Seconds(start_time_MJD, time_BCD, 0) + mUTCOffSet * 3600;

	time_BCD = (buffer[7] << 16) | r16(&buffer[8]);

	eventInfo->duration = TimeConvertor::getInstance().convertTimeBCD2Seconds(time_BCD);
	eventInfo->end_time = eventInfo->start_time + eventInfo->duration;

	eventInfo->running_status = (buffer[10] >> 5) & 0x07;
	eventInfo->free_CA_mode = (buffer[10] >> 4) & 0x01;

	descleng = DVB_LENGTH(&buffer[10]);

	DVB_Descriptor_Container* dc = new DVB_Descriptor_Container();
	uint32_t len = descleng;
	uint32_t pos = 0;

	while (len > 2) {
		uint8_t* desc_ptr = (uint8_t *) &buffer[12 + pos];
		uint8_t descriptor_id = desc_ptr[0];
		uint8_t descriptor_length = desc_ptr[1];
		if (mBXmlUse) {
			BitStream* desc_bstream = new BitStream(desc_ptr, descriptor_length + 2, false);
			TiXmlNode* desc_xml;

			desc_xml = processXMLDescriptor(desc_bstream, descriptor_length);
			if (desc_xml)
				desc_xml_list.push_back(desc_xml);

			delete desc_bstream;
		} else {
			if (descriptor_id >= 0x80) {
				BitStream* desc_bstream = new BitStream(desc_ptr, descriptor_length + 2, false);
				TiXmlNode* desc_xml;

				desc_xml = processXMLDescriptor(desc_bstream, descriptor_length);
				if (desc_xml)
					desc_xml_list.push_back(desc_xml);

				delete desc_bstream;
			} else {
				dc->descriptor(desc_ptr);
			}
		}
		len -= descriptor_length + 2;
		pos += descriptor_length + 2;
	}


	if (mBXmlUse == false)
	{
		const DescriptorList* descList = dc->getDescriptors();

		if (descList->size() > 0)
			processDescriptors(eventInfo, descList);
	}

	for (list<TiXmlNode*>::iterator itr = desc_xml_list.begin(); itr != desc_xml_list.end(); itr++) {
		TiXmlNode* n = (TiXmlNode*) *itr;
		if (n) {
			const char* name = n->ToElement()->Attribute("name");

			if (strcmp(name, "extended_event_descriptor") == 0 &&  mBXmlUse) {
				processExtendedEventDescriptor(n, eventInfo);
			} else if (strcmp(name, "short_event_descriptor") == 0 && mBXmlUse) {
				processShortEventDescriptor(n, eventInfo);
			} else if (strcmp(name, "content_descriptor") == 0 && mBXmlUse) {
				processContentDescriptor(n, eventInfo);
			} else if (strcmp(name, "parental_rating_descriptor") == 0  && mBXmlUse) {
				processParentalRatingDescriptor(n, eventInfo);
			} else {
				processChildElements(n, eventInfo);
			}
		}

		delete n;
	}

	desc_xml_list.clear();

	delete dc;
}

void EITParser::processEIT(uint8_t* ptr, uint32_t section_length) {
    if(bFirst) {
        mTableID = ptr[0];
        mServiceID = TableIDExt(ptr);
        mVersion = (ptr[5] & 0x3f) >> 1;
        mTransportStreamID = r16(&ptr[8]);
        mOriginalNetworkID = r16(&ptr[10]);
        char ch[32];
        sprintf(ch, "%u.%u.%u", mOriginalNetworkID, mTransportStreamID, mServiceID);
        mServiceUID = ch;

        bFirst = false;
    }

    vector<const uint8_t*> listData;
    size_t pos = EIT_HEADER_LENGTH;
    uint32_t entry_length = section_length - CRC_LENGTH - EIT_HEADER_LENGTH;
    while(entry_length > EIT_ENTRY_LENGTH) {
        listData.push_back(&ptr[pos]);
        uint16_t length = (DVB_LENGTH(&ptr[pos + EIT_DESC_LENGTH_POS]) + EIT_ENTRY_LENGTH);
        pos += length;
        entry_length -= length;
    }

    for(vector<const uint8_t*>::iterator itr = listData.begin(); itr != listData.end(); itr++) {
        EventInfo* eventInfo = new EventInfo();
        eventInfo->service_id = mServiceID;
        eventInfo->transport_stream_id = mTransportStreamID;
        eventInfo->original_network_id = mOriginalNetworkID;
        eventInfo->service_uid = mServiceUID;

        processEITEntry(*itr, eventInfo);
        mListProgramInfo.push_back(eventInfo);
    }

    listData.clear();
}

bool EITParser::process(void* sectionData)
{
    Table* pTable = (Table*) sectionData;

	uint8_t* ptr;

	for (int i = 0; i < pTable->m_SectionCount; i++) {
		ptr = pTable->m_pSection[i];
		if (ptr && pTable->m_pSectionLen[i] > (EIT_HEADER_LENGTH + CRC_LENGTH)) {
		    processEIT(ptr, pTable->m_pSectionLen[i]);
		} else {
#ifdef __DEBUG_LOG_PSR__
            L_ERROR(TAG, "[%d]section is NULL or section length is SHORT[%u]\n", i, pTable->m_pSectionLen[i]);
#endif
        }
	}

    return true;
}

bool EITParser::parse(BitStream* bs)
{
    return true;
}

bool EITParser::init() {
    xmlParser = new XMLParser();
    mBXmlUse = false;

    mUTCOffSet = 0;
    bFirst = true;

    return true;
}
