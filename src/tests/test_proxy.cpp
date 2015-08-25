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
#include "zmqpp/proxy.hpp"

BOOST_AUTO_TEST_SUITE( proxy )

BOOST_AUTO_TEST_CASE(simple_direction)
{
  zmqpp::context ctx;

  std::thread t1([&]()
                 {
                   zmqpp::socket sa(ctx, zmqpp::socket_type::pull);
                   zmqpp::socket sb(ctx, zmqpp::socket_type::push);
                   sa.bind("inproc://frontend");
                   sb.bind("inproc://backend");
                   zmqpp::proxy p(sa, sb);
                 });

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  zmqpp::socket puller(ctx, zmqpp::socket_type::pull);
  zmqpp::socket pusher(ctx, zmqpp::socket_type::push);

  pusher.connect(
      "inproc://frontend"); // connect to the pull socket in the other thread
  puller.connect("inproc://backend"); // ditto, push socket.

  zmqpp::message msg;
  msg << "Hello";

  pusher.send(msg);

  std::string ret;
  puller.receive(ret);
  BOOST_CHECK_EQUAL("Hello", ret);

  puller.close();
  pusher.close();
  ctx.terminate();
  t1.join();
}

BOOST_AUTO_TEST_SUITE_END()
