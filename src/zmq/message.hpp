/*
 *  Created on: 9 Aug 2011
 *      Author: Ben Gray (@benjamg)
 */

#ifndef CPPZMQ_MESSAGE_HPP_
#define CPPZMQ_MESSAGE_HPP_

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <zmq.h>

namespace zmq
{

class message
{
public:
	// The release function will be called on any void* moved part, it must be thread safe
	typedef std::function<void (void*)> release_function;

	message();
	~message();

	size_t parts() const;
	size_t size(size_t const& part = 0);
	std::string get(size_t const& part = 0);

	template<typename Type>
	void get(Type& value, size_t const& part = 0)
	{
		size_t old = _read_cursor;
		_read_cursor = part;

		*this >> value;

		_read_cursor = old;
	}

	template<typename Type>
	Type get(size_t const& part = 0)
	{
		Type value;
		get(value, part);
		return value;
	}

	// Move operators will take ownership of message parts without copying
	void move(void* part, size_t& size, release_function const& release);

	// Copy operators will take copies of any data
	void add(void const* part, size_t const& size);

	template<typename Type>
	void add(Type const& part)
	{
		*this << part;
	}

	// Stream reader style
	void reset_read_cursor();

	message& operator>>(int8_t& integer);
	message& operator>>(int16_t& integer);
	message& operator>>(int32_t& integer);
	message& operator>>(int64_t& integer);

	message& operator>>(uint8_t& unsigned_integer);
	message& operator>>(uint16_t& unsigned_integer);
	message& operator>>(uint32_t& unsigned_integer);
	message& operator>>(uint64_t& unsigned_integer);

	message& operator>>(float& floating_point);
	message& operator>>(double& double_precision);
	message& operator>>(bool& boolean);

	message& operator>>(std::string& string);

	// Stream writer style - these all use copy styles
	message& operator<<(int8_t const& integer);
	message& operator<<(int16_t const& integer);
	message& operator<<(int32_t const& integer);
	message& operator<<(int64_t const& integer);

	message& operator<<(uint8_t const& unsigned_integer);
	message& operator<<(uint16_t const& unsigned_integer);
	message& operator<<(uint32_t const& unsigned_integer);
	message& operator<<(uint64_t const& unsigned_integer);

	message& operator<<(float const& floating_point);
	message& operator<<(double const& double_precision);
	message& operator<<(bool const& boolean);

	message& operator<<(char const* c_string);
	message& operator<<(std::string const& string);

	// Move supporting
	message(message&& source);
	void operator=(message&& source);

	// Copy support
	message copy();
	void copy(message& source);

	// Used for internal tracking
	void sent(size_t const& part);

	// Access to raw zmq details
	void* raw_data(size_t const& part = 0);
	zmq_msg_t& raw_msg(size_t const& part = 0);
	zmq_msg_t& raw_new_msg();

private:
	struct zmq_msg_wrapper
	{
		bool sent;
		zmq_msg_t msg;
	};

	struct callback_releaser
	{
		release_function func;
	};

	std::vector<zmq_msg_wrapper> _parts;
	size_t _read_cursor;

	// Disable implicit copy support, code must request a copy to clone
	message(message const&);
	void operator=(message const&);

	static void release_callback(void* data, void* hint);
};

}

#endif /* CPPZMQ_MESSAGE_HPP_ */
