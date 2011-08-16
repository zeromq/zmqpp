/*
 *  Created on: 9 Aug 2011
 *      Author: Ben Gray (@benjamg)
 */

#ifndef CPPZMQ_POLLER_HPP_
#define CPPZMQ_POLLER_HPP_

#include <unordered_map>
#include <vector>

#include <zmq.h>

#include "exception.hpp"
#include "socket.hpp"

namespace zmq
{

class poller
{
public:
	static const long WAIT_FOREVER = -1;

	static const int POLL_NONE = 0;
	static const int POLL_IN = ZMQ_POLLIN;
	static const int POLL_OUT = ZMQ_POLLOUT;
	static const int POLL_ERROR = ZMQ_POLLERR;

	poller();

	void add(socket& socket, short const& event = POLL_IN);
	void add(int const& descriptor, short const& event = POLL_IN);

	void check_for(socket const& socket, short const& event);
	void check_for(int const& descriptor, short const& event);

	bool poll(long timeout = WAIT_FOREVER);

	short events(socket const& socket);
	short events(int const& descriptor);

	template<typename Watched>
	bool has_input(Watched const& watchable) { return events(watchable) & POLL_IN; }

	template<typename Watched>
	bool has_output(Watched const& watchable) { return events(watchable) & POLL_OUT; }

	template<typename Watched>
	bool has_error(Watched const& watchable) { return events(watchable) & POLL_ERROR; }

private:
	std::vector<zmq_pollitem_t> _items;
	std::unordered_map<void *, size_t> _index;
	std::unordered_map<int, size_t> _fdindex;
};

}

#endif /* CPPZMQ_POLLER_HPP_ */
