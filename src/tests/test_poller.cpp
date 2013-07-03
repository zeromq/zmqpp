/*
 *  Created on: 12 Aug 2011
 *      Author: @benjamg
 */

#include <boost/test/unit_test.hpp>

#include "zmqpp/context.hpp"
#include "zmqpp/message.hpp"
#include "zmqpp/poller.hpp"
#include "zmqpp/socket.hpp"

BOOST_AUTO_TEST_SUITE( poll )

const int max_poll_timeout = 100;

BOOST_AUTO_TEST_CASE( initialise )
{
	zmqpp::context context;

	zmqpp::socket socket(context, zmqpp::socket_type::pull);
	socket.bind("inproc://test");

	zmqpp::poller poller;
	poller.add(socket);

	BOOST_CHECK_EQUAL(zmqpp::poller::poll_none, poller.events(socket));
	BOOST_CHECK(!poller.has_input(socket));
	BOOST_CHECK(!poller.has_output(socket));
	BOOST_CHECK(!poller.has_error(socket));
}

BOOST_AUTO_TEST_CASE( simple_pull_push )
{
	zmqpp::context context;

	zmqpp::socket puller(context, zmqpp::socket_type::pull);
	puller.bind("inproc://test");

	zmqpp::socket pusher(context, zmqpp::socket_type::push);
	pusher.connect("inproc://test");

	BOOST_CHECK(pusher.send_raw("hello world!", sizeof("hello world!")));

	zmqpp::poller poller;
	poller.add(puller);
	BOOST_CHECK(poller.poll(max_poll_timeout));

	BOOST_CHECK_EQUAL(zmqpp::poller::poll_in, poller.events(puller));
	BOOST_CHECK(poller.has_input(puller));

	std::string message;
	BOOST_CHECK(puller.receive(message));

	BOOST_CHECK_EQUAL("hello world!", message.c_str());
	BOOST_CHECK(!puller.has_more_parts());
}

BOOST_AUTO_TEST_CASE( multi_socket_poll )
{
	zmqpp::context context;

	zmqpp::socket puller1(context, zmqpp::socket_type::pull);
	puller1.bind("inproc://test1");

	zmqpp::socket puller2(context, zmqpp::socket_type::pull);
	puller2.bind("inproc://test2");

	zmqpp::socket puller3(context, zmqpp::socket_type::pull);
	puller3.bind("inproc://test3");

	zmqpp::socket pusher(context, zmqpp::socket_type::push);
	pusher.connect("inproc://test1");

	BOOST_CHECK(pusher.send("hello world!"));

	zmqpp::poller poller;
	poller.add(puller1);
	poller.add(puller2);
	poller.add(puller3);

	BOOST_CHECK(poller.poll(max_poll_timeout));

	BOOST_CHECK_EQUAL(zmqpp::poller::poll_in, poller.events(puller1));
	BOOST_CHECK(poller.has_input(puller1));
	BOOST_CHECK(!poller.has_output(puller1));
	BOOST_CHECK(!poller.has_error(puller1));

	BOOST_CHECK_EQUAL(zmqpp::poller::poll_none, poller.events(puller2));
	BOOST_CHECK(!poller.has_input(puller2));
	BOOST_CHECK(!poller.has_output(puller2));
	BOOST_CHECK(!poller.has_error(puller2));

	BOOST_CHECK_EQUAL(zmqpp::poller::poll_none, poller.events(puller3));
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
	zmqpp::context context;

	zmqpp::socket puller(context, zmqpp::socket_type::pull);
	puller.bind("inproc://test");

	zmqpp::socket pusher(context, zmqpp::socket_type::push);
	pusher.connect("inproc://test");

	zmqpp::poller poller;
	poller.add(puller);

	BOOST_CHECK_THROW(poller.events(pusher), zmqpp::exception);
}

BOOST_AUTO_TEST_CASE( ignores_removed_sockets )
{
	zmqpp::context context;

	zmqpp::socket puller1(context, zmqpp::socket_type::pull);
	puller1.bind("inproc://test1");

	zmqpp::socket puller2(context, zmqpp::socket_type::pull);
	puller2.bind("inproc://test2");

	zmqpp::socket pusher(context, zmqpp::socket_type::push);
	pusher.connect("inproc://test1");

	BOOST_CHECK(pusher.send("hello world!"));

	zmqpp::poller poller;
	poller.add(puller1);
	poller.add(puller2);

	poller.remove(puller1);

	BOOST_CHECK(!poller.poll(max_poll_timeout));

	BOOST_CHECK_EQUAL(zmqpp::poller::poll_none, poller.events(puller2));
	BOOST_CHECK(!poller.has_input(puller2));
	BOOST_CHECK(!poller.has_output(puller2));
	BOOST_CHECK(!poller.has_error(puller2));

	std::string message;
	BOOST_CHECK(puller1.receive(message));

	BOOST_CHECK_EQUAL("hello world!", message);
	BOOST_CHECK(!puller1.has_more_parts());
}

BOOST_AUTO_TEST_CASE( remove_maintains_index )
{
	zmqpp::context context;

	zmqpp::socket puller1(context, zmqpp::socket_type::pull);
	puller1.bind("inproc://test1");

	zmqpp::socket puller2(context, zmqpp::socket_type::pull);
	puller2.bind("inproc://test2");

	zmqpp::socket puller3(context, zmqpp::socket_type::pull);
	puller3.bind("inproc://test3");

	zmqpp::socket pusher(context, zmqpp::socket_type::push);
	pusher.connect("inproc://test1");

	BOOST_CHECK(pusher.send("hello world!"));

	zmqpp::poller poller;
	poller.add(puller1);
	poller.add(puller2);
	poller.add(puller3);

	poller.remove(puller1);

	// removing last puller will die if index has not been updated
	poller.remove(puller3);

	BOOST_CHECK(!poller.poll(max_poll_timeout));

	BOOST_CHECK_EQUAL(zmqpp::poller::poll_none, poller.events(puller2));
	BOOST_CHECK(!poller.has_input(puller2));
	BOOST_CHECK(!poller.has_output(puller2));
	BOOST_CHECK(!poller.has_error(puller2));

	std::string message;
	BOOST_CHECK(puller1.receive(message));

	BOOST_CHECK_EQUAL("hello world!", message);
	BOOST_CHECK(!puller1.has_more_parts());
}

BOOST_AUTO_TEST_CASE( exceptions_testing_removed_sockets )
{
	zmqpp::context context;

	zmqpp::socket pusher(context, zmqpp::socket_type::push);
	pusher.bind("inproc://test1");

	zmqpp::poller poller;

	// Has no events when added
	poller.add(pusher);
	BOOST_CHECK(!poller.events(pusher));

	// Exception after removed
	poller.remove(pusher);
	BOOST_CHECK_THROW(poller.events(pusher), zmqpp::exception);
}

BOOST_AUTO_TEST_SUITE_END()
