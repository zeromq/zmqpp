#!/usr/bin/env bash

set -e

# Install libsodium is required
if [ -n ${WITH_LIBSODIUM} ]; then
    git clone git://github.com/jedisct1/libsodium.git
    (
	cd libsodium;
	./autogen.sh
	./configure
	make check
	sudo make install
	sudo ldconfig
    )    
fi

# Install specific libzmq version
git clone git://github.com/zeromq/${ZMQ_REPO}.git;
(
    cd ${ZMQ_REPO}
    ./autogen.sh
    ./configure
    make check
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
