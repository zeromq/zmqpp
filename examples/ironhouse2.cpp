/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file is part of zmqpp.
 * Copyright (c) 2011-2015 Contributors as noted in the AUTHORS file.
 */

/**
 * The Ironhouse2 Pattern
 *
 * Security doesn't get any stronger than this. An attacker is going to
 * have to break into your systems to see data before/after encryption.
 *
 * This is exactly the same example but broken into two threads
 * so you can better see what client and server do, separately.
 *
 * \date   25 Nov 2014
 * \author Prem Shankar Kumar (\@meprem)
 */
  

#include <zmqpp/zmqpp.hpp>
#include <zmqpp/curve.hpp>
#include <string>
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

/* 
 * The client task runs in its own context, and receives the 
 * client keypair and server public key as an argument.
 */
void client_task (zmqpp::curve::keypair& client_keypair, std::string server_public_key) {
  // initialize the 0MQ context
  zmqpp::context context;

  // create client socket and configure it to use full encryption
  zmqpp::socket client (context, zmqpp::socket_type::pull);
  client.set(zmqpp::socket_option::curve_server_key, server_public_key);
  client.set(zmqpp::socket_option::curve_public_key, client_keypair.public_key);
  client.set(zmqpp::socket_option::curve_secret_key, client_keypair.secret_key);
  client.connect("tcp://127.0.0.1:9000");

  // Wait for our message, that signals the test was successful  
  zmqpp::message response;
  client.receive(response);
  
  assert("Hello" == response.get(0));
  std::cout << "Ironhouse2 test OK" << std::endl;
}

/* 
 * The server task runs in its own context, and receives the 
 * server key pair and a set of allowed/valid client public keys as an argument.
 */
void server_task (zmqpp::curve::keypair& server_keypair, std::vector<std::string>& client_public_keys) {
  // initialize the 0MQ context
  zmqpp::context context;

  // Start an authentication engine for this context. This engine
  // allows or denies incoming connections (talking to the libzmq
  // core over a protocol called ZAP).
  zmqpp::auth authenticator(context);

  // Get some indication of what the authenticator is deciding
  authenticator.set_verbose (true);

  authenticator.configure_domain ("*");

  // Whitelist our address; any other address will be rejected
  authenticator.allow ("127.0.0.1");

  // Tell authenticator to do authenticate clients against all allowed client public keys
  for(auto cpk : client_public_keys)
    authenticator.configure_curve(cpk);

  // create and bind a server socket
  zmqpp::socket server (context, zmqpp::socket_type::push);
  server.set(zmqpp::socket_option::identity, "IDENT");
  int as_server = 1;
  server.set(zmqpp::socket_option::curve_server, as_server);
  server.set(zmqpp::socket_option::curve_secret_key, server_keypair.secret_key);
  server.bind("tcp://*:9000");

  // Send a single message from server to client
  zmqpp::message request;
  request << "Hello";
  server.send(request);

  std::this_thread::sleep_for(std::chrono::seconds(1));
}


int main(int argc, char *argv[]) {
  // We need two certificates, one for the client and one for
  // the server. The client must know the server's public key
  // to make a CURVE connection.
  zmqpp::curve::keypair client_keypair = zmqpp::curve::generate_keypair();
  zmqpp::curve::keypair server_keypair = zmqpp::curve::generate_keypair();

  std::cout << "Client Public Key :" << client_keypair.public_key << std::endl;
  std::cout << "Client Secret Key :" << client_keypair.secret_key << std::endl;

  // Allowed clients public keys
  std::vector<std::string> client_public_keys;
  client_public_keys.push_back(client_keypair.public_key);

  std::cout << "Server Public Key :" << server_keypair.public_key << std::endl;
  std::cout << "Server Secret Key :" << server_keypair.secret_key << std::endl;

  // Now start two threads; each of these has their own ZeroMQ context.
  std::thread client_thread(client_task, std::ref(client_keypair), std::ref(server_keypair.public_key));
  std::thread server_thread(server_task, std::ref(server_keypair), std::ref(client_public_keys));
  
  // wait to finish
  client_thread.join();
  server_thread.join();

  return 0;
}

/*
prem@prem-Vostro-2420:~/zmqpp-develop/examples$./ironhouse2
Client Public Key :/@<XW&Ti%5IkHIRdr$[ybKZ^%RjTHTv-&QAOQ$/O
Client Secret Key :.s3i(X#[x@UGMH6a0p4qMy51uN-hOLiR?f#*D[M1
Server Public Key :4UR=%Zh0v$yrOO={pq5D[IAS85Zyi>=c(x1t7Rwe
Server Secret Key :np?uPAh)tx[ExgYI(m!Dg!xyK?ViaP9+[r-ok4Q4
auth: Starting ZAP Authentication Server
auth: verbose:true
auth: API command=DOMAIN
auth: domain=*
auth: API command=ALLOW
auth: whitelisting ipaddress=127.0.0.1
auth: configure CURVE - client public key:/@<XW&Ti%5IkHIRdr$[ybKZ^%RjTHTv-&QAOQ$/O
auth: API command=CURVE
auth: configured CURVE - allow client with public key:/@<XW&Ti%5IkHIRdr$[ybKZ^%RjTHTv-&QAOQ$/O
auth: ZAP request mechanism=CURVE ipaddress=127.0.0.1
auth: passed (whitelist) address=127.0.0.1
auth: allowed (CURVE) client_key=/@<XW&Ti%5IkHIRdr$[ybKZ^%RjTHTv-&QAOQ$/O
auth: ZAP reply status_code=200 status_text=OK
Ironhouse2 test OK
auth: API command=TERMINATE
auth: Shutdown ZAP Authentication Server
prem@prem-Vostro-2420:~/zmqpp-develop/examples$
*/
