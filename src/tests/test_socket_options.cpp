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

// Note the hacky abuse of the fact we don't have float options
#define CHECK_NOSET(socket, option) check_set<float>(socket, zmqpp::socket_option::option, STRINGIFY(option), false)
#define CHECK_NOGET(socket, option) check_get<float>(socket, zmqpp::socket_option::option, STRINGIFY(option))

template<typename CheckType, typename WantedType>
void try_set(zmqpp::socket& socket, zmqpp::socket_option const& option, CheckType const& value, std::string const& option_name, std::string const& value_type)
{
	BOOST_CHECKPOINT("setting option " << option_name << " against set type '" << value_type << "'");
	try
	{
		socket.set(option, value);
		BOOST_CHECK_MESSAGE(typeid(CheckType) == typeid(WantedType), "expected exception setting option " << option_name << " against type '" << value_type << "'");
	}
	catch(zmqpp::zmq_internal_exception const& e)
	{
		BOOST_CHECK_MESSAGE(false, "threw internal exception " << e.zmq_error() << " '" << e.what() << "' setting option " << option_name << " against type '" << value_type << "'");
	}
	catch(zmqpp::exception const& e)
	{
		BOOST_CHECK_MESSAGE(typeid(CheckType) != typeid(WantedType), "threw unexpected exception '" << e.what() << "' setting option " << option_name << " against type '" << value_type << "'");
	}
}

template<typename CheckType, typename WantedType>
void try_get(zmqpp::socket const& socket, zmqpp::socket_option const& option, std::string const& option_name, std::string const& value_type)
{
	BOOST_CHECKPOINT("getting option " << option_name << " against set type '" << value_type << "'");
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

#if (ZMQ_VERSION_MAJOR == 2) or ((ZMQ_VERSION_MAJOR == 3) and (ZMQ_VERSION_MINOR < 2))
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

BOOST_AUTO_TEST_CASE( set_socket_options )
{
	zmqpp::context context;
	zmqpp::socket socket(context, zmqpp::socket_type::subscribe);
	socket.bind("inproc://test");

	CHECK_NOSET(socket, receive_more);
	CHECK_NOSET(socket, file_descriptor);
	CHECK_NOSET(socket, events);
	CHECK_NOSET(socket, type);
#if (ZMQ_VERSION_MAJOR > 3) or ((ZMQ_VERSION_MAJOR == 3) and (ZMQ_VERSION_MINOR >= 2))
	CHECK_NOSET(socket, last_endpoint);
#endif

	CHECK_SET(socket, int, linger);
	CHECK_SET(socket, int, backlog);
	CHECK_SET(socket, int, receive_timeout);
	CHECK_SET(socket, int, send_timeout);
	CHECK_SET(socket, uint64_t, affinity);
	CHECK_SET(socket, std::string, identity);
	CHECK_SET(socket, std::string, subscribe);
	CHECK_SET(socket, std::string, unsubscribe);
	// For some reason -1 not working here
	//CHECK_SET(socket, int, reconnect_interval);
	CHECK_SET_POSITIVE(socket, int, reconnect_interval_max);
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

#if (ZMQ_VERSION_MAJOR > 3) or ((ZMQ_VERSION_MAJOR == 3) and (ZMQ_VERSION_MINOR >= 1))
	CHECK_SET(socket, bool, ipv4_only);
#endif

#if (ZMQ_VERSION_MAJOR > 3) or ((ZMQ_VERSION_MAJOR == 3) and (ZMQ_VERSION_MINOR >= 2))
	CHECK_SET(socket, bool, delay_attach_on_connect);
	CHECK_SET(socket, bool, router_mandatory);
	CHECK_SET(socket, bool, xpub_verbose);
	// CHECK_SET(socket, int, tcp_keepalive);  --- special case of boolean but with -1?
	CHECK_SET(socket, int, tcp_keepalive_idle);
	CHECK_SET(socket, int, tcp_keepalive_count);
	CHECK_SET(socket, int, tcp_keepalive_interval);
	// CHECK_SET(socket, std::string, tcp_accept_filter); --- special case required to be an address
#endif

#ifdef ZMQ_EXPERIMENTAL_LABELS
	CHECK_NOSET(socket, receive_label);
#endif
}

BOOST_AUTO_TEST_CASE( get_socket_options )
{
	zmqpp::context context;
	zmqpp::socket socket(context, zmqpp::socket_type::subscribe);
	socket.bind("inproc://test");

	CHECK_NOGET(socket, subscribe);
	CHECK_NOGET(socket, unsubscribe);
#if (ZMQ_VERSION_MAJOR > 3) or ((ZMQ_VERSION_MAJOR == 3) and (ZMQ_VERSION_MINOR >= 2))
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

#if (ZMQ_VERSION_MAJOR > 3) or ((ZMQ_VERSION_MAJOR == 3) and (ZMQ_VERSION_MINOR >= 1))
	CHECK_GET(socket, bool, ipv4_only);
#endif

#if (ZMQ_VERSION_MAJOR > 3) or ((ZMQ_VERSION_MAJOR == 3) and (ZMQ_VERSION_MINOR >= 2))
	CHECK_GET(socket, std::string, last_endpoint);
	CHECK_GET(socket, bool, delay_attach_on_connect);
	CHECK_GET(socket, int, tcp_keepalive);
	CHECK_GET(socket, int, tcp_keepalive_idle);
	CHECK_GET(socket, int, tcp_keepalive_count);
	CHECK_GET(socket, int, tcp_keepalive_interval);
#endif

#ifdef ZMQ_EXPERIMENTAL_LABELS
	CHECK_GET(socket, bool, receive_label);
#endif
}

BOOST_AUTO_TEST_SUITE_END()
