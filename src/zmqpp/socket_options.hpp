/**
 * \file
 *
 * \date   23 Sep 2011
 * \author Ben Gray (\@benjamg)
 */

#ifndef ZMQPP_SOCKET_OPTIONS_HPP_
#define ZMQPP_SOCKET_OPTIONS_HPP_

namespace zmqpp
{

/** \todo Expand the information on the options to make it actually useful. */
/*!
 * \brief possible Socket options in zmq
 */
enum class socket_option {
	affinity                = ZMQ_AFFINITY,          /*!< I/O thread affinity */
	identity                = ZMQ_IDENTITY,          /*!< Socket identity */
	subscribe               = ZMQ_SUBSCRIBE,         /*!< Add topic subscription - set only */
	unsubscribe             = ZMQ_UNSUBSCRIBE,       /*!< Remove topic subscription - set only */
	rate                    = ZMQ_RATE,              /*!< Multicast data rate */
	send_buffer_size        = ZMQ_SNDBUF,            /*!< Kernel transmission buffer size */
	receive_buffer_size     = ZMQ_RCVBUF,            /*!< Kernel receive buffer size */
    receive_more            = ZMQ_RCVMORE,           /*!< Can receive more parts - get only */
    file_descriptor         = ZMQ_FD,                /*!< Socket file descriptor - get only */
    events                  = ZMQ_EVENTS,            /*!< Socket event flags - get only */
    type                    = ZMQ_TYPE,              /*!< Socket type - get only */
    linger                  = ZMQ_LINGER,            /*!< Socket linger timeout */
    backlog                 = ZMQ_BACKLOG,           /*!< Maximum length of outstanding connections - get only */
	recovery_interval       = ZMQ_RECOVERY_IVL,      /*!< Multicast recovery interval */
	reconnect_interval      = ZMQ_RECONNECT_IVL,     /*!< Reconnection interval */
	reconnect_interval_max  = ZMQ_RECONNECT_IVL_MAX, /*!< Maximum reconnection interval */
    max_messsage_size       = ZMQ_MAXMSGSIZE,        /*!< Maximum inbound message size */
    send_high_water_mark    = ZMQ_SNDHWM,            /*!< High-water mark for outbound messages */
    receive_high_water_mark = ZMQ_RCVHWM,            /*!< High-water mark for inbound messages */
    multicast_hops          = ZMQ_MULTICAST_HOPS,    /*!< Maximum number of multicast hops */
    receive_timeout         = ZMQ_RCVTIMEO,          /*!< Maximum inbound block timeout */
    send_timeout            = ZMQ_SNDTIMEO,          /*!< Maximum outbound block timeout */
    receive_label           = ZMQ_RCVLABEL           /*!< Received label part - get only */
};

}

#endif /* ZMQPP_SOCKET_OPTIONS_HPP_ */
