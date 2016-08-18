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

#include <unistd.h>
#include "SDTParser.h"
#include "Logger.h"
#include "Element.h"
#include "StringUtil.h"

#include "./descriptor/DVB_Service_Descriptor.h"
#include "./descriptor/DVB_Extended_Event_Descriptor.h"
#include "./descriptor/DVB_Content_Identifier_Descriptor.h"
#include "./descriptor/DVB_Parental_Rating_Descriptor.h"
#include "./descriptor/DVB_Linkage_Descriptor.h"
#include "./descriptor/DVB_CA_Identifier_Descriptor.h"

#define SDT_HEADER_LENGTH   11
static const char* TAG = "SDTParser";

SDTParser::~SDTParser()
{
//	L_DEBUG(TAG,"\n");
    for (list<ServiceInfo *>::iterator itr = m_ServiceInfo_List.begin(); itr != m_ServiceInfo_List.end(); itr++) {
        if((*itr) != NULL)
            delete *itr;
    }

    m_ServiceInfo_List.clear();
}

void SDTParser::processChildElements(TiXmlNode *n, ServiceInfo *sInfo)
{
    TiXmlNode* child;

    const char *name = n->Value();

    if(strcmp(name, "elm") == 0) {
        string sName = n->ToElement()->Attribute("name");
        if(hasPrefix("length_of_item", sName) || hasSuffix(sName, "_length") || hasSuffix(sName, "descriptor_number") || hasSuffix(sName, "_count")
                || sName.compare("descriptor_tag_extension") == 0) {

        } else {
            Element elm(sName.c_str(), n->ToElement()->Attribute("value"));
#ifdef __DEBUG_LOG_PSR__
            L_INFO(TAG, "Name[%s], Value[%s]\n", sName.c_str(), n->ToElement()->Attribute("value"));
#endif
            sInfo->saveData(elm);
        }
    } else if(strcmp(name, "entry") == 0) {
        string strName(n->ToElement()->Attribute("name"));

        if(strName.compare("ca_system_info") == 0) {
            CAInfo caInfo;
            for (child = n->FirstChild(); child != 0; child = child->NextSibling()) {
                const char *name = child->ToElement()->Attribute("name");
                const char *value = child->ToElement()->Attribute("value");
                if(strcmp(name, "CA_system_ID") == 0) {
                    caInfo.CA_system_ID = (uint16_t) atoi(value);
                } else if(strcmp(name, "CA_PID") == 0) {
                    caInfo.CA_PID = (uint16_t) atoi(value);
                }
            }

            sInfo->listCAInfo.push_back(caInfo);
            n = n->NextSibling();
            return;
        } else if(strName.compare("element_stream_info") == 0) {
            elementary_stream esInfo;
            for (child = n->FirstChild(); child != 0; child = child->NextSibling()) {
                const char *name = child->ToElement()->Attribute("name");
                const char *value = child->ToElement()->Attribute("value");
                if(strcmp(name, "stream_type") == 0) {
                    esInfo.stream_type = (uint8_t) atoi(value);
                } else if(strcmp(name, "elementary_PID") == 0) {
                    esInfo.stream_pid = (uint16_t) atoi(value);
                }
            }

            sInfo->list_elementary_stream.push_back(esInfo);
            n = n->NextSibling();
            return;
        } else if(strName.compare("items") == 0) {
            Element elm;
            for (child = n->FirstChild(); child != 0; child = child->NextSibling()) {
                string sName(child->ToElement()->Attribute("name"));
                string sValue(child->ToElement()->Attribute("value"));
                if(sName.compare("item_description") == 0) {
                    elm.name = sValue;
                } else if(sName.compare("item") == 0) {
                    elm.value = sValue;
                }
            }

            sInfo->saveData(elm);
            n = n->NextSibling();
            return;
        }
    }

    if(n == NULL || n->NoChildren())
        return;

    for (child = n->FirstChild(); child != 0; child = child->NextSibling()) {
        processChildElements(child, sInfo);
    }

    return;
}

void SDTParser::processIPDeliveryDescriptor(TiXmlNode *n, ServiceInfo *serviceInfo)
{
    for (TiXmlNode* elm = n->FirstChild("elm"); elm; elm = elm->NextSibling("elm")) {
        const char* name = elm->ToElement()->Attribute("name");
        if(strcmp(name, "IP_address") == 0) {
            const char* value = elm->ToElement()->Attribute("value");
            uint32_t ip = (uint32_t) atoll(value);

            serviceInfo->IP_address = ip;
        } else if(strcmp(name, "TS_port_number") == 0) {
            serviceInfo->TS_port_number = (uint16_t) atoi(elm->ToElement()->Attribute("value"));
        }
    }

    return;
}

void SDTParser::processDescriptors(ServiceInfo *serviceInfo, const DescriptorList* descriptorList)
{
    for (DescriptorConstIterator i = descriptorList->begin(); i != descriptorList->end(); i++) {
        switch ((*i)->getTag()) {
            /* 0x40 - 0x7F: ETSI EN 300 468 V1.5.1 (2003-05) */
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
            case DVB_SERVICE_DESCRIPTOR: {
                DVB_Service_Descriptor* desc = (DVB_Service_Descriptor*) (*i);

                uint8_t serviceType = desc->getServiceType();
                string serviceProviderName = "", serviceName = "";
                convertStrToExtendedASCII(desc->getServiceProviderName(), &serviceProviderName);
                convertStrToExtendedASCII(desc->getServiceName(), &serviceName);

                serviceInfo->service_type = serviceType;
                serviceInfo->service_name = serviceName;

                serviceInfo->resolution = 0;
                if(serviceInfo->service_type == 0x11) {
                    serviceInfo->resolution = 1;
                }
                break;
            }
            case DVB_COUNTRY_AVAILABILITY_DESCRIPTOR:
            L_INFO(TAG, "COUNTRY_AVAILABILITY_DESCRIPTOR\n");
                break;
            case DVB_LINKAGE_DESCRIPTOR: {
                DVB_Linkage_Descriptor* desc = (DVB_Linkage_Descriptor*) (*i);

                uint8_t linkageType = desc->getLinkageType();
                uint16_t original_network_id = desc->getOriginalNetworkId();
                uint16_t transport_stream_id = desc->getTransportStreamId();
                uint16_t service_id = desc->getServiceId();

//                L_INFO(TAG, "Linkage_desc : linkageType[%u], original_network_id[%u], transport_stream_id[%u], service_id[%u]\n",
//                        linkageType, original_network_id, transport_stream_id, service_id);
                break;
            }
            case DVB_NVOD_REFERENCE_DESCRIPTOR:
            L_INFO(TAG, "NVOD_REFERENCE_DESCRIPTOR\n");
                break;
            case DVB_TIME_SHIFTED_SERVICE_DESCRIPTOR:
            L_INFO(TAG, "TIME_SHIFTED_SERVICE_DESCRIPTOR\n");
                break;
            case DVB_SHORT_EVENT_DESCRIPTOR: {
                L_INFO(TAG, "SHORT_EVENT_DESCRIPTOR\n");
                break;
            }
            case DVB_EXTENDED_EVENT_DESCRIPTOR: {
                L_INFO(TAG, "EXTENDED_EVENT_DESCRIPTOR\n");
                break;
            }
            case DVB_TIME_SHIFTED_EVENT_DESCRIPTOR:
            L_INFO(TAG, "TIME_SHIFTED_EVENT_DESCRIPTOR\n");
                break;
            case DVB_COMPONENT_DESCRIPTOR:
            L_INFO(TAG, "COMPONENT_DESCRIPTOR\n");
                break;
            case DVB_MOSAIC_DESCRIPTOR:
            L_INFO(TAG, "MOSAIC_DESCRIPTOR\n");
                break;
            case DVB_STREAM_IDENTIFIER_DESCRIPTOR:
            L_INFO(TAG, "STREAM_IDENTIFIER_DESCRIPTOR\n");
                break;
            case DVB_CA_IDENTIFIER_DESCRIPTOR: {
                DVB_CA_Identifier_Descriptor* desc = (DVB_CA_Identifier_Descriptor*) (*i);
                const CaSystemIdList* caSystemIdList = desc->getCaSystemIds();

                for (CaSystemIdConstIterator itr = caSystemIdList->begin(); itr != caSystemIdList->end(); itr++) {
                    uint16_t caSystemId = (uint16_t)(*itr);
                    L_INFO(TAG, "CaSystemId : caSystemId[0x%04x]\n", caSystemId);
                }
                break;
            }
            case DVB_CONTENT_DESCRIPTOR:
            L_INFO(TAG, "CONTENT_DESCRIPTOR\n");
                break;
            case DVB_PARENTAL_RATING_DESCRIPTOR: {
                DVB_Parental_Rating_Descriptor* desc = (DVB_Parental_Rating_Descriptor *) (*i);
                const ParentalRatingList* ratingList = desc->getParentalRatings();

                for (ParentalRatingConstIterator itr = ratingList->begin(); itr != ratingList->end(); itr++) {
                    ParentalRating * rating = (ParentalRating *) (*itr);
                    string countryCode = rating->getCountryCode();
                    serviceInfo->channel_rating = rating->getRating() == 0 ? 0 : rating->getRating() + 3;
                }
                break;
            }
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
            case DVB_MULTILINGUAL_COMPONENT_DESCRIPTOR:
            L_INFO(TAG, "MULTILINGUAL_COMPONENT_DESCRIPTOR\n");
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
            /* 0x80 - 0xFE: User defined */
            default:
            L_INFO(TAG, "OOPS : Unknown : 0x%02x\n", (*i)->getTag());
                break;
        }

    }
}

void SDTParser::makeServiceInfo(ServiceInfo* serviceInfo, SDT_Entry* entry)
{
    serviceInfo->service_id = entry->m_service_id;
    serviceInfo->transport_stream_id = entry->transport_stream_id;
    serviceInfo->original_network_id = entry->original_network_id;
    serviceInfo->running_status = entry->m_running_status;
    serviceInfo->free_CA_mode = entry->m_free_CA_mode;

    const DescriptorList* descList = entry->dc->getDescriptors();

    if(descList->size() > 0)
        processDescriptors(serviceInfo, descList);

    for (vector<TiXmlNode*>::iterator itr = entry->m_desc_xml_list.begin(); itr != entry->m_desc_xml_list.end(); itr++) {
        TiXmlNode* n = (TiXmlNode*) *itr;
        if(n) {
            const char* name = n->ToElement()->Attribute("name");

            if(strcmp(name, "ip_delivery_descriptor") == 0) {
                processIPDeliveryDescriptor(n, serviceInfo);
            } else {
                processChildElements(n, serviceInfo);
            }
        }
    }
}

void SDTParser::process(void* sectionData)
{
    Table* pTable = (Table *) sectionData;
    vector<SDT_Section*> list_SDT_Section;
    for (int i = 0; i < pTable->m_SectionCount; i++) {
        if(pTable->m_pSection[i] && pTable->m_pSectionLen[i] > (SDT_HEADER_LENGTH + 3 + 4)) {
            SDT_Section* parser = new SDT_Section();
            parser->setXMLUse(mBXmlUse);
            parser->process(pTable->m_pSection[i], pTable->m_pSectionLen[i]);
            list_SDT_Section.push_back(parser);
        } else {
#ifdef __DEBUG_LOG_PSR__
            L_ERROR(TAG, "[%d]section is NULL or section length is SHORT[%u]\n", i, pTable->m_pSectionLen[i]);
#endif
        }
    }

#ifdef __DEBUG_LOG_PSR__
    int i = 0;
#endif
    for(vector<SDT_Section*>::iterator itr = list_SDT_Section.begin(); itr != list_SDT_Section.end(); itr++) {
        SDT_Section* parser = (SDT_Section*) (*itr);
        mVersion = parser->mVersion;
        for (vector<SDT_Entry*>::iterator itr = parser->m_channel_List.begin(); itr != parser->m_channel_List.end(); itr++) {
            SDT_Entry* entry = (SDT_Entry*) (*itr);
            ServiceInfo* serviceInfo = new ServiceInfo();
            makeServiceInfo(serviceInfo, entry);
            serviceInfo->init();
            m_ServiceInfo_List.push_back(serviceInfo);
        }
#ifdef __DEBUG_LOG_PSR__
        L_DEBUG(TAG, "[%d] : channel_List.size() [%u] : Total[%u]\n", i++, parser->m_channel_List.size(), m_ServiceInfo_List.size());
#endif
    }

    for(vector<SDT_Section*>::iterator itr = list_SDT_Section.begin(); itr != list_SDT_Section.end(); itr++) {
        delete (*itr);
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
SDT_Section::SDT_Section() : mBXmlUse(false)
{
    mVersion = 0;
}

SDT_Section::~SDT_Section()
{
    for (std::vector<SDT_Entry*>::iterator it = m_channel_List.begin(); it != m_channel_List.end(); it++) {
        if((*it) != NULL)
            delete (*it);
    }
    m_channel_List.clear();
}

////////////////////////////////////////////////////////////////////////////////
bool SDT_Section::process(const uint8_t* buffer, uint32_t sectionLength)
{
    uint32_t transport_stream_id;
    uint32_t original_network_id;

    mVersion = buffer[5] & 0x3f >> 1;
    transport_stream_id = r16(&buffer[3]);
    original_network_id = r16(&buffer[8]);

    uint16_t bytesLeft = sectionLength > 12 ? sectionLength - 12 : 0;
    uint16_t loopLength = 0;

    size_t pos = SDT_HEADER_LENGTH;
    while (bytesLeft > 4 && bytesLeft >= (loopLength = 5 + DVB_LENGTH(&buffer[pos + 3]))) {
        SDT_Entry* sdtEntry = new SDT_Entry();
        sdtEntry->setXMLUse(mBXmlUse);
        sdtEntry->transport_stream_id = transport_stream_id;
        sdtEntry->original_network_id = original_network_id;

        sdtEntry->process((uint8_t *) &buffer[pos]);

        bytesLeft -= loopLength;
        pos += loopLength;

        m_channel_List.push_back(sdtEntry);
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
SDT_Entry::SDT_Entry()
        : m_service_id(0), m_running_status(0), m_free_CA_mode(0), mBXmlUse(false)
{
    transport_stream_id = 0;
    original_network_id = 0;
    dc = new DVB_Descriptor_Container();
    xmlParser = new XMLParser();
}

SDT_Entry::~SDT_Entry()
{
    for (vector<TiXmlNode*>::iterator itr = m_desc_xml_list.begin(); itr != m_desc_xml_list.end(); itr++) {
        TiXmlNode* n = (TiXmlNode*) *itr;

        n->Clear();
        delete n;
        n = NULL;
    }

    m_desc_xml_list.clear();

    if(dc != NULL) {
        delete dc;
        dc = NULL;
    }

    if(xmlParser != NULL) {
        delete xmlParser;
        xmlParser = NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////
bool SDT_Entry::process(const uint8_t* buffer)
{
    uint32_t descriptor_length;

    m_service_id = r16(&buffer[0]);
    m_running_status = (buffer[3] >> 5) & 0x07;
    m_free_CA_mode = (buffer[3] >> 4) & 0x01;

    descriptor_length = DVB_LENGTH(&buffer[3]) + 5;

    for (size_t i = 5; i < (size_t)(descriptor_length - 2); i += buffer[i + 1] + 2) {
        uint8_t* desc_ptr = (uint8_t *) &buffer[i];
        uint8_t descriptor_id = desc_ptr[0];
        uint8_t descriptor_len = desc_ptr[1];
        if(mBXmlUse) {
            BitStream* desc_bs = new BitStream(desc_ptr, descriptor_len + 2, false);
            TiXmlNode* desc_xml;

            desc_xml = processXMLDescriptor(desc_bs, descriptor_len);
            if(desc_xml)
                m_desc_xml_list.push_back(desc_xml);

            delete desc_bs;
        } else {
            if(descriptor_id >= 0x80) {
                BitStream* desc_bs = new BitStream(desc_ptr, descriptor_len + 2, false);
                TiXmlNode* desc_xml;

                desc_xml = processXMLDescriptor(desc_bs, descriptor_len);
                if(desc_xml)
                    m_desc_xml_list.push_back(desc_xml);

                delete desc_bs;
            } else {
                dc->descriptor(desc_ptr);
            }
        }
    }

    return true;
}

TiXmlNode* SDT_Entry::processXMLDescriptor(BitStream* bs, int length)
{
    TiXmlNode* xmlNode = xmlParser->parseDescriptor(bs, length);

    return xmlNode;
}

////////////////////////////////////////////////////////////////////////////////
