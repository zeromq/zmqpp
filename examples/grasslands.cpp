/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file is part of zmqpp.
 * Copyright (c) 2011-2015 Contributors as noted in the AUTHORS file.
 */

/**
 * The Grasslands Pattern
 *
 * The Classic ZeroMQ model, plain text with no protection at all.
 *
 * \date   25 Nov 2014
 * \author Prem Shankar Kumar (\@meprem)
 */

#include <zmqpp/zmqpp.hpp>
#include <string>
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
  // initialize the 0MQ context
  zmqpp::context context;

  // create and bind a server socket
  zmqpp::socket server (context, zmqpp::socket_type::push);
  server.bind("tcp://*:9000");

  // create and connect a client socket
  zmqpp::socket client (context, zmqpp::socket_type::pull);
  client.connect("tcp://127.0.0.1:9000");

  // Send a single message from server to client
  zmqpp::message request;
  request << "Hello";
  server.send(request);

  zmqpp::message response;
  client.receive(response);
  
  assert("Hello" == response.get(0));
  std::cout << "Grasslands test OK" << std::endl;

  return 0;
}

/*
prem@prem-Vostro-2420:~/zmqpp-develop/examples$./grasslands 
Grasslands test OK
prem@prem-Vostro-2420:~/zmqpp-develop/examples$ 
  */
