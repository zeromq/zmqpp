/*
 *  Created on: 8 Aug 2011
 *      Author: @benjamg
 */

#include <boost/test/unit_test.hpp>

#include "zmqpp/context.hpp"
#include "zmqpp/socket.hpp"

BOOST_AUTO_TEST_SUITE( socket_options )

BOOST_AUTO_TEST_CASE( check_allowed_set_socket_options_int )
{
	zmqpp::context context;
	zmqpp::socket socket(context, zmqpp::socket_type::sub);

	// int only
	socket.set(zmqpp::socket_option::send_high_water_mark, 0);
	socket.set(zmqpp::socket_option::receive_high_water_mark, 0);
	socket.set(zmqpp::socket_option::rate, 100);
	socket.set(zmqpp::socket_option::send_buffer_size, 0);
	socket.set(zmqpp::socket_option::receive_buffer_size, 0);
	socket.set(zmqpp::socket_option::recovery_interval, 10000);
	socket.set(zmqpp::socket_option::linger, -1);
	socket.set(zmqpp::socket_option::reconnect_interval, 100);
	socket.set(zmqpp::socket_option::reconnect_interval_max, 0);
	socket.set(zmqpp::socket_option::backlog, 100);
	socket.set(zmqpp::socket_option::max_messsage_size, -1);
	socket.set(zmqpp::socket_option::multicast_hops, 1);
	socket.set(zmqpp::socket_option::receive_timeout, -1);
	socket.set(zmqpp::socket_option::send_timeout, -1);

	// boolean - should auto convert if int used
#if (ZMQ_VERSION_MAJOR > 3) or ((ZMQ_VERSION_MAJOR == 3) and (ZMQ_VERSION_MINOR >= 1))
	socket.set(zmqpp::socket_option::ipv4_only, 1);
#endif

	// uint64 - should auto convert if +ve int used
	socket.set(zmqpp::socket_option::affinity, 0);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::affinity, -1), zmqpp::exception);

	// string only
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::identity, 0), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::subscribe, 0), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::unsubscribe, 0), zmqpp::exception);

	// get only, should error
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::receive_more, 0), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::file_descriptor, 0), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::events, 0), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::type, 0), zmqpp::exception);
#ifdef ZMQ_EXPERIMENTAL_LABELS
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::receive_label, 0), zmqpp::exception);
#endif
}

#if (ZMQ_VERSION_MAJOR > 3) or ((ZMQ_VERSION_MAJOR == 3) and (ZMQ_VERSION_MINOR >= 1))
BOOST_AUTO_TEST_CASE( check_allowed_set_socket_options_boolean )
{
	zmqpp::context context;
	zmqpp::socket socket(context, zmqpp::socket_type::sub);

	bool value = true;

	// int only
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::send_high_water_mark, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::receive_high_water_mark, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::rate, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::send_buffer_size, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::receive_buffer_size, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::recovery_interval, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::linger, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::reconnect_interval, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::reconnect_interval_max, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::backlog, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::max_messsage_size, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::multicast_hops, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::receive_timeout, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::send_timeout, value), zmqpp::exception);

	// boolean - should auto convert if int used
	socket.set(zmqpp::socket_option::ipv4_only, value);

	// uint64 - should auto convert if +ve int used
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::affinity, value), zmqpp::exception);

	// string only
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::identity, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::subscribe, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::unsubscribe, value), zmqpp::exception);

	// get only, should error
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::receive_more, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::file_descriptor, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::events, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::type, value), zmqpp::exception);
#ifdef ZMQ_EXPERIMENTAL_LABELS
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::receive_label, value), zmqpp::exception);
#endif
}
#endif

BOOST_AUTO_TEST_CASE( check_allowed_set_socket_options_uint64 )
{
	zmqpp::context context;
	zmqpp::socket socket(context, zmqpp::socket_type::sub);

	uint64_t value = 0;

	// int only
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::send_high_water_mark, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::receive_high_water_mark, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::rate, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::send_buffer_size, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::receive_buffer_size, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::recovery_interval, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::linger, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::reconnect_interval, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::reconnect_interval_max, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::backlog, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::max_messsage_size, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::multicast_hops, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::receive_timeout, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::send_timeout, value), zmqpp::exception);

	// boolean - should auto convert if int used
#if (ZMQ_VERSION_MAJOR > 3) or ((ZMQ_VERSION_MAJOR == 3) and (ZMQ_VERSION_MINOR >= 1))
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::ipv4_only, value), zmqpp::exception);
#endif

	// uint64 - should auto convert if +ve int used
	socket.set(zmqpp::socket_option::affinity, value);

	// string only
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::identity, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::subscribe, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::unsubscribe, value), zmqpp::exception);

	// get only, should error
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::receive_more, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::file_descriptor, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::events, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::type, value), zmqpp::exception);
#ifdef ZMQ_EXPERIMENTAL_LABELS
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::receive_label, value), zmqpp::exception);
#endif
}

BOOST_AUTO_TEST_CASE( check_allowed_set_socket_options_string )
{
	zmqpp::context context;
	zmqpp::socket socket(context, zmqpp::socket_type::sub);

	std::string value("test");

	// int only
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::send_high_water_mark, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::receive_high_water_mark, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::rate, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::send_buffer_size, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::receive_buffer_size, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::recovery_interval, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::linger, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::reconnect_interval, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::reconnect_interval_max, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::backlog, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::max_messsage_size, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::multicast_hops, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::receive_timeout, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::send_timeout, value), zmqpp::exception);

	// boolean - should auto convert if int used
#if (ZMQ_VERSION_MAJOR > 3) or ((ZMQ_VERSION_MAJOR == 3) and (ZMQ_VERSION_MINOR >= 1))
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::ipv4_only, value), zmqpp::exception);
#endif

	// uint64 - should auto convert if +ve int used
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::affinity, value), zmqpp::exception);

	// string only
	socket.set(zmqpp::socket_option::identity, value);
	socket.set(zmqpp::socket_option::subscribe, value);
	socket.set(zmqpp::socket_option::unsubscribe, value);

	// get only, should error
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::receive_more, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::file_descriptor, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::events, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::type, value), zmqpp::exception);
#ifdef ZMQ_EXPERIMENTAL_LABELS
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::receive_label, value), zmqpp::exception);
#endif
}

BOOST_AUTO_TEST_CASE( check_allowed_get_socket_options_int )
{
	zmqpp::context context;
	zmqpp::socket socket(context, zmqpp::socket_type::sub);

	int value;

	// int only
	socket.get(zmqpp::socket_option::file_descriptor, value);
	socket.get(zmqpp::socket_option::send_high_water_mark, value);
	socket.get(zmqpp::socket_option::receive_high_water_mark, value);
	socket.get(zmqpp::socket_option::rate, value);
	socket.get(zmqpp::socket_option::send_buffer_size, value);
	socket.get(zmqpp::socket_option::receive_buffer_size, value);
	socket.get(zmqpp::socket_option::recovery_interval, value);
	socket.get(zmqpp::socket_option::linger, value);
	socket.get(zmqpp::socket_option::reconnect_interval, value);
	socket.get(zmqpp::socket_option::reconnect_interval_max, value);
	socket.get(zmqpp::socket_option::backlog, value);
	socket.get(zmqpp::socket_option::max_messsage_size, value);
	socket.get(zmqpp::socket_option::multicast_hops, value);
	socket.get(zmqpp::socket_option::receive_timeout, value);
	socket.get(zmqpp::socket_option::send_timeout, value);
	socket.get(zmqpp::socket_option::events, value);
	socket.get(zmqpp::socket_option::type, value);

	// boolean - should auto convert if int used
	socket.get(zmqpp::socket_option::receive_more, value);
#if (ZMQ_VERSION_MAJOR > 3) or ((ZMQ_VERSION_MAJOR == 3) and (ZMQ_VERSION_MINOR >= 1))
	socket.get(zmqpp::socket_option::ipv4_only, value);
#endif
#ifdef ZMQ_EXPERIMENTAL_LABELS
	socket.get(zmqpp::socket_option::receive_label, value);
#endif

	// uint64 only
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::affinity, value), zmqpp::exception);

	// string only
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::identity, value), zmqpp::exception);

	// set only, should error
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::subscribe, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::unsubscribe, value), zmqpp::exception);
}

BOOST_AUTO_TEST_CASE( check_allowed_get_socket_options_boolean )
{
	zmqpp::context context;
	zmqpp::socket socket(context, zmqpp::socket_type::sub);

	bool value;

	// int only
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::file_descriptor, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::send_high_water_mark, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::receive_high_water_mark, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::rate, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::send_buffer_size, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::receive_buffer_size, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::recovery_interval, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::linger, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::reconnect_interval, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::reconnect_interval_max, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::backlog, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::max_messsage_size, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::multicast_hops, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::receive_timeout, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::send_timeout, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::events, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::type, value), zmqpp::exception);

	// boolean - should auto convert if int used
	socket.get(zmqpp::socket_option::receive_more, value);
#if (ZMQ_VERSION_MAJOR > 3) or ((ZMQ_VERSION_MAJOR == 3) and (ZMQ_VERSION_MINOR >= 1))
	socket.get(zmqpp::socket_option::ipv4_only, value);
#endif
#ifdef ZMQ_EXPERIMENTAL_LABELS
	socket.get(zmqpp::socket_option::receive_label, value);
#endif

	// uint64 only
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::affinity, value), zmqpp::exception);

	// string only
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::identity, value), zmqpp::exception);

	// set only, should error
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::subscribe, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::unsubscribe, value), zmqpp::exception);
}

BOOST_AUTO_TEST_CASE( check_allowed_get_socket_options_uint64 )
{
	zmqpp::context context;
	zmqpp::socket socket(context, zmqpp::socket_type::sub);

	uint64_t value;

	// int only
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::file_descriptor, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::send_high_water_mark, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::receive_high_water_mark, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::rate, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::send_buffer_size, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::receive_buffer_size, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::recovery_interval, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::linger, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::reconnect_interval, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::reconnect_interval_max, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::backlog, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::max_messsage_size, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::multicast_hops, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::receive_timeout, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::send_timeout, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::events, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::type, value), zmqpp::exception);

	// boolean - should auto convert if int used
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::receive_more, value), zmqpp::exception);
#if ZMQ_VERSION_MAJOR > 3 or (ZMQ_VERSION_MAJOR == 3) and (ZMQ_VERSION_MINOR >= 1)
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::ipv4_only, value), zmqpp::exception);
#endif
#ifdef ZMQ_EXPERIMENTAL_LABELS
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::receive_label, value), zmqpp::exception);
#endif

	// uint64 only
	socket.get(zmqpp::socket_option::affinity, value);

	// string only
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::identity, value), zmqpp::exception);

	// set only, should error
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::subscribe, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::unsubscribe, value), zmqpp::exception);
}

BOOST_AUTO_TEST_CASE( check_allowed_get_socket_options_string )
{
	zmqpp::context context;
	zmqpp::socket socket(context, zmqpp::socket_type::sub);

	std::string value;

	// int only
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::file_descriptor, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::send_high_water_mark, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::receive_high_water_mark, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::rate, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::send_buffer_size, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::receive_buffer_size, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::recovery_interval, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::linger, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::reconnect_interval, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::reconnect_interval_max, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::backlog, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::max_messsage_size, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::multicast_hops, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::receive_timeout, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::send_timeout, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::events, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::type, value), zmqpp::exception);

	// boolean - should auto convert if int used
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::receive_more, value), zmqpp::exception);
#if (ZMQ_VERSION_MAJOR > 3) or ((ZMQ_VERSION_MAJOR == 3) and (ZMQ_VERSION_MINOR >= 1))
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::ipv4_only, value), zmqpp::exception);
#endif
#ifdef ZMQ_EXPERIMENTAL_LABELS
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::receive_label, value), zmqpp::exception);
#endif

	// uint64 only
	BOOST_CHECK_THROW(socket.set(zmqpp::socket_option::affinity, value), zmqpp::exception);

	// string only
	socket.get(zmqpp::socket_option::identity, value);

	// set only, should error
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::subscribe, value), zmqpp::exception);
	BOOST_CHECK_THROW(socket.get(zmqpp::socket_option::unsubscribe, value), zmqpp::exception);
}

BOOST_AUTO_TEST_SUITE_END()
