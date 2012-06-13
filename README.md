Introduction
============

This C++ binding is a 'high-level' library that hides most of the c-style
interface core 0mq provides. It consists of a number of header and source
files all residing in the zmq directory, these files are provided under the
MIT license (see LICENCE for details).

They can either be included directly into any 0mq using project or used as a
library. A really basic Makefile is provided for this purpose and will generate
both shared and static libraries.

There is a number of unittests covering the code but in no way should the tests
be considered complete.


Documentation
=============

Most of the code is now commented with doxygen style tags, and a basic
configuration file to generate them is in the root directory.

To build the documentation with doxygen then use;

    doxygen zmqpp.doxygen.conf

And the resulting html or latex docs will be in the docs/html or docs/latex
directories.


libzmqpp
========

There is a Makefile provided which will build, test and install the binding on
a GNU Linux system. I have not tested it on anything other than Ubuntu 11.04
and Centos 5.5.

The install process will only install headers and the shared object to the
system. The archive will remain in the build directory.

The tests for the binding (make check) require the boost unittest framework to
have been installed however these do not need to be built or run to install
the library.

Requirements
------------

ZeroMQ 2.2.x or later
C++0x compliant compiler (g++ 4.6.x, g++ 4.4.x with compatability.hpp)


Installation
------------

Installation can be done by the standard make && make install. If the boost
unittest framework is install check and installcheck can be run for sanity
checking.

    make
    make check
    sudo make install
    make installcheck

The most commonly useful overrides are setting CXX, to change the compiler
used, and PREFIX to change install location. The CXX prefix should be used on
all targets as the compiler version is used in the build path. PREFIX is only
relevant for the install target.

Debugging
---------

The makefile defaults to a production ready build, however a debug version can
be build by passing CONFIG=debug to the make command. In debug mode there is
less optimisations and a number of sanity check assert statements. If you are
not using the installed library the sanity check effect is governed by the
defining of NDEBUG.


zmqpp
=====

The make file can also build and install a client tool called zmqpp. To build
this tool add the step;

    make client

Before the install stage. The install target will install the client to the
binaries directory if it has been built.

Usage
-----

The client is a command line tool that can be used to listen or send to 0mq
sockets. Its very basic so don't expect anything clever. zmqpp --help will list
details about the five possible flags it can take;

    Usage: zmqpp [options] SOCKETTYPE ENDPOINT
    0mq command line client tool.
    SOCKETTYPE is one of the supported 0mq socket types.
      pub, pull, push, rep, req, sub
    ENDPOINT is any valid 0mq endpoint.
    
    Connection Options:
      -b [ --bind ] arg     bind to specified endpoint
      -c [ --connect ] arg  connect to specified endpoint
      -m [ --multipart ]    enable multipart message sending

    Miscellaneous Options:
      --version             display version
      --help                show this help page

Multiple uses of -c or -b are allowed to connect or bind to multiple endpoints,
if neither is specified the connect is assumed for endpoint ENDPOINT. Multipart
messages are for sending only, received messages always support multiple parts.

For send capable sockets entering text on standard in and pressing return will
send, if multipart is enabled then the message is only sent on an empty return
otherwise message parts are generated.

