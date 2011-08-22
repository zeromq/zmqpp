/*
 *  Created on: 9 Aug 2011
 *      Author: Ben Gray (@benjamg)
 */

#ifndef ZMQPP_EXCEPTION_HPP_
#define ZMQPP_EXCEPTION_HPP_

#include <stdexcept>

#include <zmq.h>

namespace zmqpp
{

class exception : public std::runtime_error
{
public:
	exception(std::string const& message)
		: std::runtime_error(message)
	{

	}
};

class zmq_internal_exception : public exception
{
public:
	zmq_internal_exception()
		: exception(zmq_strerror(zmq_errno()))
		, _error(zmq_errno())
	{

	}

	int zmq_error() const
	{
		return _error;
	}

private:
	int _error;
};

}

#endif /* ZMQPP_EXCEPTION_HPP_ */
