/**
 * \file
 *
 * \date   13 Aug 2014
 * \author Ben Gray (\@benjamg)
 */

#ifndef ZMQPP_CURVE_HPP_
#define ZMQPP_CURVE_HPP_

#include <string>

namespace zmqpp { namespace curve
{

struct keypair
{
	std::string public_key;
	std::string secret_key;
};

keypair generate_keypair();

}

bool decode_z85(uint8_t* dest, std::string src);

bool encode_z85(std::string& dest, uint8_t* src, size_t src_size);

}

#endif /* ZMQPP_CURVE_HPP_ */
