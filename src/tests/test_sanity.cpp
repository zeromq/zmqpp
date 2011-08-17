/*
 *  Created on: 8 Aug 2011
 *      Author: @benjamg
 */

// Module definition should only be in one of the tests
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE zmqpp

#include <boost/test/unit_test.hpp>
#include <zmq.h>

BOOST_AUTO_TEST_SUITE( sanity )

BOOST_AUTO_TEST_CASE( correct_zmq_version )
{
	int major, minor, patch;
	zmq_version(&major, &minor, &patch);

	BOOST_CHECK_EQUAL( 3, major );
	BOOST_CHECK_EQUAL( 0, minor );
}

BOOST_AUTO_TEST_SUITE_END()
