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

} }

#endif /* ZMQPP_CURVE_HPP_ */
