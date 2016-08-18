/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: loneowl $
 * $LastChangedDate: 2014-04-10 15:55:50 +0900 (목, 10 4월 2014) $
 * $LastChangedRevision: 580 $
 * Description:
 * Note:
 *****************************************************************************/

#ifndef DVB_DESCRIPTOR_H_
#define DVB_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"
#include "DVB_Network_Name_Descriptor.h"
#include "DVB_Service_List_Descriptor.h"
#include "DVB_Stuffing_Descriptor.h"
#include "DVB_Satellite_Delivery_System_Descriptor.h"
#include "DVB_Cable_Delivery_System_Descriptor.h"
#include "DVB_VBI_Data_Descriptor.h"
#include "DVB_VBI_Teletext_Descriptor.h"
#include "DVB_Bouquet_Name_Descriptor.h"
#include "DVB_Service_Descriptor.h"
#include "DVB_Country_Availablity_Descriptor.h"
#include "DVB_Linkage_Descriptor.h"
#include "DVB_NVOD_Reference_Descriptor.h"
#include "DVB_Time_Shifted_Service_Descriptor.h"
#include "DVB_Short_Event_Descriptor.h"
#include "DVB_Extended_Event_Descriptor.h"
#include "DVB_Time_Shifted_Event_Descriptor.h"
#include "DVB_Component_Descriptor.h"
#include "DVB_Mosaic_Descriptor.h"
#include "DVB_Stream_Identifier_Descriptor.h"
#include "DVB_CA_Identifier_Descriptor.h"
#include "DVB_Content_Descriptor.h"
#include "DVB_Parental_Rating_Descriptor.h"
#include "DVB_Teletext_Descriptor.h"
#include "DVB_Telephone_Descriptor.h"
#include "DVB_Local_Time_Offset_Descriptor.h"
#include "DVB_Subtitling_Descriptor.h"
#include "DVB_Terrestrial_Delivery_System_Descriptor.h"
#include "DVB_Multilingual_Network_Name_Descriptor.h"
#include "DVB_Multilingual_Bouquet_Name_Descriptor.h"
#include "DVB_Multilingual_Service_Name_Descriptor.h"
#include "DVB_Multilingual_Component_Name_Descriptor.h"
#include "DVB_Private_Data_Specifier_Descriptor.h"
#include "DVB_Service_Move_Descriptor.h"
#include "DVB_Short_Smoothing_Buffer_Descriptor.h"
#include "DVB_Frequency_List_Descriptor.h"
#include "DVB_Partial_Transport_Stream_Descriptor.h"
#include "DVB_Data_Broadcast_Descriptor.h"
#include "DVB_Scrambling_Descriptor.h"
#include "DVB_Data_Broadcast_ID_Descriptor.h"
#include "DVB_Transport_Stream_Descriptor.h"
#include "DVB_DSNG_Descriptor.h"
#include "DVB_PDC_Descriptor.h"
#include "DVB_AC3_Descriptor.h"
#include "DVB_Ancillary_Data_Descriptor.h"
#include "DVB_Cell_List_Descriptor.h"
#include "DVB_Cell_Frequency_Link_Descriptor.h"
#include "DVB_Announcement_Support_Descriptor.h"
#include "DVB_Application_Signalling_Descriptor.h"
#include "DVB_Adaptation_Field_Data_Descriptor.h"
#include "DVB_Service_Identifier_Descriptor.h"
#include "DVB_Service_Availablity_Descriptor.h"
#include "DVB_Default_Authority_Descriptor.h"
#include "DVB_Related_Content_Descriptor.h"
#include "DVB_TVA_ID_Descriptor.h"
#include "DVB_Content_Identifier_Descriptor.h"
#include "DVB_Time_Slice_FEC_Identifier_Descriptor.h"
#include "DVB_ECM_Repetition_Rate_Descriptor.h"
#include "DVB_S2_Satellite_Delivery_System_Descriptor.h"
#include "DVB_Enhanced_AC3_Descriptor.h"
#include "DVB_DTS_Descriptor.h"
#include "DVB_AAC_Descriptor.h"
#include "DVB_Extension_Descriptor.h"

enum DVB_Descriptor_TAG {
    /* 0x40 - 0x7F: ETSI EN 300 468 V1.9.1 (2009-03) */
    DVB_NETWORK_NAME_DESCRIPTOR             = 0x40,
    DVB_SERVICE_LIST_DESCRIPTOR             = 0x41,
    DVB_STUFFING_DESCRIPTOR             = 0x42,
    DVB_SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR        = 0x43,
    DVB_CABLE_DELIVERY_SYSTEM_DESCRIPTOR        = 0x44,
    DVB_VBI_DATA_DESCRIPTOR             = 0x45,
    DVB_VBI_TELETEXT_DESCRIPTOR             = 0x46,
    DVB_BOUQUET_NAME_DESCRIPTOR             = 0x47,
    DVB_SERVICE_DESCRIPTOR              = 0x48,
    DVB_COUNTRY_AVAILABILITY_DESCRIPTOR         = 0x49,
    DVB_LINKAGE_DESCRIPTOR              = 0x4A,
    DVB_NVOD_REFERENCE_DESCRIPTOR           = 0x4B,
    DVB_TIME_SHIFTED_SERVICE_DESCRIPTOR         = 0x4C,
    DVB_SHORT_EVENT_DESCRIPTOR              = 0x4D,
    DVB_EXTENDED_EVENT_DESCRIPTOR           = 0x4E,
    DVB_TIME_SHIFTED_EVENT_DESCRIPTOR           = 0x4F,
    DVB_COMPONENT_DESCRIPTOR                = 0x50,
    DVB_MOSAIC_DESCRIPTOR               = 0x51,
    DVB_STREAM_IDENTIFIER_DESCRIPTOR            = 0x52,
    DVB_CA_IDENTIFIER_DESCRIPTOR            = 0x53,
    DVB_CONTENT_DESCRIPTOR              = 0x54,
    DVB_PARENTAL_RATING_DESCRIPTOR          = 0x55,
    DVB_TELETEXT_DESCRIPTOR             = 0x56,
    DVB_TELEPHONE_DESCRIPTOR                = 0x57,
    DVB_LOCAL_TIME_OFFSET_DESCRIPTOR            = 0x58,
    DVB_SUBTITLING_DESCRIPTOR               = 0x59,
    DVB_TERRESTRIAL_DELIVERY_SYSTEM_DESCRIPTOR      = 0x5A,
    DVB_MULTILINGUAL_NETWORK_NAME_DESCRIPTOR        = 0x5B,
    DVB_MULTILINGUAL_BOUQUET_NAME_DESCRIPTOR        = 0x5C,
    DVB_MULTILINGUAL_SERVICE_NAME_DESCRIPTOR        = 0x5D,
    DVB_MULTILINGUAL_COMPONENT_DESCRIPTOR       = 0x5E,
    DVB_PRIVATE_DATA_SPECIFIER_DESCRIPTOR       = 0x5F,
    DVB_SERVICE_MOVE_DESCRIPTOR             = 0x60,
    DVB_SHORT_SMOOTHING_BUFFER_DESCRIPTOR       = 0x61,
    DVB_FREQUENCY_LIST_DESCRIPTOR           = 0x62,
    DVB_PARTIAL_TRANSPORT_STREAM_DESCRIPTOR     = 0x63,
    DVB_DATA_BROADCAST_DESCRIPTOR           = 0x64,
    DVB_SCRAMBLING_DESCRIPTOR               = 0x65,
    DVB_DATA_BROADCAST_ID_DESCRIPTOR            = 0x66,
    DVB_TRANSPORT_STREAM_DESCRIPTOR         = 0x67,
    DVB_DSNG_DESCRIPTOR                 = 0x68,
    DVB_PDC_DESCRIPTOR                  = 0x69,
    DVB_AC3_DESCRIPTOR                  = 0x6A,
    DVB_ANCILLARY_DATA_DESCRIPTOR           = 0x6B,
    DVB_CELL_LIST_DESCRIPTOR                = 0x6C,
    DVB_CELL_FREQUENCY_LINK_DESCRIPTOR          = 0x6D,
    DVB_ANNOUNCEMENT_SUPPORT_DESCRIPTOR         = 0x6E,
    DVB_APPLICATION_SIGNALLING_DESCRIPTOR       = 0x6F,
    DVB_ADAPTATION_FIELD_DATA_DESCRIPTOR        = 0x70,
    DVB_SERVICE_IDENTIFIER_DESCRIPTOR           = 0x71,
    DVB_SERVICE_AVAILABILITY_DESCRIPTOR         = 0x72,
    DVB_DEFAULT_AUTHORITY_DESCRIPTOR            = 0x73, /* TS 102 323 */
    DVB_RELATED_CONTENT_DESCRIPTOR          = 0x74, /* TS 102 323 */
    DVB_TVA_ID_DESCRIPTOR               = 0x75, /* TS 102 323 */
    DVB_CONTENT_IDENTIFIER_DESCRIPTOR           = 0x76, /* TS 102 323 */
    DVB_TIME_SLICE_FEC_IDENTIFIER_DESCRIPTOR        = 0x77, /* EN 301 192 */
    DVB_ECM_REPETITION_RATE_DESCRIPTOR          = 0x78, /* EN 301 192 */
    DVB_S2_SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR     = 0x79,
    DVB_ENHANCED_AC3_DESCRIPTOR             = 0x7A,
    DVB_DTS_DESCRIPTOR                  = 0x7B,
    DVB_AAC_DESCRIPTOR                  = 0x7C,
    DVB_XAIT_LOCATION_DESCRIPTOR            = 0x7D, /* TS 102 590 aka A107.MHP 1.2 Spec */
    DVB_FTA_CONTENT_MANAGEMENT_DESCRIPTOR       = 0x7E,
    DVB_EXTENSION_DESCRIPTOR                = 0x7F,

};

////////////////////////////////////////////////////////////////////////////////
class DVB_Descriptor_Container {
private:
    Descriptor *descriptorSi(const uint8_t * const buffer);

protected:
    DescriptorList descriptorList;

public:
    ~DVB_Descriptor_Container(void);

    void descriptor(const uint8_t * const buffer);
    const DescriptorList *getDescriptors(void) const;};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_DESCRIPTOR_H_ */

