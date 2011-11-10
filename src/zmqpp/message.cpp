/*
 *  Created on: 9 Aug 2011
 *      Author: Ben Gray (@benjamg)
 */

#include <cassert>
#include <cstring>

#include "exception.hpp"
#include "inet.hpp"
#include "message.hpp"

namespace zmqpp
{

/*!
 * \brief internal construct
 * \internal wraps the zmq struct block so allow us to monitor sent state
 */
struct zmq_msg_wrapper
{
	bool sent;
	zmq_msg_t msg;
};

/*!
 * \brief internal construct
 * \internal handles bubbling callback from zmq c style to the c++ functor provided
 */
struct callback_releaser
{
	message::release_function func;
};

message::message()
	: _parts()
	, _read_cursor(0)
{
}

message::~message()
{
	for(size_t i = 0; i < _parts.size(); ++i)
	{
		zmq_msg_t& msg = _parts[i].msg;

#ifndef NDEBUG // unused assert variable in release
		int result = zmq_msg_close(&msg);
		assert(0 == result);
#else
		zmq_msg_close(&msg);
#endif // NDEBUG

	}
	_parts.clear();
}

size_t message::parts() const
{
	return _parts.size();
}

size_t message::size(size_t const& part /* = 0 */)
{
	if(part >= _parts.size())
	{
		throw exception("attempting to request a message part outside the valid range");
	}

	zmq_msg_wrapper& wrap = _parts[part];
	return zmq_msg_size(&wrap.msg);
}

void* message::raw_data(size_t const& part /* = 0 */)
{
	if(part >= _parts.size())
	{
		throw exception("attempting to request a message part outside the valid range");
	}

	zmq_msg_wrapper& wrap = _parts[part];
	return zmq_msg_data(&wrap.msg);
}

zmq_msg_t& message::raw_msg(size_t const& part /* = 0 */)
{
	if(part >= _parts.size())
	{
		throw exception("attempting to request a message part outside the valid range");
	}

	zmq_msg_wrapper& wrap = _parts[part];
	return wrap.msg;
}

zmq_msg_t& message::raw_new_msg()
{
	parts_type tmp(_parts.size() + 1);

	for(size_t i = 0; i < _parts.size(); ++i)
	{
		zmq_msg_t& dest = tmp[i].msg;
		if( 0 != zmq_msg_init(&dest) )
		{
			throw zmq_internal_exception();
		}

		zmq_msg_t& src = _parts[i].msg;
		if( 0 != zmq_msg_move(&dest, &src) )
		{
			throw zmq_internal_exception();
		}
	}

	std::swap(tmp, _parts);

	zmq_msg_t& msg = _parts.back().msg;
	if( 0 != zmq_msg_init(&msg) )
	{
		throw zmq_internal_exception();
	}

	return msg;
}

std::string message::get(size_t const& part /* = 0 */)
{
	return std::string(static_cast<char*>(raw_data(part)), size(part));
}


// Move operators will take ownership of message parts without copying
void message::move(void* part, size_t& size, release_function const& release)
{
	parts_type tmp(_parts.size() + 1);

	for(size_t i = 0; i < _parts.size(); ++i)
	{
		zmq_msg_t& dest = tmp[i].msg;
		if( 0 != zmq_msg_init(&dest) )
		{
			throw zmq_internal_exception();
		}

		zmq_msg_t& src = _parts[i].msg;
		if( 0 != zmq_msg_move(&dest, &src) )
		{
			throw zmq_internal_exception();
		}
	}

	std::swap(tmp, _parts);

	callback_releaser* hint = new callback_releaser();
	hint->func = release;

	zmq_msg_t& msg = _parts.back().msg;
	if (0 != zmq_msg_init_data(&msg, part, size, &message::release_callback, hint))
	{
		throw zmq_internal_exception();
	}
}

void message::add(void const* part, size_t const& size)
{
	parts_type tmp(_parts.size() + 1);

	for(size_t i = 0; i < _parts.size(); ++i)
	{
		zmq_msg_t& dest = tmp[i].msg;
		if( 0 != zmq_msg_init(&dest) )
		{
			throw zmq_internal_exception();
		}

		zmq_msg_t& src = _parts[i].msg;
		if( 0 != zmq_msg_move(&dest, &src) )
		{
			throw zmq_internal_exception();
		}
	}

	std::swap(tmp, _parts);

	zmq_msg_t& msg = _parts.back().msg;

	if( 0 != zmq_msg_init_size(&msg, size) )
	{
		throw zmq_internal_exception();
	}

	void* msg_data = zmq_msg_data(&msg);

	memcpy(msg_data, part, size);
}

// Stream reader style
void message::reset_read_cursor()
{
	_read_cursor = 0;
}

message& message::operator>>(int8_t& integer)
{
	assert(sizeof(int8_t) == size(_read_cursor));

	int8_t* byte = static_cast<int8_t*>(raw_data(_read_cursor++));
	integer = *byte;

	return *this;
}

message& message::operator>>(int16_t& integer)
{
	assert(sizeof(int16_t) == size(_read_cursor));

	uint16_t* network_order = static_cast<uint16_t*>(raw_data(_read_cursor++));
	integer = static_cast<int16_t>(ntohs(*network_order));

	return *this;
}

message& message::operator>>(int32_t& integer)
{
	assert(sizeof(int32_t) == size(_read_cursor));

	uint32_t* network_order = static_cast<uint32_t*>(raw_data(_read_cursor++));
	integer = static_cast<int32_t>(htonl(*network_order));

	return *this;
}

message& message::operator>>(int64_t& integer)
{
	assert(sizeof(int64_t) == size(_read_cursor));

	uint64_t* network_order = static_cast<uint64_t*>(raw_data(_read_cursor++));
	integer = static_cast<int64_t>(htonll(*network_order));

	return *this;
}

message& message::operator>>(uint8_t& unsigned_integer)
{
	assert(sizeof(uint8_t) == size(_read_cursor));

	uint8_t* byte = static_cast<uint8_t*>(raw_data(_read_cursor++));
	unsigned_integer = *byte;

	return *this;
}

message& message::operator>>(uint16_t& unsigned_integer)
{
	assert(sizeof(uint16_t) == size(_read_cursor));

	uint16_t* network_order = static_cast<uint16_t*>(raw_data(_read_cursor++));
	unsigned_integer = ntohs(*network_order);

	return *this;
}

message& message::operator>>(uint32_t& unsigned_integer)
{
	assert(sizeof(uint32_t) == size(_read_cursor));

	uint32_t* network_order = static_cast<uint32_t*>(raw_data(_read_cursor++));
	unsigned_integer = ntohl(*network_order);

	return *this;
}

message& message::operator>>(uint64_t& unsigned_integer)
{
	assert(sizeof(uint64_t) == size(_read_cursor));

	uint64_t* network_order = static_cast<uint64_t*>(raw_data(_read_cursor++));
	unsigned_integer = ntohll(*network_order);

	return *this;
}

message& message::operator>>(float& floating_point)
{
	assert(sizeof(uint32_t) == size(_read_cursor));

	uint32_t* network_order = static_cast<uint32_t*>(raw_data(_read_cursor++));
	uint32_t host_order = ntohl(*network_order);
	float* temp = reinterpret_cast<float*>(&host_order);
	floating_point = *temp;

	return *this;
}

message& message::operator>>(double& double_precision)
{
	assert(sizeof(uint64_t) == size(_read_cursor));

	uint64_t* network_order = static_cast<uint64_t*>(raw_data(_read_cursor++));
	uint64_t host_order = ntohll(*network_order);
	double* temp = reinterpret_cast<double*>(&host_order);
	double_precision = *temp;

	return *this;
}

message& message::operator>>(bool& boolean)
{
	assert(sizeof(uint8_t) == size(_read_cursor));

	uint8_t* byte = static_cast<uint8_t*>(raw_data(_read_cursor++));
	boolean = (*byte != 0);

	return *this;
}

message& message::operator>>(std::string& string)
{
	string = get(_read_cursor++);

	return *this;
}


// Stream writer style - these all use copy styles
message& message::operator<<(int8_t const& integer)
{
	add(&integer, sizeof(int8_t));
	return *this;
}

message& message::operator<<(int16_t const& integer)
{
	uint16_t network_order = htons(static_cast<uint16_t>(integer));
	add(&network_order, sizeof(uint16_t));

	return *this;
}

message& message::operator<<(int32_t const& integer)
{
	uint32_t network_order = htonl(static_cast<uint32_t>(integer));
	add(&network_order, sizeof(uint32_t));

	return *this;
}

message& message::operator<<(int64_t const& integer)
{
	uint64_t network_order = htonll(static_cast<uint64_t>(integer));
	add(&network_order, sizeof(uint64_t));

	return *this;
}


message& message::operator<<(uint8_t const& unsigned_integer)
{
	add(&unsigned_integer, sizeof(uint8_t));
	return *this;
}

message& message::operator<<(uint16_t const& unsigned_integer)
{
	uint16_t network_order = htons(unsigned_integer);
	add(&network_order, sizeof(uint16_t));

	return *this;
}

message& message::operator<<(uint32_t const& unsigned_integer)
{
	uint32_t network_order = htonl(unsigned_integer);
	add(&network_order, sizeof(uint32_t));

	return *this;
}

message& message::operator<<(uint64_t const& unsigned_integer)
{
	uint64_t network_order = htonll(unsigned_integer);
	add(&network_order, sizeof(uint64_t));

	return *this;
}

message& message::operator<<(float const& floating_point)
{
	assert(sizeof(float) == 4);

	uint32_t const host_order = *reinterpret_cast<uint32_t const*>(&floating_point);
	uint32_t network_order = htonl(host_order);
	add(&network_order, sizeof(uint32_t));

	return *this;
}

message& message::operator<<(double const& double_precision)
{
	assert(sizeof(double) == 8);

	uint64_t const host_order = *reinterpret_cast<uint64_t const*>(&double_precision);
	uint64_t network_order = htonll(host_order);
	add(&network_order, sizeof(uint64_t));

	return *this;
}

message& message::operator<<(bool const& boolean)
{
	uint8_t byte = (boolean) ? 1 : 0;
	add(&byte, sizeof(uint8_t));

	return *this;
}

message& message::operator<<(char const* c_string)
{
	add(c_string, strlen(c_string));
	return *this;
}

message& message::operator<<(std::string const& string)
{
	add(string.data(), string.size());
	return *this;
}

message::message(message&& source) noexcept
{
	std::swap(_parts, source._parts);
}

message& message::operator=(message&& source) noexcept
{
	std::swap(_parts, source._parts);
	return *this;
}

message message::copy()
{
	message msg;
	msg.copy(*this);
	return msg;
}

void message::copy(message& source)
{
	_parts.resize(source._parts.size());
	for(size_t i = 0; i < source._parts.size(); ++i)
	{
		if( 0 != zmq_msg_init_size(&_parts[i].msg, zmq_msg_size(&source._parts[i].msg)) )
		{
			throw zmq_internal_exception();
		}

		if( 0 != zmq_msg_copy(&_parts[i].msg, &source._parts[i].msg) )
		{
			throw zmq_internal_exception();
		}
	}

	// we don't need a copy of the releasers as we did data copies of the internal data,
	//_releasers = source._releasers;
	//_strings = source._strings
}

// Used for internal tracking
void message::sent(size_t const& part)
{
	// sanity check
	assert(!_parts[part].sent);
	_parts[part].sent = true;
}

// Note that these releasers are not thread safe, the only safety is provided by
// the socket class taking ownership so no updates can happen while zmq does it's thing
// If used in a custom class this has to be dealt with.
void message::release_callback(void* data, void* hint)
{
	callback_releaser* releaser = static_cast<callback_releaser*>(hint);
	releaser->func(data);

	delete releaser;
}

}
