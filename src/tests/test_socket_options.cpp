/*
 *  Created on: 8 Aug 2011
 *      Author: @benjamg
 */

#include <boost/test/unit_test.hpp>

#include "zmq/context.hpp"
#include "zmq/socket.hpp"

BOOST_AUTO_TEST_SUITE( socket_options )

BOOST_AUTO_TEST_CASE( check_allowed_set_socket_options_int )
{
	zmq::context context;
	zmq::socket socket(context, zmq::socket_type::subcribe);

	// int only
	socket.set(zmq::socket_option::send_high_water_mark, 0);
	socket.set(zmq::socket_option::receive_high_water_mark, 0);
	socket.set(zmq::socket_option::rate, 100);
	socket.set(zmq::socket_option::send_buffer_size, 0);
	socket.set(zmq::socket_option::receive_buffer_size, 0);
	socket.set(zmq::socket_option::recovery_interval, 10000);
	socket.set(zmq::socket_option::linger, -1);
	socket.set(zmq::socket_option::reconnect_interval, 100);
	socket.set(zmq::socket_option::reconnect_interval_max, 0);
	socket.set(zmq::socket_option::backlog, 100);
	socket.set(zmq::socket_option::max_messsage_size, -1);
	socket.set(zmq::socket_option::multicast_hops, 1);
	socket.set(zmq::socket_option::receive_timeout, -1);
	socket.set(zmq::socket_option::send_timeout, -1);

	// uint64 - should auto convert if +ve int used
	socket.set(zmq::socket_option::affinity, 0);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::affinity, -1), zmq::exception);

	// string only
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::identity, 0), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::subscribe, 0), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::unsubscribe, 0), zmq::exception);

	// get only, should error
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::receive_more, 0), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::file_descriptor, 0), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::events, 0), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::type, 0), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::receive_label, 0), zmq::exception);
}

BOOST_AUTO_TEST_CASE( check_allowed_set_socket_options_uint64 )
{
	zmq::context context;
	zmq::socket socket(context, zmq::socket_type::subcribe);

	uint64_t value = 0;
	// int only
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::send_high_water_mark, value), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::receive_high_water_mark, value), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::rate, value), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::send_buffer_size, value), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::receive_buffer_size, value), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::recovery_interval, value), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::linger, value), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::reconnect_interval, value), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::reconnect_interval_max, value), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::backlog, value), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::max_messsage_size, value), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::multicast_hops, value), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::receive_timeout, value), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::send_timeout, value), zmq::exception);

	// uint64 - should auto convert if +ve int used
	socket.set(zmq::socket_option::affinity, value);

	// string only
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::identity, value), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::subscribe, value), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::unsubscribe, value), zmq::exception);

	// get only, should error
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::receive_more, value), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::file_descriptor, value), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::events, value), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::type, value), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::receive_label, value), zmq::exception);
}

BOOST_AUTO_TEST_CASE( check_allowed_set_socket_options_string )
{
	zmq::context context;
	zmq::socket socket(context, zmq::socket_type::subcribe);

	std::string value("test");

	// int only
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::send_high_water_mark, value), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::receive_high_water_mark, value), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::rate, value), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::send_buffer_size, value), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::receive_buffer_size, value), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::recovery_interval, value), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::linger, value), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::reconnect_interval, value), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::reconnect_interval_max, value), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::backlog, value), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::max_messsage_size, value), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::multicast_hops, value), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::receive_timeout, value), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::send_timeout, value), zmq::exception);

	// uint64 - should auto convert if +ve int used
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::affinity, value), zmq::exception);

	// string only
	socket.set(zmq::socket_option::identity, value);
	socket.set(zmq::socket_option::subscribe, value);
	socket.set(zmq::socket_option::unsubscribe, value);

	// get only, should error
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::receive_more, value), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::file_descriptor, value), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::events, value), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::type, value), zmq::exception);
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::receive_label, value), zmq::exception);
}

BOOST_AUTO_TEST_CASE( check_allowed_get_socket_options_int )
{
	zmq::context context;
	zmq::socket socket(context, zmq::socket_type::subcribe);

	int value;

	// int only
	socket.get(zmq::socket_option::file_descriptor, value);
	socket.get(zmq::socket_option::send_high_water_mark, value);
	socket.get(zmq::socket_option::receive_high_water_mark, value);
	socket.get(zmq::socket_option::rate, value);
	socket.get(zmq::socket_option::send_buffer_size, value);
	socket.get(zmq::socket_option::receive_buffer_size, value);
	socket.get(zmq::socket_option::recovery_interval, value);
	socket.get(zmq::socket_option::linger, value);
	socket.get(zmq::socket_option::reconnect_interval, value);
	socket.get(zmq::socket_option::reconnect_interval_max, value);
	socket.get(zmq::socket_option::backlog, value);
	socket.get(zmq::socket_option::max_messsage_size, value);
	socket.get(zmq::socket_option::multicast_hops, value);
	socket.get(zmq::socket_option::receive_timeout, value);
	socket.get(zmq::socket_option::send_timeout, value);
	socket.get(zmq::socket_option::events, value);
	socket.get(zmq::socket_option::type, value);

	// boolean - should auto convert if int used
	socket.get(zmq::socket_option::receive_more, value);
	socket.get(zmq::socket_option::receive_label, value);

	// uint64 only
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::affinity, value), zmq::exception);

	// string only
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::identity, value), zmq::exception);

	// set only, should error
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::subscribe, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::unsubscribe, value), zmq::exception);
}

BOOST_AUTO_TEST_CASE( check_allowed_get_socket_options_boolean )
{
	zmq::context context;
	zmq::socket socket(context, zmq::socket_type::subcribe);

	bool value;

	// int only
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::file_descriptor, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::send_high_water_mark, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::receive_high_water_mark, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::rate, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::send_buffer_size, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::receive_buffer_size, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::recovery_interval, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::linger, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::reconnect_interval, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::reconnect_interval_max, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::backlog, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::max_messsage_size, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::multicast_hops, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::receive_timeout, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::send_timeout, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::events, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::type, value), zmq::exception);

	// boolean - should auto convert if int used
	socket.get(zmq::socket_option::receive_more, value);
	socket.get(zmq::socket_option::receive_label, value);

	// uint64 only
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::affinity, value), zmq::exception);

	// string only
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::identity, value), zmq::exception);

	// set only, should error
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::subscribe, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::unsubscribe, value), zmq::exception);
}

BOOST_AUTO_TEST_CASE( check_allowed_get_socket_options_uint64 )
{
	zmq::context context;
	zmq::socket socket(context, zmq::socket_type::subcribe);

	uint64_t value;

	// int only
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::file_descriptor, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::send_high_water_mark, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::receive_high_water_mark, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::rate, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::send_buffer_size, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::receive_buffer_size, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::recovery_interval, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::linger, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::reconnect_interval, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::reconnect_interval_max, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::backlog, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::max_messsage_size, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::multicast_hops, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::receive_timeout, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::send_timeout, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::events, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::type, value), zmq::exception);

	// boolean - should auto convert if int used
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::receive_more, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::receive_label, value), zmq::exception);

	// uint64 only
	socket.get(zmq::socket_option::affinity, value);

	// string only
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::identity, value), zmq::exception);

	// set only, should error
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::subscribe, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::unsubscribe, value), zmq::exception);
}

BOOST_AUTO_TEST_CASE( check_allowed_get_socket_options_string )
{
	zmq::context context;
	zmq::socket socket(context, zmq::socket_type::subcribe);

	std::string value;

	// int only
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::file_descriptor, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::send_high_water_mark, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::receive_high_water_mark, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::rate, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::send_buffer_size, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::receive_buffer_size, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::recovery_interval, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::linger, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::reconnect_interval, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::reconnect_interval_max, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::backlog, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::max_messsage_size, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::multicast_hops, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::receive_timeout, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::send_timeout, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::events, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::type, value), zmq::exception);

	// boolean - should auto convert if int used
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::receive_more, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::receive_label, value), zmq::exception);

	// uint64 only
	BOOST_CHECK_THROW(socket.set(zmq::socket_option::affinity, value), zmq::exception);

	// string only
	socket.get(zmq::socket_option::identity, value);

	// set only, should error
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::subscribe, value), zmq::exception);
	BOOST_CHECK_THROW(socket.get(zmq::socket_option::unsubscribe, value), zmq::exception);
}

BOOST_AUTO_TEST_SUITE_END()
