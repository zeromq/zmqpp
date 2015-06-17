/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file is part of zmqpp.
 * Copyright (c) 2011-2015 Contributors as noted in the AUTHORS file.
 */

/**
 * \file
 *
 * \date   9 Aug 2011
 * \author Ben Gray (\@benjamg)
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

/**
 * Polling wrapper.
 *
 * Allows access to polling for any number of sockets or file descriptors.
 */
class poller
{
public:
	static const long wait_forever; /*!< Block forever flag, default setting. */

	static const short poll_none;   /*!< No polling flags set. */
	static const short poll_in;     /*!< Monitor inbound flag. */
	static const short poll_out;    /*!< Monitor output flag. */
	static const short poll_error;  /*!< Monitor error flag.\n Only for file descriptors. */

#if ((ZMQ_VERSION_MAJOR == 4 && ZMQ_VERSION_MINOR >= 2) || ZMQ_VERSION_MAJOR > 4)
        static const short poll_pri;    /*!< Priority input flag.\n Only for file descriptors. See POLLPRI) */
#endif

	/**
	 * Construct an empty polling model.
	 */
	poller();

	/**
	 * Cleanup poller.
	 *
	 * Any sockets will need to be closed separately.
	 */
	~poller();

	/**
	 * Add a socket to the polling model and set which events to monitor.
	 *
	 * \param socket the socket to monitor.
	 * \param event the event flags to monitor on the socket.
	 */
	void add(socket_t& socket, short const event = poll_in);

	/**
	 * Add a file descriptor to the polling model and set which events to monitor.
	 *
	 * \param descriptor the file descriptor to monitor.
	 * \param event the event flags to monitor.
	 */
	void add(int const descriptor, short const event = poll_in | poll_error);

	/**
	 * Add a zmq_pollitem_t to the poller; Events to monitor are already configured.
	 * If the zmq_pollitem_t has a null socket pointer it is added to the fdindex,
	 * otherwise it is added to the socket index.
	 *
	 * \param item the pollitem to be added
	 */
	void add(zmq_pollitem_t item);

	 /**
	  * Check if we are monitoring a given socket with this poller.
	  *
	  * \param socket the socket to check.
	  * \return true if it is there.
	  */
	 bool has(socket_t const& socket);

	/**
	 * Check if we are monitoring a given file descriptor with this poller.
	 *
	 * \param descriptor the file descriptor to check.
	 * \return true if it is there.
	 */
	bool has(int const descriptor);

	/**
	 * Check if we are monitoring a given pollitem.
	 * We assume the pollitem is a socket if it's socket is a non null pointer; otherwise,
	 * it is considered as a file descriptor.
	 *
	 * \param item the pollitem to check for
	 * \return true if it is there.
	 */
	bool has(const zmq_pollitem_t &item);

	/**
	 * Stop monitoring a socket.
	 *
	 * \param socket the socket to stop monitoring.
	 */
	void remove(socket_t const& socket);
	
	/**
	 * Stop monitoring a file descriptor.
	 *
	 * \param descriptor the file descriptor to stop monitoring.
	 */
	void remove(int const descriptor);

	/**
	 * Stop monitoring a zmq_pollitem_t
	 *
	 * \param item the pollitem to stop monitoring.
	 */
	void remove(const zmq_pollitem_t &item);

	/**
	 * Update the monitored event flags for a given socket.
	 *
	 * \param socket the socket to update event flags.
	 * \param event the event flags to monitor on the socket.
	 */
	void check_for(socket_t const& socket, short const event);
	
	/**
	 * Update the monitored event flags for a given file descriptor.
	 *
	 * \param descriptor the file descriptor to update event flags.
	 * \param event the event flags to monitor on the socket.
	 */
	void check_for(int const descriptor, short const event);
	
	/**
	 * Update the monitored event flags for a given zmq_pollitem_t
	 *
	 * \param item the item to change event flags for.
	 * \param event the event flags to monitor on the socket.
	 */
	void check_for(const zmq_pollitem_t &item, short const event);
	
	/**
	 * Poll for monitored events.
	 *
	 * By default this method will block forever or until at least one of the monitored
	 * sockets or file descriptors has events.
	 *
	 * If a timeout is set and was reached then this function returns false.
	 *
	 * \param timeout milliseconds to timeout.
	 * \return true if there is an event..
	 */
	bool poll(long timeout = wait_forever);
	
	/**
	 * Get the event flags triggered for a socket.
	 *
	 * \param socket the socket to get triggered event flags for.
	 * \return the event flags.
	 */
	short events(socket_t const& socket) const;
	
	/**
	 * Get the event flags triggered for a file descriptor.
	 *
	 * \param descriptor the file descriptor to get triggered event flags for.
	 * \return the event flags.
	 */
	short events(int const descriptor) const;
	
	/**
	 * Get the event flags triggered for a zmq_pollitem_t
	 *
	 * \param item the pollitem to get triggered event flags for.
	 * \return the event flags.
	 */
	short events(const zmq_pollitem_t &item) const;

	/**
	 * Check either a file descriptor or socket for input events.
	 *
	 * Templated helper method that calls through to event and checks for a given flag
	 *
	 * \param watchable either a file descriptor or socket known to the poller.
	 * \return true if there is input.
	 */
	template<typename Watched>
	bool has_input(Watched const& watchable) const { return (events(watchable) & poll_in) != 0; }

	/**
	 * Check either a file descriptor or socket for output events.
	 *
	 * Templated helper method that calls through to event and checks for a given flag
	 *
	 * \param watchable either a file descriptor or socket known to the poller.
	 * \return true if there is output.
	 */
	template<typename Watched>
	bool has_output(Watched const& watchable) const { return (events(watchable) & poll_out) != 0; }

	/**
	 * Check a file descriptor.
	 *
	 * Templated helper method that calls through to event and checks for a given flag
	 *
	 * Technically this template works for sockets as well but the error flag is never set for
	 * sockets so I have no idea why someone would call it.
	 *
	 * \param watchable a file descriptor know to the poller.
	 * \return true if there is an error.
	 */
	template<typename Watched>
	bool has_error(Watched const& watchable) const { return (events(watchable) & poll_error) != 0; }

private:
	std::vector<zmq_pollitem_t> _items;
	std::unordered_map<void *, size_t> _index;
	std::unordered_map<int, size_t> _fdindex;

	void reindex(size_t const index);
};

}

#endif /* ZMQPP_POLLER_HPP_ */
