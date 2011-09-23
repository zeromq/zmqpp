/*
 *  Created on: 18 Aug 2011
 *      Author: Ben Gray (@benjamg)
 */

#include "zmqpp.hpp"

namespace zmqpp
{

std::string version()
{
	return BUILD_VERSION;
}

void version(uint8_t& major, uint8_t& minor, uint8_t& revision)
{
	major = ZMQPP_VERSION_MAJOR;
	minor = ZMQPP_VERSION_MINOR;
	revision = ZMQPP_VERSION_REVISION;
}

}
