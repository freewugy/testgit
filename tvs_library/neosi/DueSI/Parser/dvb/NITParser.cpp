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

#include "NITParser.h"

#include "Logger.h"
#include "Element.h"
#include "StringUtil.h"
#include "TimeConvertor.h"
#include "Table.h"

#include "./descriptor/DVB_Cable_Delivery_System_Descriptor.h"
#include "./descriptor/DVB_Terrestrial_Delivery_System_Descriptor.h"
#include "./descriptor/DVB_Satellite_Delivery_System_Descriptor.h"
#include "./descriptor/DVB_S2_Satellite_Delivery_System_Descriptor.h"
#include "./descriptor/DVB_Private_Data_Specifier_Descriptor.h"
#include "./descriptor/DVB_Network_Name_Descriptor.h"
#include "./descriptor/DVB_Linkage_Descriptor.h"
#include "./descriptor/DVB_Extension_Descriptor.h"

#define NIT_HEADER_LENGTH 12
#define NIT_ENTRY_LENGTH 6

static const char* TAG = "NITParser";

NITParser::~NITParser()
{
    if(xmlParser != NULL) {
        delete xmlParser;
        xmlParser = NULL;
    }
}

TiXmlNode* NITParser::processXMLDescriptor(BitStream* bs, int length)
{
    TiXmlNode* xmlNode = xmlParser->parseDescriptor(bs, length);

    return xmlNode;
}

void NITParser::processChildElements(TiXmlNode *n)
{
    const char *name = n->Value();

    if(strcmp(name, "elm") == 0) {
        string sName = n->ToElement()->Attribute("name");
        if(hasPrefix("length_of_item", sName) || hasSuffix(sName, "_length") || hasSuffix(sName, "descriptor_number") || hasSuffix(sName, "_count")
                || sName.compare("descriptor_tag_extension") == 0) {

        } else {
            L_INFO(TAG, "Name[%s], Value[%s]\n", sName.c_str(), n->ToElement()->Attribute("value"));
        }
    }

    if(n == NULL || n->NoChildren())
        return;

    for (TiXmlNode *child = n->FirstChild(); child != 0; child = child->NextSibling()) {
        processChildElements(child);
    }
    return;
}

void NITParser::processDescriptors(const DescriptorList* descriptorList)
{
    for (DescriptorConstIterator i = descriptorList->begin(); i != descriptorList->end(); i++) {
        switch ((*i)->getTag()) {
            /* 0x40 - 0x7F: ETSI EN 300 468 V1.5.1 (2003-05) */
            case DVB_NETWORK_NAME_DESCRIPTOR: {
                DVB_Network_Name_Descriptor* desc = (DVB_Network_Name_Descriptor*) (*i);

                L_INFO(TAG, "NetworkNameDescriptor : NetworkName [%s]\n", stripStrUTFHeader(desc->getNetworkName()).c_str());
                break;
            }
            case DVB_SERVICE_LIST_DESCRIPTOR: {
                DVB_Service_List_Descriptor* desc = (DVB_Service_List_Descriptor*) (*i);
                mItemList = desc->getServiceList();
                bReceivedServiceList = true;
                break;
            }
            case DVB_LINKAGE_DESCRIPTOR: {
                DVB_Linkage_Descriptor* desc = (DVB_Linkage_Descriptor*) (*i);

                uint8_t linkageType = desc->getLinkageType();
                uint16_t original_network_id = desc->getOriginalNetworkId();
                uint16_t transport_stream_id = desc->getTransportStreamId();
                uint16_t service_id = desc->getServiceId();
#ifdef __DEBUG_LOG_PSR__
                L_INFO(TAG, "LinkageDescriptor : linkageType[%u], original_network_id[%u], transport_stream_id[%u], service_id[%u]\n",
                        linkageType, original_network_id, transport_stream_id, service_id);
#endif
                if(linkageType == 4 && service_id != 0) {
                    char chr[32];
                    sprintf(chr, "%u.%u.%u", original_network_id, transport_stream_id, service_id);

                    barker_channel_uid.clear();
                    barker_channel_uid.append(chr);

                    barker_channel_id = service_id;
#ifdef __DEBUG_LOG_PSR__
                    L_DEBUG(TAG, "LinkageDescriptor : barker_channel_uid [%s] ===\n", barker_channel_uid.c_str());
#endif
                }

                break;
            }
            case DVB_SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR: {
                DVB_Satellite_Delivery_System_Descriptor* desc = (DVB_Satellite_Delivery_System_Descriptor*) (*i);

                mTunerDeliveryParam.satellite.frequency = desc->getFrequency(); // (e.g. 011,75725GHz)
                mTunerDeliveryParam.satellite.orbitalPosition = desc->getOrbitalPosition();
                mTunerDeliveryParam.satellite.westEastFlag = desc->getWestEastFlag();
                mTunerDeliveryParam.satellite.polarization = desc->getPolarization();
                mTunerDeliveryParam.satellite.rollOff = desc->getRollOff();
                mTunerDeliveryParam.satellite.modulationSystem = desc->getModulationSystem();
                mTunerDeliveryParam.satellite.modulationType = desc->getModulation();
                mTunerDeliveryParam.satellite.symbolRate = desc->getSymbolRate();
                mTunerDeliveryParam.satellite.fecInner = desc->getFecInner();

                break;
            }
            case DVB_CABLE_DELIVERY_SYSTEM_DESCRIPTOR: {
                DVB_Cable_Delivery_System_Descriptor* desc = (DVB_Cable_Delivery_System_Descriptor*) (*i);

                mTunerDeliveryParam.cable.frequency = desc->getFrequency() / 10;
                // the frequency is coded in MHz, where the decimal occurs after the fourth charactor (e.g. 012,0000 MHz)
                mTunerDeliveryParam.cable.modulation = desc->getModulation();
                mTunerDeliveryParam.cable.symbolRate = desc->getSymbolRate() / 10;
                // symbol_rate in Msymbol/s where the decimal point occurs after the third charactor (e.g. 027,4500)
                mTunerDeliveryParam.cable.fecInner = desc->getFecInner();
                break;
            }
            case DVB_TERRESTRIAL_DELIVERY_SYSTEM_DESCRIPTOR: {
                DVB_Terrestrial_Delivery_System_Descriptor* desc = (DVB_Terrestrial_Delivery_System_Descriptor *) (*i);

                mTunerDeliveryParam.terrestrial.centreFrequency = desc->getCentreFrequency();
                mTunerDeliveryParam.terrestrial.bandwidth = desc->getBandwidth();
                mTunerDeliveryParam.terrestrial.constellation = desc->getConstellation();
                mTunerDeliveryParam.terrestrial.hierarchyInformation = desc->getHierarchyInformation();
                mTunerDeliveryParam.terrestrial.codeRateHpStream = desc->getCodeRateHpStream();
                mTunerDeliveryParam.terrestrial.codeRateLpStream = desc->getCodeRateLpStream();
                mTunerDeliveryParam.terrestrial.guardInterval = desc->getGuardInterval();
                mTunerDeliveryParam.terrestrial.transmissionMode = desc->getTransmissionMode();
                mTunerDeliveryParam.terrestrial.otherFrequencyFlag = desc->getOtherFrequencyFlag();

                break;
            }
            case DVB_S2_SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR: {
                DVB_S2_Satellite_Delivery_System_Descriptor* desc = (DVB_S2_Satellite_Delivery_System_Descriptor*) (*i);

                mTunerDeliveryParam.s2satellite.scramblingSequenceSelector = desc->getScramblingSequenceSelector();
                mTunerDeliveryParam.s2satellite.multipleInputStreamFlag = desc->getMultipleInputStreamFlag();
                mTunerDeliveryParam.s2satellite.backwardsCompatibilityIndicator = desc->getBackwardsCompatibilityIndicator();
                mTunerDeliveryParam.s2satellite.scramblingSequenceIndex = desc->getScramblingSequenceIndex();
                mTunerDeliveryParam.s2satellite.inputStreamIdentifier = desc->getInputStreamIdentifier();

                break;
            }
            case DVB_EXTENSION_DESCRIPTOR: {
                DVB_Extension_Descriptor* desc = (DVB_Extension_Descriptor*) (*i);
                uint8_t extensionTag = desc->getExtensionTag();
                const SelectorByteVector* extensionBytes = desc->getSelectorBytes();
                uint8_t length = extensionBytes->size();
                L_DEBUG(TAG, "Extension Descriptor : Extension_TAG [0x%02x], length [%u]\n", extensionTag, length);

                if(extensionTag == 0x87) {
                    uint8_t desc_ptr[length -2];
                    memcpy(desc_ptr, &(*extensionBytes)[2], length - 2);
                    string str;
                    str.assign((const char*)desc_ptr, length - 2);
                    setBarkerChannelData(str);
                }
                break;
            }
            case DVB_PRIVATE_DATA_SPECIFIER_DESCRIPTOR: {
                DVB_Private_Data_Specifier_Descriptor* desc = (DVB_Private_Data_Specifier_Descriptor *) (*i);
                uint32_t privateDataSpecifier = desc->getPrivateDataSpecifier();

                L_INFO(TAG, "PRIVATE_DATA_SPECIFIER_DESCRIPTOR : privateDataSpecifier[%u]\n", privateDataSpecifier);
                break;
            }
            case DVB_STUFFING_DESCRIPTOR:
            L_INFO(TAG, "STUFFING_DESCRIPTOR\n");
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
            case DVB_NVOD_REFERENCE_DESCRIPTOR:
            L_INFO(TAG, "NVOD_REFERENCE_DESCRIPTOR\n");
                break;
            case DVB_TIME_SHIFTED_SERVICE_DESCRIPTOR:
            L_INFO(TAG, "TIME_SHIFTED_SERVICE_DESCRIPTOR\n");
                break;
            case DVB_SHORT_EVENT_DESCRIPTOR:
                L_INFO(TAG, "SHORT_EVENT_DESCRIPTOR\n");
                break;
            case DVB_EXTENDED_EVENT_DESCRIPTOR:
                L_INFO(TAG, "EXTENDED_EVENT_DESCRIPTOR\n");
                break;
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
            case DVB_CA_IDENTIFIER_DESCRIPTOR:
            L_INFO(TAG, "CA_IDENTIFIER_DESCRIPTOR\n");
                break;
            case DVB_CONTENT_DESCRIPTOR:
            L_INFO(TAG, "CONTENT_DESCRIPTOR\n");
                break;
            case DVB_PARENTAL_RATING_DESCRIPTOR:
            L_INFO(TAG, "PARENTAL_RATING_DESCRIPTOR\n");
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
            /* 0x80 - 0xFE: User defined */
            default:
            L_INFO(TAG, "OOPS : Unknown : 0x%02x\n", (*i)->getTag());
                break;
        }

    }
}

void NITParser::setBarkerChannelData(string barker_data) {
    string sub;
    vector<string> elm;
    splitStringByDelimiter(barker_data, "/", elm);

    sub = elm[elm.size() - 1];
    elm.clear();

    splitStringByDelimiter(sub, ".", elm);
    if(elm.size() != 3) {
        L_ERROR(TAG, "Wrong Data Format : %s\n", sub.c_str());
        return;
    }
    uint16_t original_network_id = hexCharToInt(elm[0].c_str(), elm[0].length());
    uint16_t transport_stream_id = hexCharToInt(elm[1].c_str(), elm[1].length());
    this->barker_channel_id = hexCharToInt(elm[2].c_str(), elm[2].length());

    char chr[32];
    sprintf(chr, "%u.%u.%u", original_network_id, transport_stream_id, barker_channel_id);

    barker_channel_uid.clear();
    barker_channel_uid.append(chr);

    L_INFO(TAG, "Barker Channel Info : UID[%s], id[%d]\n", barker_channel_uid.c_str(), barker_channel_id);
}

void NITParser::processChannelDescriptor(TiXmlNode *n)
{
    for (TiXmlNode *entry = n->FirstChild("entry"); entry != 0; entry = entry->NextSibling("entry")) {
        uint16_t service_id = 0, channel_number = 0;

        for (TiXmlNode* elm = entry->FirstChild("elm"); elm; elm = elm->NextSibling("elm")) {
            string sName(elm->ToElement()->Attribute("name"));
            if(sName.compare("service_id") == 0) {
                service_id = (uint16_t) atoi(elm->ToElement()->Attribute("value"));
            } else if(sName.compare("channel_number") == 0) {
                channel_number = (uint16_t) atoi(elm->ToElement()->Attribute("value"));
            }
        }

        if(service_id > 0 && channel_number > 0)
            mMapServiceIDChannelNumber.insert(make_pair(service_id, channel_number));
    }
}

void NITParser::processProductListDescriptor(TiXmlNode *n)
{
    ProductInfo productInfo;
    productInfo.product_type = 0;
    productInfo.id_product = "";
    productInfo.nm_product = "";
    productInfo.desc_product = "";
    productInfo.FG_term = 0;
    productInfo.FG_value = 0;
    productInfo.amt_price = 0;
    productInfo.amt_dc_incr = 0;
    productInfo.dt_product_start = 0;
    productInfo.dt_product_end = 0;
    uint32_t start_time_MJD = 0, start_time = 0, end_time_MJD = 0, end_time = 0;

    for (TiXmlNode* child = n->FirstChild("elm"); child; child = child->NextSibling("elm")) {
        const char* ename = child->ToElement()->Attribute("name");
        const char* evalue = child->ToElement()->Attribute("value");

        if(strcmp(ename, "product_type") == 0) {
            productInfo.product_type = (uint8_t) strtoul(evalue, NULL, 0);
        } else if(strcmp(ename, "id_product") == 0) {
#ifdef __CONVERT_UTF__
            productInfo.id_product = stripStrUTFHeader(evalue);
#else
            productInfo.id_product = evalue;
#endif
        } else if(strcmp(ename, "nm_product") == 0) {
#ifdef __CONVERT_UTF__
            productInfo.nm_product = stripStrUTFHeader(evalue);
#else
            productInfo.nm_product = evalue;
#endif
        } else if(strcmp(ename, "desc_product") == 0) {
#ifdef __CONVERT_UTF__
            productInfo.desc_product = stripStrUTFHeader(evalue);
#else
            productInfo.desc_product = evalue;
#endif
        } else if(strcmp(ename, "amt_price") == 0) {
            productInfo.amt_price = (uint32_t) strtoul(evalue, NULL, 0);
        } else if(strcmp(ename, "amt_dc_incr") == 0) {
            productInfo.amt_dc_incr = (uint32_t) strtoul(evalue, NULL, 0);
        } else if(strcmp(ename, "FG_term") == 0) {
            productInfo.FG_term = (uint8_t) strtoul(evalue, NULL, 0);
        } else if(strcmp(ename, "FG_value") == 0) {
            productInfo.FG_value = (uint8_t) strtoul(evalue, NULL, 0);
        } else if(strcmp(ename, "dt_product_start_MJD") == 0) {
            start_time_MJD = (uint32_t) strtoul(evalue, NULL, 0);
        } else if(strcmp(ename, "dt_product_start") == 0) {
            start_time = (uint32_t) strtoul(evalue, NULL, 0);
        } else if(strcmp(ename, "dt_product_end_MJD") == 0) {
            end_time_MJD = (uint32_t) strtoul(evalue, NULL, 0);
        } else if(strcmp(ename, "dt_product_end") == 0) {
            end_time = (uint32_t) strtoul(evalue, NULL, 0);
        }
    }

    if(start_time_MJD > 0 && start_time > 0) {
        productInfo.dt_product_start = TimeConvertor::getInstance().convertTimeMJDBCD2Seconds(start_time_MJD, start_time) + mUTCOffSet * 3600;
    }

    if(end_time_MJD > 0 && end_time > 0) {
        productInfo.dt_product_end = TimeConvertor::getInstance().convertTimeMJDBCD2Seconds(end_time_MJD, end_time) + mUTCOffSet * 3600;
    }

    m_ProductInfo_List.push_back(productInfo);
}

void NITParser::processLogicalChannelDescriptor(TiXmlNode *n)
{
    for (TiXmlNode *entry = n->FirstChild("entry"); entry != 0; entry = entry->NextSibling("entry")) {
        uint16_t service_id = 0, logical_channel_number = 0;
        uint8_t visible_service_flag = 0;

        for (TiXmlNode* elm = entry->FirstChild("elm"); elm; elm = elm->NextSibling("elm")) {
            string sName(elm->ToElement()->Attribute("name"));
            if(sName.compare("service_id") == 0) {
                service_id = (uint16_t) atoi(elm->ToElement()->Attribute("value"));
            } else if(sName.compare("logical_channel_number") == 0) {
                logical_channel_number = (uint16_t) atoi(elm->ToElement()->Attribute("value"));
            } else if(sName.compare("visible_service_flag") == 0) {
                visible_service_flag = (uint8_t) atoi(elm->ToElement()->Attribute("value"));
            }
        }

        L_DEBUG(TAG, "logical_channel_descriptor : service_id[%u], visible_service_flag[%u], logical_channel_number[%u]", service_id, visible_service_flag,
                logical_channel_number);
    }
}

void NITParser::processNITEntry(uint8_t* ptr, uint32_t entry_length, uint16_t network_id)
{
    uint16_t transport_stream_id;
    uint16_t original_network_id;
    uint32_t descriptor_length;

    vector<TiXmlNode*> desc_xml_list;

    transport_stream_id = r16(&ptr[0]);
    original_network_id = r16(&ptr[2]);
    descriptor_length = DVB_LENGTH(&ptr[4]);

    DVB_Descriptor_Container* dc = new DVB_Descriptor_Container();

    uint32_t len = descriptor_length;
    uint32_t pos = 0;
    while (len > 2) {
        uint8_t* desc_ptr = &ptr[6 + pos];
        uint8_t descriptor_id = desc_ptr[0];
        uint8_t descriptor_len = desc_ptr[1];
        if(descriptor_id >= 0x80) {
            BitStream* desc_bstream = new BitStream(desc_ptr, descriptor_len + 2, false);
            TiXmlNode* desc_xml;

            desc_xml = processXMLDescriptor(desc_bstream, descriptor_len);

            if(desc_xml)
                desc_xml_list.push_back(desc_xml);

            delete desc_bstream;
            desc_bstream = NULL;
        } else {
            dc->descriptor(desc_ptr);
        }
        len -= descriptor_len + 2;
        pos += descriptor_len + 2;
    }

    const DescriptorList* descList = dc->getDescriptors();

    if(descList->size() > 0) {
        processDescriptors(descList);
    }

    for (vector<TiXmlNode*>::iterator itr = desc_xml_list.begin(); itr != desc_xml_list.end(); itr++) {
        TiXmlNode* n = (TiXmlNode*) *itr;

        if(n) {
            const char* name = n->ToElement()->Attribute("name");

            if(strcmp(name, "channel_descriptor") == 0) {
                processChannelDescriptor(n);
            } else if(strcmp(name, "logical_channel_descriptor") == 0) {
                processLogicalChannelDescriptor(n);
            } else {
                processChildElements(n);
            }
        }
    }
    if(bReceivedServiceList) {
        for (ServiceListItemConstIterator itr = mItemList->begin(); itr != mItemList->end(); itr++) {
            ServiceListItem* item = (ServiceListItem*) (*itr);
            NITInfo* nitInfo = new NITInfo();

            uint16_t service_id = item->getServiceId();

            nitInfo->network_id = network_id;
            nitInfo->transport_stream_id = transport_stream_id;
            nitInfo->original_network_id = original_network_id;

            nitInfo->service_id = service_id;
            nitInfo->service_type = item->getServiceType();

            nitInfo->channel_number = service_id;
            nitInfo->mTunerDeliveryParam = mTunerDeliveryParam;
            m_NIT_List.push_back(nitInfo);
        }
    }

    for (vector<TiXmlNode*>::iterator itr = desc_xml_list.begin(); itr != desc_xml_list.end(); itr++) {
        TiXmlNode* n = (TiXmlNode*) *itr;

        n->Clear();
        delete n;
        n = NULL;
    }

    desc_xml_list.clear();

    if(dc != NULL) {
        delete dc;
        dc = NULL;
    }
}

void NITParser::processNIT(uint8_t* ptr, uint32_t section_length)
{
    uint16_t network_id;
    uint32_t descriptor_length;

    network_id = r16(&ptr[3]);
    mVersion = (ptr[5] & 0x3f) >> 1;
    descriptor_length = DVB_LENGTH(&ptr[8]);

    vector<TiXmlNode*> desc_xml_list;

    DVB_Descriptor_Container* dc = new DVB_Descriptor_Container();
    uint32_t len = descriptor_length;
    uint32_t pos = 0;
    while (len > 2) {
        uint8_t* desc_ptr = &ptr[10 + pos];
        uint8_t descriptor_id = desc_ptr[0];
        uint8_t descriptor_len = desc_ptr[1];
        if(descriptor_id >= 0x80) {
            BitStream* desc_bstream = new BitStream(desc_ptr, descriptor_len + 2, false);
            TiXmlNode* desc_xml;

            desc_xml = processXMLDescriptor(desc_bstream, descriptor_len);

            if(desc_xml)
                desc_xml_list.push_back(desc_xml);

            delete desc_bstream;
            desc_bstream = NULL;
        } else {
            dc->descriptor(desc_ptr);
        }
        len -= descriptor_len + 2;
        pos += descriptor_len + 2;
    }

    const DescriptorList* descList = dc->getDescriptors();

    if(descList->size() > 0)
        processDescriptors(descList);

    for (vector<TiXmlNode*>::iterator itr = desc_xml_list.begin(); itr != desc_xml_list.end(); itr++) {
        TiXmlNode* n = (TiXmlNode*) *itr;

        if(n) {
            const char* name = n->ToElement()->Attribute("name");

            if(strcmp(name, "channel_descriptor") == 0) {
                processChannelDescriptor(n);
            } else if(strcmp(name, "logical_channel_descriptor") == 0) {
                processLogicalChannelDescriptor(n);
            } else if(strcmp(name, "product_list_descriptor") == 0) {
                processProductListDescriptor(n);
            } else {
                processChildElements(n);
            }
        }
    }

    for (vector<TiXmlNode*>::iterator itr = desc_xml_list.begin(); itr != desc_xml_list.end(); itr++) {
        TiXmlNode* n = (TiXmlNode*) *itr;

        n->Clear();
        delete n;
        n = NULL;
    }

    desc_xml_list.clear();

    if(dc != NULL) {
        delete dc;
        dc = NULL;
    }

    pos = NIT_HEADER_LENGTH + descriptor_length;
    uint32_t entry_length = section_length - pos - 4;
    while (entry_length > NIT_ENTRY_LENGTH) {
        processNITEntry(&ptr[pos], entry_length, network_id);
        uint16_t length = DVB_LENGTH(&ptr[pos + 4]) + NIT_ENTRY_LENGTH;

        pos += length;
        entry_length -= length;
    }
}

bool NITParser::process(void* sectionData)
{
    Table* pTable = (Table*) sectionData;

    for (int i = 0; i < pTable->m_SectionCount; i++) {
        uint8_t* ptr = pTable->m_pSection[i];
        if(ptr && pTable->m_pSectionLen[i] > 12) {
            processNIT(pTable->m_pSection[i], pTable->m_pSectionLen[i]);
        } else {
#ifdef __DEBUG_LOG_PSR__
            L_ERROR(TAG, "[%d]section is NULL or section length is SHORT[%u]\n", i, pTable->m_pSectionLen[i]);
#endif
        }
    }

    return true;
}

void NITParser::init()
{
    barker_channel_id = 0;
    barker_channel_uid = "0.0.0";
    bReceivedServiceList = false;
    xmlParser = new XMLParser();
}

