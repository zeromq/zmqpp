/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file is part of zmqpp.
 * Copyright (c) 2011-2015 Contributors as noted in the AUTHORS file.
 */

/**
 * The Stonehouse Pattern
 *
 * Where we allow any clients to connect, but we promise clients
 * that we are who we claim to be, and our conversations won't be
 * tampered with or modified, or spied on.
 *
 * \date   25 Nov 2014
 * \author Prem Shankar Kumar (\@meprem)
 */
  

#include <zmqpp/zmqpp.hpp>
#include <zmqpp/curve.hpp>
#include <string>
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
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

  // Tell the authenticator how to handle CURVE requests
  authenticator.configure_curve ("CURVE_ALLOW_ANY");

  // We need two certificates, one for the client and one for
  // the server. The client must know the server's public key
  // to make a CURVE connection.
  zmqpp::curve::keypair client_keypair = zmqpp::curve::generate_keypair();
  zmqpp::curve::keypair server_keypair = zmqpp::curve::generate_keypair();

  std::cout << "Client Public Key :" << client_keypair.public_key << std::endl;
  std::cout << "Client Secret Key :" << client_keypair.secret_key << std::endl;

  std::cout << "Server Public Key :" << server_keypair.public_key << std::endl;
  std::cout << "Server Secret Key :" << server_keypair.secret_key << std::endl;

  // create and bind a server socket
  zmqpp::socket server (context, zmqpp::socket_type::push);
  server.set(zmqpp::socket_option::identity, "IDENT");
  int as_server = 1;
  server.set(zmqpp::socket_option::curve_server, as_server);
  server.set(zmqpp::socket_option::curve_secret_key, server_keypair.secret_key);
  server.bind("tcp://*:9000");

  // create and connect a client socket
  zmqpp::socket client (context, zmqpp::socket_type::pull);
  client.set(zmqpp::socket_option::curve_server_key, server_keypair.public_key);
  client.set(zmqpp::socket_option::curve_public_key, client_keypair.public_key);
  client.set(zmqpp::socket_option::curve_secret_key, client_keypair.secret_key);
  client.connect("tcp://127.0.0.1:9000");

  // Send a single message from server to client
  zmqpp::message request;
  request << "Hello";
  server.send(request);

  zmqpp::message response;
  client.receive(response);
  
  assert("Hello" == response.get(0));
  std::cout << "Stonehouse test OK" << std::endl;

  return 0;
}

/*
prem@prem-Vostro-2420:~/zmqpp-develop/examples$./stonehouse 
auth: Starting ZAP Authentication Server
auth: verbose:true
auth: API command=DOMAIN
auth: domain=*
auth: API command=ALLOW
auth: whitelisting ipaddress=127.0.0.1
auth: configure CURVE - client public key:CURVE_ALLOW_ANY
auth: API command=CURVE
auth: configured CURVE - allow ALL clients
Client Public Key :RKsg#n}EYFY[&byc-s6kIv1ri9Pe:Sjxp8LK8ES[
Client Secret Key :D/p?iaw=utRNVB^8pfX944(0pz/ADAAwx.k&h#xd
Server Public Key :bP4=Ej(W^ecT:](PmXa{U3O6Y*HwLsrQ<!2c}a!p
Server Secret Key :O>%Yb)H5joFpAA=EB?UrDaj]wQZh*}x4dsr@D:++
auth: ZAP request mechanism=CURVE ipaddress=127.0.0.1
auth: passed (whitelist) address=127.0.0.1
auth: allowed (CURVE allow any client)
auth: ZAP reply status_code=200 status_text=OK
Stonehouse test OK
auth: API command=TERMINATE
auth: Shutdown ZAP Authentication Server
prem@prem-Vostro-2420:~/zmqpp-develop/examples$
*/
