Introduction
============

This C++ binding is a 'high-level' library that hides most of the c-style interface
core 0mq provides. It consists of a number of header and source files all
residing in the zmq directory, these files are provided under the MIT license
(see zmq/zmq.hpp for details).

They can either be included directly into any 0mq using project or used as a library.
A really basic Makefile is provided for this purpose and will generate both shared
and static libraries.

There is a number of unittests covering the code but in no way should the tests
be considered complete.


libzmqpp
==============================

There is a Makefile provided which will build, test and install the second
binding on a GNU Linux system I have not tested it on anything other than
Ubuntu 11.04.

The tests for the binding (make check) require the boost unittest framework to
have been installed however these do not need to be built or run to install
the library.

Requirements
------------

ZeroMQ 3.0.0 or later
C++0x compliant compiler (g++ 4.6.1 works)

Installation
------------

Installation can be done by the standard make && make install. If the boost
unittest framework is install check and installcheck can be run for sanity
checking.

    make
    make check
    sudo make install
    make installcheck
    
Debugging
---------

The makefile defaults to a production ready build, however a debug version can
be build by passing CONFIG=debug to the make command. In debug mode there is
less optimisations and a number of sanity check assert statements. If you are
not using the installed library the same effect is governed by the defining
NDEBUG

Zero-copy
=========

The zmq::socket and zmq::message classes both have a number of zero-copy
methods.

For the message these are prefixed 'move_' and were possible will use
stl::swap to steal your data. Do not manually free raw data passed to a move
method. STL containers, including std::string, passed to the move methods
will result in you being left with a new blank version.

For the socket using send or receive with a message object will trigger
move semantics automatically. If you need to retain a copy of the message
after sending call the copy functions on it. Note that message is no-copy 
so copying can only be achieved if you ask for it directly.