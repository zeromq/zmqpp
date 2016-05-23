/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file is part of zmqpp.
 * Copyright (c) 2011-2015 Contributors as noted in the AUTHORS file.
 */

/*
 *  Created on: 9 Aug 2011
 *      Author: Ben Gray (@benjamg)
 */

#include <array>
#include <cassert>
#include <cstring>
#include <functional>

#include "context.hpp"
#include "exception.hpp"
#include "message.hpp"
#include "socket.hpp"

namespace zmqpp
{

const int max_socket_option_buffer_size = 256;
const int max_stream_buffer_size = 4096;

socket::socket(const context& context, socket_type const type)
	: _socket(nullptr)
	, _type(type)
	, _recv_buffer()
{
	_socket = zmq_socket(context, static_cast<int>(type));
	if(nullptr == _socket)
	{
		throw zmq_internal_exception();
	}

	zmq_msg_init(&_recv_buffer);
}

socket::~socket()
{
	zmq_msg_close(&_recv_buffer);

	if (nullptr != _socket)
	{

#ifndef NDEBUG // unused assert variable in release
		int result = zmq_close(_socket);
		assert(0 == result);
#else
		zmq_close(_socket);
#endif // NDEBUG

		_socket = nullptr;
	}
}

void socket::bind(endpoint_t const& endpoint)
{
	int result = zmq_bind(_socket, endpoint.c_str());

	if (0 != result)
	{
		throw zmq_internal_exception();
	}
}

#if (ZMQ_VERSION_MAJOR > 3) || ((ZMQ_VERSION_MAJOR == 3) && (ZMQ_VERSION_MINOR >= 2))
void socket::unbind(endpoint_t const& endpoint)
{
	int result = zmq_unbind(_socket, endpoint.c_str());

	if (0 != result)
	{
		throw zmq_internal_exception();
	}
}
#endif

void socket::connect(endpoint_t const& endpoint)
{
	int result = zmq_connect(_socket, endpoint.c_str());

	if (0 != result)
	{
		throw zmq_internal_exception();
	}
}

#if (ZMQ_VERSION_MAJOR > 3) || ((ZMQ_VERSION_MAJOR == 3) && (ZMQ_VERSION_MINOR >= 2))
void socket::disconnect(endpoint_t const& endpoint)
{
	int result = zmq_disconnect(_socket, endpoint.c_str());

	if (0 != result)
	{
		throw zmq_internal_exception();
	}
}
#endif

void socket::close()
{
	int result = zmq_close(_socket);

	if (0 != result)
	{
		throw zmq_internal_exception();
	}

	_socket = nullptr;
}

bool socket::send(zmqpp::signal sig, bool dont_block/* = false */)
{
    message msg(sig);
    return send(msg, dont_block);
}

  bool socket::receive(zmqpp::signal &sig, bool dont_block /* = false */)
{
    message msg;
    bool ret = receive(msg, dont_block);
    if (ret)
    {
        assert(msg.is_signal());
        msg.get(sig, 0);
    }
    return ret;
}

bool socket::send(std::string const& string, int const flags /* = NORMAL */)
{
	return send_raw(string.data(), string.size(), flags);
}

bool socket::receive(std::string& string, int const flags /* = NORMAL */)
{
#if (ZMQ_VERSION_MAJOR == 2)
		int result = zmq_recv( _socket, &_recv_buffer, flags );
#elif (ZMQ_VERSION_MAJOR < 3) || ((ZMQ_VERSION_MAJOR == 3) && (ZMQ_VERSION_MINOR < 2))
		int result = zmq_recvmsg( _socket, &_recv_buffer, flags );
#else
		int result = zmq_msg_recv( &_recv_buffer, _socket, flags );
#endif

	if(result >= 0)
	{
		string.reserve(zmq_msg_size(&_recv_buffer));
		string.assign(static_cast<char*>(zmq_msg_data(&_recv_buffer)), zmq_msg_size(&_recv_buffer));

		return true;
	}

	if (EAGAIN == zmq_errno() || EINTR == zmq_errno())
	{
		return false;
	}

	throw zmq_internal_exception();
}


bool socket::send_raw(char const* buffer, size_t const length, int const flags /* = NORMAL */)
{
#if (ZMQ_VERSION_MAJOR == 2)
    zmq_msg_t msg;
    int result = zmq_msg_init_size(&msg, length);
    if (result != 0)
    {
    	zmq_internal_exception();
    }

    memcpy(zmq_msg_data(&msg), buffer, length);
    result = zmq_send(_socket, &msg, flags);
#else
	int result = zmq_send(_socket, buffer, length, flags);
#endif
	if(result >= 0)
	{
		return true;
	}

#if (ZMQ_VERSION_MAJOR == 2)
	// only actually need to close this on error
    zmq_msg_close(&msg);
#endif

	if (EAGAIN == zmq_errno() || EINTR == zmq_errno())
	{
		return false;
	}

	throw zmq_internal_exception();
}

bool socket::receive_raw(char* buffer, size_t& length, int const flags /* = NORMAL */)
{
#if (ZMQ_VERSION_MAJOR == 2)
		int result = zmq_recv( _socket, &_recv_buffer, flags );
#elif (ZMQ_VERSION_MAJOR < 3) || ((ZMQ_VERSION_MAJOR == 3) && (ZMQ_VERSION_MINOR < 2))
		int result = zmq_recvmsg( _socket, &_recv_buffer, flags );
#else
		int result = zmq_msg_recv( &_recv_buffer, _socket, flags );
#endif

	if(result >= 0)
	{
		length = zmq_msg_size(&_recv_buffer);
		memcpy(buffer, zmq_msg_data(&_recv_buffer), length);

		return true;
	}

	if (EAGAIN == zmq_errno() || EINTR == zmq_errno())
	{
		return false;
	}

	throw zmq_internal_exception();
}


// Helper
void socket::subscribe(std::string const& topic)
{
	set(socket_option::subscribe, topic);
}

void socket::unsubscribe(std::string const& topic)
{
	set(socket_option::unsubscribe, topic);
}

bool socket::has_more_parts() const
{
	return get<bool>(socket_option::receive_more);
}


// Set socket options for different types of option
void socket::set(socket_option const option, int const value)
{
	switch(option)
	{
	// unsigned 64bit Integers
#if (ZMQ_VERSION_MAJOR == 2)
	case socket_option::high_water_mark:
	case socket_option::send_buffer_size:
	case socket_option::receive_buffer_size:
#endif
	case socket_option::affinity:
		if (value < 0) { throw exception("attempting to set an unsigned 64 bit integer option with a negative integer"); }
		set(option, static_cast<uint64_t>(value));
		break;

	// 64bit Integers
#if (ZMQ_VERSION_MAJOR == 2)
	case socket_option::rate:
	case socket_option::recovery_interval:
	case socket_option::recovery_interval_seconds:
	case socket_option::swap_size:
#else
	case socket_option::max_messsage_size:
#endif
		set(option, static_cast<int64_t>(value));
		break;

	// Boolean
#if (ZMQ_VERSION_MAJOR > 3) || ((ZMQ_VERSION_MAJOR == 3) && (ZMQ_VERSION_MINOR >= 1))
	case socket_option::ipv4_only:
#endif
#if (ZMQ_VERSION_MAJOR == 2)
	case socket_option::multicast_loopback:
#endif
#if (ZMQ_VERSION_MAJOR > 3) || ((ZMQ_VERSION_MAJOR == 3) && (ZMQ_VERSION_MINOR >= 2))
#if (ZMQ_VERSION_MAJOR == 3 && ZMQ_VERSION_MINOR == 2)
	case socket_option::delay_attach_on_connect:
#else
	case socket_option::immediate:
#endif
	case socket_option::router_mandatory:
	case socket_option::xpub_verbose:
#endif
#if (ZMQ_VERSION_MAJOR >= 4)
	case socket_option::ipv6:
	case socket_option::plain_server:
	case socket_option::conflate:
	case socket_option::curve_server:
	case socket_option::probe_router:
	case socket_option::request_correlate:
	case socket_option::request_relaxed:
	case socket_option::router_raw:
#endif
#if (ZMQ_VERSION_MAJOR > 4 || ZMQ_VERSION_MAJOR == 4 && ZMQ_VERSION_MINOR >= 1)
	case socket_option::router_handover:
#endif
#if (ZMQ_VERSION_MAJOR > 4 || ZMQ_VERSION_MAJOR == 4 && ZMQ_VERSION_MINOR >= 2)
	case socket_option::xpub_manual:
	case socket_option::xpub_verboser:
#endif

		if (value == 0) { set(option, false); }
		else if (value == 1) { set(option, true); }
		else { throw exception("attempting to set a boolean option with a non 0 or 1 integer"); }
		break;

	// Default or Boolean
#if (ZMQ_VERSION_MAJOR > 3) || ((ZMQ_VERSION_MAJOR == 3) && (ZMQ_VERSION_MINOR >= 2))
	case socket_option::tcp_keepalive:
		if (value < -1 || value > 1) { throw exception("attempting to set a default or boolean option with a non -1, 0 or 1 integer"); }
		if (0 != zmq_setsockopt(_socket, static_cast<int>(option), &value, sizeof(value)))
		{
			throw zmq_internal_exception();
		}
		break;
#endif

	// Integers that require +ve numbers
	case socket_option::backlog:
#if (ZMQ_VERSION_MAJOR == 2)
	case socket_option::reconnect_interval_max:
#else
	case socket_option::reconnect_interval_max:
	case socket_option::send_buffer_size:
	case socket_option::recovery_interval:
	case socket_option::receive_buffer_size:
	case socket_option::send_high_water_mark:
	case socket_option::receive_high_water_mark:
	case socket_option::multicast_hops:
	case socket_option::rate:
#endif
		if (value < 0) { throw exception("attempting to set a positive only integer option with a negative integer"); }
		// Integers
	case socket_option::reconnect_interval:
	case socket_option::linger:
	case socket_option::receive_timeout:
	case socket_option::send_timeout:
#if (ZMQ_VERSION_MAJOR > 3) || ((ZMQ_VERSION_MAJOR == 3) && (ZMQ_VERSION_MINOR >= 2))
	case socket_option::tcp_keepalive_idle:
	case socket_option::tcp_keepalive_count:
	case socket_option::tcp_keepalive_interval:
#endif
		if (0 != zmq_setsockopt(_socket, static_cast<int>(option), &value, sizeof(value)))
		{
			throw zmq_internal_exception();
		}
		break;
	default:
		throw exception("attempting to set a non signed integer option with a signed integer value");
	}
}

void socket::set(socket_option const option, bool const value)
{
	switch(option)
	{
#if (ZMQ_VERSION_MAJOR == 2)
	case socket_option::multicast_loopback:
#endif
#if (ZMQ_VERSION_MAJOR > 3) || ((ZMQ_VERSION_MAJOR == 3) && (ZMQ_VERSION_MINOR >= 1))
	case socket_option::ipv4_only:
#endif
#if (ZMQ_VERSION_MAJOR > 3) || ((ZMQ_VERSION_MAJOR == 3) && (ZMQ_VERSION_MINOR >= 2))
#if (ZMQ_VERSION_MAJOR == 3 && ZMQ_VERSION_MINOR == 2)
	case socket_option::delay_attach_on_connect:
#else
	case socket_option::immediate:
#endif
	case socket_option::router_mandatory:
	case socket_option::xpub_verbose:
#endif
#if (ZMQ_VERSION_MAJOR >= 4)
	case socket_option::ipv6:
	case socket_option::plain_server:
	case socket_option::conflate:
	case socket_option::curve_server:
	case socket_option::probe_router:
	case socket_option::request_correlate:
	case socket_option::request_relaxed:
	case socket_option::router_raw:
#endif
#if (ZMQ_VERSION_MAJOR > 4 || ZMQ_VERSION_MAJOR == 4 && ZMQ_VERSION_MINOR >= 1)
	case socket_option::router_handover:
#endif
#if (ZMQ_VERSION_MAJOR > 4 || ZMQ_VERSION_MAJOR == 4 && ZMQ_VERSION_MINOR >= 2)
	case socket_option::xpub_manual:
	case socket_option::xpub_verboser:
#endif
	{
		int ivalue = value ? 1 : 0;
		if (0 != zmq_setsockopt(_socket, static_cast<int>(option), &ivalue, sizeof(ivalue)))
		{
			throw zmq_internal_exception();
		}
		break;
	}
	default:
		throw exception("attempting to set a non boolean option with a boolean value");
	}
}

void socket::set(socket_option const option, uint64_t const value)
{
	switch(option)
	{
#if (ZMQ_VERSION_MAJOR == 2)
	// unsigned 64bit Integers
	case socket_option::high_water_mark:
	case socket_option::send_buffer_size:
	case socket_option::receive_buffer_size:
#endif
	case socket_option::affinity:
		if (0 != zmq_setsockopt(_socket, static_cast<int>(option), &value, sizeof(value)))
		{
			throw zmq_internal_exception();
		}
		break;
	default:
		throw exception("attempting to set a non unsigned 64 bit integer option with a unsigned 64 bit integer value");
	}
}

void socket::set(socket_option const option, int64_t const value)
{
	switch(option)
	{
#if (ZMQ_VERSION_MAJOR == 2)
	case socket_option::rate:
	case socket_option::recovery_interval:
	case socket_option::recovery_interval_seconds:
	case socket_option::swap_size:
#else
	case socket_option::max_messsage_size:
#endif
		// zmq only allowed +ve int64_t options
		if (value < 0) { throw exception("attempting to set a positive only 64 bit integer option with a negative 64bit integer"); }
		if (0 != zmq_setsockopt(_socket, static_cast<int>(option), &value, sizeof(value)))
		{
			throw zmq_internal_exception();
		}
		break;
	default:
		throw exception("attempting to set a non 64 bit integer option with a 64 bit integer value");
	}
}

void socket::set(socket_option const option, char const* value, size_t const length)
{
	switch(option)
	{
	case socket_option::identity:
	case socket_option::subscribe:
	case socket_option::unsubscribe:
#if (ZMQ_VERSION_MAJOR > 3) || ((ZMQ_VERSION_MAJOR == 3) && (ZMQ_VERSION_MINOR >= 2))
	case socket_option::tcp_accept_filter:
#endif
#if (ZMQ_VERSION_MAJOR >= 4)
	case socket_option::plain_password:
	case socket_option::plain_username:
	case socket_option::zap_domain:
	case socket_option::curve_public_key:
	case socket_option::curve_secret_key:
	case socket_option::curve_server_key:
#endif
		if (0 != zmq_setsockopt(_socket, static_cast<int>(option), value, length))
		{
			throw zmq_internal_exception();
		}
		break;
	default:
		throw exception("attempting to set a non string option with a string value");
	}
}

// Get socket options, multiple versions for easy of use
void socket::get(socket_option const option, int& value) const
{
	size_t value_size = sizeof(int);

	switch(option)
	{
#if (ZMQ_VERSION_MAJOR == 2)
	case socket_option::receive_more:
	case socket_option::multicast_loopback:
		value = static_cast<int>(get<int64_t>(option));
		break;
#endif
	case socket_option::type:
	case socket_option::linger:
	case socket_option::backlog:
	case socket_option::reconnect_interval:
	case socket_option::reconnect_interval_max:
	case socket_option::receive_timeout:
	case socket_option::send_timeout:
	case socket_option::file_descriptor:
	case socket_option::events:
#if (ZMQ_VERSION_MAJOR > 2)
	case socket_option::receive_more:
	case socket_option::send_buffer_size:
	case socket_option::receive_buffer_size:
	case socket_option::rate:
	case socket_option::recovery_interval:
	case socket_option::send_high_water_mark:
	case socket_option::receive_high_water_mark:
	case socket_option::multicast_hops:
#endif
#if (ZMQ_VERSION_MAJOR > 3) || ((ZMQ_VERSION_MAJOR == 3) && (ZMQ_VERSION_MINOR >= 1))
	case socket_option::ipv4_only:
#endif
#if (ZMQ_VERSION_MAJOR > 3) || ((ZMQ_VERSION_MAJOR == 3) && (ZMQ_VERSION_MINOR >= 2))
#if (ZMQ_VERSION_MAJOR == 3 && ZMQ_VERSION_MINOR == 2)
	case socket_option::delay_attach_on_connect:
#else
	case socket_option::immediate:
#endif
	case socket_option::tcp_keepalive:
	case socket_option::tcp_keepalive_idle:
	case socket_option::tcp_keepalive_count:
	case socket_option::tcp_keepalive_interval:
#endif
#ifdef ZMQ_EXPERIMENTAL_LABELS
	case socket_option::receive_label:
#endif
#if (ZMQ_VERSION_MAJOR >= 4)
	case socket_option::ipv6:
	case socket_option::plain_server:
	case socket_option::mechanism:
#endif
		if (0 != zmq_getsockopt(_socket, static_cast<int>(option), &value, &value_size))
		{
			throw zmq_internal_exception();
		}

		// sanity check
		assert(value_size <= sizeof(int));
		break;
	default:
		throw exception("attempting to get a non integer option with an integer value");
	}
}

void socket::get(socket_option const option, bool& value) const
{
#if (ZMQ_VERSION_MAJOR == 2)
	int64_t int_value = 0;
	size_t value_size = sizeof(int64_t);
#else
	int int_value = 0;
	size_t value_size = sizeof(int);
#endif

	switch(option)
	{
	case socket_option::receive_more:
#if (ZMQ_VERSION_MAJOR == 2)
	case socket_option::multicast_loopback:
#endif
#if (ZMQ_VERSION_MAJOR > 3) || ((ZMQ_VERSION_MAJOR == 3) && (ZMQ_VERSION_MINOR >= 1))
	case socket_option::ipv4_only:
#endif
#if (ZMQ_VERSION_MAJOR > 3) || ((ZMQ_VERSION_MAJOR == 3) && (ZMQ_VERSION_MINOR >= 2))
#if (ZMQ_VERSION_MAJOR == 3 && ZMQ_VERSION_MINOR == 2)
	case socket_option::delay_attach_on_connect:
#else
	case socket_option::immediate:
#endif
#endif
#ifdef ZMQ_EXPERIMENTAL_LABELS
	case socket_option::receive_label:
#endif
#if (ZMQ_VERSION_MAJOR >= 4)
	case socket_option::ipv6:
	case socket_option::plain_server:
#endif
		if (0 != zmq_getsockopt(_socket, static_cast<int>(option), &int_value, &value_size))
		{
			throw zmq_internal_exception();
		}

		value = (int_value == 1) ? true : false;
		break;
	default:
		throw exception("attempting to get a non boolean option with a boolean value");
	}
}

void socket::get(socket_option const option, uint64_t& value) const
{
	size_t value_size = sizeof(uint64_t);

	switch(option)
	{
#if (ZMQ_VERSION_MAJOR == 2)
	case socket_option::high_water_mark:
	case socket_option::send_buffer_size:
	case socket_option::receive_buffer_size:
#endif
	case socket_option::affinity:
		if(0 != zmq_getsockopt(_socket, static_cast<int>(option), &value, &value_size))
		{
			throw zmq_internal_exception();
		}
		break;
	default:
		throw exception("attempting to get a non unsigned 64 bit integer option with an unsigned 64 bit integer value");
	}
}

void socket::get(socket_option const option, int64_t& value) const
{
	size_t value_size = sizeof(int64_t);

	switch(option)
	{
#if (ZMQ_VERSION_MAJOR == 2)
	case socket_option::rate:
	case socket_option::recovery_interval:
	case socket_option::recovery_interval_seconds:
	case socket_option::swap_size:
	case socket_option::receive_more:
	case socket_option::multicast_loopback:
#else
	case socket_option::max_messsage_size:
#endif
		if(0 != zmq_getsockopt(_socket, static_cast<int>(option), &value, &value_size))
		{
			throw zmq_internal_exception();
		}
		break;
	default:
		throw exception("attempting to get a non 64 bit integer option with an 64 bit integer value");
	}
}

void socket::get(socket_option const option, std::string& value) const
{
	static std::array<char, max_socket_option_buffer_size> buffer;
	size_t size = max_socket_option_buffer_size;

	switch(option)
	{
	case socket_option::identity:
#if (ZMQ_VERSION_MAJOR > 3) || ((ZMQ_VERSION_MAJOR == 3) && (ZMQ_VERSION_MINOR >= 2))
	case socket_option::last_endpoint:
#endif
#if (ZMQ_VERSION_MAJOR >= 4)
	case socket_option::plain_password:
	case socket_option::plain_username:
	case socket_option::zap_domain:
	case socket_option::curve_public_key:
	case socket_option::curve_secret_key:
	case socket_option::curve_server_key:
#endif
		if(0 != zmq_getsockopt(_socket, static_cast<int>(option), buffer.data(), &size))
		{
			throw zmq_internal_exception();
		}

		value.assign(buffer.data(), size > 0 ? size-1 : 0);
		break;
	default:
		throw exception("attempting to get a non string option with a string value");
	}
}

socket::socket(socket&& source) NOEXCEPT
	: _socket(source._socket)
	, _type(source._type)
	, _recv_buffer()
{
	// we steal the zmq_msg_t from the valid socket, we only init our own because it's cheap
	// and zmq_msg_move does a valid check
	zmq_msg_init(&_recv_buffer);
	zmq_msg_move(&_recv_buffer, &source._recv_buffer);

	// Clean up source a little, we will handle the deinit, it doesn't need to
	source._socket = nullptr;
}

socket& socket::operator=(socket&& source) NOEXCEPT
{
	std::swap(_socket, source._socket);

	_type = source._type; // just clone?

	// we steal the zmq_msg_t from the valid socket, we only init our own because it's cheap
	// and zmq_msg_move does a valid check
	zmq_msg_init(&_recv_buffer);
	zmq_msg_move(&_recv_buffer, &source._recv_buffer);

	return *this;
}


socket::operator bool() const
{
	return nullptr != _socket;
}


socket::operator void*() const
{
	return _socket;
}

template<template<class T, class = std::allocator<T>> class container>
void socket::track_message(basic_message<container> const& /* message */, uint32_t const parts, bool& should_delete)
{
	if (parts == 0)
	{
		should_delete = true;
	}
}

#if (ZMQ_VERSION_MAJOR >= 4)
void socket::monitor(endpoint_t const monitor_endpoint, int events_required)
{
	int result = zmq_socket_monitor( _socket, monitor_endpoint.c_str(), events_required );

	if (0 != result)
	{
		throw zmq_internal_exception();
	}
}
#endif

signal socket::wait()
{
    for (;;)
    {
        message msg;
        while(!receive(msg));

        if (msg.is_signal())
        {
            signal sig;
            msg.get(sig, 0);
            return sig;
        }
    }
}

}
