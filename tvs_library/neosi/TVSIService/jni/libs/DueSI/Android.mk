#/*****************************************************************************
# * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
# * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
# *
# * $LastChangedBy: eschoi $
# * $LastChangedDate: 2012-12-17 22:28:48 +0900 (Mon, 17 Dec 2012) $
# * $LastChangedRevision: 5573 $
# * Description:
# * Note:
# *****************************************************************************/


LOCAL_PATH	:= $(call my-dir)
NEOSI		:= NeoSILinknetCableSecond
DUESI		:= DueSI
TDI 		:= TDI

TARGET		:= LINKNET

NEOSI_PATH := $(PWD)/../..
DUESI_PATH := $(NEOSI_PATH)/$(DUESI)
TDI_PATH := $(NEOSI_PATH)/$(TDI)

$(info LOCAL_PATH : $(LOCAL_PATH))
$(info NEOSI_PATH : $(NEOSI_PATH))
$(info DUESI_PATH : $(DUESI_PATH))
$(info TDI_PATH : $(TDI_PATH))

STL_PATH := $(ANDROID_NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/$(TOOLCHAIN_VERSION)/include

include $(CLEAR_VARS)

LOCAL_MODULE := tvs_duesi

LOCAL_C_INCLUDES := \
		$(TDI_PATH)/interface		\
		$(DUESI_PATH)/				\
		$(DUESI_PATH)/include		\
		$(DUESI_PATH)/include/mmf		\
		$(DUESI_PATH)/util			\
		$(DUESI_PATH)/ScanManager			\
		$(DUESI_PATH)/Parser			\
		$(DUESI_PATH)/database			\
		$(DUESI_PATH)/MMF			\
		$(LOCAL_PATH)/../include \

LOCAL_SRC_FILES := \
	$(DUESI_PATH)/SIManager.cpp \
	$(DUESI_PATH)/ServiceInfo.cpp \
	$(DUESI_PATH)/EventInfo.cpp \
	$(DUESI_PATH)/util/BitStream.cpp  \
	$(DUESI_PATH)/util/StringUtil.cpp  \
	$(DUESI_PATH)/util/StreamTypeUtil.cpp \
	$(DUESI_PATH)/util/TimeConvertor.cpp \
	$(DUESI_PATH)/util/Timer.cpp \
	$(DUESI_PATH)/util/Logger.cpp \
	$(DUESI_PATH)/util/xmlUtil.cpp	\
	$(DUESI_PATH)/MMF/MMFInterface.cpp \
	$(DUESI_PATH)/ScanManager/Table.cpp \
	$(DUESI_PATH)/ScanManager/TableFilter.cpp \
	$(DUESI_PATH)/ScanManager/TableBaseFilter.cpp \
	$(DUESI_PATH)/ScanManager/Section.cpp \
	$(DUESI_PATH)/ScanManager/SimpleBaseFilter.cpp \
	$(DUESI_PATH)/ScanManager/SimpleFilter.cpp \
	$(DUESI_PATH)/ScanManager/MonitorBaseFilter.cpp \
	$(DUESI_PATH)/ScanManager/MonitorFilter.cpp \
	$(DUESI_PATH)/ScanManager/ScanManager.cpp \
	$(DUESI_PATH)/Parser/DescriptorFactory.cpp \
	$(DUESI_PATH)/Parser/dvb/SDTParser.cpp \
	$(DUESI_PATH)/Parser/dvb/BATInfo.cpp \
	$(DUESI_PATH)/Parser/dvb/BATParser.cpp \
	$(DUESI_PATH)/Parser/dvb/EITParser.cpp \
	$(DUESI_PATH)/Parser/dvb/NITParser.cpp \
	$(DUESI_PATH)/Parser/dvb/NITInfo.cpp \
	$(DUESI_PATH)/Parser/dvb/TDTParser.cpp	\
	$(DUESI_PATH)/Parser/mpeg/PMTParser.cpp \
	$(DUESI_PATH)/Parser/mpeg/PMTInfo.cpp \
	$(DUESI_PATH)/Parser/XML/XMLParser.cpp \
	$(DUESI_PATH)/Parser/BaseDescriptor.cpp \
	$(DUESI_PATH)/Parser/BaseParser.cpp \
	$(DUESI_PATH)/Parser/Parser.cpp \
	$(DUESI_PATH)/database/SqliteDB.cpp \
	$(DUESI_PATH)/database/SIDataManager.cpp \
	$(DUESI_PATH)/util/tinyxml/tinystr.cpp \
	$(DUESI_PATH)/util/tinyxml/tinyxml.cpp \
	$(DUESI_PATH)/util/tinyxml/tinyxmlerror.cpp \
	$(DUESI_PATH)/util/tinyxml/tinyxmlparser.cpp \
	$(DUESI_PATH)/util/HttpHandler_curl.cpp \

CLIENT_SOURCES_MPEG_DESC := \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_AVC_Timing_And_HRD_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_AVC_Video_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_Association_Tag_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_AudioStream_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_CA_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_Carousel_Identifier_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_ContentLabeling_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_Copyright_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_DataStreamAlignment_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_Deferred_Association_Tags_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_External_ES_ID_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_FMC_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_FMXBufferSize_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_FlexMuxTiming_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_Hierarchy_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_IBP_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_IOD_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_IPMP_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_ISO639_Lang_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_MPEG2_AAC_Audio_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_MPEG4_Audio_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_MPEG4_Video_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_MaxBitrate_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_MultiplexBufUtil_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_MultiplexBuffer_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_MuxCode_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_NPT_Endpoint_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_NPT_Reference_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_PrivateDataIndicator_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_Registration_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_SL_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_STD_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_SmoothingBuf_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_Stream_Event_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_Stream_Mode_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_SystemClock_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_TVA_Metadata_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_TVA_Metadata_Pointer_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_TVA_Metadata_STD_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_TargetBackgroundGrid_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_VideoStream_Descriptor.cpp \
	$(DUESI_PATH)/Parser/mpeg/descriptor/MPEG_VideoWindow_Descriptor.cpp \

CLIENT_SOURCES_DVB := \
	$(DUESI_PATH)/Parser/dvb/descriptor/Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_AAC_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_AC3_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Adaptation_Field_Data_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Ancillary_Data_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Announcement_Support_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Application_Signalling_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Bouguet_Name_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_CA_Identifier_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Cable_Delivery_System_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Cell_Frequency_Link_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Cell_List_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Component_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Content_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Content_Identifier_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Country_Availablity_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_DSNG_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_DTS_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Data_Broadcast_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Data_Broadcast_ID_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Default_Authority_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Descriptor_Container.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_ECM_Repetition_Rate_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Enhanced_AC3_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Extended_Event_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Extension_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Frequency_List_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Linkage_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Local_Time_Offset_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Mosaic_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Multilingual_Bouquet_Name_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Multilingual_Component_Name_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Multilingual_Network_Name_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Multilingual_Service_Name_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_NVOD_Reference_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Network_Name_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_PDC_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Parental_Rating_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Partial_Transport_Stream_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Private_Data_Specifier_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Related_Content_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_S2_Satellite_Delivery_System_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Satellite_Delivery_System_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Scrambling_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Service_Availablity_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Service_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Service_Identifier_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Service_List_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Service_Move_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Short_Event_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Short_Smoothing_Buffer_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Stream_Identifier_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Stuffing_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Subtitling_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_TVA_ID_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Telephone_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Teletext_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Terrestrial_Delivery_System_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Time_Shifted_Event_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Time_Shifted_Service_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Time_Slice_FEC_Identifier_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_Transport_Stream_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_VBI_Data_Descriptor.cpp \
	$(DUESI_PATH)/Parser/dvb/descriptor/DVB_VBI_Teletext_Descriptor.cpp \

CLIENT_SOURCES_WEB_EPG := \
	$(DUESI_PATH)/WEPG/WepgDataHandler.cpp \
	
CLIENT_SOURCES_BAT_LCN := \
	$(DUESI_PATH)/Parser/dvbc/linknet/descriptor_container.cpp \
	$(DUESI_PATH)/Parser/dvbc/linknet/DVB_Logical_Channel_Descriptor.cpp \
	
CLIENT_SOURCES_DUMMY_EPG := \
	$(DUESI_PATH)/DummyEPG/DummyEPGHandler.cpp \

	
CLIENT_SOURCES_WEBSI := \
	$(DUESI_PATH)/WebSI/WebSIHandler.cpp								\
	$(DUESI_PATH)/WebSI/WebXMLParser.cpp								\

CLIENT_SOURCES_PSI_MONITOR := \
	$(DUESI_PATH)/PSIMonitor/PSIMonitor.cpp								\
	$(DUESI_PATH)/PSIMonitor/PMTGetter.cpp								\	

ifeq (YES, $(CONVERT_UTF))
	LOCAL_SRC_FILES += \
		$(DUESI_PATH)/charset/Charset.cpp \
		$(DUESI_PATH)/charset/CharsetConverter.cpp \
		$(DUESI_PATH)/charset/tvstring.cpp \
	LOCAL_CFLAGS += -D__CONVERT_UTF__
endif

LOCAL_SRC_FILES += ${CLIENT_SOURCES_WEB_EPG}
LOCAL_SRC_FILES += ${CLIENT_SOURCES_DUMMY_EPG}
LOCAL_SRC_FILES += ${CLIENT_SOURCES_DVB}
LOCAL_SRC_FILES += ${CLIENT_SOURCES_PSI_MONITOR}
LOCAL_SRC_FILES += ${CLIENT_SOURCES_BAT_LCN}

LOCAL_CFLAGS += -D__FOR_LINKNET__

LOCAL_CFLAGS += -DUSE_SQLITE3

LOCAL_CFLAGS += -D__SAVE_LOG__

LOCAL_LDLIBS := -L$(NEOSI_PATH)/lib

LOCAL_LDLIBS += -llog -ldl -lsqlite3 -lcurl -ltvs_mmfclient

ifeq (YES, $(CONVERT_UTF))
	LOCAL_LDLIBS += -liconv
endif

include $(BUILD_SHARED_LIBRARY)
