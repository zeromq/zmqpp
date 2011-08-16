/*
 *  Created on: 12 Aug 2011
 *      Author: @benjamg
 */

#ifdef LOADTEST

#include <boost/test/unit_test.hpp>

#include <boost/thread.hpp>
#include <boost/timer.hpp>

#include "zmq/zmq.hpp"


BOOST_AUTO_TEST_SUITE( load )

BOOST_AUTO_TEST_CASE( push_messages_baseline )
{
	boost::timer t;

	long max_poll_timeout = 2000;
	uint64_t messages = 1e6;

	auto context = zmq_init(1);
	auto pusher = zmq_socket(context, ZMQ_PUSH);
	zmq_connect(pusher, "tcp://localhost:12345");

	auto puller = zmq_socket(context, ZMQ_PULL);
	zmq_bind(puller, "tcp://*:12345");

	auto pusher_func = [messages, &pusher](void) {
		auto remaining = messages;

		do
		{
			zmq_send(pusher, "hello world!", strlen("hello world!"), 0);
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

		zmq_recvmsg(puller, &message, 0);
		std::string str_message(static_cast<char*>(zmq_msg_data(&message)), zmq_msg_size(&message));

		BOOST_CHECK_EQUAL("hello world!", str_message);
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

	long max_poll_timeout = 2000;
	uint64_t messages = 1e6;

	zmq::context context;
	zmq::socket pusher(context, zmq::socket_type::push);
	pusher.connect("tcp://localhost:12345");

	zmq::socket puller(context, zmq::socket_type::pull);
	puller.bind("tcp://*:12345");

	auto pusher_func = [messages, &pusher](void) {
		auto remaining = messages;
		std::string data("hello world!");
		zmq::message message;

		do
		{
			message.copy_part(data);
			pusher.send(message);
		}
		while(--remaining > 0);
	};

	zmq::poller poller;
	poller.add(puller);

	boost::thread thread(pusher_func);

	uint64_t processed = 0;
	while(poller.poll(max_poll_timeout))
	{
		BOOST_REQUIRE(poller.has_input(puller));

		std::string message;
		puller.receive(message);

		BOOST_CHECK_EQUAL("hello world!", message);
		++processed;
	}

	double elapsed_run = t.elapsed();

	BOOST_CHECK_MESSAGE(thread.timed_join(boost::posix_time::milliseconds(max_poll_timeout)), "hung while joining pusher thread");
	BOOST_CHECK_EQUAL(processed, messages);

	BOOST_TEST_MESSAGE("Copy String");
	BOOST_TEST_MESSAGE("Messages pushed    : " << processed);
	BOOST_TEST_MESSAGE("Run time           : " << elapsed_run << " seconds");
	BOOST_TEST_MESSAGE("Messages per second: " << processed / elapsed_run);
	BOOST_TEST_MESSAGE("\n");
}

BOOST_AUTO_TEST_CASE( push_messages_move )
{
        boost::timer t;

        long max_poll_timeout = 2000;
        uint64_t messages = 1e6;

        zmq::context context;
        zmq::socket pusher(context, zmq::socket_type::push);
        pusher.connect("tcp://localhost:12345");

        zmq::socket puller(context, zmq::socket_type::pull);
        puller.bind("tcp://*:12345");

        auto pusher_func = [messages, &pusher](void) {
                auto remaining = messages;
                std::string data;
                zmq::message message;

                do
                {
			data = "hello world!";
                        message.move_part(data);
                        pusher.send(message);
                }
                while(--remaining > 0);
        };

        zmq::poller poller;
        poller.add(puller);

        boost::thread thread(pusher_func);

        uint64_t processed = 0;
        while(poller.poll(max_poll_timeout))
        {
                BOOST_REQUIRE(poller.has_input(puller));

                std::string message;
                puller.receive(message);

                BOOST_CHECK_EQUAL("hello world!", message);
                ++processed;
        }

        double elapsed_run = t.elapsed();

        BOOST_CHECK_MESSAGE(thread.timed_join(boost::posix_time::milliseconds(max_poll_timeout)), "hung while joining pusher thread");
        BOOST_CHECK_EQUAL(processed, messages);

        BOOST_TEST_MESSAGE("Move String");
        BOOST_TEST_MESSAGE("Messages pushed    : " << processed);
        BOOST_TEST_MESSAGE("Run time           : " << elapsed_run << " seconds");
        BOOST_TEST_MESSAGE("Messages per second: " << processed / elapsed_run);
        BOOST_TEST_MESSAGE("\n");
}

BOOST_AUTO_TEST_SUITE_END()

#endif // LOADTEST
