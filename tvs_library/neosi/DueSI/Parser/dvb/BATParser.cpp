/*
 * BATParser.cpp
 *
 *  Created on: Jan 16, 2015
 *      Author: wjpark
 */

#include "BATParser.h"

#include "Logger.h"
#include "Element.h"
#include "StringUtil.h"
#include "TimeConvertor.h"
#include "ScanManager/Table.h"

#include "./descriptor/DVB_Linkage_Descriptor.h"
#include "./descriptor/DVB_Extension_Descriptor.h"
#include "./descriptor/DVB_Private_Data_Specifier_Descriptor.h"
#include "./descriptor/DVB_Bouquet_Name_Descriptor.h"
#include "./descriptor/DVB_Linkage_Descriptor.h"
#include "./descriptor/DVB_Extension_Descriptor.h"
#include "../dvbc/linknet/DVB_Logical_Channel_Descriptor.h"
#include "./descriptor/DVB_Network_Name_Descriptor.h"


static const char* TAG = "BATParser";

#define BAT_HEADER_LENGTH 12
#define BAT_ENTRY_LENGTH 6

#define __DEBUG_LOG_PSR__

BATParser::~BATParser()
{
}

void BATParser::processBATEntry(uint8_t* ptr, uint32_t entry_length, uint16_t bouquet_id)
{
    uint16_t transport_stream_id;
    uint16_t original_network_id;
    uint32_t descriptor_length;
    transport_stream_id = r16(&ptr[0]);
    original_network_id = r16(&ptr[2]);
    descriptor_length = DVB_LENGTH(&ptr[4]);

    DVB_Descriptor_Container* dc = new DVB_Descriptor_Container();
    DescriptorContainerLinknet* dc_linknet = new DescriptorContainerLinknet();

    uint32_t len = descriptor_length;
    uint32_t pos = 0;
    while (len > 2) {
        uint8_t* desc_ptr = &ptr[6 + pos];
        uint8_t descriptor_id = desc_ptr[0];
        uint8_t descriptor_length = desc_ptr[1];
		if(descriptor_id >= 0x93) { // logical channel
			dc_linknet->descriptor(desc_ptr);
		} else {
			dc->descriptor(desc_ptr);
		}
        len -= descriptor_length + 2;
        pos += descriptor_length + 2;
    }

    const DescriptorList* descList = dc->getDescriptors();

    if(descList->size() > 0) {
        processDescriptors(descList);
    }

    const DescriptorList* descList_linknet = dc_linknet->getDescriptors();

       if(descList_linknet->size() > 0)
           processLinknetDescriptors(descList_linknet);

    if(bReceivedLogicalChannel) {
        for (LogicalChannelItemConstIterator itr = mItemList->begin(); itr != mItemList->end(); itr++) {
        	LogicalChannelItem* item = (LogicalChannelItem*) (*itr);
            BATInfo* batInfo = new BATInfo();

            uint16_t service_id = item->getServiceId();
            batInfo->bouquet_id = bouquet_id;
            batInfo->transport_stream_id = transport_stream_id;
            batInfo->original_network_id = original_network_id;
            batInfo->service_id = service_id;
            batInfo->local_channel_number = item->getChannelNumber();
            L_INFO(TAG,"service id = %d, channel number = %d\n", batInfo->service_id, batInfo->local_channel_number);
            m_BAT_List.push_back(batInfo);
        }
    }
    delete dc;
    delete dc_linknet;
}

void BATParser::processBAT(uint8_t* ptr, uint32_t section_length)
{
    uint16_t bouquet_id;
    uint16_t descriptor_length;
    bouquet_id = r16(&ptr[3]);
    mVersion = (ptr[5] & 0x3f) >> 1;
    descriptor_length = DVB_LENGTH(&ptr[8]);

    DVB_Descriptor_Container* dc = new DVB_Descriptor_Container();
    DescriptorContainerLinknet* dc_linknet = new DescriptorContainerLinknet();

    uint32_t len = descriptor_length;
    uint32_t pos = 0;
    while (len > 2) {
        uint8_t* desc_ptr = &ptr[10 + pos];
        uint8_t descriptor_id = desc_ptr[0];
        uint8_t descriptor_length = desc_ptr[1];

        if(descriptor_id >= 0x93) {	// logical channel
        	dc_linknet->descriptor(desc_ptr);
        	L_INFO(TAG,"\n");
		} else {
			dc->descriptor(desc_ptr);
        	L_INFO(TAG,"\n");

		}
        len -= descriptor_length + 2;
        pos += descriptor_length + 2;
    }

    const DescriptorList* descList = dc->getDescriptors();

    if(descList->size() > 0)
        processDescriptors(descList);

     const DescriptorList* descList_linknet = dc_linknet->getDescriptors();

     if(descList_linknet->size() > 0)
         processLinknetDescriptors(descList_linknet);

    delete dc;
    dc = NULL;

    delete dc_linknet;
    dc_linknet = NULL;

    pos = BAT_HEADER_LENGTH + descriptor_length;
    uint32_t entry_length = section_length - pos - 4;
    while (entry_length > BAT_ENTRY_LENGTH) {
        processBATEntry(&ptr[pos], entry_length, bouquet_id);
        uint16_t length = DVB_LENGTH(&ptr[pos + 4]) + BAT_ENTRY_LENGTH;

        pos += length;
        entry_length -= length;
    }
}

bool BATParser::process(void* sectionData)
{
    Table* pTable = (Table*) sectionData;

    for (int i = 0; i < pTable->m_SectionCount; i++) {
        uint8_t* ptr = pTable->m_pSection[i];
        if(ptr && pTable->m_pSectionLen[i] > 12) {
            processBAT(pTable->m_pSection[i], pTable->m_pSectionLen[i]);
        } else {
#ifdef __DEBUG_LOG_PSR__
            L_ERROR(TAG, "[%d]section is NULL or section length is SHORT[%u]\n", i, pTable->m_pSectionLen[i]);
#endif
        }
    }

    return true;
}

void BATParser::init()
{
    bReceivedLogicalChannel = false;
}

void BATParser::processDescriptors(const DescriptorList* descriptorList)
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
                L_INFO(TAG, "DVB_SERVICE_LIST_DESCRIPTOR\n");
                DVB_Service_List_Descriptor* desc = (DVB_Service_List_Descriptor*) (*i);
                const ServiceListItemList* serviceList = desc->getServiceList();
				for(ServiceListItemConstIterator itr = serviceList->begin(); itr != serviceList->end(); itr++) {
					DVB_Service_List_Descriptor* sl = (DVB_Service_List_Descriptor*) (*itr);
				}
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
            case DVB_BOUQUET_NAME_DESCRIPTOR: {
            	DVB_Bouquet_Name_Descriptor* desc = (DVB_Bouquet_Name_Descriptor *) (*i);
            	L_INFO(TAG, "BOUQUET_NAME_DESCRIPTOR [%s]\n",desc->getBouquetName().c_str());
                break;
            }
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
            case DVB_MULTILINGUAL_NETWORK_NAME_DESCRIPTOR:{
				L_INFO(TAG, "MULTILINGUAL_NETWORK_NAME_DESCRIPTOR\n");
				DVB_Multilingual_Network_Name_Descriptor* desc = (DVB_Multilingual_Network_Name_Descriptor *) (*i);

				const MultilingualNetworkNameList* nwnameList = desc->getMultilingualNetworkNames();
				for(MultilingualNetworkNameConstIterator itr = nwnameList->begin(); itr != nwnameList->end(); itr++) {
					MultilingualNetworkName* nwname = (MultilingualNetworkName*) (*itr);
					string iso639LanguageCode = nwname->getIso639LanguageCode();
					string networkname = nwname->getNetworkName();
					L_INFO(TAG, " iso639LanguageCode[%s] networkname [%s]\n", iso639LanguageCode.c_str(), networkname.c_str());
				}
            }
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

//            case DVB_LOGICAL_CHANNEL_DESCRIPTOR: {
//            	L_INFO(TAG, "DVB_LOGICAL_CHANNEL_DESCRIPTOR\n");
//            				DVB_Logical_Channel_Descriptor* desc = (DVB_Logical_Channel_Descriptor*) (*i);
//            				mItemList = desc->getChannelList();
//            				bReceivedLogicalChannel = true;
//				break;
//			}

            default:
            L_INFO(TAG, "OOPS : Unknown : 0x%02x\n", (*i)->getTag());
                break;
        }

    }
}

void BATParser::processLinknetDescriptors(const DescriptorList* descriptorList)
{
	for (DescriptorConstIterator i = descriptorList->begin(); i != descriptorList->end(); i++) {
	switch ((*i)->getTag()) {
		case LOGICAL_CHANNEL_DESCRIPTOR: {
			L_INFO(TAG, "DVB_LOGICAL_CHANNEL_DESCRIPTOR\n");
			DVB_Logical_Channel_Descriptor* desc = (DVB_Logical_Channel_Descriptor*) (*i);
			mItemList = desc->getChannelList();
			bReceivedLogicalChannel = true;
			break;
		}
		default:
		L_INFO(TAG, "OOPS : Unknown : 0x%02x\n", (*i)->getTag());
			break;
		}
	}
}

