/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file is part of zmqpp.
 * Copyright (c) 2011-2015 Contributors as noted in the AUTHORS file.
 */

#include <boost/test/unit_test.hpp>
#include <thread>

#include "zmqpp/context.hpp"
#include "zmqpp/message.hpp"
#include "zmqpp/reactor.hpp"
#include "zmqpp/socket.hpp"

BOOST_AUTO_TEST_SUITE( reactor )

const int max_poll_timeout = 100;

BOOST_AUTO_TEST_CASE(initialise)
{
    zmqpp::context context;

    zmqpp::socket socket(context, zmqpp::socket_type::pull);
    socket.bind("inproc://test");

    zmqpp::reactor reactor;
    reactor.add(socket, []()->void
    {
    });

    BOOST_CHECK_EQUAL(zmqpp::poller::poll_none, reactor.events(socket));
}

BOOST_AUTO_TEST_CASE(simple_pull_push)
{
    zmqpp::context context;

    zmqpp::socket puller(context, zmqpp::socket_type::pull);
    puller.bind("inproc://test");

    zmqpp::socket pusher(context, zmqpp::socket_type::push);
    pusher.connect("inproc://test");

    BOOST_CHECK(pusher.send_raw("hello world!", sizeof ("hello world!")));

    zmqpp::reactor reactor;
    int test = 0;
    auto callable = [&test]() -> void
    {
        test = 1;
    };
    reactor.add(puller, callable);
    BOOST_CHECK(reactor.poll(max_poll_timeout));

    BOOST_CHECK_EQUAL(1, test);

    BOOST_CHECK_EQUAL(zmqpp::poller::poll_in, reactor.events(puller));

    std::string message;
    BOOST_CHECK(puller.receive(message));

    BOOST_CHECK_EQUAL("hello world!", message.c_str());
    BOOST_CHECK(!puller.has_more_parts());
}

BOOST_AUTO_TEST_CASE(multiple_socket)
{
    zmqpp::context context;

    zmqpp::socket pub(context, zmqpp::socket_type::pub);
    pub.bind("inproc://test");

    zmqpp::socket sub(context, zmqpp::socket_type::sub);
    sub.connect("inproc://test");
    sub.subscribe("hello");

    zmqpp::socket sub2(context, zmqpp::socket_type::sub);
    sub2.connect("inproc://test");
    sub2.subscribe("hello");

    BOOST_CHECK(pub.send("hello", zmqpp::socket::send_more));
    BOOST_CHECK(pub.send("hello world!"));

    zmqpp::reactor reactor;

    int test1 = 0;
    int test2 = 0;
    int test3 = 0;
    auto callable = [](int *value_to_update, int val) -> void
    {
        *value_to_update = val;
    };

    reactor.add(sub, std::bind(callable, &test1, 1));
    reactor.add(sub2, std::bind(callable, &test2, 2));
    reactor.add(pub, std::bind(callable, &test3, 3), zmqpp::poller::poll_out);
    BOOST_CHECK(reactor.poll(max_poll_timeout));

    BOOST_CHECK_EQUAL(1, test1);
    BOOST_CHECK_EQUAL(2, test2);
    BOOST_CHECK_EQUAL(3, test3);


    test1 = 0;
    test2 = 0;
    test3 = 0;

    reactor.remove(sub);
    BOOST_CHECK(pub.send("hello", zmqpp::socket::send_more));
    BOOST_CHECK(pub.send("hello world!"));

    BOOST_CHECK(reactor.poll(max_poll_timeout));

    BOOST_CHECK_EQUAL(0, test1);
    BOOST_CHECK_EQUAL(2, test2);
    BOOST_CHECK_EQUAL(3, test3);
}

BOOST_AUTO_TEST_CASE(remove_socket_in_handler)
{
    zmqpp::context context;

    zmqpp::socket pub(context, zmqpp::socket_type::pub);
    pub.bind("inproc://test");

    zmqpp::socket sub(context, zmqpp::socket_type::sub);
    sub.connect("inproc://test");
    sub.subscribe("hello");

    zmqpp::socket sub2(context, zmqpp::socket_type::sub);
    sub2.connect("inproc://test");
    sub2.subscribe("hello");

    BOOST_CHECK(pub.send("hello", zmqpp::socket::send_more));
    BOOST_CHECK(pub.send("hello world!"));

    zmqpp::reactor reactor;

    int test1 = 0;
    int test2 = 0;
    auto callable = [&](int *value_to_update, int val) -> void
    {
	if (val == 1)
	  {
	    reactor.remove(sub);
	  }
	
        *value_to_update = val;
    };

    reactor.add(sub, std::bind(callable, &test1, 1));
    reactor.add(sub2, std::bind(callable, &test2, 2));
    BOOST_CHECK(reactor.poll(max_poll_timeout));

    BOOST_CHECK_EQUAL(1, test1);
    BOOST_CHECK_EQUAL(2, test2);
    
    test1 = test2 = 0;

    BOOST_CHECK(pub.send("hello", zmqpp::socket::send_more));
    BOOST_CHECK(pub.send("hello world!"));
    BOOST_CHECK(reactor.poll(max_poll_timeout));

    BOOST_CHECK_EQUAL(0, test1);
    BOOST_CHECK_EQUAL(2, test2);
}


BOOST_AUTO_TEST_SUITE_END()
