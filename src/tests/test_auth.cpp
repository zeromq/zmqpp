
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

BOOST_AUTO_TEST_SUITE( auth )


BOOST_AUTO_TEST_CASE(grasslands)
{
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

    BOOST_CHECK_EQUAL("Hello", response.get(0));
}

BOOST_AUTO_TEST_CASE(strawhouse)
{
    // initialize the 0MQ context
    zmqpp::context context;
  
    // Start an authentication engine for this context. This engine
    // allows or denies incoming connections (talking to the libzmq
    // core over a protocol called ZAP).
    zmqpp::auth authenticator{context};

    // Get some indication of what the authenticator is deciding
    authenticator.set_verbose (true);

    // Configure ZAP domain
    authenticator.configure_domain ("global");

    // Whitelist our address; any other address will be rejected
    authenticator.allow ("127.0.0.1");

    // create and bind a server socket
    zmqpp::socket server (context, zmqpp::socket_type::push);
    //server.set(zmqpp::socket_option::zap_domain, "global");
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
    
    BOOST_CHECK_EQUAL("Hello", response.get(0));
}

BOOST_AUTO_TEST_CASE(woodhouse)
{
    // initialize the 0MQ context
    zmqpp::context context;

    // Start an authentication engine for this context. This engine
    // allows or denies incoming connections (talking to the libzmq
    // core over a protocol called ZAP).
    zmqpp::auth authenticator{context};

    // Get some indication of what the authenticator is deciding
    authenticator.set_verbose (true);

    authenticator.configure_domain ("*");

    // Tell the authenticator how to handle PLAIN requests
    authenticator.configure_plain ("admin", "password");

    // Whitelist our address; any other address will be rejected
    authenticator.allow ("127.0.0.1");

    // create and bind a server socket
    zmqpp::socket server (context, zmqpp::socket_type::push);
    server.set(zmqpp::socket_option::plain_server, 1);
    server.bind("tcp://*:9000");

    // create and connect a client socket
    zmqpp::socket client (context, zmqpp::socket_type::pull);
    client.set(zmqpp::socket_option::plain_username, "admin");
    client.set(zmqpp::socket_option::plain_password, "password");
    client.connect("tcp://127.0.0.1:9000");

    // Send a single message from server to client
    zmqpp::message request;
    request << "Hello";
    server.send(request);

    zmqpp::message response;
    client.receive(response);

    BOOST_CHECK_EQUAL("Hello", response.get(0));
}

BOOST_AUTO_TEST_CASE(stonehouse)
{
    // initialize the 0MQ context
    zmqpp::context context;

    // Start an authentication engine for this context. This engine
    // allows or denies incoming connections (talking to the libzmq
    // core over a protocol called ZAP).
    zmqpp::auth authenticator{context};

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
  
    BOOST_CHECK_EQUAL("Hello", response.get(0));
}

BOOST_AUTO_TEST_CASE(ironhouse)
{
    // initialize the 0MQ context
    zmqpp::context context;

    // Start an authentication engine for this context. This engine
    // allows or denies incoming connections (talking to the libzmq
    // core over a protocol called ZAP).
    zmqpp::auth authenticator{context};

    // Get some indication of what the authenticator is deciding
    authenticator.set_verbose (true);

    authenticator.configure_domain ("*");

    // Whitelist our address; any other address will be rejected
    authenticator.allow ("127.0.0.1");

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

    BOOST_CHECK_EQUAL("Hello", response.get(0));
}

/* 
 * The client task runs in its own context, and receives the 
 * client keypair and server public key as an argument.
 */
static void client_task (zmqpp::curve::keypair& client_keypair, std::string server_public_key) {
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

    BOOST_CHECK_EQUAL("Hello", response.get(0));
}

/* 
 * The server task runs in its own context, and receives the 
 * server key pair and a set of allowed/valid client public keys as an argument.
 */
static void server_task (zmqpp::curve::keypair& server_keypair, std::vector<std::string>& client_public_keys) {
    // initialize the 0MQ context
    zmqpp::context context;

    // Start an authentication engine for this context. This engine
    // allows or denies incoming connections (talking to the libzmq
    // core over a protocol called ZAP).
    zmqpp::auth authenticator{context};

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


BOOST_AUTO_TEST_CASE(ironhouse2)
{
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


BOOST_AUTO_TEST_SUITE_END()
