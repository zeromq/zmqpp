/*
 *  Created on: 8 Aug 2011
 *      Author: @benjamg
 */

// Module definition should only be in one of the tests
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE zmqpp

#include <boost/test/unit_test.hpp>
#include <zmq.h>
#include <zmqpp/zmqpp.hpp>

BOOST_AUTO_TEST_SUITE( sanity )

BOOST_AUTO_TEST_CASE( correct_zmq_version )
{
	int major, minor, patch;
	zmq_version(&major, &minor, &patch);

	BOOST_CHECK_EQUAL( ZMQPP_REQUIRED_ZMQ_MAJOR, major );
	BOOST_CHECK_EQUAL( ZMQPP_REQUIRED_ZMQ_MINOR, minor );
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

BOOST_AUTO_TEST_SUITE_END()
