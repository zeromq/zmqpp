/*
 *  Created on: 18 Aug 2011
 *      Author: Ben Gray (@benjamg)
 */

#include "zmq.hpp"

namespace
{

std::string version()
{
	return BUILD_VERSION;
}

void version(uint8_t& api, uint8_t& revision, uint8_t& age)
{
	api = BUILD_VERSION_API;
	revision = BUILD_VERSION_REVISION;
	age = BUILD_VERSION_AGE;
}


}
