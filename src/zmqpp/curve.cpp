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

}
