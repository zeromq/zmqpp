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
#include <exception>

#include "zmqpp/context.hpp"
#include "zmqpp/message.hpp"
#include "zmqpp/loop.hpp"
#include "zmqpp/socket.hpp"

BOOST_AUTO_TEST_SUITE( loop )

BOOST_AUTO_TEST_CASE(basic_usage)
{
    zmqpp::context context;

    zmqpp::socket output(context, zmqpp::socket_type::pair);
    output.bind("inproc://test");
    zmqpp::socket input(context, zmqpp::socket_type::pair);
    input.connect("inproc://test");

    zmqpp::loop loop;

    auto sendPing = [](zmqpp::socket & socket) -> bool { socket.send("PING"); return true; };
    auto neverRun = []() -> bool { throw std::runtime_error("Not supposed to run"); return false; };

    auto t1 = loop.add(std::chrono::milliseconds(1000), 1, neverRun);
    loop.add(std::chrono::milliseconds(5), 1, [&loop, t1]() -> bool { loop.remove(t1); return true; });

    loop.add(std::chrono::milliseconds(20), 1, std::bind(sendPing, std::ref(output)));

    loop.add(std::chrono::milliseconds(2000), 1, neverRun);
    loop.add(std::chrono::milliseconds(2000), 1, neverRun);
    loop.add(std::chrono::milliseconds(2000), 1, neverRun);

    loop.add(input, [&input]() -> bool { return false; });

    BOOST_CHECK_NO_THROW(loop.start());
}

BOOST_AUTO_TEST_CASE(socket_removed_in_timer)
{
    zmqpp::context context;

    zmqpp::socket output(context, zmqpp::socket_type::pair);
    output.bind("inproc://test");
    zmqpp::socket input(context, zmqpp::socket_type::pair);
    input.connect("inproc://test");

    zmqpp::loop loop;

    bool socket_called = false;

    loop.add(output, [&socket_called]() -> bool { socket_called = true; return false; });
    loop.add(std::chrono::milliseconds(0), 1, [&loop, &output]() -> bool {
        loop.remove(output);
        loop.add(std::chrono::milliseconds(10), 1, []() -> bool { return false; });
        return true;
    });

    input.send("PING");

    BOOST_CHECK_NO_THROW(loop.start());
    BOOST_CHECK(socket_called == false);
}

BOOST_AUTO_TEST_CASE(simple_pull_push)
{
    zmqpp::context context;

    zmqpp::socket puller(context, zmqpp::socket_type::pull);
    puller.bind("inproc://test");

    zmqpp::socket pusher(context, zmqpp::socket_type::push);
    pusher.connect("inproc://test");

    BOOST_CHECK(pusher.send_raw("hello world!", sizeof ("hello world!")));

    zmqpp::loop loop;
    std::string message;
    auto callable = [&message, &puller]() -> bool
    {
        BOOST_CHECK(puller.receive(message));
        return true;
    };

    auto end_loop = []() -> bool { return false; };

    loop.add(puller, callable);
    loop.add(std::chrono::milliseconds(100), 1, end_loop);


    BOOST_CHECK_NO_THROW(loop.start());

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

    zmqpp::loop loop;

    int test1 = 0;
    int test2 = 0;
    int test3 = 0;
    auto callable = [](int *value_to_update, int val) -> bool
    {
        *value_to_update = val;
        return true;
    };

    auto end_loop = []() -> bool { return false; };

    loop.add(sub, std::bind(callable, &test1, 1));
    loop.add(sub2, std::bind(callable, &test2, 2));
    loop.add(pub, std::bind(callable, &test3, 3), zmqpp::poller::poll_out);

    loop.add(std::chrono::milliseconds(100), 1, end_loop);
    BOOST_CHECK_NO_THROW(loop.start());

    BOOST_CHECK_EQUAL(1, test1);
    BOOST_CHECK_EQUAL(2, test2);
    BOOST_CHECK_EQUAL(3, test3);


    test1 = 0;
    test2 = 0;
    test3 = 0;

    loop.remove(sub);

    BOOST_CHECK(pub.send("hello", zmqpp::socket::send_more));
    BOOST_CHECK(pub.send("hello world!"));

    loop.add(std::chrono::milliseconds(100), 1, end_loop);
    BOOST_CHECK_NO_THROW(loop.start());

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

    zmqpp::loop loop;
    auto end_loop = []() -> bool { return false; };

    int test1 = 0;
    int test2 = 0;
    auto callable = [&](int *value_to_update, int val) -> bool
    {
        if (val == 1)
        {
            loop.remove(sub);
        }

        *value_to_update = val;
        return true;
    };

    loop.add(sub, std::bind(callable, &test1, 1));
    loop.add(sub2, std::bind(callable, &test2, 2));

    loop.add(std::chrono::milliseconds(100), 1, end_loop);
    BOOST_CHECK_NO_THROW(loop.start());

    BOOST_CHECK_EQUAL(1, test1);
    BOOST_CHECK_EQUAL(2, test2);

    test1 = test2 = 0;

    BOOST_CHECK(pub.send("hello", zmqpp::socket::send_more));
    BOOST_CHECK(pub.send("hello world!"));

    loop.add(std::chrono::milliseconds(100), 1, end_loop);
    BOOST_CHECK_NO_THROW(loop.start());

    BOOST_CHECK_EQUAL(0, test1);
    BOOST_CHECK_EQUAL(2, test2);
}


BOOST_AUTO_TEST_SUITE_END()
