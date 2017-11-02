/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file is part of zmqpp.
 * Copyright (c) 2011-2015 Contributors as noted in the AUTHORS file.
 */

/*
 *  Created on: 10 Aug 2011
 *      Author: @benjmag
 */

#include <boost/test/unit_test.hpp>

#include "zmqpp/message.hpp"

BOOST_AUTO_TEST_SUITE( message_stream )

BOOST_AUTO_TEST_CASE( stream_bool )
{
	bool input_value = true;
	bool output_value = false;
	zmqpp::message message;

	message << input_value;

	BOOST_REQUIRE_EQUAL(1, message.parts());
	BOOST_CHECK_EQUAL(1, message.size(0));

	unsigned char const* data = static_cast<unsigned char const*>(message.raw_data(0));
	BOOST_CHECK_EQUAL(1, data[0]);

	message >> output_value;
	BOOST_CHECK_EQUAL(input_value, output_value);
}

BOOST_AUTO_TEST_CASE( stream_float )
{
	float input_value = 3.14f;
	float output_value = 0.0f;
	zmqpp::message message;

	message << input_value;

	BOOST_REQUIRE_EQUAL(1, message.parts());
	BOOST_CHECK_EQUAL(sizeof(input_value), message.size(0));

	message >> output_value;
	BOOST_CHECK_EQUAL(input_value, output_value);
}

BOOST_AUTO_TEST_CASE( stream_double )
{
	double input_value = 3.14;
	double output_value = 0.0;
	zmqpp::message message;

	message << input_value;

	BOOST_REQUIRE_EQUAL(1, message.parts());
	BOOST_CHECK_EQUAL(sizeof(input_value), message.size(0));

	message >> output_value;
	BOOST_CHECK_EQUAL(input_value, output_value);
}

BOOST_AUTO_TEST_CASE( stream_int8 )
{
	int8_t input_value = -42;
	int8_t output_value = 0;
	zmqpp::message message;

	message << input_value;

	BOOST_REQUIRE_EQUAL(1, message.parts());
	BOOST_CHECK_EQUAL(1, message.size(0));

	unsigned char const* data = static_cast<unsigned char const*>(message.raw_data(0));
	BOOST_CHECK_EQUAL(0xD6, data[0]);

	message >> output_value;
	BOOST_CHECK_EQUAL(input_value, output_value);
}

BOOST_AUTO_TEST_CASE( stream_int16 )
{
	int16_t input_value = 512;
	int16_t output_value = 0;
	zmqpp::message message;

	message << input_value;

	BOOST_REQUIRE_EQUAL(1, message.parts());
	BOOST_CHECK_EQUAL(2, message.size(0));

	unsigned char const* data = static_cast<unsigned char const*>(message.raw_data(0));
	BOOST_CHECK_EQUAL(0x02, data[0]);
	BOOST_CHECK_EQUAL(0x00, data[1]);

	message >> output_value;
	BOOST_CHECK_EQUAL(input_value, output_value);
}

BOOST_AUTO_TEST_CASE( stream_int32 )
{
	int32_t input_value = -19088744; //0xfedcba98
	int32_t output_value = 0;
	zmqpp::message message;

	message << input_value;

	BOOST_REQUIRE_EQUAL(1, message.parts());
	BOOST_CHECK_EQUAL(4, message.size(0));

	unsigned char const* data = static_cast<unsigned char const*>(message.raw_data(0));
	BOOST_CHECK_EQUAL(0xFE, data[0]);
	BOOST_CHECK_EQUAL(0xDC, data[1]);
	BOOST_CHECK_EQUAL(0xBA, data[2]);
	BOOST_CHECK_EQUAL(0x98, data[3]);

	message >> output_value;
	BOOST_CHECK_EQUAL(input_value, output_value);
}

BOOST_AUTO_TEST_CASE( stream_int64 )
{
	int64_t input_value = 1234;
	int64_t output_value = 0;
	zmqpp::message message;

	message << input_value;

	BOOST_REQUIRE_EQUAL(1, message.parts());
	BOOST_CHECK_EQUAL(8, message.size(0));

	unsigned char const* data = static_cast<unsigned char const*>(message.raw_data(0));
	BOOST_CHECK_EQUAL(0x00, data[0]);
	BOOST_CHECK_EQUAL(0x00, data[1]);
	BOOST_CHECK_EQUAL(0x00, data[2]);
	BOOST_CHECK_EQUAL(0x00, data[3]);
	BOOST_CHECK_EQUAL(0x00, data[4]);
	BOOST_CHECK_EQUAL(0x00, data[5]);
	BOOST_CHECK_EQUAL(0x04, data[6]);
	BOOST_CHECK_EQUAL(0xD2, data[7]);

	message >> output_value;
	BOOST_CHECK_EQUAL(input_value, output_value);
}

BOOST_AUTO_TEST_CASE( stream_uint8 )
{
	uint8_t input_value = 1;
	uint8_t output_value = 0;
	zmqpp::message message;

	message << input_value;

	BOOST_REQUIRE_EQUAL(1, message.parts());
	BOOST_CHECK_EQUAL(1, message.size(0));

	message >> output_value;
	BOOST_CHECK_EQUAL(input_value, output_value);
}

BOOST_AUTO_TEST_CASE( stream_uint16 )
{
	uint16_t input_value = 12;
	uint16_t output_value = 0;
	zmqpp::message message;

	message << input_value;

	BOOST_REQUIRE_EQUAL(1, message.parts());
	BOOST_CHECK_EQUAL(2, message.size(0));

	message >> output_value;
	BOOST_CHECK_EQUAL(input_value, output_value);
}

BOOST_AUTO_TEST_CASE( stream_uint32 )
{
	uint32_t input_value = 123;
	uint32_t output_value = 0;
	zmqpp::message message;

	message << input_value;

	BOOST_REQUIRE_EQUAL(1, message.parts());
	BOOST_CHECK_EQUAL(4, message.size(0));

	message >> output_value;
	BOOST_CHECK_EQUAL(input_value, output_value);
}

BOOST_AUTO_TEST_CASE( stream_uint64 )
{
	uint64_t input_value = 1234;
	uint64_t output_value = 0;
	zmqpp::message message;

	message << input_value;

	BOOST_REQUIRE_EQUAL(1, message.parts());
	BOOST_CHECK_EQUAL(8, message.size(0));

	message >> output_value;
	BOOST_CHECK_EQUAL(input_value, output_value);
}

BOOST_AUTO_TEST_SUITE_END()
