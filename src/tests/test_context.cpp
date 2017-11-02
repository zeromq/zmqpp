/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file is part of zmqpp.
 * Copyright (c) 2011-2015 Contributors as noted in the AUTHORS file.
 */

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

BOOST_AUTO_TEST_CASE( defaults )
{
	zmqpp::context context;
	BOOST_CHECK_GE(context.get(zmqpp::context_option::io_threads), ZMQ_IO_THREADS_DFLT);
	BOOST_CHECK_GE(context.get(zmqpp::context_option::max_sockets), ZMQ_MAX_SOCKETS_DFLT);
}

BOOST_AUTO_TEST_CASE( validaty )
{
	zmqpp::context context;
	BOOST_CHECK(context);

	context.terminate();
	BOOST_CHECK(!context);

	BOOST_CHECK_THROW(context.get(zmqpp::context_option::io_threads), zmqpp::invalid_instance);
}

BOOST_AUTO_TEST_CASE( throws_exception )
{
#if (ZMQ_VERSION_MAJOR < 3) || ((ZMQ_VERSION_MAJOR == 3) && (ZMQ_VERSION_MINOR < 2))
	BOOST_CHECK_THROW(new zmqpp::context(-1), zmqpp::zmq_internal_exception);
#else
	zmqpp::context context;
	BOOST_CHECK_THROW(context.set(zmqpp::context_option::io_threads, -1), zmqpp::zmq_internal_exception);
#endif
}

BOOST_AUTO_TEST_SUITE_END()

