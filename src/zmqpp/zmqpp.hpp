/*
 * License: http://www.opensource.org/licenses/MIT
 *
 * Copyright (C) 2011 by Ben Gray (@benjamg)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 *  Created on: 9 Aug 2011
 *      Author: Ben Gray (@benjamg)
 */

#ifndef ZMQPP_ZMQPP_HPP_
#define ZMQPP_ZMQPP_HPP_

#include <zmq.h>

#if ZMQ_VERSION_MAJOR < 3
#error cppzmq requires a later version of 0mq
#endif

#include "context.hpp"
#include "exception.hpp"
#include "message.hpp"
#include "poller.hpp"
#include "socket.hpp"

namespace zmqpp
{

std::string version();
void version(uint8_t& api, uint8_t& revision, uint8_t& age);

}

#endif /* ZMQPP_ZMQPP_HPP_ */
