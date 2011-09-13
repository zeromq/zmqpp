/*
 *  Created on: 9 Aug 2011
 *      Author: Ben Gray (@benjamg)
 */

#ifndef ZMQPP_POLLER_HPP_
#define ZMQPP_POLLER_HPP_

#include <unordered_map>
#include <vector>

#include "compatibility.hpp"

namespace zmqpp
{
class socket;

typedef socket socket_t;

class poller
{
public:
	static const long WAIT_FOREVER;

	static const short POLL_NONE;
	static const short POLL_IN;
	static const short POLL_OUT;
	static const short POLL_ERROR;

	poller();
	~poller();

	void add(socket_t& socket, short const& event = POLL_IN);
	void add(int const& descriptor, short const& event = POLL_IN);

	void check_for(socket_t const& socket, short const& event);
	void check_for(int const& descriptor, short const& event);

	bool poll(long timeout = WAIT_FOREVER);

	short events(socket_t const& socket) const;
	short events(int const& descriptor) const;

	template<typename Watched>
	bool has_input(Watched const& watchable) const { return events(watchable) & POLL_IN; }

	template<typename Watched>
	bool has_output(Watched const& watchable) const { return events(watchable) & POLL_OUT; }

	template<typename Watched>
	bool has_error(Watched const& watchable) const { return events(watchable) & POLL_ERROR; }

private:
	std::vector<zmq_pollitem_t> _items;
	std::unordered_map<void *, size_t> _index;
	std::unordered_map<int, size_t> _fdindex;
};

}

#endif /* ZMQPP_POLLER_HPP_ */
