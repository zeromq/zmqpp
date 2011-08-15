/*
 *  Created on: 9 Aug 2011
 *      Author: @benjamg
 */

#include <boost/test/unit_test.hpp>

#include <cstdlib>

#include "zmq/exception.hpp"
#include "zmq/message.hpp"

BOOST_AUTO_TEST_SUITE( message )

BOOST_AUTO_TEST_CASE( initialising )
{
	zmq::message message;

	BOOST_CHECK_EQUAL(0, message.parts());
}

BOOST_AUTO_TEST_CASE( throws_exception_reading_invalid_part )
{
	zmq::message message;

	BOOST_CHECK_THROW(message.get(0), zmq::exception);
}

BOOST_AUTO_TEST_CASE( move_supporting )
{
	zmq::message first;
	first.copy_part("string");
	BOOST_CHECK_EQUAL(1, first.parts());

	zmq::message second( std::move(first) );
	BOOST_CHECK_EQUAL(1, second.parts());
	BOOST_CHECK_EQUAL(0, first.parts());
}

BOOST_AUTO_TEST_CASE( copyable )
{
	zmq::message second;

	{
		zmq::message first;
		first.copy_part("string");
		BOOST_CHECK_EQUAL(1, first.parts());

		second = first.copy();
	}

	BOOST_REQUIRE_EQUAL(1, second.parts());
	BOOST_CHECK_EQUAL(strlen("string"), second.size(0));
	BOOST_CHECK_EQUAL("string", second.get(0));
}

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

	zmq::message* msg = new zmq::message();
	msg->move_part(data, data_size, release_func);

	BOOST_REQUIRE_EQUAL(1, msg->parts());
	BOOST_CHECK_EQUAL(strlen("tests"), msg->size(0));
	BOOST_CHECK_EQUAL("tests", msg->get(0));

	delete msg;

	BOOST_CHECK(called);
}

BOOST_AUTO_TEST_CASE( move_part_string )
{
	zmq::message* msg = new zmq::message();
	std::string part("tests");

	msg->move_part(part);
	BOOST_CHECK_EQUAL("", part);

	BOOST_REQUIRE_EQUAL(1, msg->parts());
	BOOST_CHECK_EQUAL(strlen("tests"), msg->size(0));
	BOOST_CHECK_EQUAL("tests", msg->get(0));

	delete msg;
}

BOOST_AUTO_TEST_CASE( copy_part )
{
	size_t data_size = strlen("tests");
	void* data = malloc(data_size);
	memset(data, 0, data_size);
	memcpy(data, "tests", data_size);

	zmq::message* msg = new zmq::message();
	msg->copy_part(data, data_size);

	BOOST_REQUIRE_EQUAL(1, msg->parts());
	BOOST_CHECK_EQUAL(strlen("tests"), msg->size(0));
	BOOST_CHECK_EQUAL("tests", msg->get(0));

	delete msg;

	BOOST_CHECK_EQUAL("tests", std::string(static_cast<char*>(data), data_size));

	free(data);
}

BOOST_AUTO_TEST_CASE( copy_part_string )
{
	zmq::message* msg = new zmq::message();
	std::string part("tests");

	msg->copy_part(part);

	BOOST_REQUIRE_EQUAL(1, msg->parts());
	BOOST_CHECK_EQUAL(strlen("tests"), msg->size(0));
	BOOST_CHECK_EQUAL("tests", msg->get(0));

	delete msg;

	BOOST_CHECK_EQUAL("tests", part);
}

BOOST_AUTO_TEST_CASE( multi_part_message )
{
	zmq::message message;
	message.copy_part("this is the first part");
	message.copy_part("some other content here");
	message.copy_part("and finally");

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
	zmq::message message;
	std::string part;

	BOOST_CHECK_THROW(message >> part, zmq::exception);
}

BOOST_AUTO_TEST_CASE( stream_output_string )
{
	zmq::message message;
	message.copy_part("part");

	std::string part;
	message >> part;

	BOOST_CHECK_EQUAL("part", part);
}

BOOST_AUTO_TEST_CASE( stream_copy_input_c_string )
{
	zmq::message message;
	message << "test part";

	BOOST_REQUIRE_EQUAL(1, message.parts());
	BOOST_CHECK_EQUAL(strlen("test part"), message.size(0));
	BOOST_CHECK_EQUAL("test part", message.get(0));
}

BOOST_AUTO_TEST_CASE( stream_copy_input_string )
{
	const std::string part("test part");
	zmq::message message;

	message << part;
	BOOST_CHECK_EQUAL("test part", part);

	BOOST_REQUIRE_EQUAL(1, message.parts());
	BOOST_CHECK_EQUAL(strlen("test part"), message.size(0));
	BOOST_CHECK_EQUAL("test part", message.get(0));
}

BOOST_AUTO_TEST_CASE( stream_multiple_parts )
{
	zmq::message message;
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
	zmq::message message;
	message << "test part";

	std::string first;
	message >> first;

	BOOST_CHECK_EQUAL("test part", first);

	message.reset_read_cursor();

	std::string second;
	message >> second;

	BOOST_CHECK_EQUAL("test part", second);
}

#ifdef ZMQ_NON_CONST_STREAM_OPERATORS_MOVE
BOOST_AUTO_TEST_CASE( stream_move_input_string )
{
	std::string part("test part");
	zmq::message message;

	message << part;
	BOOST_CHECK_EQUAL("", part);

	BOOST_REQUIRE_EQUAL(1, message.parts());
	BOOST_CHECK_EQUAL(strlen("test part"), message.size(0));
	BOOST_CHECK_EQUAL("test part", message.get(0));
}
#endif

BOOST_AUTO_TEST_SUITE_END()
