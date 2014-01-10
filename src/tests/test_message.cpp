/*
 *  Created on: 9 Aug 2011
 *      Author: @benjamg
 */

#include <boost/test/unit_test.hpp>

#include <cstdlib>
#include <iostream>

#include <boost/lexical_cast.hpp>

#include "zmqpp/exception.hpp"
#include "zmqpp/message.hpp"

BOOST_AUTO_TEST_SUITE( message )

BOOST_AUTO_TEST_CASE( initialising )
{
	zmqpp::message message;

	BOOST_CHECK_EQUAL(0, message.parts());
}

BOOST_AUTO_TEST_CASE( throws_exception_reading_invalid_part )
{
	zmqpp::message message;

	BOOST_CHECK_THROW(message.get(0), zmqpp::exception);
}

BOOST_AUTO_TEST_CASE( move_supporting )
{
	zmqpp::message first;
	first.add("string");
	BOOST_CHECK_EQUAL(1, first.parts());

	zmqpp::message second( std::move(first) );
	BOOST_CHECK_EQUAL(1, second.parts());
	BOOST_CHECK_EQUAL(0, first.parts());
}

BOOST_AUTO_TEST_CASE( copyable )
{
	zmqpp::message second;

	{
		zmqpp::message first;
		first.add("string");
		BOOST_CHECK_EQUAL(1, first.parts());

		second = first.copy();
	}

	BOOST_REQUIRE_EQUAL(1, second.parts());
	BOOST_CHECK_EQUAL(strlen("string"), second.size(0));
	BOOST_CHECK_EQUAL("string", second.get(0));
}

#ifndef ZMQPP_IGNORE_LAMBDA_FUNCTION_TESTS
BOOST_AUTO_TEST_CASE( move_part )
{
	bool called = false;
	size_t data_size = 5;
	void* data = malloc(data_size);
	memset(data, 0, data_size);
	memcpy(data, "tests", strlen("tests"));

	auto release_func = [data, &called](void* val) {
		BOOST_CHECK_EQUAL(val, data);
		free(val);
		called = true;
	};

	zmqpp::message* msg = new zmqpp::message();
	msg->move(data, data_size, release_func);

	BOOST_REQUIRE_EQUAL(1, msg->parts());
	BOOST_CHECK_EQUAL(strlen("tests"), msg->size(0));
	BOOST_CHECK_EQUAL("tests", msg->get(0));

	delete msg;

	BOOST_CHECK(called);
}
#endif

BOOST_AUTO_TEST_CASE( copy_part )
{
	size_t data_size = strlen("tests");
	void* data = malloc(data_size);
	memset(data, 0, data_size);
	memcpy(data, "tests", data_size);

	zmqpp::message* msg = new zmqpp::message();
	msg->add(data, data_size);

	BOOST_REQUIRE_EQUAL(1, msg->parts());
	BOOST_CHECK_EQUAL(strlen("tests"), msg->size(0));
	BOOST_CHECK_EQUAL("tests", msg->get(0));

	delete msg;

	BOOST_CHECK_EQUAL("tests", std::string(static_cast<char*>(data), data_size));

	free(data);
}

BOOST_AUTO_TEST_CASE( copy_part_string )
{
	zmqpp::message* msg = new zmqpp::message();
	std::string part("tests");

	msg->add(part);

	BOOST_REQUIRE_EQUAL(1, msg->parts());
	BOOST_CHECK_EQUAL(strlen("tests"), msg->size(0));
	BOOST_CHECK_EQUAL("tests", msg->get(0));

	delete msg;

	BOOST_CHECK_EQUAL("tests", part);
}

BOOST_AUTO_TEST_CASE( multi_part_message )
{
	zmqpp::message message;
	message.add("this is the first part");
	message.add("some other content here");
	message.add("and finally");

	BOOST_REQUIRE_EQUAL(3, message.parts());
	BOOST_CHECK_EQUAL(strlen("this is the first part"), message.size(0));
	BOOST_CHECK_EQUAL("this is the first part", message.get(0));
	BOOST_CHECK_EQUAL(strlen("some other content here"), message.size(1));
	BOOST_CHECK_EQUAL("some other content here", message.get(1));
	BOOST_CHECK_EQUAL(strlen("and finally"), message.size(2));
	BOOST_CHECK_EQUAL("and finally", message.get(2));
}

BOOST_AUTO_TEST_CASE( stream_throws_exception )
{
	zmqpp::message message;
	std::string part;

	BOOST_CHECK_THROW(message >> part, zmqpp::exception);
}

BOOST_AUTO_TEST_CASE( stream_output_string )
{
	zmqpp::message message;
	message.add("part");

	std::string part;
	message >> part;

	BOOST_CHECK_EQUAL("part", part);
}

BOOST_AUTO_TEST_CASE( stream_copy_input_c_string )
{
	zmqpp::message message;
	message << "test part";

	BOOST_REQUIRE_EQUAL(1, message.parts());
	BOOST_CHECK_EQUAL(strlen("test part"), message.size(0));
	BOOST_CHECK_EQUAL("test part", message.get(0));
}

BOOST_AUTO_TEST_CASE( stream_copy_input_string )
{
	const std::string part("test part");
	zmqpp::message message;

	message << part;
	BOOST_CHECK_EQUAL("test part", part);

	BOOST_REQUIRE_EQUAL(1, message.parts());
	BOOST_CHECK_EQUAL(strlen("test part"), message.size(0));
	BOOST_CHECK_EQUAL("test part", message.get(0));
}

BOOST_AUTO_TEST_CASE( stream_multiple_parts )
{
	zmqpp::message message;
	message << "test part";
	message << 42;

	BOOST_REQUIRE_EQUAL(2, message.parts());

	std::string part1;
	uint32_t part2;

	message >> part1;
	message >> part2;

	BOOST_CHECK_EQUAL("test part", part1);
	BOOST_CHECK_EQUAL(42, part2);
}

BOOST_AUTO_TEST_CASE( output_stream_resetable )
{
	zmqpp::message message;
	message << "test part";

	std::string first;
	message >> first;

	BOOST_CHECK_EQUAL("test part", first);

	message.reset_read_cursor();

	std::string second;
	message >> second;

	BOOST_CHECK_EQUAL("test part", second);
}

BOOST_AUTO_TEST_CASE( many_part_queue_check )
{
	std::array<std::string, 150> parts;
	for( size_t i = 0; i < parts.size(); i += 2 )
	{
		parts[i] = "message frame " + boost::lexical_cast<std::string>( i + 1 );
		parts[i + 1] = "this part is a much longer test frame, message frame " + boost::lexical_cast<std::string>( i + 2 );
	}

	zmqpp::message message;
	for( size_t loop = 0; loop < parts.size(); ++loop )
	{
		message << parts[loop];

		for( size_t i = 0; i <= loop; ++i )
		{
			BOOST_REQUIRE_MESSAGE( parts[i].compare( message.get(i) ) == 0, "invalid frame " << i << " on loop " << loop << ": '" << message.get(i) << "' != '" << parts[i] << "'" );
		}
	}
}

BOOST_AUTO_TEST_CASE( reserve_zmq_frame )
{
	zmqpp::message message;
	zmq_msg_t& raw = message.raw_new_msg( strlen("hello world") );
	void* data = zmq_msg_data( &raw );
	memcpy( data, "hello world", strlen("hello world") );

	BOOST_REQUIRE_EQUAL( 1, message.parts() );
	BOOST_CHECK_EQUAL( "hello world", message.get(0) );
}

BOOST_AUTO_TEST_CASE( push_end_of_frame_queue )
{
	std::array<std::string, 2> parts = {
		"test frame 1",
		"a much much longer test frame 2 to go over the small message size limitation"
	};

	zmqpp::message message;
	message.push_back( parts[0] );
	message.push_back( parts[1] );

	BOOST_REQUIRE_EQUAL( parts.size(), message.parts() );
	for( size_t i = 0; i < parts.size(); ++i )
	{
		BOOST_CHECK_EQUAL( parts[i].size(), message.size(i) );
		BOOST_CHECK_EQUAL( parts[i], message.get(i) );
	}
}

BOOST_AUTO_TEST_CASE( pop_end_of_frame_queue )
{
	std::array<std::string, 3> parts = {
		"a long test frame 1 to go over the small message size limitation",
		"another frame 2",
		"some final frame 3"
	};

	zmqpp::message message;
	message << parts[0] << parts[1] << parts[2];

	BOOST_REQUIRE_EQUAL( parts.size(), message.parts() );

	message.pop_back();
	BOOST_REQUIRE_EQUAL( parts.size() - 1, message.parts() );
	for( size_t i = 0; i < parts.size() - 1; ++i )
	{
		BOOST_CHECK_EQUAL( parts[i].size(), message.size(i) );
		BOOST_CHECK_EQUAL( parts[i], message.get(i) );
	}
}

BOOST_AUTO_TEST_CASE( push_front_of_frame_queue )
{
	std::array<std::string, 3> parts = {
		"a long test frame 1 to go over the small message size limitation",
		"another frame 2",
		"some final frame 3"
	};

	zmqpp::message message;
	message << parts[1] << parts[2];

	BOOST_REQUIRE_EQUAL( parts.size() - 1, message.parts() );

	message.push_front( parts[0] );
	BOOST_REQUIRE_EQUAL( parts.size(), message.parts() );
	for( size_t i = 0; i < parts.size(); ++i )
	{
		BOOST_CHECK_EQUAL( parts[i].size(), message.size(i) );
		BOOST_CHECK_EQUAL( parts[i], message.get(i) );
	}
}

BOOST_AUTO_TEST_CASE( pop_front_of_frame_queue )
{
	std::array<std::string, 3> parts = {
		"a long test frame 1 to go over the small message size limitation",
		"another frame 2",
		"some final frame 3"
	};

	zmqpp::message message;
	message << parts[0] << parts[1] << parts[2];

	BOOST_REQUIRE_EQUAL( parts.size(), message.parts() );

	message.pop_front();
	BOOST_REQUIRE_EQUAL( parts.size() - 1, message.parts() );
	for( size_t i = 0; i < parts.size() - 1; ++i )
	{
		BOOST_CHECK_EQUAL( parts[i + 1].size(), message.size(i) );
		BOOST_CHECK_EQUAL( parts[i + 1], message.get(i) );
	}
}

#ifdef ZMQPP_NON_CONST_STREAM_OPERATORS_MOVE
BOOST_AUTO_TEST_CASE( stream_move_input_string )
{
	std::string part("test part");
	zmqpp::message message;

	message << part;
	BOOST_CHECK_EQUAL("", part);

	BOOST_REQUIRE_EQUAL(1, message.parts());
	BOOST_CHECK_EQUAL(strlen("test part"), message.size(0));
	BOOST_CHECK_EQUAL("test part", message.get(0));
}
#endif

BOOST_AUTO_TEST_SUITE_END()
