/*
 *  Created on: 9 Aug 2011
 *      Author: Ben Gray (@benjamg)
 */

#ifndef ZMQPP_SOCKET_HPP_
#define ZMQPP_SOCKET_HPP_

#include <string>
#include <list>

#include <zmq.h>

namespace zmq
{

class context;
class message;

enum socket_type {
	pair       = ZMQ_PAIR,
	pub        = ZMQ_PUB,
	sub        = ZMQ_SUB,
	req        = ZMQ_REQ,
	rep        = ZMQ_REP,
	pull       = ZMQ_PULL,
	push       = ZMQ_PUSH,
	xpub       = ZMQ_XPUB,
	xsub       = ZMQ_XSUB,
	xreq       = ZMQ_XREQ,
	xrep       = ZMQ_XREP,
	router     = ZMQ_ROUTER,
	dealer     = ZMQ_DEALER
};

enum socket_option {
	affinity                = ZMQ_AFFINITY,
	identity                = ZMQ_IDENTITY,
	subscribe               = ZMQ_SUBSCRIBE,
	unsubscribe             = ZMQ_UNSUBSCRIBE,
	rate                    = ZMQ_RATE,
	send_buffer_size        = ZMQ_SNDBUF,
	receive_buffer_size     = ZMQ_RCVBUF,
    receive_more            = ZMQ_RCVMORE,
    file_descriptor         = ZMQ_FD,
    events                  = ZMQ_EVENTS,
    type                    = ZMQ_TYPE,
    linger                  = ZMQ_LINGER,
    backlog                 = ZMQ_BACKLOG,
	recovery_interval       = ZMQ_RECOVERY_IVL,
	reconnect_interval      = ZMQ_RECONNECT_IVL,
	reconnect_interval_max  = ZMQ_RECONNECT_IVL_MAX,
    max_messsage_size       = ZMQ_MAXMSGSIZE,
    send_high_water_mark    = ZMQ_SNDHWM,
    receive_high_water_mark = ZMQ_RCVHWM,
    multicast_hops          = ZMQ_MULTICAST_HOPS,
    receive_timeout         = ZMQ_RCVTIMEO,
    send_timeout            = ZMQ_SNDTIMEO,
    receive_label           = ZMQ_RCVLABEL
};

class socket
{
public:
	static const int NORMAL     = 0;
	static const int DONT_WAIT  = ZMQ_DONTWAIT;
	static const int SEND_MORE  = ZMQ_SNDMORE;
	static const int SEND_LABEL = ZMQ_SNDLABEL;

	socket(context const& context, socket_type const& type);
	~socket();

	void bind(std::string const& endpoint);
	void connect(std::string const& endpoint);

	// Upon sending a message zmq takes ownership of it and your left with a blank one.
	bool send(message& message, bool const& dont_block = false);
	bool receive(message& message, bool const& dont_block = false);

	/**
	 * returns true if sent, false if would block and ZMQ_DONTWAIT was used
	 */
	bool send(std::string const& string, int const& flags = NORMAL);

	/**
	 * returns true if recieved, false if would block and ZMQ_DONTWAIT was used
	 */
	bool receive(std::string& string, int const& flags = NORMAL);

	bool send_raw(char const* buffer, int const& length, int const& flags = NORMAL);
	bool receive_raw(char* buffer, int& length, int const& flags = NORMAL);

	// Helper
	void subscribe(std::string const& topic);
	void unsubscribe(std::string const& topic);
	bool has_more_parts();

	// Set socket options for different types of option
	void set(socket_option const& option, int const& value);
	void set(socket_option const& option, uint64_t const& value);
	void set(socket_option const& option, std::string const& value);

	// Get socket options, multiple versions for easy of use
	void get(socket_option const& option, int& value) const;
	void get(socket_option const& option, bool& value) const;
	void get(socket_option const& option, uint64_t& value) const;
	void get(socket_option const& option, std::string& value) const;

	template<typename Type>
	Type get(socket_option const& option) const
	{
		Type value;
		get(option, value);
		return value;
	}

	// Move supporting
	socket(socket&& source);
	void operator=(socket&& source);

	// Validity
	operator bool() const;

	// Access to raw zmq details
	operator void*() const;

private:
	void* _socket;
	socket_type _type;
	zmq_msg_t _recv_buffer;

	// No copy
	socket(socket const&);
	void operator=(socket const&);

	void track_message(message const&, uint32_t const&, bool&);
};

}

#endif /* ZMQPP_SOCKET_HPP_ */
