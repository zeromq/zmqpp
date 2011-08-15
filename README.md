Introduction
============

There are two separate bindings for c++ under this code base.

The first is a branch of the original 0mq 2.x bindings updated for 0mq 3.0 and
with a simplistic zmq_device header. This binding is two files 'zmq.hpp' and
'zmq_device.hpp' and provides a header only binding. These files are provided
under the original branch's lgpl license (see zmq.hpp for details)

The second is a 'high-level' library that hides most of the c-style interface
core 0mq provides. It consists of a number of header and source files all
residing in the zmq directory, these files are not derived from or
use the original branch in any form and are provided under the MIT license
(see zmq/zmq.hpp for details)   


High-level binding (libcppzmq)
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
---------

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