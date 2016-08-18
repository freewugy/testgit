DEBUG          	:=NO
PROFILE        	:=NO
CONVERT_UTF		:=NO
USE_SQLITE3	 	:=YES

LOG_ENABLE		:= YES

TOP_PATH 		:= $(shell pwd)/..
TARGET_DIR 		:= $(shell pwd)/../

DEBUG_CFLAGS     := -Wno-format -g -DDEBUG -O3
RELEASE_CFLAGS   := -Wno-unknown-pragmas -Wno-format -O3

DEBUG_LDFLAGS    := -g -shared
RELEASE_LDFLAGS  := -shared

CLIENT_SOURCES := \
	SIManager.cpp 			\
	ServiceInfo.cpp 		\
	EventInfo.cpp 			\
	util/BitStream.cpp  	\
	util/StringUtil.cpp  	\
	util/StreamTypeUtil.cpp  	\
	util/TimeConvertor.cpp 	\
	util/Timer.cpp			\
	util/Logger.cpp			\
	ScanManager/Table.cpp	\
	ScanManager/TableFilter.cpp	\
	ScanManager/TableBaseFilter.cpp	\
	ScanManager/Section.cpp		\
	ScanManager/SimpleBaseFilter.cpp	\
	ScanManager/SimpleFilter.cpp	\
	ScanManager/MonitorBaseFilter.cpp	\
	ScanManager/MonitorFilter.cpp	\
	util/xmlUtil.cpp		\
	database/SIDataManager.cpp \
	charset/Charset.cpp	\
	charset/CharsetConverter.cpp	\
	charset/tvstring.cpp	\
	Parser/DescriptorFactory.cpp \
	Parser/dvb/SDTParser.cpp \
	Parser/dvb/EITParser.cpp \
	Parser/dvb/NITParser.cpp \
	Parser/dvb/NITInfo.cpp \
	Parser/dvb/TDTParser.cpp \
	Parser/XML/XMLParser.cpp \
	Parser/BaseDescriptor.cpp				\
	Parser/BaseParser.cpp					\
	Parser/mpeg/PMTParser.cpp \
	Parser/mpeg/PMTInfo.cpp \
	Parser/Parser.cpp						\
	util/tinyxml/tinystr.cpp				\
	util/tinyxml/tinyxml.cpp				\
	util/tinyxml/tinyxmlerror.cpp			\
	util/tinyxml/tinyxmlparser.cpp			\
	util/HttpHandler_curl.cpp				\

DEFS := -DTIXML_USE_STL

CLIENT_SOURCES += \

CLIENT_SOURCES_MPEG_DESC := \
	Parser/mpeg/descriptor/MPEG_Descriptor.cpp	\
	Parser/mpeg/descriptor/MPEG_AVC_Timing_And_HRD_Descriptor.cpp	\
	Parser/mpeg/descriptor/MPEG_AVC_Video_Descriptor.cpp					\
	Parser/mpeg/descriptor/MPEG_Association_Tag_Descriptor.cpp				\
	Parser/mpeg/descriptor/MPEG_AudioStream_Descriptor.cpp					\
	Parser/mpeg/descriptor/MPEG_CA_Descriptor.cpp							\
	Parser/mpeg/descriptor/MPEG_Carousel_Identifier_Descriptor.cpp			\
	Parser/mpeg/descriptor/MPEG_ContentLabeling_Descriptor.cpp				\
	Parser/mpeg/descriptor/MPEG_Copyright_Descriptor.cpp					\
	Parser/mpeg/descriptor/MPEG_DataStreamAlignment_Descriptor.cpp			\
	Parser/mpeg/descriptor/MPEG_Deferred_Association_Tags_Descriptor.cpp	\
	Parser/mpeg/descriptor/MPEG_External_ES_ID_Descriptor.cpp				\
	Parser/mpeg/descriptor/MPEG_FMC_Descriptor.cpp							\
	Parser/mpeg/descriptor/MPEG_FMXBufferSize_Descriptor.cpp				\
	Parser/mpeg/descriptor/MPEG_FlexMuxTiming_Descriptor.cpp				\
	Parser/mpeg/descriptor/MPEG_Hierarchy_Descriptor.cpp					\
	Parser/mpeg/descriptor/MPEG_IBP_Descriptor.cpp							\
	Parser/mpeg/descriptor/MPEG_IOD_Descriptor.cpp							\
	Parser/mpeg/descriptor/MPEG_IPMP_Descriptor.cpp							\
	Parser/mpeg/descriptor/MPEG_ISO639_Lang_Descriptor.cpp					\
	Parser/mpeg/descriptor/MPEG_MPEG2_AAC_Audio_Descriptor.cpp				\
	Parser/mpeg/descriptor/MPEG_MPEG4_Audio_Descriptor.cpp					\
	Parser/mpeg/descriptor/MPEG_MPEG4_Video_Descriptor.cpp					\
	Parser/mpeg/descriptor/MPEG_MaxBitrate_Descriptor.cpp					\
	Parser/mpeg/descriptor/MPEG_MultiplexBufUtil_Descriptor.cpp				\
	Parser/mpeg/descriptor/MPEG_MultiplexBuffer_Descriptor.cpp				\
	Parser/mpeg/descriptor/MPEG_MuxCode_Descriptor.cpp						\
	Parser/mpeg/descriptor/MPEG_NPT_Endpoint_Descriptor.cpp					\
	Parser/mpeg/descriptor/MPEG_NPT_Reference_Descriptor.cpp				\
	Parser/mpeg/descriptor/MPEG_PrivateDataIndicator_Descriptor.cpp			\
	Parser/mpeg/descriptor/MPEG_Registration_Descriptor.cpp					\
	Parser/mpeg/descriptor/MPEG_SL_Descriptor.cpp							\
	Parser/mpeg/descriptor/MPEG_STD_Descriptor.cpp							\
	Parser/mpeg/descriptor/MPEG_SmoothingBuf_Descriptor.cpp					\
	Parser/mpeg/descriptor/MPEG_Stream_Event_Descriptor.cpp					\
	Parser/mpeg/descriptor/MPEG_Stream_Mode_Descriptor.cpp					\
	Parser/mpeg/descriptor/MPEG_SystemClock_Descriptor.cpp					\
	Parser/mpeg/descriptor/MPEG_TVA_Metadata_Descriptor.cpp					\
	Parser/mpeg/descriptor/MPEG_TVA_Metadata_Pointer_Descriptor.cpp			\
	Parser/mpeg/descriptor/MPEG_TVA_Metadata_STD_Descriptor.cpp				\
	Parser/mpeg/descriptor/MPEG_TargetBackgroundGrid_Descriptor.cpp			\
	Parser/mpeg/descriptor/MPEG_VideoStream_Descriptor.cpp					\
	Parser/mpeg/descriptor/MPEG_VideoWindow_Descriptor.cpp					\

CLIENT_SOURCES_MMF := \
	MMF/MMFInterface.cpp			\

CLIENT_SOURCES_DVB := \
	Parser/dvb/descriptor/Descriptor.cpp								\
	Parser/dvb/descriptor/DVB_AAC_Descriptor.cpp						\
	Parser/dvb/descriptor/DVB_AC3_Descriptor.cpp						\
	Parser/dvb/descriptor/DVB_Adaptation_Field_Data_Descriptor.cpp		\
	Parser/dvb/descriptor/DVB_Ancillary_Data_Descriptor.cpp				\
	Parser/dvb/descriptor/DVB_Announcement_Support_Descriptor.cpp		\
	Parser/dvb/descriptor/DVB_Application_Signalling_Descriptor.cpp		\
	Parser/dvb/descriptor/DVB_Bouguet_Name_Descriptor.cpp				\
	Parser/dvb/descriptor/DVB_CA_Identifier_Descriptor.cpp				\
	Parser/dvb/descriptor/DVB_Cable_Delivery_System_Descriptor.cpp		\
	Parser/dvb/descriptor/DVB_Cell_Frequency_Link_Descriptor.cpp		\
	Parser/dvb/descriptor/DVB_Cell_List_Descriptor.cpp					\
	Parser/dvb/descriptor/DVB_Component_Descriptor.cpp					\
	Parser/dvb/descriptor/DVB_Content_Descriptor.cpp					\
	Parser/dvb/descriptor/DVB_Content_Identifier_Descriptor.cpp			\
	Parser/dvb/descriptor/DVB_Country_Availablity_Descriptor.cpp		\
	Parser/dvb/descriptor/DVB_DSNG_Descriptor.cpp						\
	Parser/dvb/descriptor/DVB_DTS_Descriptor.cpp						\
	Parser/dvb/descriptor/DVB_Data_Broadcast_Descriptor.cpp				\
	Parser/dvb/descriptor/DVB_Data_Broadcast_ID_Descriptor.cpp			\
	Parser/dvb/descriptor/DVB_Default_Authority_Descriptor.cpp			\
	Parser/dvb/descriptor/DVB_Descriptor_Container.cpp							\
	Parser/dvb/descriptor/DVB_ECM_Repetition_Rate_Descriptor.cpp		\
	Parser/dvb/descriptor/DVB_Enhanced_AC3_Descriptor.cpp				\
	Parser/dvb/descriptor/DVB_Extended_Event_Descriptor.cpp				\
	Parser/dvb/descriptor/DVB_Extension_Descriptor.cpp					\
	Parser/dvb/descriptor/DVB_Frequency_List_Descriptor.cpp				\
	Parser/dvb/descriptor/DVB_Linkage_Descriptor.cpp					\
	Parser/dvb/descriptor/DVB_Local_Time_Offset_Descriptor.cpp			\
	Parser/dvb/descriptor/DVB_Mosaic_Descriptor.cpp						\
	Parser/dvb/descriptor/DVB_Multilingual_Bouquet_Name_Descriptor.cpp	\
	Parser/dvb/descriptor/DVB_Multilingual_Component_Name_Descriptor.cpp	\
	Parser/dvb/descriptor/DVB_Multilingual_Network_Name_Descriptor.cpp	\
	Parser/dvb/descriptor/DVB_Multilingual_Service_Name_Descriptor.cpp	\
	Parser/dvb/descriptor/DVB_NVOD_Reference_Descriptor.cpp				\
	Parser/dvb/descriptor/DVB_Network_Name_Descriptor.cpp				\
	Parser/dvb/descriptor/DVB_PDC_Descriptor.cpp						\
	Parser/dvb/descriptor/DVB_Parental_Rating_Descriptor.cpp			\
	Parser/dvb/descriptor/DVB_Partial_Transport_Stream_Descriptor.cpp	\
	Parser/dvb/descriptor/DVB_Private_Data_Specifier_Descriptor.cpp		\
	Parser/dvb/descriptor/DVB_Related_Content_Descriptor.cpp			\
	Parser/dvb/descriptor/DVB_S2_Satellite_Delivery_System_Descriptor.cpp	\
	Parser/dvb/descriptor/DVB_Satellite_Delivery_System_Descriptor.cpp	\
	Parser/dvb/descriptor/DVB_Scrambling_Descriptor.cpp					\
	Parser/dvb/descriptor/DVB_Service_Availablity_Descriptor.cpp		\
	Parser/dvb/descriptor/DVB_Service_Descriptor.cpp					\
	Parser/dvb/descriptor/DVB_Service_Identifier_Descriptor.cpp			\
	Parser/dvb/descriptor/DVB_Service_List_Descriptor.cpp				\
	Parser/dvb/descriptor/DVB_Service_Move_Descriptor.cpp				\
	Parser/dvb/descriptor/DVB_Short_Event_Descriptor.cpp				\
	Parser/dvb/descriptor/DVB_Short_Smoothing_Buffer_Descriptor.cpp		\
	Parser/dvb/descriptor/DVB_Stream_Identifier_Descriptor.cpp			\
	Parser/dvb/descriptor/DVB_Stuffing_Descriptor.cpp					\
	Parser/dvb/descriptor/DVB_Subtitling_Descriptor.cpp					\
	Parser/dvb/descriptor/DVB_TVA_ID_Descriptor.cpp						\
	Parser/dvb/descriptor/DVB_Telephone_Descriptor.cpp					\
	Parser/dvb/descriptor/DVB_Teletext_Descriptor.cpp					\
	Parser/dvb/descriptor/DVB_Terrestrial_Delivery_System_Descriptor.cpp	\
	Parser/dvb/descriptor/DVB_Time_Shifted_Event_Descriptor.cpp			\
	Parser/dvb/descriptor/DVB_Time_Shifted_Service_Descriptor.cpp		\
	Parser/dvb/descriptor/DVB_Time_Slice_FEC_Identifier_Descriptor.cpp	\
	Parser/dvb/descriptor/DVB_Transport_Stream_Descriptor.cpp			\
	Parser/dvb/descriptor/DVB_VBI_Data_Descriptor.cpp					\
	Parser/dvb/descriptor/DVB_VBI_Teletext_Descriptor.cpp				\

CLIENT_SOURCES_DUMMYEPG := \
	DummyEPG/DummyEPGHandler.cpp								\

CLIENT_SOURCES_WEBSI := \
	WebSI/WebSIHandler.cpp								\
	WebSI/WebXMLParser.cpp								\

CROSS=
CC=g++

ifeq (YES, ${DEBUG})
   CFLAGS       := ${DEBUG_CFLAGS}
   CXXFLAGS     := ${DEBUG_CXXFLAGS}
   LDFLAGS      := ${DEBUG_LDFLAGS}
else
   CFLAGS       := ${RELEASE_CFLAGS}
   CXXFLAGS     := ${RELEASE_CXXFLAGS}
   LDFLAGS      := ${RELEASE_LDFLAGS}
endif

ifeq (YES, ${PROFILE})
   CFLAGS   := ${CFLAGS} -pg -O3
   CXXFLAGS := ${CXXFLAGS} -pg -O3
   LDFLAGS  := ${LDFLAGS} -pg
endif

	CLIENT_SOURCES += ${CLIENT_SOURCES_MPEG_DESC} ${CLIENT_SOURCES_DUMMYEPG} 
	CFLAGS 	+= -D__SUPPORT_MPEG_DESC__
	CFLAGS 	+= -D__FOR_LINKNET__

ifeq (YES, $(CONVERT_UTF))
	CFLAGS += -D__CONVERT_UTF__
endif

ifeq (YES, $(LOG_ENABLE))
	CFLAGS	+= \
		-DDEBUGLOG_ENABLE
endif

CLIENT_SOURCES += ${CLIENT_SOURCES_DVB}
CLIENT_SOURCES += ${CLIENT_SOURCES_MMF}

CFLAGS	:= $(CFLAGS) $(DEFS)
CXXFLAGS := ${CXXFLAGS} ${DEFS}

LOCAL_C_INCLUDES := 					\
		$(TOP_PATH)/TDI/interface		\
		$(TOP_PATH)/DueSI \
		$(TOP_PATH)/DueSI/include \
		$(TOP_PATH)/DueSI/include/mmf \
		$(TOP_PATH)/DueSI/util \
		$(TOP_PATH)/DueSI/ScanManager \
		$(TOP_PATH)/DueSI/Parser \
		$(TOP_PATH)/DueSI/MMF \
		$(TOP_PATH)/DueSI/Parser/XML \
		$(TOP_PATH)/DueSI/database \
		$(TOP_PATH)/DueSI/util/tinyxml \

CFLAGS += $(addprefix -I,$(LOCAL_C_INCLUDES))

CLIENT_SOURCES+= \
  database/SqliteDB.cpp	\

CFLAGS += -DUSE_SQLITE3

#CFLAGS += -D__DEBUG_LOG_DM__
#CFLAGS += -D__DEBUG_LOG_SM__
#CFLAGS += -D__DEBUG_LOG_PSR__
#CFLAGS += -D__DEBUG_LOG_SIM__
#CFLAGS += -D__USE_TINYXML_2__
CFLAGS += -D__USE_TINYXML__
#CFLAGS += -D__DEBUG_LOG_WEPG__
CFLAGS += -fPIC
CFLAGS += -D__SAVE_LOG__

#CLIENT_SOURCES := $(addprefix src/, $(CLIENT_SOURCES))	
CLIENT_OBJECTS=$(CLIENT_SOURCES:.cpp=.o)

RESULTS= libDueSI.so

all:	$(RESULTS) 

$(RESULTS) :	$(CLIENT_OBJECTS) $(LIBRARY)
	$(CROSS)$(CC) $(CFLAGS) $(LDFLAGS) $(DEFS) -o $@ $(CLIENT_OBJECTS)
	$(CROSS)strip $@
	cp $@ $(TARGET_DIR)/lib


.SUFFIXES:	.cpp .o

.cpp.o:
	$(CROSS)$(CC) -g -c -o $*.o $(CFLAGS) $*.cpp

clean:
#	rcsclean *
	find . -name \*.o | xargs rm -rf
	rm -f $(RESULTS)
	rm -f $(TARGET_DIR)/lib/$(RESULTS)

