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
 * \date   8 Jan 2014
 * \author Ben Gray (\@benjamg)
 */

#ifndef ZMQPP_MESSAGE_FRAME_HPP_
#define ZMQPP_MESSAGE_FRAME_HPP_

#include <zmq.h>

#include "compatibility.hpp"

namespace zmqpp {

/**
 * An internal frame wrapper for a single zmq message
 *
 * This frame wrapper consists of a zmq message and meta data it is used
 * by the zmqpp message class to keep track of parts in the internal
 * queue. It is unlikely you need to use this class.
 */
class frame
{
public:
	frame();
	frame(size_t const size);
	frame(void const* part, size_t const size);
	frame(void* part, size_t const size, zmq_free_fn *ffn, void *hint);

	~frame();

	bool is_sent() const { return _sent; }
	void const* data() const { return zmq_msg_data( const_cast<zmq_msg_t*>(&_msg) ); }
	size_t size() const { return zmq_msg_size( const_cast<zmq_msg_t*>(&_msg) ); }

	void mark_sent() { _sent = true; }
	zmq_msg_t& msg() { return _msg; }

	// Move operators
	frame(frame&& other);
	frame& operator=(frame&& other);

	frame copy() const;

private:
	bool _sent;
	zmq_msg_t _msg;

	// Disable implicit copy support, code must request a copy to clone
	frame(frame const&) NOEXCEPT ZMQPP_EXPLICITLY_DELETED;
	frame& operator=(frame const&) NOEXCEPT ZMQPP_EXPLICITLY_DELETED;
};

} // namespace zmqpp

#endif /* ZMQPP_MESSAGE_FRAME_HPP_ */
