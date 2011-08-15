/*
 *  Created on: 9 Aug 2011
 *      Author: @benjamg
 */

#include <boost/test/unit_test.hpp>

#include "zmq/context.hpp"

BOOST_AUTO_TEST_SUITE( context )

BOOST_AUTO_TEST_CASE( initialises )
{
	zmq::context context;
	BOOST_CHECK(context);
}

BOOST_AUTO_TEST_CASE( move_supporting )
{
	zmq::context first;
	BOOST_CHECK(first);

	zmq::context second( std::move(first) );
	BOOST_CHECK(second);
	BOOST_CHECK(!first);
}

BOOST_AUTO_TEST_CASE( throws_exception )
{
	BOOST_CHECK_THROW(new zmq::context(-1), zmq::zmq_internal_exception);
}

BOOST_AUTO_TEST_SUITE_END()

