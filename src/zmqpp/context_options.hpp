/**
 * \file
 *
 * \date   3 Jul 2013
 * \author Ben Gray (\@benjamg)
 */

#ifndef ZMQPP_CONTEXT_OPTIONS_HPP_
#define ZMQPP_CONTEXT_OPTIONS_HPP_

namespace zmqpp
{

/** \todo Expand the information on the options to make it actually useful. */
/*!
 * \brief possible Context options in zmq
 */

ZMQPP_COMPARABLE_ENUM context_option {
	io_threads  = ZMQ_IO_THREADS,          /*!< I/O thread count */
	max_sockets = ZMQ_MAX_SOCKETS,         /*!< Maximum supported sockets */
#if (ZMQ_VERSION_MAJOR >= 4)
	ipv6        = ZMQ_IPV6,                /*!< IPv6 support */
#endif
};

}

#endif /* ZMQPP_CONTEXT_OPTIONS_HPP_ */
