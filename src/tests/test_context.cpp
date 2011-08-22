/*
 *  Created on: 9 Aug 2011
 *      Author: @benjamg
 */

#include <boost/test/unit_test.hpp>

#include "zmqpp/context.hpp"

BOOST_AUTO_TEST_SUITE( context )

BOOST_AUTO_TEST_CASE( initialises )
{
	zmqpp::context context;
	BOOST_CHECK(context);
}

BOOST_AUTO_TEST_CASE( move_supporting )
{
	zmqpp::context first;
	BOOST_CHECK(first);

	zmqpp::context second( std::move(first) );
	BOOST_CHECK(second);
	BOOST_CHECK(!first);
}

BOOST_AUTO_TEST_CASE( throws_exception )
{
	BOOST_CHECK_THROW(new zmqpp::context(-1), zmqpp::zmq_internal_exception);
}

BOOST_AUTO_TEST_SUITE_END()

