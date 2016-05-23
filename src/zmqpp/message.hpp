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

#ifndef ZMQPP_MESSAGE_HPP_
#define ZMQPP_MESSAGE_HPP_

#include <cassert>
#include <functional>
#include <string>
#include <cstring>
#include <cassert>
#include <unordered_map>
#include <vector>
#include <deque>
#include <utility>

#include <zmq.h>

#include "compatibility.hpp"
#include "frame.hpp"
#include "signal.hpp"
#include "exception.hpp"

namespace zmqpp
{

/**
 * \brief a zmq message with optional multipart support
 *
 * A zmq message is made up of one or more parts which are sent together to
 * the target endpoints. zmq guarantees either the whole message or none
 * of the message will be delivered.
 */
template<template<class T, class = std::allocator<T> > class container_type>
class basic_message
{
public:
	/**
	 * \brief callback to release user allocated data.
	 *
	 * The release function will be called on any void* moved part.
	 * It must be thread safe to the extent that the callback may occur on
	 * one of the context threads.
	 *
	 * The function called will be passed a single variable which is the
	 * pointer to the memory allocated.
	 */
	typedef std::function<void (void*)> release_function;

	basic_message()
		: _parts()
		, _read_cursor(0)
	{
		static_assert(std::is_same<parts_type, std::vector<frame>>::value || std::is_same<parts_type, std::deque<frame>>::value, "Container not supported.");
	}
	~basic_message()
	{
		clear();
	}

    template <typename T, typename ...Args>
    basic_message(T const &part, Args &&...args)
        : basic_message()
    {
        add(part, std::forward<Args>(args)...);
    }

	size_t parts() const
	{
		return _parts.size();
	}
	size_t size(size_t const part) const
	{
		if (part >= _parts.size())
		{
			throw exception("attempting to request a basic_message part outside the valid range");
		}

		return _parts[part].size();
	}
	std::string get(size_t const part) const
	{
		return std::string(static_cast<char const*>(raw_data(part)), size(part));
	}

	void get(int8_t& integer, size_t const part) const
	{
		assert(sizeof(int8_t) == size(part));

		int8_t const* byte = static_cast<int8_t const*>(raw_data(part));
		integer = *byte;
	}
	void get(int16_t& integer, size_t const part) const
	{
		assert(sizeof(int16_t) == size(part));

		uint16_t const* network_order = static_cast<uint16_t const*>(raw_data(part));
		integer = static_cast<int16_t>(ntohs(*network_order));
	}
	void get(int32_t& integer, size_t const part) const
	{
		assert(sizeof(int32_t) == size(part));

		uint32_t const* network_order = static_cast<uint32_t const*>(raw_data(part));
		integer = static_cast<int32_t>(htonl(*network_order));
	}
	void get(int64_t& integer, size_t const part) const
	{
		assert(sizeof(int64_t) == size(part));

		uint64_t const* network_order = static_cast<uint64_t const*>(raw_data(part));
		integer = static_cast<int64_t>(htonll(*network_order));
	}
	void get(signal& sig, size_t const part) const
	{
		assert(sizeof(signal) == size(part));
		int64_t v;
		get(v, part);

		sig = static_cast<signal>(v);
	}

	void get(uint8_t& unsigned_integer, size_t const part) const
	{
		assert(sizeof(uint8_t) == size(part));

		uint8_t const* byte = static_cast<uint8_t const*>(raw_data(part));
		unsigned_integer = *byte;
	}
	void get(uint16_t& unsigned_integer, size_t const part) const
	{
		assert(sizeof(uint16_t) == size(part));

		uint16_t const* network_order = static_cast<uint16_t const*>(raw_data(part));
		unsigned_integer = ntohs(*network_order);
	}
	void get(uint32_t& unsigned_integer, size_t const part) const
	{
		assert(sizeof(uint32_t) == size(part));

		uint32_t const* network_order = static_cast<uint32_t const*>(raw_data(part));
		unsigned_integer = ntohl(*network_order);
	}
	void get(uint64_t& unsigned_integer, size_t const part) const
	{
		assert(sizeof(uint64_t) == size(part));

		uint64_t const* network_order = static_cast<uint64_t const*>(raw_data(part));
		unsigned_integer = ntohll(*network_order);
	}

	void get(float& floating_point, size_t const part) const
	{
		assert(sizeof(float) == size(part));

		float const* network_order = static_cast<float const*>(raw_data(part));
		floating_point = zmqpp::ntohf(*network_order);
	}
	void get(double& double_precision, size_t const part) const
	{
		assert(sizeof(double) == size(part));

		double const* network_order = static_cast<double const*>(raw_data(part));
		double_precision = zmqpp::ntohd(*network_order);
	}
	void get(bool& boolean, size_t const part) const
	{
		assert(sizeof(uint8_t) == size(part));

		uint8_t const* byte = static_cast<uint8_t const*>(raw_data(part));
		boolean = (*byte != 0);
	}

	void get(std::string& string, size_t const part) const
	{
		string.assign(get(part));
	}

	// Warn: If a pointer type is requested the basic_message (well zmq) still 'owns'
	// the data and will release it when the basic_message object is freed.
	template<typename Type>
	Type get(size_t const part)
	{
		Type value;
		get(value, part);
		return value;
	}

    template<int part=0, typename T, typename ...Args>
    void extract(T &nextpart, Args &...args)
    {
        assert(part < parts());
        get(nextpart,part);
        extract<part+1>(args...);
    }

    template<int part=0, typename T>
    void extract(T &nextpart)
    {
        assert(part < parts());
        get(nextpart,part);
    }

	// Raw get data operations, useful with data structures more than anything else
	// Warn: The basic_message (well zmq) still 'owns' the data and will release it
	// when the basic_message object is freed.
	template<typename Type>
	void get(Type*& value, size_t const part) const
	{
		value = static_cast<Type*>(raw_data(part));
	}

	// Warn: The basic_message (well zmq) still 'owns' the data and will release it
	// when the basic_message object is freed.
	template<typename Type>
	void get(Type** value, size_t const part) const
	{
		*value = static_cast<Type*>(raw_data(part));
	}

	// Move operators will take ownership of basic_message parts without copying
	void move(void* part, size_t const size, release_function const& release)
	{
		callback_releaser* hint = new callback_releaser();
		hint->func = release;

		_parts.push_back(frame(part, size, &basic_message::release_callback, hint));
	}

	// Raw move data operation, useful with data structures more than anything else
	template<typename Object>
	void move(Object *part)
	{
		move(part, sizeof(Object), &deleter_callback<Object>);
	}

	// Copy operators will take copies of any data
	template<typename Type, typename ...Args>
	void add(Type const& part, Args &&...args)
	{
		*this << part;
		add(std::forward<Args>(args)...);
	}

	template<typename Type>
	void add(Type const part)
	{
		*this << part;
	}

	// Copy operators will take copies of any data with a given size
	template<typename Type>
	void add_raw(Type *part, size_t const data_size)
	{
		_parts.push_back( frame( part, data_size ) );
	}

	// Use exact data past, neither zmqpp nor 0mq will copy, alter or delete
	// this data. It must remain as valid for at least the lifetime of the
	// 0mq basic_message, recommended only with const data.
	template<typename Type>
	ZMQPP_DEPRECATED("Use add_nocopy() or add_nocopy_const() instead.")
	void add_const(Type *part, size_t const data_size)
	{
		_parts.push_back( frame( part, data_size, nullptr, nullptr ) );
	}

	/**
	 * Add a no-copy frame.
	 *
	 * This means that neither zmqpp nor libzmq will make a copy of the
	 * data. The pointed-to data must remain valid for the lifetime of
	 * the underlying zmq_msg_t. Note that you cannot always know about
	 * this lifetime, so be careful.
	 *
	 * @param part The pointed-to data that will be send in the basic_message.
	 * @param data_size The number of byte pointed-to by "part".
	 * @param ffn The free function called by libzmq when it doesn't need
	 * your buffer anymore. It defaults to nullptr, meaning your data
	 * will not be freed.
	 * @param hint A hint to help your free function do its job.
	 *
	 * @note This is similar to what `move()` does. While `move()` provide a safe
	 * (wrt to type) deleter (at the cost of 1 memory allocation) add_nocopy
	 * let you pass the low-level callback that libzmq will invoke.
	 *
	 * @note The free function must be thread-safe as it can be invoke from
	 * any libzmq's context threads.
	 *
	 * @see add_nocopy_const
	 * @see move
	 */
	template<typename Type>
	void add_nocopy(Type *part, size_t const data_size,
					zmq_free_fn *ffn = nullptr, void *hint = nullptr)
	{
		static_assert(!std::is_const<Type>::value,
                      "Data part must not be const. Use add_nocopy_const() instead (and read its documentation)");
		_parts.push_back(frame(part, data_size, ffn, hint));
	}

	/**
	 * Add a no-copy frame where pointed-to data are const.
	 *
	 * This means that neither zmqpp nor libzmq will make a copy of the
	 * data. The pointed-to data must remain valid for the lifetime of
	 * the underlying zmq_msg_t. Note that you cannot always know about
	 * this lifetime, so be careful.
	 *
	 * @warning About constness: The library will cast away constness from
	 * your pointer. However, it promises that both libzmq and zmqpp will
	 * not alter the pointed-to data. *YOU* must however be careful: zmqpp or libzmq
	 * will happily return a non-const pointer to your data. It's your responsibility
	 * to not modify it.
	 *
	 * @param part The pointed-to data that will be send in the basic_message.
	 * @param data_size The number of byte pointed-to by "part".
	 * @param ffn The free function called by libzmq when it doesn't need
	 * your buffer anymore. It defaults to nullptr, meaning your data
	 * will not be freed.
	 * @param hint A hint to help your free function do its job.
	 *
	 * @note The free function must be thread-safe as it can be invoke from
	 * any libzmq's context threads.
	 *
	 * @see add_nocopy
	 */
	template<typename Type>
	void add_nocopy_const(const Type *part, size_t const data_size,
				   zmq_free_fn *ffn = nullptr, void *hint = nullptr)
	{
		add_nocopy(const_cast<typename std::remove_const<Type *>::type>(part),
				   data_size, ffn, hint);
	}

	// Stream reader style
	void reset_read_cursor()
	{
		_read_cursor = 0;
	}

	template<typename Type>
	basic_message& operator>>(Type& value)
	{
		get(value, _read_cursor++);
		return *this;
	}

	// Stream writer style - these all use copy styles
	basic_message& operator<<(int8_t const integer)
	{
		add_raw(reinterpret_cast<void const*>(&integer), sizeof(int8_t));
		return *this;
	}
	basic_message& operator<<(int16_t const integer)
	{
		uint16_t network_order = htons(static_cast<uint16_t>(integer));
		add_raw(reinterpret_cast<void const*>(&network_order), sizeof(uint16_t));

		return *this;
	}
	basic_message& operator<<(int32_t const integer)
	{
		uint32_t network_order = htonl(static_cast<uint32_t>(integer));
		add_raw(reinterpret_cast<void const*>(&network_order), sizeof(uint32_t));

		return *this;
	}
	basic_message& operator<<(int64_t const integer)
	{
		uint64_t network_order = htonll(static_cast<uint64_t>(integer));
		add_raw(reinterpret_cast<void const*>(&network_order), sizeof(uint64_t));

		return *this;
	}
	basic_message& operator<<(signal const sig)
	{
		return (*this) << static_cast<int64_t>(sig);
	}

	basic_message& operator<<(uint8_t const unsigned_integer)
	{
		add_raw(reinterpret_cast<void const*>(&unsigned_integer), sizeof(uint8_t));
		return *this;
	}
	basic_message& operator<<(uint16_t const unsigned_integer)
	{
		uint16_t network_order = htons(unsigned_integer);
		add_raw(reinterpret_cast<void const*>(&network_order), sizeof(uint16_t));

		return *this;
	}
	basic_message& operator<<(uint32_t const unsigned_integer)
	{
		uint32_t network_order = htonl(unsigned_integer);
		add_raw(reinterpret_cast<void const*>(&network_order), sizeof(uint32_t));

		return *this;
	}
	basic_message& operator<<(uint64_t const unsigned_integer)
	{
		uint64_t network_order = htonll(unsigned_integer);
		add_raw(reinterpret_cast<void const*>(&network_order), sizeof(uint64_t));

		return *this;
	}

	basic_message& operator<<(float const floating_point)
	{
		assert(sizeof(float) == 4);

		float network_order = zmqpp::htonf(floating_point);
		add_raw(&network_order, sizeof(float));

		return *this;
	}
	basic_message& operator<<(double const double_precision)
	{
		assert(sizeof(double) == 8);

		double network_order = zmqpp::htond(double_precision);
		add_raw(&network_order, sizeof(double));

		return *this;
	}
	basic_message& operator<<(bool const boolean)
	{
		uint8_t byte = (boolean) ? 1 : 0;
		add_raw(reinterpret_cast<void const*>(&byte), sizeof(uint8_t));

		return *this;
	}

	basic_message& operator<<(char const* c_string)
	{
		add_raw(reinterpret_cast<void const*>(c_string), strlen(c_string));
		return *this;
	}
	basic_message& operator<<(std::string const& string)
	{
		add_raw(reinterpret_cast<void const*>(string.data()), string.size());
		return *this;
	}

	// Queue manipulation
	void push_front(void const* part, size_t const size)
	{
		if (std::is_same<parts_type, std::vector<frame>>::value)
		{
			_parts.emplace(_parts.begin(), part, size);
		}
		else if (std::is_same<parts_type, std::deque<frame>>::value)
		{
			_parts.emplace_front(part, size);
		}
	}

	// TODO: unify conversion of types with the stream operators
	void push_front(int8_t const integer)
	{
		push_front(&integer, sizeof(int8_t));
	}
	void push_front(int16_t const integer)
	{
		uint16_t network_order = htons(static_cast<uint16_t>(integer));
		push_front(&network_order, sizeof(uint16_t));
	}
	void push_front(int32_t const integer)
	{
		uint32_t network_order = htonl(static_cast<uint32_t>(integer));
		push_front(&network_order, sizeof(uint32_t));
	}
	void push_front(int64_t const integer)
	{
		uint64_t network_order = htonll(static_cast<uint64_t>(integer));
		push_front(&network_order, sizeof(uint64_t));
	}
	void push_front(signal const sig)
	{
		push_front(static_cast<int64_t>(sig));
	}

	void push_front(uint8_t const unsigned_integer)
	{
		push_front(&unsigned_integer, sizeof(uint8_t));
	}
	void push_front(uint16_t const unsigned_integer)
	{
		uint16_t network_order = htons(unsigned_integer);
		push_front(&network_order, sizeof(uint16_t));
	}
	void push_front(uint32_t const unsigned_integer) 
	{
		uint32_t network_order = htonl(unsigned_integer);
		push_front(&network_order, sizeof(uint32_t));
	}
	void push_front(uint64_t const unsigned_integer)
	{
		uint64_t network_order = htonll(unsigned_integer);
		push_front(&network_order, sizeof(uint64_t));
	}

	void push_front(float const floating_point)
	{
		assert(sizeof(float) == 4);

		float network_order = zmqpp::htonf(floating_point);
		push_front(&network_order, sizeof(float));
	}
	void push_front(double const double_precision)
	{
		assert(sizeof(double) == 8);

		double network_order = zmqpp::htond(double_precision);
		push_front(&network_order, sizeof(double));
	}
	void push_front(bool const boolean)
	{
		uint8_t byte = (boolean) ? 1 : 0;
		push_front(&byte, sizeof(uint8_t));
	}

	void push_front(char const* c_string)
	{
		push_front(c_string, strlen(c_string));
	}
	void push_front(std::string const& string)
	{
		push_front(string.data(), string.size());
	}

	void pop_front()
	{
		if (std::is_same<parts_type, std::vector<frame>>::value)
		{
			_parts.erase(_parts.begin());
		}
		else if (std::is_same<parts_type, std::deque<frame>>::value)
		{
			_parts.pop_front();
		}
	}

	void push_back(void const* part, size_t const data_size)
	{
		add_raw( part, data_size );
	}

	template<typename Type>
	void push_back(Type const part)
	{
		*this << part;
	}

	void pop_back()
	{
		_parts.pop_back();
	}

	void remove(size_t const part)
	{
		_parts.erase(_parts.begin() + part);
	}

	// Move supporting
	basic_message(basic_message&& source) NOEXCEPT
		: _parts()
		, _read_cursor(source._read_cursor)
	{
		std::swap(_parts, source._parts);
		source._read_cursor = 0;
	}
	basic_message& operator=(basic_message<container_type>&& source) NOEXCEPT
	{
		_read_cursor = source._read_cursor;
		source._read_cursor = 0;
		_parts = std::move(source._parts);
		return *this;
	}

	// Copy support
	basic_message copy() const
	{
		basic_message<container_type> msg;
		msg.copy(*this);
		return msg;
	}
	void copy(basic_message<container_type> const& source)
	{
		_parts.resize(source._parts.size());
		for (size_t i = 0; i < source._parts.size(); ++i)
		{
			_parts[i] = source._parts[i].copy();
		}

		// we don't need a copy of the releasers as we did data copies of the internal data,
		//_releasers = source._releasers;
		//_strings = source._strings
	}

	// Used for internal tracking
	void sent(size_t const part)
	{
		// sanity check
		assert(!_parts[part].is_sent());
		_parts[part].mark_sent();
	}

	// Access to raw zmq details
	void const* raw_data(size_t const part = 0) const
	{
		if (part >= _parts.size())
		{
			throw zmqpp::exception("attempting to request a basic_message part outside the valid range");
		}

		return _parts[part].data();
	}
	zmq_msg_t& raw_msg(size_t const part = 0)
	{
		if (part >= _parts.size())
		{
			throw zmqpp::exception("attempting to request a basic_message part outside the valid range");
		}

		return _parts[part].msg();
	}
	zmq_msg_t& raw_new_msg()
	{
		_parts.push_back(frame());

		return _parts.back().msg();
	}
	zmq_msg_t& raw_new_msg(size_t const reserve_data_size)
	{
		_parts.push_back(frame(reserve_data_size));

		return _parts.back().msg();
	}
	
	/**
	 * Check if the basic_message is a signal.
	 * If the basic_message has 1 part, has the correct size and if the 7 first bytes match
	 * the signal header we consider the basic_message a signal.
	 * @return true if the basic_message is a signal, false otherwise
	 */
	bool is_signal() const
	{
		if (parts() == 1 && size(0) == sizeof(signal))
		{
			signal s;
			get(s, 0);
			if ((static_cast<int64_t>(s) >> 8) == static_cast<int64_t>(signal::header))
				return true;
		}
		return false;
	}

	/**
	 * Gets the read cursor. For using get_raw() with stream-style reading.
	 */
	size_t read_cursor() const NOEXCEPT { return _read_cursor; }

	/**
	 * Gets the remaining number of parts in the basic_message.
	 */
	size_t remaining() const NOEXCEPT { return  _parts.size() - _read_cursor; }

	/**
	 * Moves the read cursor to the next element.
	 * @return the new read_cursor
	 */
	size_t next() NOEXCEPT { return ++_read_cursor; }

	void clear() { _parts.clear(); }


#if (ZMQ_VERSION_MAJOR == 4 && ZMQ_VERSION_MINOR >= 1)
	/**
	* Attemps to retrieve a metadata property from a basic_message.
	* The underlying call is `zmq_msg_gets()`.
	*
	* @note The basic_message MUST have at least one frame, otherwise this wont work.
	*/
	bool get_property(const std::string &property, std::string &out)
	{
		zmq_msg_t *zmq_raw_msg;
		try
		{
			zmq_raw_msg = &raw_msg();
		}
		catch (zmqpp::exception const&) // empty
		{
			return false;
		}

		const char *property_value = zmq_msg_gets(zmq_raw_msg, property.c_str());
		if (property_value == NULL)
		{
			// EINVAL is the only error code
			assert(errno == EINVAL);
			return false;
		}

		out = std::string(property_value);
		return true;
	}
#endif

private:
	typedef container_type<frame> parts_type;
	parts_type _parts;
	size_t _read_cursor;

	// Disable implicit copy support, code must request a copy to clone
	basic_message(basic_message const&) NOEXCEPT ZMQPP_EXPLICITLY_DELETED;
	basic_message& operator=(basic_message const&) NOEXCEPT ZMQPP_EXPLICITLY_DELETED;

	static void release_callback(void* data, void* hint)
	{
		callback_releaser* releaser = static_cast<callback_releaser*>(hint);
		releaser->func(data);

		delete releaser;
	}

	template<typename Object>
	static void deleter_callback(void* data)
	{
		delete static_cast<Object*>(data);
	}
};

typedef basic_message<std::vector> message;

}

#endif /* ZMQPP_MESSAGE_HPP_ */
