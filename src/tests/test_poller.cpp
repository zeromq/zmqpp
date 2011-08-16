/*
 *  Created on: 12 Aug 2011
 *      Author: @benjamg
 */

#include <boost/test/unit_test.hpp>

#include "zmq/context.hpp"
#include "zmq/message.hpp"
#include "zmq/poller.hpp"
#include "zmq/socket.hpp"

BOOST_AUTO_TEST_SUITE( poll )

const int max_poll_timeout = 100;

BOOST_AUTO_TEST_CASE( initialise )
{
	zmq::context context;

	zmq::socket socket(context, zmq::socket_type::pull);
	socket.bind("inproc://test");

	zmq::poller poller;
	poller.add(socket);

	BOOST_CHECK_EQUAL(zmq::poller::POLL_NONE, poller.events(socket));
	BOOST_CHECK(!poller.has_input(socket));
	BOOST_CHECK(!poller.has_output(socket));
	BOOST_CHECK(!poller.has_error(socket));
}

BOOST_AUTO_TEST_CASE( simple_pull_push )
{
	zmq::context context;

	zmq::socket puller(context, zmq::socket_type::pull);
	puller.bind("inproc://test");

	zmq::socket pusher(context, zmq::socket_type::push);
	pusher.connect("inproc://test");

	BOOST_CHECK(pusher.send("hello world!"));

	zmq::poller poller;
	poller.add(puller);
	BOOST_CHECK(poller.poll(max_poll_timeout));

	BOOST_CHECK_EQUAL(zmq::poller::POLL_IN, poller.events(puller));
	BOOST_CHECK(poller.has_input(puller));

	std::string message;
	BOOST_CHECK(puller.receive(message));

	BOOST_CHECK_EQUAL("hello world!", message);
	BOOST_CHECK(!puller.has_more_parts());
}

BOOST_AUTO_TEST_CASE( multi_socket_poll )
{
	zmq::context context;

	zmq::socket puller1(context, zmq::socket_type::pull);
	puller1.bind("inproc://test1");

	zmq::socket puller2(context, zmq::socket_type::pull);
	puller2.bind("inproc://test2");

	zmq::socket puller3(context, zmq::socket_type::pull);
	puller3.bind("inproc://test3");

	zmq::socket pusher(context, zmq::socket_type::push);
	pusher.connect("inproc://test1");

	BOOST_CHECK(pusher.send("hello world!"));

	zmq::poller poller;
	poller.add(puller1);
	poller.add(puller2);
	poller.add(puller3);

	BOOST_CHECK(poller.poll(max_poll_timeout));

	BOOST_CHECK_EQUAL(zmq::poller::POLL_IN, poller.events(puller1));
	BOOST_CHECK(poller.has_input(puller1));
	BOOST_CHECK(!poller.has_output(puller1));
	BOOST_CHECK(!poller.has_error(puller1));

	BOOST_CHECK_EQUAL(zmq::poller::POLL_NONE, poller.events(puller2));
	BOOST_CHECK(!poller.has_input(puller2));
	BOOST_CHECK(!poller.has_output(puller2));
	BOOST_CHECK(!poller.has_error(puller2));

	BOOST_CHECK_EQUAL(zmq::poller::POLL_NONE, poller.events(puller3));
	BOOST_CHECK(!poller.has_input(puller3));
	BOOST_CHECK(!poller.has_output(puller3));
	BOOST_CHECK(!poller.has_error(puller3));

	std::string message;
	BOOST_CHECK(puller1.receive(message));

	BOOST_CHECK_EQUAL("hello world!", message);
	BOOST_CHECK(!puller1.has_more_parts());
}

BOOST_AUTO_TEST_CASE( throws_exception_unknown_socket )
{
	zmq::context context;

	zmq::socket puller(context, zmq::socket_type::pull);
	puller.bind("inproc://test");

	zmq::socket pusher(context, zmq::socket_type::push);
	pusher.connect("inproc://test");

	zmq::poller poller;
	poller.add(puller);

	BOOST_CHECK_THROW(poller.events(pusher), zmq::exception);
}

BOOST_AUTO_TEST_SUITE_END()
