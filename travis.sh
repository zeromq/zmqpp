#!/usr/bin/env bash

set -e

RCol='\e[0m' # Text Reset
Red='\e[0;31m';

# Install libsodium is required
if [ -n ${WITH_LIBSODIUM} ]; then
    git clone git://github.com/jedisct1/libsodium.git
    (
	cd libsodium;
	./autogen.sh
	./configure
	make check || echo ${Red}"Warning: Libsodium tests failed. The build will continue but may fail."${RCol}
	sudo make install
	sudo ldconfig
    )    
fi

# Install specific libzmq version
git clone git://github.com/zeromq/${ZMQ_REPO}.git;
(
    cd ${ZMQ_REPO}
    ./autogen.sh
    if [ -n ${WITH_LIBSODIUM} ]; then
	./configure --with-libsodium
    else
	./configure
    fi
    make check || echo ${Red}"Warning: ZeroMQ tests failed. The build will continue but may fail."${RCol}
    sudo make install
    sudo ldconfig
)

# Build and test zmqpp
if [ ${BUILD_TYPE} = "cmake" ]; then
    mkdir build;
    cd build;
    cmake ../ -DZMQPP_BUILD_TESTS=1
    make
    ./zmqpp-test-runner
else
    make CXXFLAGS='-DTRAVIS_CI_BUILD' check
fi
