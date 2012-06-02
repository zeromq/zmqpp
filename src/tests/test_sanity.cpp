/*
 *  Created on: 8 Aug 2011
 *      Author: @benjamg
 */

// Module definition should only be in one of the tests
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE zmqpp

#include <boost/test/unit_test.hpp>

#include <cstring>

#include <zmq.h>
#include <zmqpp/zmqpp.hpp>

const int max_poll_timeout = 100;

BOOST_AUTO_TEST_SUITE( sanity )

BOOST_AUTO_TEST_CASE( correct_zmq_version )
{
	int major, minor, patch;
	zmq_version(&major, &minor, &patch);

	BOOST_CHECK_LE( ZMQPP_REQUIRED_ZMQ_MAJOR, major );
	BOOST_CHECK_LE( ZMQPP_REQUIRED_ZMQ_MINOR, minor );
}

BOOST_AUTO_TEST_CASE( correct_zmqpp_version )
{
	uint8_t major, minor, patch;
	zmqpp::version(major, minor, patch);

	BOOST_CHECK_EQUAL( BUILD_VERSION_MAJOR, major );
	BOOST_CHECK_EQUAL( BUILD_VERSION_MINOR, minor );
	BOOST_CHECK_EQUAL( BUILD_VERSION_REVISION, patch );

	BOOST_CHECK_EQUAL( BUILD_VERSION, zmqpp::version() );
}

BOOST_AUTO_TEST_CASE( same_zmq_version_as_built_against )
{
	uint8_t major, minor, patch;
	zmqpp::zmq_version(major, minor, patch);

	BOOST_CHECK_EQUAL( ZMQ_VERSION_MAJOR, major );
	BOOST_CHECK_EQUAL( ZMQ_VERSION_MINOR, minor );
	BOOST_CHECK_EQUAL( ZMQ_VERSION_PATCH, patch );
}

BOOST_AUTO_TEST_CASE( zmq_basic_push_pull )
{
	std::string data = "hello world!";

	void* context = zmq_init(1);

	void* puller = zmq_socket(context, ZMQ_PULL);
	zmq_bind(puller, "inproc://test");

	void* pusher = zmq_socket(context, ZMQ_PUSH);
	zmq_connect(pusher, "inproc://test");

	zmq_msg_t sent_message;
	zmq_msg_init_size(&sent_message, data.size());
	memcpy(zmq_msg_data(&sent_message), data.data(), data.size());

	BOOST_CHECK_EQUAL(0, zmq_send(pusher, &sent_message, 0));

	zmq_pollitem_t items[] = { { puller, ZMQ_POLLIN, 0 } };
	BOOST_CHECK_EQUAL(0, zmq_poll(items, 1, max_poll_timeout));

	zmq_msg_t recieved_message;
	zmq_msg_init(&recieved_message);
	zmq_recv(puller, &recieved_message, 0);

	BOOST_CHECK_EQUAL(static_cast<char*>(zmq_msg_data(&sent_message)), data.data());

	zmq_msg_close(&sent_message);
	zmq_msg_close(&recieved_message);

	zmq_close(pusher);
	zmq_close(puller);
	zmq_term(context);
}

BOOST_AUTO_TEST_SUITE_END()
