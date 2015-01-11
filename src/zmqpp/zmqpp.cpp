/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file is part of zmqpp.
 * Copyright (c) 2011-2015 Contributors as noted in the AUTHORS file.
 */

/*
 *  Created on: 18 Aug 2011
 *      Author: Ben Gray (@benjamg)
 */

#include "zmqpp.hpp"

#define STRINGIZE(val) #val

namespace zmqpp
{

std::string version()
{
	return STRINGIZE(ZMQPP_VERSION_MAJOR) "." STRINGIZE(ZMQPP_VERSION_MINOR) "." STRINGIZE(ZMQPP_VERSION_REVISION);
}

void version(uint8_t& major, uint8_t& minor, uint8_t& revision)
{
	major = ZMQPP_VERSION_MAJOR;
	minor = ZMQPP_VERSION_MINOR;
	revision = ZMQPP_VERSION_REVISION;
}

void zmq_version(uint8_t& major, uint8_t& minor, uint8_t& patch)
{
	major = ZMQ_VERSION_MAJOR;
	minor = ZMQ_VERSION_MINOR;
	patch = ZMQ_VERSION_PATCH;
}

}
