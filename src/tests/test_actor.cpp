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
#include "zmqpp/actor.hpp"
#include "zmqpp/poller.hpp"
#include <iostream>

BOOST_AUTO_TEST_SUITE( actor )


BOOST_AUTO_TEST_CASE(simple_test)
{
    auto lambda = [](zmqpp::socket * pipe)
    {
        zmqpp::message msg("boap");
        msg.add(42);
        msg.add(13);
        pipe->send(zmqpp::signal::ok); // signal we successfully started
        pipe->send(msg);
        return true; // will send signal::ok to signal successful shutdown
    };
    zmqpp::actor actor(lambda);


    zmqpp::message msg;
    actor.pipe()->receive(msg);
    int a, b;
    std::string str;

    msg >> str >> a >> b;
    BOOST_CHECK_EQUAL("boap", str);
    BOOST_CHECK_EQUAL(42, a);
    BOOST_CHECK_EQUAL(13, b);
}

BOOST_AUTO_TEST_CASE(test_actor_movable)
{
    auto lambda = [](zmqpp::socket * pipe)
    {
        pipe->send(zmqpp::signal::ok); // signal we successfully started
        pipe->send("boap1");
        pipe->send("boap2");
        pipe->send("boap3");
        return true;
    };
    zmqpp::actor actor1(lambda);

    std::string str;    
    actor1.pipe()->receive(str);
    BOOST_CHECK_EQUAL("boap1", str);

    zmqpp::actor actor2(std::move(actor1));
    actor2.pipe()->receive(str);
    BOOST_CHECK_EQUAL("boap2", str);

    // move from an "empty" actor and make sure it doesnt have a pointer to the pipe anymore
    zmqpp::actor actor_empty(std::move(actor1));
    BOOST_CHECK(actor_empty.pipe() == nullptr);

    zmqpp::actor actor3(std::move(actor2));
    actor3.pipe()->receive(str);
    BOOST_CHECK_EQUAL("boap3", str);
    BOOST_CHECK_EQUAL(actor3.stop(true), true);

    zmqpp::actor actor4(std::move(actor3));
    BOOST_CHECK_EQUAL(actor4.stop(true), true);
}

BOOST_AUTO_TEST_CASE(child_thread_wait_for_stop)
{
    auto lambda = [](zmqpp::socket * pipe)
    {
        int count = 0;
        zmqpp::message msg;
        zmqpp::signal sig;
        pipe->send(zmqpp::signal::ok);

        while (pipe->receive(msg))
        {
            if (msg.is_signal())
            {
                msg.get(sig, 0);
                if (sig == zmqpp::signal::stop)
                    break;
            }
            zmqpp::message rep("hello");
            pipe->send(rep);
            count++;
        }
        zmqpp::message final_res;
        final_res << count;
        // send the total count to the parent thread
        pipe->send(final_res);
        return true;
    };
    zmqpp::actor *actor = new zmqpp::actor(lambda);
    zmqpp::message msg;

    for (int i = 0; i < 12; i++)
    {
        actor->pipe()->send("hey");
        actor->pipe()->receive(msg);
    }
    actor->stop(); // sends the stop signal, but doesn't wait for a response: the destructor will wait instead
    actor->pipe()->receive(msg);
    int count;
    msg >> count;
    delete actor;
    BOOST_CHECK_EQUAL(12, count);
}

/**
 * Helper function that runs a echo server (router socket) bound to endpoint.
 * It will send signal::ko if the bind fails (invalid endpoint) and signal::ok if initialization worked.
 * 
 * It exits when receiving signal::stop from the pipe, and send the number of messages the router sent
 * when receiving the GET_COUNT command.
 */
static bool count_echo_server(zmqpp::socket * pipe, zmqpp::context *context, std::string endpoint)
{
    zmqpp::socket router(*context, zmqpp::socket_type::router);
    zmqpp::poller p;
    zmqpp::signal sig;
    int count = 0; // how many message sent (by the router))

    try
    {
        router.bind(endpoint);
        pipe->send(zmqpp::signal::ok);
    }
    catch (zmqpp::zmq_internal_exception &e)
    {
        // by returning false here, the actor will send signal::ko
        // this will make the actor constructor throw.
        // we could also to the ourselves: pipe->send(signal::ko);)
        return false;
    }

    p.add(*pipe);
    p.add(router);
    while (p.poll())
    {
        zmqpp::message msg;
        if (p.has_input(router))
        {
            router.receive(msg);
            router.send(msg);
            count++;
        }
        if (p.has_input(*pipe))
        {
            pipe->receive(msg);
            if (msg.is_signal())
            {
                msg.get(sig, 0);
                if (sig == zmqpp::signal::stop)
                    break;
            }
            if (msg.get(0) == "GET_COUNT")
            {
                zmqpp::message msg_count(count);
                pipe->send(msg_count);
            }
        }
    }
    return true;
}

BOOST_AUTO_TEST_CASE(test_actor_init_failed)
{
    zmqpp::context application_context; // what would be the default context we use generally.

    // the actor thread will fail and send signal::ko, so actor constructor should throw.
    // the placeholder is for the pipe socket. 
    BOOST_CHECK_THROW(zmqpp::actor actor(std::bind(&count_echo_server, std::placeholders::_1,
            &application_context, "InvalidEndpointForRouter")),
            zmqpp::actor_initialization_exception);
}

BOOST_AUTO_TEST_CASE(test_actor_stop_failed)
{
    // a task that simulates failure to stop
    auto lambda_fail = [] (zmqpp::socket * pipe) -> bool
    {
        pipe->send(zmqpp::signal::ok); // signal start ok

        //wait for stop signal
        while (true)
        {
            if (pipe->wait() == zmqpp::signal::stop)
                break;
        }
        // for some reason let's say we failed to properly stopped the task.
        return false;
    };

    // a task that successfully stops
    auto lambda_success = [] (zmqpp::socket * pipe) -> bool
    {
        pipe->send(zmqpp::signal::ok); // signal start ok

        //wait for stop signal
        while (true)
        {
            if (pipe->wait() == zmqpp::signal::stop)
                break;
        }
        return true;
    };

    zmqpp::actor actor1(lambda_success);
    zmqpp::actor actor2(lambda_fail);
    BOOST_CHECK_EQUAL(true, actor1.stop(true));
    BOOST_CHECK_EQUAL(false, actor2.stop(true));
}

BOOST_AUTO_TEST_CASE(test_sending_command)
{
    zmqpp::context application_context; // what would be the default context we use generally.
    zmqpp::actor actor(std::bind(&count_echo_server, std::placeholders::_1, &application_context, "inproc://router-endpoint"));

    zmqpp::socket client(application_context, zmqpp::socket_type::dealer);
    client.connect("inproc://router-endpoint"); // simulate a random client of the router

    int c;

    for (int i = 0; i < 10; i++)
    {
        if (i % 3 == 0)
        {
            zmqpp::message msg;
            actor.pipe()->send("GET_COUNT");
            actor.pipe()->receive(msg);
            msg >> c;
            BOOST_CHECK_EQUAL(i, c);
        }
        client.send("Hello");
        std::string bla;
        client.receive(bla); // we need to receive otherwise the count may be wrong
    }
    // assert total count
    zmqpp::message msg;
    actor.pipe()->send("GET_COUNT");
    actor.pipe()->receive(msg);
    msg >> c;
    BOOST_CHECK_EQUAL(10, c);
}

BOOST_AUTO_TEST_SUITE_END()
