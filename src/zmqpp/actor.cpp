/* 
 * File:   actor.cpp
 * Author: xaqq
 * 
 * Created on May 20, 2014, 10:51 PM
 */

#include "actor.hpp"
#include "socket.hpp"
#include <cassert>
#include "message.hpp"
#include "exception.hpp"
#include "context.hpp"

#include <iostream>

zmqpp::context zmqpp::actor::actor_pipe_ctx_;

namespace zmqpp
{

    actor::actor(ActorStartRoutine routine) :
    parent_pipe_(nullptr),
    child_pipe_(nullptr),
    stopped_(false)
    {
        std::string pipe_endpoint = "inproc://zmqpp::actor::" + std::to_string(reinterpret_cast<ptrdiff_t> (this));
        parent_pipe_ = new socket(actor_pipe_ctx_, socket_type::pair);
        parent_pipe_->bind(pipe_endpoint);

        child_pipe_ = new socket(actor_pipe_ctx_, socket_type::pair);
        child_pipe_->connect(pipe_endpoint);

        std::thread t(&actor::start_routine, this, child_pipe_, routine);
        t.detach();

        signal sig;
        parent_pipe_->receive(sig);
        assert(sig == signal::ok || sig == signal::ko);
        if (sig == signal::ko)
        {
            delete parent_pipe_;
            throw actor_initialization_exception();
        }
    }

    actor::~actor()
    {
        stop(true);
        delete parent_pipe_;
    }

    bool actor::stop(bool block)
    {
        parent_pipe_->send(signal::stop, true);
        if (!block)
        {
            return true;
        }
        else
        {
            if (stopped_)
                return retval_;

            // wait for a response from the child. Either it successfully shutdown, or not.
            signal sig = parent_pipe_->wait();
            stopped_ = true;
            assert(sig == signal::ok || sig == signal::ko);
            return (retval_ = (sig == signal::ok));
        }
    }

    void actor::start_routine(socket *child_pipe, ActorStartRoutine routine)
    {
        if (routine(child_pipe))
            child_pipe->send(signal::ok);
        else
            child_pipe->send(signal::ko);
        delete child_pipe;
    }

    socket* actor::pipe()
    {
        return parent_pipe_;
    }

    const socket* actor::pipe() const
    {
        return parent_pipe_;
    }

}