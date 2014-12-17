/**
 * \file
 *
 * \date   13 Aug 2014
 * \author Ben Gray (\@benjamg)
 */

#ifndef ZMQPP_CURVE_HPP_
#define ZMQPP_CURVE_HPP_

#include <string>
#include <zmq.h>

namespace zmqpp { namespace curve
{

struct keypair
{
	std::string public_key;
	std::string secret_key;
};

#if (ZMQ_VERSION_MAJOR >= 4)
keypair generate_keypair();
#endif

} }

#endif /* ZMQPP_CURVE_HPP_ */
