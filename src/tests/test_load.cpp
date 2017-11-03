/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file is part of zmqpp.
 * Copyright (c) 2011-2015 Contributors as noted in the AUTHORS file.
 */

/*
 *  Created on: 12 Aug 2011
 *      Author: @benjamg
 */

#ifdef LOADTEST

#include <boost/test/unit_test.hpp>

#include <boost/thread.hpp>
#include <boost/timer.hpp>

#include "zmqpp/zmqpp.hpp"


BOOST_AUTO_TEST_SUITE( load )

// Number of messages to push
const uint64_t messages = 1e7;

// Short message to send
const std::string short_message = "hello world!";

// Timeout for polling in milliseconds
const long max_poll_timeout = 500;

BOOST_AUTO_TEST_CASE( push_messages_baseline )
{
	boost::timer t;

	void* context = zmq_init(1);
	void* puller = zmq_socket(context, ZMQ_PULL);
	BOOST_REQUIRE_MESSAGE(0 == zmq_bind(puller, "tcp://*:0"), "bind: " << zmq_strerror(zmq_errno()));

	std::array<char, 512> endpoint;
	size_t length = endpoint.size();
	BOOST_REQUIRE_MESSAGE(0 == zmq_getsockopt(puller, ZMQ_LAST_ENDPOINT, endpoint.data(), &length), "getsockopt: " << zmq_strerror(zmq_errno()));
	
	void* pusher = zmq_socket(context, ZMQ_PUSH);
	BOOST_REQUIRE_MESSAGE(0 == zmq_connect(pusher, endpoint.data()), "connect: " << zmq_strerror(zmq_errno()));

	auto pusher_func = [&pusher](void) {
		auto remaining = messages;

		do
		{
#if (ZMQ_VERSION_MAJOR == 2)
			zmq_msg_t msg;
			zmq_msg_init_size(&msg, short_message.size());
			memcpy(zmq_msg_data(&msg), short_message.data(), short_message.size());
			zmq_send(pusher, &msg, 0);
#else
			zmq_send(pusher, short_message.data(), short_message.size(), 0);
#endif
		}
		while(--remaining > 0);
	};

	zmq_pollitem_t item = { puller, 0, ZMQ_POLLIN, 0 };

	boost::thread thread(pusher_func);

	uint64_t processed = 0;
	zmq_msg_t message;
	zmq_msg_init(&message);
	while(zmq_poll(&item, 1, max_poll_timeout) > 0)
	{
		BOOST_REQUIRE(item.revents & ZMQ_POLLIN);

#if (ZMQ_VERSION_MAJOR == 2)
		zmq_recv(puller, &message, 0);
#elif (ZMQ_VERSION_MAJOR < 3) || ((ZMQ_VERSION_MAJOR == 3) && (ZMQ_VERSION_MINOR < 2))
		zmq_recvmsg(puller, &message, 0);
#else
		zmq_msg_recv(&message, puller, 0);
#endif

		BOOST_CHECK_EQUAL(0, strncmp(short_message.data(), static_cast<char*>(zmq_msg_data(&message)), short_message.size()));
		++processed;
	}
	zmq_msg_close(&message);

	double elapsed_run = t.elapsed();

	BOOST_CHECK_MESSAGE(thread.timed_join(boost::posix_time::milliseconds(max_poll_timeout)), "hung while joining pusher thread");
	BOOST_CHECK_EQUAL(processed, messages);

	BOOST_TEST_MESSAGE("Baseline");
	BOOST_TEST_MESSAGE("Messages pushed    : " << processed);
	BOOST_TEST_MESSAGE("Run time           : " << elapsed_run << " seconds");
	BOOST_TEST_MESSAGE("Messages per second: " << processed / elapsed_run);
	BOOST_TEST_MESSAGE("\n");

	zmq_close(pusher);
	zmq_close(puller);

	zmq_term(context);
}

BOOST_AUTO_TEST_CASE( push_messages )
{
	boost::timer t;

	zmqpp::context context;
	zmqpp::socket puller(context, zmqpp::socket_type::pull);
	puller.bind("tcp://*:0");
	const std::string endpoint = puller.get<std::string>(zmqpp::socket_option::last_endpoint);

	zmqpp::socket pusher(context, zmqpp::socket_type::push);
	pusher.connect(endpoint);

	auto pusher_func = [&pusher](void) {
		auto remaining = messages;
		zmqpp::message message;

		do
		{
			message.add(short_message);
			pusher.send(message);
		}
		while(--remaining > 0);
	};

	zmqpp::poller poller;
	poller.add(puller);

	boost::thread thread(pusher_func);

	uint64_t processed = 0;
	std::string message;
	while(poller.poll(max_poll_timeout))
	{
		BOOST_REQUIRE(poller.has_input(puller));

		puller.receive(message);

		BOOST_CHECK_EQUAL(short_message, message);
		++processed;
	}

	double elapsed_run = t.elapsed();

	BOOST_CHECK_MESSAGE(thread.timed_join(boost::posix_time::milliseconds(max_poll_timeout)), "hung while joining pusher thread");
	BOOST_CHECK_EQUAL(processed, messages);

	BOOST_TEST_MESSAGE("ZMQPP: Copy String");
	BOOST_TEST_MESSAGE("Messages pushed    : " << processed);
	BOOST_TEST_MESSAGE("Run time           : " << elapsed_run << " seconds");
	BOOST_TEST_MESSAGE("Messages per second: " << processed / elapsed_run);
	BOOST_TEST_MESSAGE("\n");
}

BOOST_AUTO_TEST_SUITE_END()

#endif // LOADTEST
