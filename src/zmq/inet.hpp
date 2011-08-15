/*
 *  Created on: 10 Aug 2011
 *      Author: Ben Gray (@benjamg)
 */

#ifndef CPPZMQ_INET_HPP_
#define CPPZMQ_INET_HPP_

// We get htons and htonl from here
//TODO: cross-platform
#include <netinet/in.h>

namespace zmq
{

enum class order { unknown, big_endian, little_endian };

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
	return zmq::swap_if_needed(hostlonglong);
}

inline uint64_t ntohll(uint64_t const& hostlonglong)
{
	return zmq::swap_if_needed(hostlonglong);
}

#endif /* INET_HPP_ */
