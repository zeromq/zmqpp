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
#include "zmqpp/curve.hpp"
#include "zmqpp/zap_request.hpp"
#include "zmqpp/auth.hpp"
#include <iostream>
#include <string>
#include <netinet/in.h>

BOOST_AUTO_TEST_SUITE( auth )

// The whole test suite should be ran only against libzmq > 3
// Also, for some reason those tests fail against zmq4.0.x
#if ((ZMQ_VERSION_MAJOR > 3) &&  !(ZMQ_VERSION_MAJOR == 4 && ZMQ_VERSION_MINOR == 0))

BOOST_AUTO_TEST_CASE(grasslands)
{
    // initialize the 0MQ context
    zmqpp::context context;

    // create and bind a server socket
    zmqpp::socket server(context, zmqpp::socket_type::push);
    server.bind("tcp://*:0");
    const std::string endpoint = server.get<std::string>(zmqpp::socket_option::last_endpoint);

    // create and connect a client socket
    zmqpp::socket client(context, zmqpp::socket_type::pull);
    client.connect(endpoint);

    // Send a single message from server to client
    zmqpp::message request;
    request << "Hello";
    server.send(request);

    zmqpp::message response;
    client.receive(response);

    BOOST_CHECK_EQUAL("Hello", response.get(0));
}

BOOST_AUTO_TEST_CASE(strawhouse)
{
    // initialize the 0MQ context
    zmqpp::context context;
  
    // Start an authentication engine for this context. This engine
    // allows or denies incoming connections (talking to the libzmq
    // core over a protocol called ZAP).
    zmqpp::auth authenticator(context);

    // Get some indication of what the authenticator is deciding
    authenticator.set_verbose(true);

    // Configure ZAP domain
    authenticator.configure_domain("global");

    // Whitelist our address; any other address will be rejected
    authenticator.allow("127.0.0.1");

    // create and bind a server socket
    zmqpp::socket server(context, zmqpp::socket_type::push);
    //server.set(zmqpp::socket_option::zap_domain, "global");
    server.bind("tcp://*:0");
    const std::string endpoint = server.get<std::string>(zmqpp::socket_option::last_endpoint);

    // create and connect a client socket
    zmqpp::socket client(context, zmqpp::socket_type::pull);
    client.connect(endpoint);

    // Send a single message from server to client
    zmqpp::message request;
    request << "Hello";
    server.send(request);

    zmqpp::message response;
    client.receive(response);
    
    BOOST_CHECK_EQUAL("Hello", response.get(0));
}

BOOST_AUTO_TEST_CASE(woodhouse)
{
    // initialize the 0MQ context
    zmqpp::context context;

    // Start an authentication engine for this context. This engine
    // allows or denies incoming connections (talking to the libzmq
    // core over a protocol called ZAP).
    zmqpp::auth authenticator(context);

    // Get some indication of what the authenticator is deciding
    authenticator.set_verbose(true);

    authenticator.configure_domain("*");

    // Tell the authenticator how to handle PLAIN requests
    authenticator.configure_plain("admin", "password");

    // Whitelist our address; any other address will be rejected
    authenticator.allow("127.0.0.1");

    // create and bind a server socket
    zmqpp::socket server(context, zmqpp::socket_type::pull);
    server.set(zmqpp::socket_option::plain_server, 1);
    server.bind("tcp://*:0");
    const std::string endpoint = server.get<std::string>(zmqpp::socket_option::last_endpoint);

    // create and connect a client socket
    zmqpp::socket client(context, zmqpp::socket_type::push);
    client.set(zmqpp::socket_option::plain_username, "admin");
    client.set(zmqpp::socket_option::plain_password, "password");
    client.connect(endpoint);

    // Send a single message from client to server
    zmqpp::message request;
    request << "Hello";
    client.send(request);

    zmqpp::message response;
    server.receive(response);

    std::string user_id;
    BOOST_CHECK_EQUAL(true, response.get_property("User-Id", user_id));
    BOOST_CHECK_EQUAL("admin", user_id);
    BOOST_CHECK_EQUAL("Hello", response.get(0));
}

BOOST_AUTO_TEST_CASE(stonehouse)
{
    if (!zmq_has("curve"))
    {
      std::cout << "Skipping stonehouse test since zmq wasn't build with libsodium" << std::endl;
      return;
    }

    // initialize the 0MQ context
    zmqpp::context context;

    // Start an authentication engine for this context. This engine
    // allows or denies incoming connections (talking to the libzmq
    // core over a protocol called ZAP).
    zmqpp::auth authenticator(context);

    // Get some indication of what the authenticator is deciding
    authenticator.set_verbose(true);

    authenticator.configure_domain("*");

    // Whitelist our address; any other address will be rejected
    authenticator.allow("127.0.0.1");

    // Tell the authenticator how to handle CURVE requests
    authenticator.configure_curve("CURVE_ALLOW_ANY");

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
    zmqpp::socket server(context, zmqpp::socket_type::pull);
    server.set(zmqpp::socket_option::identity, "IDENT");
    int as_server = 1;
    server.set(zmqpp::socket_option::curve_server, as_server);
    server.set(zmqpp::socket_option::curve_secret_key, server_keypair.secret_key);
    server.bind("tcp://*:0");
    const std::string endpoint = server.get<std::string>(zmqpp::socket_option::last_endpoint);

    // create and connect a client socket
    zmqpp::socket client(context, zmqpp::socket_type::push);
    client.set(zmqpp::socket_option::curve_server_key, server_keypair.public_key);
    client.set(zmqpp::socket_option::curve_public_key, client_keypair.public_key);
    client.set(zmqpp::socket_option::curve_secret_key, client_keypair.secret_key);
    client.connect(endpoint);

    // Send a single message from client to server
    zmqpp::message request;
    request << "Hello";
    client.send(request);

    zmqpp::message response;
    server.receive(response);

    std::string user_id;
    BOOST_CHECK_EQUAL(true, response.get_property("User-Id", user_id));
    BOOST_CHECK_EQUAL(client_keypair.public_key, user_id);
    BOOST_CHECK_EQUAL("Hello", response.get(0));
}

BOOST_AUTO_TEST_CASE(custom_metadata)
{
    zmqpp::context context;
    zmqpp::socket server(context, zmqpp::socket_type::rep);
    server.bind("tcp://*:0");
    const std::string endpoint = server.get<std::string>(zmqpp::socket_option::last_endpoint);

    zmqpp::socket client(context, zmqpp::socket_type::req);
    client.connect(endpoint);

    zmqpp::message request;
    request << "1.0" << "0001" << "test" << "192.168.55.1" << "BOB" << "PLAIN" << "admin" << "secret";
    client.send(request);

    // let zap_request class handle the message
    zmqpp::zap_request zap_request(server, false);

    // create some metadata pairs
    const std::unordered_map<std::string, std::string> pairs = {
            {"foo", "bar"},
            {"name", "value"}
    };

    // send metadata together with ZAP reply
    zap_request.reply("200", "OK", "admin", pairs);

    zmqpp::message response;
    client.receive(response);

    BOOST_CHECK_EQUAL("1.0", response.get(0));
    BOOST_CHECK_EQUAL("0001", response.get(1));
    BOOST_CHECK_EQUAL("200", response.get(2));
    BOOST_CHECK_EQUAL("OK", response.get(3));
    BOOST_CHECK_EQUAL("admin", response.get(4));

    // the last frame contains ZMTP/3.0 encoded metadata
    const void *metadata;
    response.get(metadata, 5);

    const uint8_t *chars = reinterpret_cast<const uint8_t*>(metadata);

    for (size_t i = 0; i < pairs.size(); ++i) {
        const auto name_size = *chars;
        chars += 1;

        const std::string name(chars, chars + name_size);
        chars += name_size;

        const auto value_size = ntohl(*reinterpret_cast<const uint32_t*>(chars));
        chars += 4;

        const std::string value(chars, chars + value_size);
        chars += value_size;

        BOOST_CHECK_EQUAL(1, pairs.count(name));
        BOOST_CHECK_EQUAL(pairs.at(name), value);
    }
}

BOOST_AUTO_TEST_CASE(ironhouse)
{
    if (!zmq_has("curve"))
    {
      std::cout << "Skipping ironhouse test since zmq wasn't build with libsodium" << std::endl;
      return;
    }

    // initialize the 0MQ context
    zmqpp::context context;

    // Start an authentication engine for this context. This engine
    // allows or denies incoming connections (talking to the libzmq
    // core over a protocol called ZAP).
    zmqpp::auth authenticator(context);

    // Get some indication of what the authenticator is deciding
    authenticator.set_verbose(true);

    authenticator.configure_domain("*");

    // Whitelist our address; any other address will be rejected
    authenticator.allow("127.0.0.1");

    // We need two certificates, one for the client and one for
    // the server. The client must know the server's public key
    // to make a CURVE connection.
    zmqpp::curve::keypair client_keypair = zmqpp::curve::generate_keypair();
    zmqpp::curve::keypair server_keypair = zmqpp::curve::generate_keypair();

    std::cout << "Client Public Key :" << client_keypair.public_key << std::endl;
    std::cout << "Client Secret Key :" << client_keypair.secret_key << std::endl;

    std::cout << "Server Public Key :" << server_keypair.public_key << std::endl;
    std::cout << "Server Secret Key :" << server_keypair.secret_key << std::endl;

    // Tell authenticator to use the client public key
    authenticator.configure_curve(client_keypair.public_key);

    // create and bind a server socket
    zmqpp::socket server(context, zmqpp::socket_type::push);
    server.set(zmqpp::socket_option::identity, "IDENT");
    int as_server = 1;
    server.set(zmqpp::socket_option::curve_server, as_server);
    server.set(zmqpp::socket_option::curve_secret_key, server_keypair.secret_key);
    server.bind("tcp://*:0");
    const std::string endpoint = server.get<std::string>(zmqpp::socket_option::last_endpoint);

    // create and connect a client socket
    zmqpp::socket client(context, zmqpp::socket_type::pull);
    client.set(zmqpp::socket_option::curve_server_key, server_keypair.public_key);
    client.set(zmqpp::socket_option::curve_public_key, client_keypair.public_key);
    client.set(zmqpp::socket_option::curve_secret_key, client_keypair.secret_key);
    client.connect(endpoint);

    // Send a single message from server to client
    zmqpp::message request;
    request << "Hello";
    server.send(request);

    zmqpp::message response;
    client.receive(response);

    BOOST_CHECK_EQUAL("Hello", response.get(0));
}

/* 
 * The client task runs in its own context, and receives the 
 * client keypair and server public key as an argument.
 */
static void client_task(zmqpp::curve::keypair& client_keypair, std::string server_public_key) {
    // initialize the 0MQ context
    zmqpp::context context;

    // create client socket and configure it to use full encryption
    zmqpp::socket client(context, zmqpp::socket_type::pull);
    client.set(zmqpp::socket_option::curve_server_key, server_public_key);
    client.set(zmqpp::socket_option::curve_public_key, client_keypair.public_key);
    client.set(zmqpp::socket_option::curve_secret_key, client_keypair.secret_key);
    client.connect("tcp://127.0.0.1:9006");

    // Wait for our message, that signals the test was successful  
    zmqpp::message response;
    client.receive(response);

    BOOST_CHECK_EQUAL("Hello", response.get(0));
}

/* 
 * The server task runs in its own context, and receives the 
 * server key pair and a set of allowed/valid client public keys as an argument.
 */
static void server_task(zmqpp::curve::keypair& server_keypair, std::vector<std::string>& client_public_keys) {
    // initialize the 0MQ context
    zmqpp::context context;

    // Start an authentication engine for this context. This engine
    // allows or denies incoming connections (talking to the libzmq
    // core over a protocol called ZAP).
    zmqpp::auth authenticator(context);

    // Get some indication of what the authenticator is deciding
    authenticator.set_verbose(true);

    authenticator.configure_domain("*");

    // Whitelist our address; any other address will be rejected
    authenticator.allow("127.0.0.1");

    // Tell authenticator to do authenticate clients against all allowed client public keys
    for(auto cpk : client_public_keys)
    authenticator.configure_curve(cpk);

    // create and bind a server socket
    zmqpp::socket server(context, zmqpp::socket_type::push);
    server.set(zmqpp::socket_option::identity, "IDENT");
    int as_server = 1;
    server.set(zmqpp::socket_option::curve_server, as_server);
    server.set(zmqpp::socket_option::curve_secret_key, server_keypair.secret_key);
    server.bind("tcp://*:9006");

    // Send a single message from server to client
    zmqpp::message request;
    request << "Hello";
    server.send(request);

    std::this_thread::sleep_for(std::chrono::seconds(1));
}


BOOST_AUTO_TEST_CASE(ironhouse2)
{
    if (!zmq_has("curve"))
    {
      std::cout << "Skipping ironhouse2 test since zmq wasn't build with libsodium" << std::endl;
      return;
    }

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
}

#endif

BOOST_AUTO_TEST_SUITE_END()
