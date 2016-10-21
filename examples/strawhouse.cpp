/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file is part of zmqpp.
 * Copyright (c) 2011-2015 Contributors as noted in the AUTHORS file.
 */

/**
 * The Strawhouse Pattern.
 *
 * We allow or deny clients according to their IP address. It may keep 
 * spammers and idiots away, but won't stop a real attacker for more 
 * than a heartbeat.
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
  
    // Start an authentication engine for this context. This engine
    // allows or denies incoming connections (talking to the libzmq
    // core over a protocol called ZAP).
    zmqpp::auth authenticator(context);

    // Get some indication of what the authenticator is deciding
    authenticator.set_verbose (true);

    // Configure ZAP domain
    authenticator.configure_domain ("global");

    // Whitelist our address; any other address will be rejected
    authenticator.allow ("127.0.0.1");

    // create and bind a server socket
    zmqpp::socket server (context, zmqpp::socket_type::push);
    server.set(zmqpp::socket_option::zap_domain, "global");
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
    std::cout << "Strawhouse test OK" << std::endl;

    return 0;
}

/*
prem@prem-Vostro-2420:~/zmqpp-develop/examples$./strawhouse 
auth: Starting ZAP Authentication Server
auth: verbose:true
auth: API command=DOMAIN
auth: domain=global
auth: API command=ALLOW
auth: whitelisting ipaddress=127.0.0.1
auth: ZAP request mechanism=NULL ipaddress=127.0.0.1
auth: passed (whitelist) address=127.0.0.1
auth: ZAP reply status_code=200 status_text=OK user_id=
Strawhouse test OK
auth: API command=TERMINATE
auth: Shutdown ZAP Authentication Server

prem@prem-Vostro-2420:~/zmqpp-develop/examples$
*/
