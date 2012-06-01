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
#define CHECK_SET(socket, type, option) check_set<type>(socket, zmqpp::socket_option::option, STRINGIFY(option))
#define CHECK_GET(socket, type, option) check_get<type>(socket, zmqpp::socket_option::option, STRINGIFY(option))

// Note the hacky abuse of the fact we don't have float options
#define CHECK_NOSET(socket, option) check_set<float>(socket, zmqpp::socket_option::option, STRINGIFY(option))
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
	catch(zmqpp::exception const& e)
	{
		BOOST_CHECK_MESSAGE(typeid(CheckType) != typeid(WantedType), "threw unexpected exception setting option " << option_name << " against type '" << value_type << "'");
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
	catch(zmqpp::exception const& e)
	{
		BOOST_CHECK_MESSAGE(typeid(CheckType) != typeid(WantedType), "threw unexpected exception getting option " << option_name << " against type '" << value_type << "'");
	}
}

template<typename Type>
void check_set(zmqpp::socket& socket, zmqpp::socket_option const& option, std::string const& option_name)
{
	// Boolean
	try_set<bool, Type>(socket, option, true, option_name, "boolean (true)");
	try_set<bool, Type>(socket, option, false, option_name, "boolean (false)");

	// Integer - Masquerade of unsigned 64bit integer
	if (typeid(uint64_t) == typeid(Type))
	{
		// Positive integers are valid as unsigned 64bit
		try_set<int, Type>(socket, option, -1, option_name, "signed integer (negative)");
		try_set<int, int>(socket, option, 1, option_name, "signed_integer (positive / masquerade)");
	}
	// Integer - Masquerade of boolean
	else if (typeid(bool) == typeid(Type))
	{
		// Integers are valid as boolean
		try_set<int, int>(socket, option, -1, option_name, "signed integer (negative / masquerade)");
		try_set<int, int>(socket, option, 1, option_name, "signed integer (positive / masquerade)");
	}
	// Integer - Others
	else
	{
		try_set<int, Type>(socket, option, -1, option_name, "signed integer (negative)");
		try_set<int, Type>(socket, option, 1, option_name, "signed integer (positive)");
	}

	// Unsigned 64bit integer
	try_set<uint64_t, Type>(socket, option, 1, option_name, "unsigned 64bit integer");

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
		try_get<int, int>(socket, option, option_name, "signed integer");
	}
	// Integer - Others
	else
	{
		try_get<int, Type>(socket, option, option_name, "signed integer");
	}

	// Unsigned 64bit integer
	try_get<uint64_t, Type>(socket, option, option_name, "unsigned 64bit integer");

	// Strings
	try_get<std::string, Type>(socket, option, option_name, "string");
}

BOOST_AUTO_TEST_CASE( set_socket_options )
{
	zmqpp::context context;
	zmqpp::socket socket(context, zmqpp::socket_type::sub);

	CHECK_SET(socket, uint64_t, affinity);
	CHECK_SET(socket, std::string, identity);
	CHECK_SET(socket, std::string, subscribe);
	CHECK_SET(socket, std::string, unsubscribe);
	CHECK_SET(socket, int, rate);
	CHECK_SET(socket, int, send_buffer_size);
	CHECK_SET(socket, int, receive_buffer_size);
	CHECK_NOSET(socket, receive_more);
	CHECK_NOSET(socket, file_descriptor);
	CHECK_NOSET(socket, events);
	CHECK_NOSET(socket, type);
	CHECK_SET(socket, int, linger);
	CHECK_SET(socket, int, backlog);
	CHECK_SET(socket, int, recovery_interval);
#if (ZMQ_VERSION_MAJOR == 2)
	CHECK_SET(socket, int, recovery_interval_seconds);
#endif
	CHECK_SET(socket, int, reconnect_interval);
	CHECK_SET(socket, int, reconnect_interval_max);
	CHECK_SET(socket, int, receive_timeout);
	CHECK_SET(socket, int, send_timeout);
#if (ZMQ_VERSION_MAJOR == 2)
	CHECK_SET(socket, uint64_t, high_water_mark);
	CHECK_SET(socket, int, swap_size);
	CHECK_SET(socket, bool, multicast_loopback);
#else
	CHECK_SET(socket, int, max_messsage_size);
	CHECK_SET(socket, int, send_high_water_mark);
	CHECK_SET(socket, int, receive_high_water_mark);
	CHECK_SET(socket, int, multicast_hops);
#endif
#if (ZMQ_VERSION_MAJOR > 3) or ((ZMQ_VERSION_MAJOR == 3) and (ZMQ_VERSION_MINOR >= 1))
	CHECK_SET(socket, bool, ipv4_only);
#endif
#ifdef ZMQ_EXPERIMENTAL_LABELS
	CHECK_NOSET(socket, receive_label);
#endif
}

BOOST_AUTO_TEST_CASE( get_socket_options )
{
	zmqpp::context context;
	zmqpp::socket socket(context, zmqpp::socket_type::sub);

	CHECK_GET(socket, uint64_t, affinity);
	CHECK_GET(socket, std::string, identity);
	CHECK_NOGET(socket, subscribe);
	CHECK_NOGET(socket, unsubscribe);
	CHECK_GET(socket, int, rate);
	CHECK_GET(socket, int, send_buffer_size);
	CHECK_GET(socket, int, receive_buffer_size);
	CHECK_GET(socket, bool, receive_more);
	CHECK_GET(socket, int, file_descriptor);
	CHECK_GET(socket, int, events);
	CHECK_GET(socket, int, type);
	CHECK_GET(socket, int, linger);
	CHECK_GET(socket, int, backlog);
	CHECK_GET(socket, int, recovery_interval);
#if (ZMQ_VERSION_MAJOR == 2)
	CHECK_GET(socket, int, recovery_interval_seconds);
#endif
	CHECK_GET(socket, int, reconnect_interval);
	CHECK_GET(socket, int, reconnect_interval_max);
	CHECK_GET(socket, int, receive_timeout);
	CHECK_GET(socket, int, send_timeout);
#if (ZMQ_VERSION_MAJOR == 2)
	CHECK_GET(socket, uint64_t, high_water_mark);
	CHECK_GET(socket, int, swap_size);
	CHECK_GET(socket, bool, multicast_loopback);
#else
	CHECK_GET(socket, int, max_messsage_size);
	CHECK_GET(socket, int, send_high_water_mark);
	CHECK_GET(socket, int, receive_high_water_mark);
	CHECK_GET(socket, int, multicast_hops);
#endif
#if (ZMQ_VERSION_MAJOR > 3) or ((ZMQ_VERSION_MAJOR == 3) and (ZMQ_VERSION_MINOR >= 1))
	CHECK_GET(socket, bool, ipv4_only);
#endif
#ifdef ZMQ_EXPERIMENTAL_LABELS
	CHECK_GET(socket, bool, receive_label);
#endif
}

BOOST_AUTO_TEST_SUITE_END()
