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

enum poll_event : short
{
	none = 0,
	input = ZMQ_POLLIN,
	output = ZMQ_POLLOUT,
	error = ZMQ_POLLERR,
};

class poller
{
public:
	static const long WAIT_FOREVER = -1;

	poller()
		: _items()
		, _index()
	{

	}

	void add(socket& socket, poll_event const& event = poll_event::input)
	{
		zmq_pollitem_t item { socket, 0, event, 0 };

		size_t index = _items.size();
		_items.push_back(item);
		_index[socket] = index;
	}

	bool poll(long timeout = WAIT_FOREVER)
	{
		int result = zmq_poll(_items.data(), _items.size(), timeout);
		if (result < 0)
		{
			throw zmq_internal_exception();
		}

		return (result > 0);
	}

	short events(socket& socket)
	{
		auto found = _index.find(socket);
		if (_index.end() == found)
		{
			throw exception("this socket is not represented within this poller");
		}

		return _items[(*found).second].revents;
	}

	bool has_input(socket& socket) { return events(socket) & poll_event::input; }
	bool has_output(socket& socket) { return events(socket) & poll_event::output; }
	bool has_error(socket& socket) { return events(socket) & poll_event::error; }

private:
	std::vector<zmq_pollitem_t> _items;
	std::unordered_map<void *, size_t> _index;
};

}

#endif /* CPPZMQ_POLLER_HPP_ */
