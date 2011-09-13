/*
 *  Created on: 10 Aug 2011
 *      Author: Ben Gray (@benjamg)
 */

#ifndef ZMQPP_INET_HPP_
#define ZMQPP_INET_HPP_

// We get htons and htonl from here
//TODO: cross-platform
#include <netinet/in.h>

#include "compatibility.hpp"

namespace zmqpp
{

#define ZMQPP_COMPARABLE_ENUM enum class

// deal with older compilers not supporting C++0x typesafe enum class name {} construct
#ifdef __GNUC__
#if __GNUC__ == 4
#if __GNUC_MINOR__ == 4
#if __GNUC_PATCHLEVEL__ < 5
#undef ZMQPP_COMPARABLE_ENUM
#define ZMQPP_COMPARABLE_ENUM enum
#endif // if __GNUC_PATCHLEVEL__ < 5
#endif // if __GNUC_MINOR__ = 4
#endif // if __GNUC_
#endif // ifdef

ZMQPP_COMPARABLE_ENUM order { unknown, big_endian, little_endian };

inline uint64_t swap_if_needed(uint64_t const& hostlonglong)
{
	static order host_order = order::unknown;

	union {
		uint64_t integer;
		uint8_t bytes[8];
	} value;

	if (order::unknown == host_order)
	{
		value.integer = 1;
		host_order = (1 == value.bytes[0]) ? order::little_endian : order::big_endian;
	}

	if (order::big_endian == host_order)
	{
		return hostlonglong;
	}

	value.integer = hostlonglong;

	std::swap(value.bytes[0], value.bytes[7]);
	std::swap(value.bytes[1], value.bytes[6]);
	std::swap(value.bytes[2], value.bytes[5]);
	std::swap(value.bytes[3], value.bytes[4]);

	return value.integer;
}

}

inline uint64_t htonll(uint64_t const& hostlonglong)
{
	return zmqpp::swap_if_needed(hostlonglong);
}

inline uint64_t ntohll(uint64_t const& hostlonglong)
{
	return zmqpp::swap_if_needed(hostlonglong);
}

#endif /* INET_HPP_ */
