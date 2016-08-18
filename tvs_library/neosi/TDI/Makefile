DEBUG	:=NO
PROFILE	:=NO
LOG_ENABLE	:= NO

TOP_PATH := $(shell pwd)/..
TARGET_DIR := $(shell pwd)/../

DEBUG_CFLAGS     := -Wno-format -g -DDEBUG -O3 
RELEASE_CFLAGS   := -Wno-unknown-pragmas -Wno-format -O3

DEBUG_LDFLAGS    := -g -shared
RELEASE_LDFLAGS  := -shared

CLIENT_SOURCES := \
	TDI_Tuner.cpp	\
	TDI_System.cpp	\
	TDI_Dmux.cpp	\
	TDI_SWSectionFilter.cpp	\
	TDI_TSParser.cpp	\

DEFS := -DTIXML_USE_STL

CLIENT_SOURCES += \

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

CFLAGS	:= $(CFLAGS) $(DEFS)
CXXFLAGS := ${CXXFLAGS} ${DEFS}

LINK_FLAGS	+=\

LOCAL_C_INCLUDES := 					\
		$(TOP_PATH)/TDI/interface		\
		$(TOP_PATH)/TDI/include	\

CFLAGS += $(addprefix -I,$(LOCAL_C_INCLUDES))

CFLAGS += -DRTOS=LINUX  -fPIC \

ifeq (YES, $(LOG_ENABLE))
CFLAGS	+= \
	-DDEBUGLOG_ENABLE

endif

CLIENT_SOURCES := $(addprefix src/, $(CLIENT_SOURCES))	
CLIENT_OBJECTS=$(CLIENT_SOURCES:.cpp=.o)

RESULTS= libtvs_tdi.so

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
	rm -f *.o
	rm -f src/*.o
	rm -f $(RESULTS)
	rm -f $(TARGET_DIR)/lib/$(RESULTS)

