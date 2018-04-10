/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file is part of zmqpp.
 * Copyright (c) 2011-2015 Contributors as noted in the AUTHORS file.
 */

/*
 *  Created on: 8 Aug 2011
 *      Author: @benjamg
 */

#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>

#include "zmqpp/context.hpp"
#include "zmqpp/socket.hpp"

BOOST_AUTO_TEST_SUITE( socket_options )

#define STRINGIFY(x) #x
#define CHECK_SET(socket, type, option) check_set<type>(socket, zmqpp::socket_option::option, STRINGIFY(option), false)
#define CHECK_SET_POSITIVE(socket, type, option) check_set<type>(socket, zmqpp::socket_option::option, STRINGIFY(option), true)
#define CHECK_GET(socket, type, option) check_get<type>(socket, zmqpp::socket_option::option, STRINGIFY(option))
#define CHECK_SET_GET(socket, type, option) check_set_get<type>(socket, zmqpp::socket_option::option, STRINGIFY(option), false)

// Note the hacky abuse of the fact we don't have float options
#define CHECK_NOSET(socket, option) check_set<float>(socket, zmqpp::socket_option::option, STRINGIFY(option), false)
#define CHECK_NOGET(socket, option) check_get<float>(socket, zmqpp::socket_option::option, STRINGIFY(option))

template<typename CheckType, typename WantedType>
void try_set(zmqpp::socket& socket, zmqpp::socket_option const& option, CheckType const& value, std::string const& option_name, std::string const& value_type)
{
	BOOST_TEST_CHECKPOINT("setting option " << option_name << " against set type '" << value_type << "'");
	try
	{
		socket.set(option, value);
		BOOST_CHECK_MESSAGE(typeid(CheckType) == typeid(WantedType), "expected exception setting option '" << option_name << "' against type '" << value_type << "'");
	}
	catch(zmqpp::zmq_internal_exception const& e)
	{
		BOOST_CHECK_MESSAGE(false, "threw internal exception " << e.zmq_error() << " '" << e.what() << "' setting option '" << option_name << "' against type '" << value_type << "'");
	}
	catch(zmqpp::exception const& e)
	{
		BOOST_CHECK_MESSAGE(typeid(CheckType) != typeid(WantedType), "threw unexpected exception '" << e.what() << "' setting option '" << option_name << "' against type '" << value_type << "'");
	}
}

template<typename CheckType, typename WantedType>
void try_get(zmqpp::socket const& socket, zmqpp::socket_option const& option, std::string const& option_name, std::string const& value_type)
{
	BOOST_TEST_CHECKPOINT("getting option " << option_name << " against set type '" << value_type << "'");
	try
	{
		CheckType value;
		socket.get(option, value);
		BOOST_CHECK_MESSAGE(typeid(CheckType) == typeid(WantedType), "expected exception getting option " << option_name << " against type '" << value_type << "'");
	}
	catch(zmqpp::zmq_internal_exception const& e)
	{
		BOOST_CHECK_MESSAGE(false, "threw internal exception " << e.zmq_error() << " '" << e.what() << "' getting option " << option_name << " against type '" << value_type << "'");
	}
	catch(zmqpp::exception const& e)
	{
		BOOST_CHECK_MESSAGE(typeid(CheckType) != typeid(WantedType), "threw unexpected exception '" << e.what() << "' getting option " << option_name << " against type '" << value_type << "'");
	}
}

template<typename CheckType, typename WantedType>
void try_set_get(zmqpp::socket& socket, zmqpp::socket_option const& option, CheckType const& value, std::string const& option_name, std::string const& value_type)
{
	BOOST_TEST_CHECKPOINT("setting and getting option " << option_name << " against set type '" << value_type << "'");
	try
	{
		socket.set(option, value);
		CheckType retrieveValue;
		socket.get(option, retrieveValue);
		BOOST_CHECK_MESSAGE(value == retrieveValue, "wrong value retrieving option " << option_name << " against type '" << value_type << "'");
	}
	catch(zmqpp::zmq_internal_exception const& e)
	{
		BOOST_CHECK_MESSAGE(false, "threw internal exception " << e.zmq_error() << " '" << e.what() << "' getting option " << option_name << " against type '" << value_type << "'");
	}
	catch(zmqpp::exception const& e)
	{
		BOOST_CHECK_MESSAGE(typeid(CheckType) != typeid(WantedType), "threw unexpected exception '" << e.what() << "' getting option " << option_name << " against type '" << value_type << "'");
	}
}

template<typename Type>
void check_set(zmqpp::socket& socket, zmqpp::socket_option const& option, std::string const& option_name, bool positive_only)
{
	// Boolean
	try_set<bool, Type>(socket, option, true, option_name, "boolean (true)");
	try_set<bool, Type>(socket, option, false, option_name, "boolean (false)");

	// Integer - Masquerade of unsigned 64bit integer
	if (typeid(uint64_t) == typeid(Type))
	{
		// Positive integers are valid as unsigned 64bit
		try_set<int, Type>(socket, option, -1, option_name, "signed integer (negative)");
		try_set<int, int>(socket, option, 42, option_name, "signed_integer (positive / masquerade)");
	}
	else if (typeid(int64_t) == typeid(Type))
	{
		if (positive_only) { try_set<int, Type>(socket, option, -1, option_name, "signed integer (negative)");	}
		else { try_set<int, int>(socket, option, -1, option_name, "signed integer (negative / masquerade)"); }
		try_set<int, int>(socket, option, 42, option_name, "signed integer (positive / masquerade)");
	}
	// Integer - Masquerade of boolean
	else if (typeid(bool) == typeid(Type))
	{
		// 1 and 0 Integers are valid as boolean
		try_set<int, Type>(socket, option, -1, option_name, "signed integer (negative)");
		try_set<int, int>(socket, option, 0, option_name, "signed integer (false / masquerade)");
		try_set<int, int>(socket, option, 1, option_name, "signed integer (true / masquerade)");
		try_set<int, Type>(socket, option, 42, option_name, "signed integer (positive)");
	}
	// Integer - Others
	else
	{
		if (positive_only) { try_set<int, float>(socket, option, -1, option_name, "signed integer (negative / masquerade)");	}
		else { try_set<int, Type>(socket, option, -1, option_name, "signed integer (negative)"); }
		try_set<int, Type>(socket, option, 42, option_name, "signed integer (positive)");
	}

	// Unsigned 64bit integer
	try_set<uint64_t, Type>(socket, option, 1, option_name, "unsigned 64bit integer");

#if (ZMQ_VERSION_MAJOR == 2) || ((ZMQ_VERSION_MAJOR == 3) && (ZMQ_VERSION_MINOR < 2))
	// 64bit integer
	try_set<int64_t, Type>(socket, option, 1, option_name, "signed 64bit integer");
#endif

	// Strings
	try_set<std::string, Type>(socket, option, "test", option_name, "string");
}

template<typename Type>
void check_get(zmqpp::socket& socket, zmqpp::socket_option const& option, std::string const& option_name)
{
	// Boolean
	try_get<bool, Type>(socket, option, option_name, "boolean");

	// Integer - Masquerade of boolean
	if (typeid(bool) == typeid(Type))
	{
		try_get<int, int>(socket, option, option_name, "signed integer (masquerade)");
	}
	// Integer - Others
	else
	{
		try_get<int, Type>(socket, option, option_name, "signed integer");
	}

	// Unsigned 64bit integer
	try_get<uint64_t, Type>(socket, option, option_name, "unsigned 64bit integer");

#if (ZMQ_VERSION_MAJOR == 2)
	// 64bit integer - Masquerade of boolean
	if (typeid(bool) == typeid(Type))
	{
		try_get<int64_t, int64_t>(socket, option, option_name, "signed 64bit integer (masquerade)");
	}
	// 64bit integer - Others
	else
	{
		try_get<int64_t, Type>(socket, option, option_name, "signed 64bit integer");
	}
#else
	// 64bit integer
	try_get<int64_t, Type>(socket, option, option_name, "signed 64bit integer");
#endif

	// Strings
	try_get<std::string, Type>(socket, option, option_name, "string");
}

template<typename Type>
void check_set_get(zmqpp::socket& socket, zmqpp::socket_option const& option, std::string const& option_name, bool positive_only)
{
	// Integer - Masquerade of unsigned 64bit integer
	if (typeid(uint64_t) == typeid(Type))
	{
		try_set_get<uint64_t, uint64_t>(socket, option, 42, option_name, "unsigned 64 integer");
	}
	else if (typeid(int64_t) == typeid(Type))
	{
		try_set_get<int64_t, int64_t>(socket, option, -1, option_name, "signed 64 integer");
	}
	// Integer - Masquerade of boolean
	else if (typeid(bool) == typeid(Type))
	{
		try_set_get<bool, bool>(socket, option, true, option_name, "boolean");
	}
	else if (typeid(std::string) == typeid(Type))
	{
		// Strings
		try_set_get<std::string, std::string>(socket, option, "test", option_name, "string");
	}
}

BOOST_AUTO_TEST_CASE( set_socket_options )
{
	zmqpp::context context;
	zmqpp::socket socket(context, zmqpp::socket_type::subscribe);
	socket.bind("inproc://test");

	CHECK_SET_GET(socket, int, receive_more);
	CHECK_SET_GET(socket, int, file_descriptor);
	CHECK_SET_GET(socket, int, events);
	CHECK_SET_GET(socket, int, type);
	CHECK_SET_GET(socket, int, linger);
	CHECK_SET_GET(socket, int, backlog);
	CHECK_SET_GET(socket, int, receive_timeout);
	CHECK_SET_GET(socket, int, send_timeout);
	CHECK_SET_GET(socket, uint64_t, affinity);
	CHECK_SET_GET(socket, std::string, identity);
	// For some reason -1 not working here
	//CHECK_SET(socket, int, reconnect_interval);
	CHECK_SET_GET(socket, int, reconnect_interval_max);
	CHECK_SET_GET(socket, int, backlog);
#if (ZMQ_VERSION_MAJOR > 2)
	CHECK_SET_GET(socket, int, send_buffer_size);
	CHECK_SET_GET(socket, int, receive_buffer_size);
	CHECK_SET_GET(socket, int, rate);
	CHECK_SET_GET(socket, int, recovery_interval);
	CHECK_SET_GET(socket, int, send_high_water_mark);
	CHECK_SET_GET(socket, int, receive_high_water_mark);
	CHECK_SET_GET(socket, int, multicast_hops);
#else
	CHECK_SET_GET(socket, bool, multicast_loopback);
	CHECK_SET_GET(socket, int64_t, rate);
	CHECK_SET_GET(socket, int64_t, recovery_interval);
	CHECK_SET_GET(socket, int64_t, recovery_interval_seconds);
	CHECK_SET_GET(socket, int64_t, swap_size);
	CHECK_SET_GET(socket, uint64_t, send_buffer_size);
	CHECK_SET_GET(socket, uint64_t, receive_buffer_size);
	CHECK_SET_GET(socket, uint64_t, high_water_mark);
#endif




#if (ZMQ_VERSION_MAJOR > 3) || ((ZMQ_VERSION_MAJOR == 3) && (ZMQ_VERSION_MINOR >= 1))
	CHECK_SET_GET(socket, bool, ipv4_only);
#endif

#if (ZMQ_VERSION_MAJOR > 3) || ((ZMQ_VERSION_MAJOR == 3) && (ZMQ_VERSION_MINOR >= 2))
#if (ZMQ_VERSION_MAJOR == 3 && ZMQ_VERSION_MINOR == 2)
	CHECK_SET_GET(socket, bool, delay_attach_on_connect);
#else
	CHECK_SET_GET(socket, bool, immediate);
#endif
	// CHECK_SET_GET(socket, int, tcp_keepalive);  --- special case of boolean but with -1?
	CHECK_SET_GET(socket, int, tcp_keepalive_idle);
	CHECK_SET_GET(socket, int, tcp_keepalive_count);
	CHECK_SET_GET(socket, int, tcp_keepalive_interval);
	// CHECK_SET_GET(socket, std::string, tcp_accept_filter); --- special case required to be an address
#endif

#if (ZMQ_VERSION_MAJOR >= 4)
	CHECK_SET_GET(socket, bool, ipv6);
	CHECK_SET_GET(socket, int, mechanism);
	CHECK_SET_GET(socket, std::string, plain_password);
	CHECK_SET_GET(socket, bool, plain_server);
	CHECK_SET_GET(socket, std::string, plain_username);
	CHECK_SET_GET(socket, std::string, zap_domain);
	//CHECK_SET_GET(socket, std::string, curve_public_key);
	//CHECK_SET_GET(socket, std::string, curve_secret_key);
	//CHECK_SET_GET(socket, std::string, curve_server_key);
#endif

#if (ZMQ_VERSION_MAJOR > 4 || ZMQ_VERSION_MAJOR == 4 && ZMQ_VERSION_MINOR >= 2)
	//CHECK_SET_GET(socket, std::string, gssapi_principal);
	//CHECK_SET_GET(socket, std::string, gssapi_service_principal);
	CHECK_SET_GET(socket, std::string, socks_proxy);
#endif

#ifdef ZMQ_EXPERIMENTAL_LABELS
	CHECK_NOSET(socket, receive_label);
#endif
}

BOOST_AUTO_TEST_CASE( set_get_socket_options )
{
	zmqpp::context context;
	zmqpp::socket socket(context, zmqpp::socket_type::subscribe);
	socket.bind("inproc://test");

	CHECK_NOSET(socket, receive_more);
	CHECK_NOSET(socket, file_descriptor);
	CHECK_NOSET(socket, events);
	CHECK_NOSET(socket, type);
#if (ZMQ_VERSION_MAJOR > 3) || ((ZMQ_VERSION_MAJOR == 3) && (ZMQ_VERSION_MINOR >= 2))
	CHECK_NOSET(socket, last_endpoint);
#endif

	CHECK_SET(socket, int, linger);
	CHECK_SET_POSITIVE(socket, int, backlog);
	CHECK_SET(socket, int, receive_timeout);
	CHECK_SET(socket, int, send_timeout);
	CHECK_SET(socket, uint64_t, affinity);
	CHECK_SET(socket, std::string, identity);
	CHECK_SET(socket, std::string, subscribe);
	CHECK_SET(socket, std::string, unsubscribe);
	// For some reason -1 not working here
	//CHECK_SET(socket, int, reconnect_interval);
	CHECK_SET_POSITIVE(socket, int, reconnect_interval_max);
	CHECK_SET_POSITIVE(socket, int, backlog);
#if (ZMQ_VERSION_MAJOR > 2)
	CHECK_SET_POSITIVE(socket, int, send_buffer_size);
	CHECK_SET_POSITIVE(socket, int, receive_buffer_size);
	CHECK_SET_POSITIVE(socket, int, rate);
	CHECK_SET_POSITIVE(socket, int, recovery_interval);
	CHECK_SET_POSITIVE(socket, int, send_high_water_mark);
	CHECK_SET_POSITIVE(socket, int, receive_high_water_mark);
	CHECK_SET_POSITIVE(socket, int, multicast_hops);
	CHECK_SET_POSITIVE(socket, int64_t, max_messsage_size);
#else
	CHECK_SET(socket, bool, multicast_loopback);
	CHECK_SET_POSITIVE(socket, int64_t, rate);
	CHECK_SET_POSITIVE(socket, int64_t, recovery_interval);
	CHECK_SET_POSITIVE(socket, int64_t, recovery_interval_seconds);
	CHECK_SET_POSITIVE(socket, int64_t, swap_size);
	CHECK_SET(socket, uint64_t, send_buffer_size);
	CHECK_SET(socket, uint64_t, receive_buffer_size);
	CHECK_SET(socket, uint64_t, high_water_mark);
#endif

#if (ZMQ_VERSION_MAJOR > 3) || ((ZMQ_VERSION_MAJOR == 3) && (ZMQ_VERSION_MINOR >= 1))
	CHECK_SET(socket, bool, ipv4_only);
#endif

#if (ZMQ_VERSION_MAJOR > 3) || ((ZMQ_VERSION_MAJOR == 3) && (ZMQ_VERSION_MINOR >= 2))
#if (ZMQ_VERSION_MAJOR == 3 && ZMQ_VERSION_MINOR == 2)
	CHECK_SET(socket, bool, delay_attach_on_connect);
#else
	CHECK_SET(socket, bool, immediate);
#endif
	// CHECK_SET(socket, int, tcp_keepalive);  --- special case of boolean but with -1?
	CHECK_SET(socket, int, tcp_keepalive_idle);
	CHECK_SET(socket, int, tcp_keepalive_count);
	CHECK_SET(socket, int, tcp_keepalive_interval);
	// CHECK_SET(socket, std::string, tcp_accept_filter); --- special case required to be an address
#endif

#if (ZMQ_VERSION_MAJOR >= 4)
	CHECK_SET(socket, bool, ipv6);
	CHECK_NOSET(socket, mechanism);
	CHECK_SET(socket, std::string, plain_password);
	CHECK_SET(socket, bool, plain_server);
	CHECK_SET(socket, std::string, plain_username);
	CHECK_SET(socket, std::string, zap_domain);
	CHECK_SET(socket, bool, conflate);
#endif

#ifdef ZMQ_EXPERIMENTAL_LABELS
	CHECK_NOSET(socket, receive_label);
#endif
}


#if (ZMQ_VERSION_MAJOR >= 4)
BOOST_AUTO_TEST_CASE( set_socket_options_tcp_only )
{
	zmqpp::context context;
	zmqpp::socket socket(context, zmqpp::socket_type::subscribe);
	socket.bind("tcp://*:0");

// TODO: reenable once I have curve key generation working to test against
//	CHECK_SET(socket, std::string, curve_public_key);
//	CHECK_SET(socket, std::string, curve_secret_key);
//	CHECK_SET(socket, std::string, curve_server_key);
//	CHECK_SET(socket, bool, curve_server);
}

BOOST_AUTO_TEST_CASE( set_socket_options_router_types )
{
	zmqpp::context context;
	zmqpp::socket socket(context, zmqpp::socket_type::router);
	socket.bind("inproc://test");

	CHECK_SET(socket, bool, router_raw);
	CHECK_SET(socket, bool, probe_router);
}

BOOST_AUTO_TEST_CASE( set_socket_options_request_types )
{
	zmqpp::context context;
	zmqpp::socket socket(context, zmqpp::socket_type::request);
	socket.bind("inproc://test");

	CHECK_SET(socket, bool, request_correlate);
	CHECK_SET(socket, bool, request_relaxed);
}
#endif

BOOST_AUTO_TEST_CASE( get_socket_options )
{
	zmqpp::context context;
	zmqpp::socket socket(context, zmqpp::socket_type::subscribe);
	socket.bind("inproc://test");

	CHECK_NOGET(socket, subscribe);
	CHECK_NOGET(socket, unsubscribe);
#if (ZMQ_VERSION_MAJOR > 3) || ((ZMQ_VERSION_MAJOR == 3) && (ZMQ_VERSION_MINOR >= 2))
	CHECK_NOGET(socket, router_mandatory);
	CHECK_NOGET(socket, xpub_verbose);
	CHECK_NOGET(socket, tcp_accept_filter);
#endif

	CHECK_GET(socket, bool, receive_more);
	CHECK_GET(socket, int, file_descriptor);
	CHECK_GET(socket, int, events);
	CHECK_GET(socket, int, type);
	CHECK_GET(socket, int, linger);
	CHECK_GET(socket, int, backlog);
	CHECK_GET(socket, int, reconnect_interval);
	CHECK_GET(socket, int, reconnect_interval_max);
	CHECK_GET(socket, int, receive_timeout);
	CHECK_GET(socket, int, send_timeout);
	CHECK_GET(socket, uint64_t, affinity);
	CHECK_GET(socket, std::string, identity);

#if (ZMQ_VERSION_MAJOR > 2)
	CHECK_GET(socket, int, send_buffer_size);
	CHECK_GET(socket, int, receive_buffer_size);
	CHECK_GET(socket, int, rate);
	CHECK_GET(socket, int, recovery_interval);
	CHECK_GET(socket, int, send_high_water_mark);
	CHECK_GET(socket, int, receive_high_water_mark);
	CHECK_GET(socket, int, multicast_hops);
	CHECK_GET(socket, int64_t, max_messsage_size);
#else
	CHECK_GET(socket, bool, multicast_loopback);
	CHECK_GET(socket, int64_t, rate);
	CHECK_GET(socket, int64_t, recovery_interval);
	CHECK_GET(socket, int64_t, recovery_interval_seconds);
	CHECK_GET(socket, int64_t, swap_size);
	CHECK_GET(socket, uint64_t, send_buffer_size);
	CHECK_GET(socket, uint64_t, receive_buffer_size);
	CHECK_GET(socket, uint64_t, high_water_mark);
#endif

#if (ZMQ_VERSION_MAJOR > 3) || ((ZMQ_VERSION_MAJOR == 3) && (ZMQ_VERSION_MINOR >= 1))
	CHECK_GET(socket, bool, ipv4_only);
#endif

#if (ZMQ_VERSION_MAJOR > 3) || ((ZMQ_VERSION_MAJOR == 3) && (ZMQ_VERSION_MINOR >= 2))
#if (ZMQ_VERSION_MAJOR == 3 && ZMQ_VERSION_MINOR == 2)
	CHECK_GET(socket, bool, delay_attach_on_connect);
#else
	CHECK_GET(socket, bool, immediate);
#endif
	CHECK_GET(socket, std::string, last_endpoint);
	CHECK_GET(socket, int, tcp_keepalive);
	CHECK_GET(socket, int, tcp_keepalive_idle);
	CHECK_GET(socket, int, tcp_keepalive_count);
	CHECK_GET(socket, int, tcp_keepalive_interval);
#endif

#if (ZMQ_VERSION_MAJOR >= 4)
	CHECK_GET(socket, bool, ipv6);
	CHECK_GET(socket, int, mechanism);
	CHECK_GET(socket, std::string, plain_password);
	CHECK_GET(socket, bool, plain_server);
	CHECK_GET(socket, std::string, plain_username);
	CHECK_GET(socket, std::string, zap_domain);
	CHECK_NOGET(socket, conflate);
	CHECK_NOGET(socket, probe_router);
	CHECK_NOGET(socket, request_correlate);
	CHECK_NOGET(socket, request_relaxed);
	CHECK_NOGET(socket, router_raw);
#endif

#ifdef ZMQ_EXPERIMENTAL_LABELS
	CHECK_GET(socket, bool, receive_label);
#endif
}

#if (ZMQ_VERSION_MAJOR >= 4)
BOOST_AUTO_TEST_CASE( get_socket_options_tcp_only )
{
	zmqpp::context context;
	zmqpp::socket socket(context, zmqpp::socket_type::subscribe);
	socket.bind("tcp://*:0");

// TODO: reenable once I have curve key generation working to test against
//	CHECK_GET(socket, std::string, curve_public_key);
//	CHECK_GET(socket, std::string, curve_secret_key);
//	CHECK_GET(socket, std::string, curve_server_key);
//  CHECK_NOGET(socket, curve_server);
}
#endif
#if (ZMQ_VERSION_MAJOR > 4) || ((ZMQ_VERSION_MAJOR == 4) && (ZMQ_VERSION_MINOR >= 2))
BOOST_AUTO_TEST_CASE( use_fd_socket_option )
{
	zmqpp::context context;
	zmqpp::socket socket(context, zmqpp::socket_type::push);
	CHECK_GET(socket, int, use_fd);
	CHECK_SET(socket, int, use_fd);
	CHECK_GET(socket, int, use_fd);
}
#endif

BOOST_AUTO_TEST_SUITE_END()
