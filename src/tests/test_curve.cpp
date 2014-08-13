/*
 *  Created on: 13 Aug 2014
 *      Author: @benjamg
 */

#include <array>

#include <boost/test/unit_test.hpp>

#include "zmqpp/curve.hpp"

BOOST_AUTO_TEST_SUITE( curve )

BOOST_AUTO_TEST_CASE( encode_z85 )
{
	std::array<uint8_t, 8> blob { 0x86, 0x4F, 0xD2, 0x6F, 0xB5, 0x59, 0xF7, 0x5B };

	std::string displayable;
	BOOST_CHECK( zmqpp::encode_z85( displayable, blob.data(), blob.size() ) );
	BOOST_CHECK_EQUAL( "HelloWorld" , displayable.c_str() );
}

BOOST_AUTO_TEST_CASE( decode_z85 )
{
	std::array<uint8_t, 8> blob { 0x86, 0x4F, 0xD2, 0x6F, 0xB5, 0x59, 0xF7, 0x5B };
	std::string source = "HelloWorld";

	std::array<uint8_t, 8> data;
	BOOST_CHECK( zmqpp::decode_z85( data.data(), source ) );

	for( size_t i = 0; i < blob.size(); ++i )
	{
		BOOST_CHECK_MESSAGE(blob[i] == data[i], blob[i] << " != " << data[i] << " on index " << i);
	}
}

BOOST_AUTO_TEST_SUITE_END()
