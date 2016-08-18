/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: loneowl $
 * $LastChangedDate: 2014-04-15 11:11:57 +0900 (화, 15 4월 2014) $
 * $LastChangedRevision: 610 $
 * Description:
 * Note:
 *****************************************************************************/

#include "DVB_Descriptor_Container.h"
#include "Logger.h"

static const char* TAG = "DVB_Descriptor_Container";

////////////////////////////////////////////////////////////////////////////////
DVB_Descriptor_Container::~DVB_Descriptor_Container()
{
    for (DescriptorIterator i = descriptorList.begin(); i != descriptorList.end(); ++i)
        delete *i;
}

void DVB_Descriptor_Container::descriptor(const uint8_t * const buffer)
{
    Descriptor *d;

    d = descriptorSi(buffer);

    if(!d->isValid())
        delete d;

    else descriptorList.push_back(d);
}

Descriptor *DVB_Descriptor_Container::descriptorSi(const uint8_t * const buffer)
{
	Descriptor* descriptor = NULL;
	switch (buffer[0])
	{
        /* 0x40 - 0x7F: ETSI EN 300 468 V1.5.1 (2003-05) */
        case DVB_NETWORK_NAME_DESCRIPTOR:               descriptor = new DVB_Network_Name_Descriptor(buffer); break;
        case DVB_SERVICE_LIST_DESCRIPTOR:               descriptor = new DVB_Service_List_Descriptor(buffer); break;
        case DVB_STUFFING_DESCRIPTOR:                   descriptor = new DVB_Stuffing_Descriptor(buffer); break;
        case DVB_SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR:  descriptor = new DVB_Satellite_Delivery_System_Descriptor(buffer); break;
        case DVB_CABLE_DELIVERY_SYSTEM_DESCRIPTOR:      descriptor = new DVB_Cable_Delivery_System_Descriptor(buffer); break;
        case DVB_VBI_DATA_DESCRIPTOR:                   descriptor = new DVB_VBI_Data_Descriptor(buffer); break;
        case DVB_VBI_TELETEXT_DESCRIPTOR:               descriptor = new DVB_VBI_Teletext_Descriptor(buffer); break;
        case DVB_BOUQUET_NAME_DESCRIPTOR:               descriptor = new DVB_Bouquet_Name_Descriptor(buffer); break;
        case DVB_SERVICE_DESCRIPTOR:                    descriptor = new DVB_Service_Descriptor(buffer); break;
        case DVB_COUNTRY_AVAILABILITY_DESCRIPTOR:       descriptor = new DVB_Country_Availablity_Descriptor(buffer); break;
        case DVB_LINKAGE_DESCRIPTOR:                    descriptor = new DVB_Linkage_Descriptor(buffer); break;
        case DVB_NVOD_REFERENCE_DESCRIPTOR:             descriptor = new DVB_NVOD_Reference_Descriptor(buffer); break;
        case DVB_TIME_SHIFTED_SERVICE_DESCRIPTOR:       descriptor = new DVB_Time_Shifted_Service_Descriptor(buffer); break;
        case DVB_SHORT_EVENT_DESCRIPTOR:                descriptor = new DVB_Short_Event_Descriptor(buffer); break;
        case DVB_EXTENDED_EVENT_DESCRIPTOR:             descriptor = new DVB_Extended_Event_Descriptor(buffer); break;
        case DVB_TIME_SHIFTED_EVENT_DESCRIPTOR:         descriptor = new DVB_Time_Shifted_Event_Descriptor(buffer); break;
        case DVB_COMPONENT_DESCRIPTOR:                  descriptor = new DVB_Component_Descriptor(buffer); break;
        case DVB_MOSAIC_DESCRIPTOR:                     descriptor = new DVB_Mosaic_Descriptor(buffer); break;
        case DVB_STREAM_IDENTIFIER_DESCRIPTOR:          descriptor = new DVB_Stream_Identifier_Descriptor(buffer); break;
        case DVB_CA_IDENTIFIER_DESCRIPTOR:              descriptor = new DVB_CA_Identifier_Descriptor(buffer); break;
        case DVB_CONTENT_DESCRIPTOR:                    descriptor = new DVB_Content_Descriptor(buffer); break;
        case DVB_PARENTAL_RATING_DESCRIPTOR:            descriptor = new DVB_Parental_Rating_Descriptor(buffer); break;
        case DVB_TELETEXT_DESCRIPTOR:                   descriptor = new DVB_Teletext_Descriptor(buffer); break;
        case DVB_TELEPHONE_DESCRIPTOR:                  descriptor = new DVB_Telephone_Descriptor(buffer); break;
        case DVB_LOCAL_TIME_OFFSET_DESCRIPTOR:          descriptor = new DVB_Local_Time_Offset_Descriptor(buffer); break;
        case DVB_SUBTITLING_DESCRIPTOR:                 descriptor = new DVB_Subtitling_Descriptor(buffer); break;
        case DVB_TERRESTRIAL_DELIVERY_SYSTEM_DESCRIPTOR:    descriptor = new DVB_Terrestrial_Delivery_System_Descriptor(buffer); break;
        case DVB_MULTILINGUAL_NETWORK_NAME_DESCRIPTOR:  descriptor = new DVB_Multilingual_Network_Name_Descriptor(buffer); break;
        case DVB_MULTILINGUAL_BOUQUET_NAME_DESCRIPTOR:  descriptor = new DVB_Multilingual_Bouquet_Name_Descriptor(buffer); break;
        case DVB_MULTILINGUAL_SERVICE_NAME_DESCRIPTOR:  descriptor = new DVB_Multilingual_Service_Name_Descriptor(buffer); break;
        case DVB_MULTILINGUAL_COMPONENT_DESCRIPTOR:     descriptor = new DVB_Multilingual_Component_Name_Descriptor(buffer); break;
        case DVB_PRIVATE_DATA_SPECIFIER_DESCRIPTOR:     descriptor = new DVB_Private_Data_Specifier_Descriptor(buffer); break;
        case DVB_SERVICE_MOVE_DESCRIPTOR:               descriptor = new DVB_Service_Move_Descriptor(buffer); break;
        case DVB_SHORT_SMOOTHING_BUFFER_DESCRIPTOR:     descriptor = new DVB_Short_Smoothing_Buffer_Descriptor(buffer); break;
        case DVB_FREQUENCY_LIST_DESCRIPTOR:             descriptor = new DVB_Frequency_List_Descriptor(buffer); break;
        case DVB_PARTIAL_TRANSPORT_STREAM_DESCRIPTOR:   descriptor = new DVB_Partial_Transport_Stream_Descriptor(buffer); break;
        case DVB_DATA_BROADCAST_DESCRIPTOR:             descriptor = new DVB_Data_Broadcast_Descriptor(buffer); break;
        case DVB_SCRAMBLING_DESCRIPTOR:                 descriptor = new DVB_Scrambling_Descriptor(buffer); break;
        case DVB_DATA_BROADCAST_ID_DESCRIPTOR:          descriptor = new DVB_Data_Broadcast_ID_Descriptor(buffer); break;
        case DVB_TRANSPORT_STREAM_DESCRIPTOR:           descriptor = new DVB_Transport_Stream_Descriptor(buffer); break;
        case DVB_DSNG_DESCRIPTOR:                       descriptor = new DVB_DSNG_Descriptor(buffer); break;
        case DVB_PDC_DESCRIPTOR:                        descriptor = new DVB_PDC_Descriptor(buffer); break;
        case DVB_AC3_DESCRIPTOR:                        descriptor = new DVB_AC3_Descriptor(buffer);        break;
        case DVB_ANCILLARY_DATA_DESCRIPTOR:             descriptor = new DVB_Ancillary_Data_Descriptor(buffer); break;
        case DVB_CELL_LIST_DESCRIPTOR:                  descriptor = new DVB_Cell_List_Descriptor(buffer); break;
        case DVB_CELL_FREQUENCY_LINK_DESCRIPTOR:        descriptor = new DVB_Cell_Frequency_Link_Descriptor(buffer); break;
        case DVB_ANNOUNCEMENT_SUPPORT_DESCRIPTOR:       descriptor = new DVB_Announcement_Support_Descriptor(buffer); break;
        case DVB_APPLICATION_SIGNALLING_DESCRIPTOR:     descriptor = new DVB_Application_Signalling_Descriptor(buffer); break;
        case DVB_ADAPTATION_FIELD_DATA_DESCRIPTOR:      descriptor = new DVB_Adaptation_Field_Data_Descriptor(buffer); break;
        case DVB_SERVICE_IDENTIFIER_DESCRIPTOR:         descriptor = new DVB_Service_Identifier_Descriptor(buffer); break;
        case DVB_SERVICE_AVAILABILITY_DESCRIPTOR:       descriptor = new DVB_Service_Availablity_Descriptor(buffer); break;
        case DVB_DEFAULT_AUTHORITY_DESCRIPTOR:          descriptor = new DVB_Default_Authority_Descriptor(buffer); break;
        case DVB_RELATED_CONTENT_DESCRIPTOR:            descriptor = new DVB_Related_Content_Descriptor(buffer); break;
        case DVB_TVA_ID_DESCRIPTOR:                     descriptor = new DVB_TVA_ID_Descriptor(buffer); break;
        case DVB_CONTENT_IDENTIFIER_DESCRIPTOR:         descriptor = new DVB_Content_Identifier_Descriptor(buffer); break;
        case DVB_TIME_SLICE_FEC_IDENTIFIER_DESCRIPTOR:  descriptor = new DVB_Time_Slice_FEC_Identifier_Descriptor(buffer); break;
        case DVB_ECM_REPETITION_RATE_DESCRIPTOR:        descriptor = new DVB_ECM_Repetition_Rate_Descriptor(buffer); break;
        case DVB_S2_SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR:   descriptor = new DVB_S2_Satellite_Delivery_System_Descriptor(buffer); break;
        case DVB_ENHANCED_AC3_DESCRIPTOR:               descriptor = new DVB_Enhanced_AC3_Descriptor(buffer); break;
        case DVB_DTS_DESCRIPTOR:                        descriptor = new DVB_DTS_Descriptor(buffer); break;
        case DVB_AAC_DESCRIPTOR:                        descriptor = new DVB_AAC_Descriptor(buffer);        break;
        case DVB_EXTENSION_DESCRIPTOR:                  descriptor = new DVB_Extension_Descriptor(buffer); break;

	default:
		L_INFO(TAG, "unsupported descriptor_tag[0x%2x]\n", buffer[0]);
		descriptor = new Descriptor(buffer);
		break;
	} // END switch ()

	return descriptor;
}

const DescriptorList *DVB_Descriptor_Container::getDescriptors(void) const
{
    return &descriptorList;
}


////////////////////////////////////////////////////////////////////////////////



