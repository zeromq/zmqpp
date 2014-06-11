/*
 *  Created on: 8 Aug 2011
 *      Author: @benjamg
 */

#include <array>
#include <list>
#include <memory>
#include <string>

#include <boost/test/unit_test.hpp>

#include "zmqpp/context.hpp"
#include "zmqpp/socket.hpp"
#include "zmqpp/message.hpp"
#include "zmqpp/signal.hpp"

BOOST_AUTO_TEST_SUITE( socket )

const int bubble_poll_timeout = 1;
const int max_poll_timeout = 100;

void bubble_subscriptions(zmqpp::socket& socket)
{
	zmq_pollitem_t item = { socket, 0, ZMQ_POLLIN, 0 };
	int result = zmq_poll(&item, 1, bubble_poll_timeout);
	BOOST_REQUIRE_MESSAGE(0 == result, "polling command failed to timeout during subscription bubble");
}

void wait_for_socket(zmqpp::socket& socket)
{
	zmq_pollitem_t item = { socket, 0, ZMQ_POLLIN, 0 };
	int result = zmq_poll(&item, 1, max_poll_timeout);
	BOOST_REQUIRE_MESSAGE(result >= 0, "polling command returned without expected value: " << zmq_strerror(zmq_errno()));
	BOOST_REQUIRE_MESSAGE(0 != result, "polling command returned with timeout after " << max_poll_timeout << " milliseconds");
	BOOST_REQUIRE_MESSAGE(1 == result, "polling command claims " << result << " sockets have events but we only gave it one");
	BOOST_REQUIRE_MESSAGE(item.revents & ZMQ_POLLIN, "events do not match expected POLLIN event: " << item.revents);
}

BOOST_AUTO_TEST_CASE( socket_creation )
{
	zmqpp::context context;
	zmqpp::socket socket(context, zmqpp::socket_type::pull);
}

BOOST_AUTO_TEST_CASE( socket_creation_bad_type )
{
	zmqpp::context context;
	BOOST_CHECK_THROW(zmqpp::socket socket(context, static_cast<zmqpp::socket_type>(-1)), zmqpp::zmq_internal_exception)
}

BOOST_AUTO_TEST_CASE( valid_socket )
{
	zmqpp::context context;
	zmqpp::socket socket(context, zmqpp::socket_type::pull);
	socket.bind("inproc://test");

	zmqpp::message message;
	BOOST_CHECK(!socket.receive(message, true));
}

BOOST_AUTO_TEST_CASE( valid_move_supporting )
{
	zmqpp::context context;
	std::unique_ptr<zmqpp::socket> original(new zmqpp::socket(context, zmqpp::socket_type::pull));
	original->bind("inproc://test");

	zmqpp::socket clone(std::move(*original));
	//original.reset();

	zmqpp::message message;
	BOOST_CHECK(!clone.receive(message, true));
}

BOOST_AUTO_TEST_CASE( simple_pull_push )
{
	zmqpp::context context;

	zmqpp::socket puller(context, zmqpp::socket_type::pull);
	puller.bind("inproc://test");

	zmqpp::socket pusher(context, zmqpp::socket_type::push);
	pusher.connect("inproc://test");

	BOOST_CHECK(pusher.send("hello world!"));

	wait_for_socket(puller);

	std::string message;
	BOOST_CHECK(puller.receive(message));

	BOOST_CHECK_EQUAL("hello world!", message);
	BOOST_CHECK(!puller.has_more_parts());
}

BOOST_AUTO_TEST_CASE( multipart_pair )
{
	zmqpp::context context;

	zmqpp::socket alpha(context, zmqpp::socket_type::pair);
	alpha.bind("inproc://test");

	zmqpp::socket omega(context, zmqpp::socket_type::pair);
	omega.connect("inproc://test");

	BOOST_CHECK(alpha.send("hello", zmqpp::socket::send_more));
	BOOST_CHECK(alpha.send("world", zmqpp::socket::send_more));
	BOOST_CHECK(alpha.send("!"));

	wait_for_socket(omega);

	std::string message;

	BOOST_CHECK(omega.receive(message));
	BOOST_CHECK_EQUAL("hello", message);
	BOOST_REQUIRE(omega.has_more_parts());

	BOOST_CHECK(omega.receive(message));
	BOOST_CHECK_EQUAL("world", message);
	BOOST_REQUIRE(omega.has_more_parts());

	BOOST_CHECK(omega.receive(message));
	BOOST_CHECK_EQUAL("!", message);
	BOOST_CHECK(!omega.has_more_parts());
}

BOOST_AUTO_TEST_CASE( subscribe_via_option )
{
	zmqpp::context context;

	zmqpp::socket publisher(context, zmqpp::socket_type::publish);
	publisher.bind("inproc://test");

	zmqpp::socket subscriber(context, zmqpp::socket_type::subscribe);
	subscriber.connect("inproc://test");
	subscriber.set(zmqpp::socket_option::subscribe, "watch1");

	BOOST_CHECK(publisher.send("watch0", zmqpp::socket::send_more));
	BOOST_CHECK(publisher.send("contents0"));
	BOOST_CHECK(publisher.send("watch1", zmqpp::socket::send_more));
	BOOST_CHECK(publisher.send("contents1"));

	wait_for_socket(subscriber);

	std::string message;
	BOOST_CHECK(subscriber.receive(message));
	BOOST_CHECK_EQUAL("watch1", message);
	BOOST_REQUIRE(subscriber.has_more_parts());
	BOOST_CHECK(subscriber.receive(message));
	BOOST_CHECK_EQUAL("contents1", message);
	BOOST_CHECK(!subscriber.has_more_parts());
}

BOOST_AUTO_TEST_CASE( subscribe_helpers )
{
	zmqpp::context context;

	zmqpp::socket publisher(context, zmqpp::socket_type::publish);
	publisher.bind("inproc://test");

	zmqpp::socket subscriber(context, zmqpp::socket_type::subscribe);
	subscriber.connect("inproc://test");
	subscriber.subscribe("watch1");
	subscriber.subscribe("watch2");

	BOOST_CHECK(publisher.send("watch0", zmqpp::socket::send_more));
	BOOST_CHECK(publisher.send("contents0"));
	BOOST_CHECK(publisher.send("watch1", zmqpp::socket::send_more));
	BOOST_CHECK(publisher.send("contents1"));
	BOOST_CHECK(publisher.send("watch2", zmqpp::socket::send_more));
	BOOST_CHECK(publisher.send("contents2"));
	BOOST_CHECK(publisher.send("watch3", zmqpp::socket::send_more));
	BOOST_CHECK(publisher.send("contents3"));

	wait_for_socket(subscriber);

	std::string message;
	BOOST_CHECK(subscriber.receive(message));
	BOOST_CHECK_EQUAL("watch1", message);
	BOOST_REQUIRE(subscriber.has_more_parts());
	BOOST_CHECK(subscriber.receive(message));
	BOOST_CHECK_EQUAL("contents1", message);
	BOOST_CHECK(!subscriber.has_more_parts());

	wait_for_socket(subscriber);

	BOOST_CHECK(subscriber.receive(message));
	BOOST_CHECK_EQUAL("watch2", message);
	BOOST_REQUIRE(subscriber.has_more_parts());
	BOOST_CHECK(subscriber.receive(message));
	BOOST_CHECK_EQUAL("contents2", message);
	BOOST_CHECK(!subscriber.has_more_parts());

	subscriber.unsubscribe("watch1");
	bubble_subscriptions(subscriber);

	BOOST_CHECK(publisher.send("watch0", zmqpp::socket::send_more));
	BOOST_CHECK(publisher.send("contents0"));
	BOOST_CHECK(publisher.send("watch1", zmqpp::socket::send_more));
	BOOST_CHECK(publisher.send("contents1"));
	BOOST_CHECK(publisher.send("watch2", zmqpp::socket::send_more));
	BOOST_CHECK(publisher.send("contents2"));

	wait_for_socket(subscriber);

	BOOST_CHECK(subscriber.receive(message));
	BOOST_CHECK_EQUAL("watch2", message);
	BOOST_REQUIRE(subscriber.has_more_parts());
	BOOST_CHECK(subscriber.receive(message));
	BOOST_CHECK_EQUAL("contents2", message);
	BOOST_CHECK(!subscriber.has_more_parts());
}

BOOST_AUTO_TEST_CASE( subscribe_helpers_multitopic_method )
{
	std::list<std::string> topics = { "watch1", "watch2" };

	zmqpp::context context;

	zmqpp::socket publisher(context, zmqpp::socket_type::publish);
	publisher.bind("inproc://test");

	zmqpp::socket subscriber(context, zmqpp::socket_type::subscribe);
	subscriber.connect("inproc://test");
	subscriber.subscribe(topics.begin(), topics.end());

	BOOST_CHECK(publisher.send("watch0", zmqpp::socket::send_more));
	BOOST_CHECK(publisher.send("contents0"));
	BOOST_CHECK(publisher.send("watch1", zmqpp::socket::send_more));
	BOOST_CHECK(publisher.send("contents1"));
	BOOST_CHECK(publisher.send("watch2", zmqpp::socket::send_more));
	BOOST_CHECK(publisher.send("contents2"));
	BOOST_CHECK(publisher.send("watch3", zmqpp::socket::send_more));
	BOOST_CHECK(publisher.send("contents3"));

	wait_for_socket(subscriber);

	std::string message;
	BOOST_CHECK(subscriber.receive(message));
	BOOST_CHECK_EQUAL("watch1", message);
	BOOST_REQUIRE(subscriber.has_more_parts());
	BOOST_CHECK(subscriber.receive(message));
	BOOST_CHECK_EQUAL("contents1", message);
	BOOST_CHECK(!subscriber.has_more_parts());

	wait_for_socket(subscriber);

	BOOST_CHECK(subscriber.receive(message));
	BOOST_CHECK_EQUAL("watch2", message);
	BOOST_REQUIRE(subscriber.has_more_parts());
	BOOST_CHECK(subscriber.receive(message));
	BOOST_CHECK_EQUAL("contents2", message);
	BOOST_CHECK(!subscriber.has_more_parts());
}

BOOST_AUTO_TEST_CASE( sending_messages )
{
	zmqpp::context context;

	zmqpp::socket pusher(context, zmqpp::socket_type::push);
	pusher.bind("inproc://test");

	zmqpp::socket puller(context, zmqpp::socket_type::pull);
	puller.connect("inproc://test");

	zmqpp::message message;
	std::string part("another world");

	message.add("hello world!");
	message.add(part);

	pusher.send(message);
	BOOST_CHECK_EQUAL(0, message.parts());

	wait_for_socket(puller);

	BOOST_CHECK(puller.receive(part));
	BOOST_CHECK_EQUAL("hello world!", part);
	BOOST_REQUIRE(puller.has_more_parts());
	BOOST_CHECK(puller.receive(part));
	BOOST_CHECK_EQUAL("another world", part);
	BOOST_CHECK(!puller.has_more_parts());
}

BOOST_AUTO_TEST_CASE( receiving_messages )
{
	zmqpp::context context;

	zmqpp::socket pusher(context, zmqpp::socket_type::push);
	pusher.bind("inproc://test");

	zmqpp::socket puller(context, zmqpp::socket_type::pull);
	puller.connect("inproc://test");

	zmqpp::message message;
	std::string part("another world");

	message.add("hello world!");
	message.add(part);

	pusher.send(message);
	BOOST_CHECK_EQUAL(0, message.parts());

	wait_for_socket(puller);

	BOOST_CHECK(puller.receive(message));
	BOOST_REQUIRE_EQUAL(2, message.parts());
	BOOST_CHECK_EQUAL("hello world!", message.get(0));
	BOOST_CHECK_EQUAL("another world", message.get(1));
	BOOST_CHECK(!puller.has_more_parts());
}

BOOST_AUTO_TEST_CASE( receive_over_old_messages )
{
	zmqpp::context context;

	zmqpp::socket pusher( context, zmqpp::socket_type::push );
	pusher.bind( "inproc://test" );

	zmqpp::socket puller( context, zmqpp::socket_type::pull );
	puller.connect( "inproc://test");

	pusher.send( "first message" );
	pusher.send( "second message" );

	wait_for_socket( puller );

	zmqpp::message message;
	BOOST_CHECK( puller.receive( message ) );
	BOOST_REQUIRE_EQUAL( 1, message.parts() );
	BOOST_CHECK_EQUAL( "first message", message.get(0) );

	BOOST_CHECK( puller.receive( message ) );
	BOOST_REQUIRE_EQUAL( 1, message.parts() );
	BOOST_CHECK_EQUAL( "second message", message.get(0) );
}

BOOST_AUTO_TEST_CASE( cleanup_safe_with_pending_data )
{
	zmqpp::context context;

	zmqpp::socket pusher(context, zmqpp::socket_type::push);
	pusher.bind("inproc://test");

	zmqpp::socket puller(context, zmqpp::socket_type::pull);
	puller.connect("inproc://test");

	zmqpp::message message;
	std::string part("another world");

	message.add("hello world!");
	message.add(part);

	pusher.send(message);
	BOOST_CHECK_EQUAL(0, message.parts());
}

BOOST_AUTO_TEST_CASE( multitarget_puller )
{
	std::vector<std::string> endpoints = { "inproc://test1", "inproc://test2" };

	zmqpp::context context;

	zmqpp::socket pusher1(context, zmqpp::socket_type::push);
	pusher1.bind(endpoints[0]);

	zmqpp::socket pusher2(context, zmqpp::socket_type::push);
	pusher2.bind(endpoints[1]);

	zmqpp::socket puller(context, zmqpp::socket_type::pull);
	puller.connect(endpoints.begin(), endpoints.end());

	BOOST_CHECK(pusher1.send("hello world!"));
	BOOST_CHECK(pusher2.send("a test message"));

	wait_for_socket(puller);

	std::string message;
	BOOST_CHECK(puller.receive(message));
	BOOST_CHECK_EQUAL("hello world!", message);
	BOOST_CHECK(puller.receive(message));
	BOOST_CHECK_EQUAL("a test message", message);
}


BOOST_AUTO_TEST_CASE( test_receive_send_signals )
{
    zmqpp::context ctx;
    zmqpp::socket p1(ctx, zmqpp::socket_type::pair);
    zmqpp::socket p2(ctx, zmqpp::socket_type::pair);

    p1.bind("inproc://test");
    p2.connect("inproc://test");

    p1.send(zmqpp::signal::test);
    p1.send("....");
    p1.send(zmqpp::signal::stop);

    zmqpp::signal s;
    std::string str;

    p2.receive(s);
    BOOST_CHECK_EQUAL(zmqpp::signal::test, s);
    p2.receive(str);
    p2.send(zmqpp::signal::test);
    p2.receive(s);
    BOOST_CHECK_EQUAL(zmqpp::signal::stop, s);

    p1.receive(s);
    BOOST_CHECK_EQUAL(zmqpp::signal::test, s);
}

BOOST_AUTO_TEST_CASE( test_wait )
{
    zmqpp::context ctx;
    zmqpp::socket p1(ctx, zmqpp::socket_type::pair);
    zmqpp::socket p2(ctx, zmqpp::socket_type::pair);

    p1.bind("inproc://test");
    p2.connect("inproc://test");

    p1.send(zmqpp::signal::test);
    p1.send("....");
    p1.send("___");
    p1.send(zmqpp::signal::stop);

    // test wait(): the non signal message must be discarded.
    BOOST_CHECK_EQUAL(zmqpp::signal::test, p2.wait());
    BOOST_CHECK_EQUAL(zmqpp::signal::stop, p2.wait());
}

BOOST_AUTO_TEST_CASE( sending_large_messages_string )
{
	zmqpp::context context;

	zmqpp::socket pusher(context, zmqpp::socket_type::push);
	pusher.bind("tcp://127.0.0.1:5567");

	zmqpp::socket puller(context, zmqpp::socket_type::pull);
	puller.connect("tcp://127.0.0.1:5567");

	std::string message;
    const size_t bytes_to_send = static_cast<size_t>(2.1 * 1024 * 1024 * 1024);
    message.reserve(bytes_to_send);
    for (size_t i = 0; i < bytes_to_send; i++)
    {
        message.push_back('A' + (i % 26));
    }

	BOOST_CHECK(pusher.send(message));

	zmq_pollitem_t item = { puller, 0, ZMQ_POLLIN, 0 };
    const int poll_timeout = 1000000;
	int result = zmq_poll(&item, 1, poll_timeout);
	BOOST_REQUIRE_MESSAGE(result >= 0, "polling command returned without expected value: " << zmq_strerror(zmq_errno()));
	BOOST_REQUIRE_MESSAGE(0 != result, "polling command returned with timeout after " << poll_timeout << " milliseconds");
	BOOST_REQUIRE_MESSAGE(1 == result, "polling command claims " << result << " sockets have events but we only gave it one");
	BOOST_REQUIRE_MESSAGE(item.revents & ZMQ_POLLIN, "events do not match expected POLLIN event: " << item.revents);

    std::string received_message;

	BOOST_CHECK(puller.receive(received_message));
	BOOST_CHECK_EQUAL(0, message.compare(received_message));
	BOOST_CHECK(!puller.has_more_parts());
}

BOOST_AUTO_TEST_SUITE_END()
