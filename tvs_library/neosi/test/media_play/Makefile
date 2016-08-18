########################################
# Configuration
########################################

# DEBUG can be set to YES to include debugging info, or NO otherwise
DEBUG          := NO

# PROFILE can be set to YES to include profiling info, or NO otherwise
PROFILE        := NO

ANDROID 	:= NO

TOP_DIR:=$(shell cd ../..; pwd)

TOOL_PREFIX := 
CC    = $(TOOL_PREFIX)gcc
GPP   = $(TOOL_PREFIX)g++ 
AR    = $(TOOL_PREFIX)ar
STRIP = $(TOOL_PREFIX)strip
AT=@

BASE_DIR:=$(shell pwd)
OBJ_DIR:=$(BASE_DIR)/obj
TEST_DIR :=/home/$(USER)/test/linknet_iptv_test

########################################
# INCLUDES
########################################	
INCLUDES+= \
	-I$(TOP_DIR)/DueSI/include	\
	-I$(TOP_DIR)/DueSI	\

DEBUG_CFLAGS     := -Wall -Wno-format -g -DDEBUG
RELEASE_CFLAGS   := -Wall -Wno-unknown-pragmas -Wno-format -O3

ifeq (YES, ${DEBUG})
   CFLAGS       := ${DEBUG_CFLAGS}
   CXXFLAGS     := ${DEBUG_CXXFLAGS}
   LDFLAGS      := ${DEBUG_LDFLAGS}
else
   CFLAGS       := ${RELEASE_CFLAGS}
   CXXFLAGS     := ${RELEASE_CXXFLAGS}
   LDFLAGS      := ${RELEASE_LDFLAGS}
endif


########################################
# LINK_FLAGS
########################################
LINK_FLAGS:= \

DEFS := -DTIXML_USE_STL -D__SAVE_LOG__
LINK_FLAGS += \
  	-L$(TOP_DIR)/lib \
	-lDueSI \
	-lsqlite3 \
	-lcurl		\
	-liconv		\
	-finput-charset=iso-8859-1 \
	-fexec-charset=cp437

LINK_FLAGS += \
	-lm \
	-lpthread \

OBJ_NAMES:= \
	Main.o \
	NeoSIHandler.o	\
	
	
OBJS:=$(patsubst %.o,$(OBJ_DIR)/%.o,$(OBJ_NAMES))

CFLAGS		:= ${CFLAGS}	${DEFS}
CXXFLAGS	:= ${CXXFLAGS}	${DEFS}
########################################
# vpath
########################################

vpath %.cpp $(BASE_DIR)/src
vpath %.cpp $(BASE_DIR)/src


########################################
# TARGET
########################################

TARGET_TYPE:=EXECUTABLE_TEST
TARGET:=$(TEST_DIR)/iptv_test

########################################
# Targets & Rules
########################################


.PHONY: all
all: mkdir_obj $(OBJS) app

$(OBJ_DIR)/%.o: %.cpp
	@echo "$(GPP) $@"
	$(AT) $(GPP) -o $@ -c -fPIC $(CFLAGS) $(RMCFLAGS) $(INCLUDES)  $(DEFINES) $<

mkdir_obj:
	@echo "make objects directory..."
	@if [ ! -d $(OBJ_DIR) ]; then mkdir -p $(OBJ_DIR); fi

mkdir_target:
	@echo "make objects directory..."
	@if [ ! -d $(TARGET_INSTALL_DIR)/test ]; then mkdir -p $(TARGET_INSTALL_DIR)/test; fi
		
app:
	rm -vf $(TARGET)
	$(GPP) -o $(TARGET) $(CFLAGS) $(OBJS) $(LINK_FLAGS) $(INCLUDES)
	
clean:
	rm -rf $(TARGET)
