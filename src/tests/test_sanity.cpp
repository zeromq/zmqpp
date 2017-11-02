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

// Module definition should only be in one of the tests
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE zmqpp

#include <boost/test/unit_test.hpp>

#include <array>
#include <cstring>

#include <zmq.h>
#include <zmqpp/zmqpp.hpp>

const int max_poll_timeout = 1000;

BOOST_AUTO_TEST_SUITE( sanity )

BOOST_AUTO_TEST_CASE( correct_zmq_version )
{
	int major, minor, patch;
	zmq_version(&major, &minor, &patch);

	BOOST_CHECK_LE( ZMQPP_REQUIRED_ZMQ_MAJOR, major );
	if(ZMQPP_REQUIRED_ZMQ_MAJOR == major)
	{
		BOOST_CHECK_LE( ZMQPP_REQUIRED_ZMQ_MINOR, minor );
	}
}

BOOST_AUTO_TEST_CASE( correct_zmqpp_version )
{
	uint8_t major, minor, patch;
	zmqpp::version(major, minor, patch);

	BOOST_CHECK_EQUAL( ZMQPP_VERSION_MAJOR, major );
	BOOST_CHECK_EQUAL( ZMQPP_VERSION_MINOR, minor );
	BOOST_CHECK_EQUAL( ZMQPP_VERSION_REVISION, patch );
}

BOOST_AUTO_TEST_CASE( same_zmq_version_as_built_against )
{
	uint8_t major, minor, patch;
	zmqpp::zmq_version(major, minor, patch);

	BOOST_CHECK_EQUAL( ZMQ_VERSION_MAJOR, major );
	BOOST_CHECK_EQUAL( ZMQ_VERSION_MINOR, minor );
	BOOST_CHECK_EQUAL( ZMQ_VERSION_PATCH, patch );
}

BOOST_AUTO_TEST_CASE( zmq_basic_push_pull )
{
	std::string data = "hello world!";

	void* context = zmq_init(1);

	void* puller = zmq_socket(context, ZMQ_PULL);
	zmq_bind(puller, "inproc://test");

	void* pusher = zmq_socket(context, ZMQ_PUSH);
	zmq_connect(pusher, "inproc://test");

#if (ZMQ_VERSION_MAJOR == 2)
	zmq_msg_t sent_message;
	zmq_msg_init_size(&sent_message, data.size());
	memcpy(zmq_msg_data(&sent_message), data.data(), data.size());

	BOOST_CHECK_EQUAL(0, zmq_send(pusher, &sent_message, 0));
#else
	BOOST_CHECK_EQUAL(data.size(), zmq_send(pusher, data.data(), data.size(), 0));
#endif

	zmq_pollitem_t items[] = { { puller, 0, ZMQ_POLLIN, 0 } };
	BOOST_CHECK_EQUAL(1, zmq_poll(items, 1, max_poll_timeout));

	zmq_msg_t received_message;
	zmq_msg_init(&received_message);
#if (ZMQ_VERSION_MAJOR == 2)
	zmq_recv(puller, &received_message, 0);
	BOOST_CHECK_EQUAL(static_cast<char*>(zmq_msg_data(&received_message)), data.c_str());

	zmq_msg_close(&sent_message);
	zmq_msg_close(&received_message);
#else
	std::array<char, 32> buffer;
	memset(buffer.data(), 0, buffer.size());
	zmq_recv(puller, buffer.data(), buffer.size(), 0);
	BOOST_CHECK_EQUAL(buffer.data(), data.c_str());
#endif

	zmq_close(pusher);
	zmq_close(puller);
	zmq_term(context);
}

BOOST_AUTO_TEST_CASE( zmq_basic_options )
{
	std::string data = "hello world!";

	void* context = zmq_init(1);
	void* socket = zmq_socket(context, ZMQ_SUB);
	zmq_bind(socket, "inproc://test");

	int value;
	size_t value_size = sizeof(value);
	BOOST_CHECK_EQUAL(0, zmq_getsockopt(socket, ZMQ_TYPE, &value, &value_size));
	BOOST_CHECK_EQUAL(ZMQ_SUB, value);
}

BOOST_AUTO_TEST_SUITE_END()
