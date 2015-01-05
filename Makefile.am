# For using custom m4 macros
ACLOCAL_AMFLAGS = -I m4

# Compiler build flags
AM_CPPFLAGS = -I${top_srcdir}/src ${ZEROMQ_CFLAGS}

# Build rules for zmqpp library
pkginclude_HEADERS = \
	src/zmqpp/compatibility.hpp \
	src/zmqpp/context.hpp \
	src/zmqpp/context_options.hpp \
	src/zmqpp/exception.hpp \
	src/zmqpp/message.hpp \
	src/zmqpp/poller.hpp \
	src/zmqpp/socket.hpp \
	src/zmqpp/socket_options.hpp \
	src/zmqpp/socket_types.hpp \
	src/zmqpp/zmqpp.hpp

lib_LTLIBRARIES = \
	src/zmqpp/libzmqpp.la

src_zmqpp_libzmqpp_la_SOURCES = \
	src/zmqpp/context.cpp    \
	src/zmqpp/message.cpp     \
	src/zmqpp/poller.cpp   \
	src/zmqpp/socket.cpp \
	src/zmqpp/zmqpp.cpp
src_zmqpp_libzmqpp_la_LDFLAGS = -version-info 0:0:0
src_zmqpp_libzmqpp_la_LIBADD = $(ZEROMQ_LIBS)

# Build rules for pkgconfig
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = zmqpp.pc

# Build rules for client program
bin_PROGRAMS = \
	src/client/zmqpp

src_client_zmqpp_SOURCES = \
	src/client/options.hpp \
	src/client/options.cpp \
	src/client/main.cpp
src_client_zmqpp_CPPFLAGS = -I${top_srcdir}/src ${ZEROMQ_CFLAGS} $(BOOST_CPPFLAGS)
src_client_zmqpp_LDFLAGS = $(BOOST_LDFLAGS)
src_client_zmqpp_LDADD = $(top_builddir)/src/zmqpp/libzmqpp.la $(ZEROMQ_LIBS) $(BOOST_PROGRAM_OPTIONS_LIB) $(LDADD)

# Build rules for tests.
# TESTS_ENVIRONMENT: Set environment variables for the test run
# TESTS: Define programs run automatically by "make check"
# UNIT_TESTS: Define programs run automatically by "make check-unit-tests"
# check_PROGRAMS: Define programs built by "make check" but not necessarily run
TESTS_ENVIRONMENT=
TESTS=
UNIT_TESTS=
check_PROGRAMS=

# Define unit tests
UNIT_TESTS += src/tests/zmqpp_test
check_PROGRAMS += src/tests/zmqpp_test
src_tests_zmqpp_test_SOURCES = \
	src/tests/test_context.cpp \
	src/tests/test_inet.cpp \
	src/tests/test_load.cpp \
	src/tests/test_message.cpp \
	src/tests/test_message_stream.cpp \
	src/tests/test_poller.cpp \
	src/tests/test_sanity.cpp \
	src/tests/test_socket.cpp \
	src/tests/test_socket_options.cpp
src_tests_zmqpp_test_CFLAGS = -I$(top_srcdir)/src $(ZEROMQ_CFLAGS) $(BOOST_CPPFLAGS)
src_tests_zmqpp_test_LDADD = $(top_builddir)/src/zmqpp/libzmqpp.la $(BOOST_UNIT_TEST_FRAMEWORK_LIB) $(LDADD)

TESTS += $(UNIT_TESTS)

TESTS_ENVIRONMENT_WITH_VALGRIND="libtool --mode=execute valgrind --leak-check=full"

.PHONY: check-with-valgrind
check-with-valgrind:
	$(MAKE) $(AM_MAKEFLAGS) check TESTS_ENVIRONMENT=$(TESTS_ENVIRONMENT_WITH_VALGRIND)

.PHONY: check-unit-tests
check-unit-tests:
	$(MAKE) $(AM_MAKEFLAGS) check TESTS=$(UNIT_TESTS)

.PHONY: check-unit-tests-with-valgrind
check-unit-tests-with-valgrind:
	$(MAKE) $(AM_MAKEFLAGS) check TESTS=$(UNIT_TESTS) TESTS_ENVIRONMENT=$(TESTS_ENVIRONMENT_WITH_VALGRIND)

# Nonstandard package files for distribution
EXTRA_DIST = \
	CHANGES.md \
	LICENSE \
	README.md \
	autogen.sh \
	zmqpp.pc

include Makefile.am.coverage
