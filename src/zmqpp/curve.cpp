/*
 *  Created on: 13 Aug 2014
 *      Author: Ben Gray (@benjamg)
 */

#include <vector>

#include <zmq_utils.h>

#include "compatibility.hpp"

#include "curve.hpp"
#include "exception.hpp"

namespace zmqpp { namespace curve {

keypair generate_keypair()
{
	char public_key [41];
	char secret_key [41];

	int result = zmq_curve_keypair( public_key, secret_key );
	if (0 != result)
	{
		throw zmq_internal_exception();
	}

	return keypair{ public_key, secret_key };
}

} // end curve

// I have left these outside of the the curve namespace as they feel generic and not curve specific even if thats all that uses them right now

bool decode_z85(uint8_t* dest, std::string src)
{
	// const_cast here is something I don't like however decode should not modify source string
	return ( nullptr != zmq_z85_decode( dest, const_cast<char*>(src.c_str()) ) );
}

bool encode_z85(std::string& dest, uint8_t* src, size_t src_size)
{
	std::vector<char> buffer( (src_size * 1.25) + 1 );
	if (nullptr == zmq_z85_encode( buffer.data(), src, src_size ))
	{
		return false;
	}

	dest.assign( buffer.begin(), buffer.end() );
	return true;
}

}
