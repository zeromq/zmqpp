#
# Instance values, command line user specifiable
#

CONFIG   = max
CPPFLAGS = 
CXXFLAGS = 
LDFLAGS  = 

PREFIX = /usr/local
BINDIR = $(DESTDIR)$(PREFIX)/bin
LIBDIR = $(DESTDIR)$(PREFIX)/lib
INCLUDEDIR = $(DESTDIR)$(PREFIX)/include

#
# Tools
#

#CXX      = g++
LD       = $(CXX)
AR       = ar

#
# Build values
#

LIBRARY_NAME     = zmqpp
VERSION_MAJOR    = 4
VERSION_MINOR    = 1
VERSION_REVISION = 1

#
# Paths
#

LIBRARY_DIR  = $(LIBRARY_NAME)
CLIENT_DIR   = client
TESTS_DIR    = tests

SRC_PATH     = ./src
LIBRARY_PATH = $(SRC_PATH)/$(LIBRARY_DIR)
CLIENT_PATH  = $(SRC_PATH)/$(CLIENT_DIR)
TESTS_PATH   = $(SRC_PATH)/$(TESTS_DIR)

BUILD_PATH   = ./build/$(CONFIG)-$(CXX)
OBJECT_PATH  = $(BUILD_PATH)/obj

#
# Core values
#

APP_VERSION    = $(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_REVISION)
APP_DATESTAMP  = $(shell date '+"%Y-%m-%d %H:%M"')

LIBRARY_SHARED  = lib$(LIBRARY_NAME).so
LIBRARY_ARCHIVE = lib$(LIBRARY_NAME).a
CLIENT_TARGET   = $(LIBRARY_NAME)
TESTS_TARGET    = $(LIBRARY_NAME)-tests

CONFIG_FLAGS =
ifeq ($(CONFIG),debug)
	CONFIG_FLAGS = -g -fno-inline -ftemplate-depth-1000
endif
ifeq ($(CONFIG),valgrind)
	CONFIG_FLAGS = -g -O1 -DNO_DEBUG_LOG -DNO_TRACE_LOG
endif
ifeq ($(CONFIG),max)
	CONFIG_FLAGS = -O3 -funroll-loops -ffast-math -finline-functions -fomit-frame-pointer -DNDEBUG
endif
ifneq (,$(findstring $(CONFIG),release loadtest))
	CONFIG_FLAGS = -O3 -funroll-loops -ffast-math -finline-functions -fomit-frame-pointer -DNO_DEBUG_LOG -DNO_TRACE_LOG -DNDEBUG
endif

COMMON_FLAGS = -MMD -std=c++11 -pipe -Wall -fPIC \
	-DBUILD_ENV=$(CONFIG) \
	-DBUILD_DATESTAMP='$(APP_DATESTAMP)' \
	-DBUILD_LIBRARY_NAME='"$(LIBRARY_NAME)"' \
	-DBUILD_CLIENT_NAME='"$(CLIENT_TARGET)"' \
	-I$(SRC_PATH)

COMMON_LIBS = -lzmq

LIBRARY_LIBS =  

CLIENT_LIBS = -L$(BUILD_PATH) \
	-l$(LIBRARY_NAME) \
	-lboost_program_options

TEST_LIBS = -L$(BUILD_PATH) \
	-l$(LIBRARY_NAME) \
	-lboost_unit_test_framework

ifeq ($(CONFIG),loadtest)
	CONFIG_FLAGS := $(CONFIG_FLAGS) -DLOADTEST
	TEST_LIBS := $(TEST_LIBS) -lboost_thread -lboost_system
endif

ALL_LIBRARY_OBJECTS := $(patsubst $(SRC_PATH)/%.cpp, $(OBJECT_PATH)/%.o, $(shell find $(LIBRARY_PATH) -iname '*.cpp'))

ALL_LIBRARY_INCLUDES := $(shell find $(LIBRARY_PATH) -iname '*.hpp')

ALL_CLIENT_OBJECTS := $(patsubst $(SRC_PATH)/%.cpp, $(OBJECT_PATH)/%.o, $(shell find $(CLIENT_PATH) -iname '*.cpp'))

ALL_TEST_OBJECTS := $(patsubst $(SRC_PATH)/%.cpp, $(OBJECT_PATH)/%.o, $(shell find $(TESTS_PATH) -iname '*.cpp'))

TEST_SUITES := ${addprefix test-,${sort ${shell find ${TESTS_PATH} -iname *.cpp | xargs grep BOOST_AUTO_TEST_SUITE\( | sed 's/.*BOOST_AUTO_TEST_SUITE( \(.*\) )/\1/' }}}

#
# BUILD Targets - Standardised
#

.PHONY: clean uninstall test $(TEST_SUITES)

main: $(LIBRARY_SHARED) $(LIBRARY_ARCHIVE)
	@echo "use make check to test the build"

all: $(LIBRARY_SHARED) $(LIBRARY_ARCHIVE) $(CLIENT_TARGET)
	@echo "use make check to test the build"

check: $(LIBRARY_SHARED) $(LIBRARY_ARCHIVE) test

install:
	mkdir -p $(INCLUDEDIR)/$(LIBRARY_DIR)
	install -m 644 $(ALL_LIBRARY_INCLUDES) $(INCLUDEDIR)/$(LIBRARY_DIR)
	install -m 755 $(BUILD_PATH)/$(LIBRARY_SHARED).$(VERSION_MAJOR) $(LIBDIR)/$(LIBRARY_SHARED).$(APP_VERSION)
	ln -sf $(LIBRARY_SHARED).$(APP_VERSION) $(LIBDIR)/$(LIBRARY_SHARED).$(VERSION_MAJOR)
	ln -sf $(LIBRARY_SHARED).$(APP_VERSION) $(LIBDIR)/$(LIBRARY_SHARED)
	if [ -f $(BUILD_PATH)/$(CLIENT_TARGET) ]; then install -m 755 $(BUILD_PATH)/$(CLIENT_TARGET) $(BINDIR); fi
	$(LDCONFIG)
	@echo "use make installcheck to test the install"

installcheck: $(TESTS_TARGET)
	$(BUILD_PATH)/$(TESTS_TARGET)

uninstall:
	rm -rf $(INCLUDEDIR)/$(LIBRARY_DIR)
	rm -f $(LIBDIR)/$(LIBRARY_SHARED).$(APP_VERSION)
	rm -f $(LIBDIR)/$(LIBRARY_SHARED).$(VERSION_MAJOR)
	rm -f $(LIBDIR)/$(LIBRARY_SHARED)
	rm -f $(BINDIR)/$(CLIENT_TARGET)

clean:
	rm -rf build/*
	rm -rf docs

client: $(CLIENT_TARGET)

library: $(LIBRARY_SHARED) $(LIBRARY_ARCHIVE)

#
# BUILD Targets
#

$(LIBRARY_SHARED): $(ALL_LIBRARY_OBJECTS)
	$(LD) $(LDFLAGS) -shared -rdynamic -Wl,-soname -Wl,$(LIBRARY_SHARED).$(VERSION_MAJOR) -o $(BUILD_PATH)/$@.$(VERSION_MAJOR) $^ $(LIBRARY_LIBS) $(COMMON_LIBS)

$(LIBRARY_ARCHIVE): $(ALL_LIBRARY_OBJECTS)
	$(AR) crf $(BUILD_PATH)/$@ $^

$(CLIENT_TARGET): $(LIBRARY_SHARED) $(LIBRARY_ARCHIVE) $(ALL_CLIENT_OBJECTS)
	$(LD) $(LDFLAGS) -o $(BUILD_PATH)/$@ $(ALL_CLIENT_OBJECTS) $(CLIENT_LIBS) $(COMMON_LIBS)

$(TESTS_TARGET): $(LIBRARY_SHARED) $(LIBRARY_ARCHIVE) $(ALL_TEST_OBJECTS)
	$(LD) $(LDFLAGS) -o $(BUILD_PATH)/$@ $(ALL_TEST_OBJECTS) $(TEST_LIBS) $(COMMON_LIBS) 

$(TEST_SUITES): $(TESTS_TARGET)
	LD_LIBRARY_PATH=$(BUILD_PATH):$(LD_LIBRARY_PATH) $(BUILD_PATH)/$(TESTS_TARGET) --log_level=message --run_test=$(patsubst test-%,%,$@)

test: $(TESTS_TARGET)
	@echo "running all test targets ($(TEST_SUITES))"
	LD_LIBRARY_PATH=$(BUILD_PATH):$(LD_LIBRARY_PATH) $(BUILD_PATH)/$(TESTS_TARGET)

#
# Dependencies
# We don't care if they don't exist as the object won't have been built
#

-include $(patsubst %.o,%.d,$(ALL_LIBRARY_OBJECTS) $(ALL_TEST_OBJECTS))

#
# Object file generation
#

$(OBJECT_PATH)/%.o: $(SRC_PATH)/%.cpp
	-mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(COMMON_FLAGS) $(CONFIG_FLAGS) -c -o $@ $<

