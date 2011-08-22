/*
 *  Created on: 16 Aug 2011
 *      Author: Ben Gray (@benjamg)
 */

#include "socket.hpp"
#include "poller.hpp"

namespace zmqpp
{

const long poller::WAIT_FOREVER;
const int poller::POLL_NONE;
const int poller::POLL_IN;
const int poller::POLL_OUT;
const int poller::POLL_ERROR;

poller::poller()
	: _items()
	, _index()
{

}

void poller::add(socket& socket, short const& event /* = POLL_IN */)
{
	zmq_pollitem_t item { socket, 0, event, 0 };

	size_t index = _items.size();
	_items.push_back(item);
	_index[socket] = index;
}

void poller::add(int const& descriptor, short const& event /* = POLL_IN */)
{
	zmq_pollitem_t item { nullptr, descriptor, event, 0 };

	size_t index = _items.size();
	_items.push_back(item);
	_fdindex[descriptor] = index;
}

void poller::check_for(socket const& socket, short const& event)
{
	auto found = _index.find(socket);
	if (_index.end() == found)
	{
		throw exception("this socket is not represented within this poller");
	}

	_items[(*found).second].events = event;
}

void poller::check_for(int const& descriptor, short const& event)
{
	auto found = _fdindex.find(descriptor);
	if (_fdindex.end() == found)
	{
		throw exception("this socket is not represented within this poller");
	}

	_items[(*found).second].events = event;
}

bool poller::poll(long timeout /* = WAIT_FOREVER */)
{
	int result = zmq_poll(_items.data(), _items.size(), timeout);
	if (result < 0)
	{
		throw zmq_internal_exception();
	}

	return (result > 0);
}

short poller::events(socket const& socket)
{
	auto found = _index.find(socket);
	if (_index.end() == found)
	{
		throw exception("this socket is not represented within this poller");
	}

	return _items[(*found).second].revents;
}

short poller::events(int const& descriptor)
{
	auto found = _fdindex.find(descriptor);
	if (_fdindex.end() == found)
	{
		throw exception("this file descriptor is not represented within this poller");
	}

	return _items[(*found).second].revents;
}

}
