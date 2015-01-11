/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file is part of zmqpp.
 * Copyright (c) 2011-2015 Contributors as noted in the AUTHORS file.
 */

/*
 *  Created on: 8 Aug 2011
 *      Author: @benjamg
 */

#include <boost/test/unit_test.hpp>

#include "zmqpp/inet.hpp"

BOOST_AUTO_TEST_SUITE( inet )

BOOST_AUTO_TEST_CASE( swaping_64bit_byteorder )
{
	uint64_t host = 0x1122334455667788;

	uint64_t network = zmqpp::swap_if_needed(host);

	uint8_t* bytes = reinterpret_cast<uint8_t*>(&network);

	BOOST_CHECK_EQUAL(0x11, bytes[0]);
	BOOST_CHECK_EQUAL(0x22, bytes[1]);
	BOOST_CHECK_EQUAL(0x33, bytes[2]);
	BOOST_CHECK_EQUAL(0x44, bytes[3]);
	BOOST_CHECK_EQUAL(0x55, bytes[4]);
	BOOST_CHECK_EQUAL(0x66, bytes[5]);
	BOOST_CHECK_EQUAL(0x77, bytes[6]);
	BOOST_CHECK_EQUAL(0x88, bytes[7]);
}

BOOST_AUTO_TEST_CASE( swaping_64bit_reversable )
{
	uint64_t host = 0x1122334455667788;

	uint64_t network = zmqpp::swap_if_needed(host);

	BOOST_CHECK_EQUAL(host, zmqpp::swap_if_needed(network));
}

BOOST_AUTO_TEST_SUITE_END()
