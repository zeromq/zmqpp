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
 * \date   25 Nov 2014
 * \author Prem Shankar Kumar (\@meprem)
 */

#ifndef ZMQPP_ZAP_REQUEST_HPP_
#define ZMQPP_ZAP_REQUEST_HPP_

#include <string>
#include "socket.hpp"

#if (ZMQ_VERSION_MAJOR > 3)

namespace zmqpp
{

/**
 * A class for working with ZAP requests and replies.
 * Used in auth to simplify working with RFC 27 messages.
 *
 */
class zap_request {
public:
    /**
     * Receive a ZAP valid request from the handler socket
     */
    zap_request(socket& handler, bool logging);
    
    /** 
     * Send a ZAP reply to the handler socket
     */
    void reply (std::string status_code, std::string status_text);

    /** 
     * Get Version
     */
    const std::string & get_version() const {
        return version;
    }

    /** 
     * Get Domain
     */
    const std::string & get_domain() const {
        return domain;
    }

    /** 
     * Get Address
     */
    const std::string & get_address() const {
        return address;
    }

    /** 
     * Get Identity
     */
    const std::string & get_identity() const {
        return identity;
    }

    /** 
     * Get Security Mechanism
     */
    const std::string & get_mechanism() const {
        return mechanism;
    }

    /** 
     * Get username for PLAIN security mechanism
     */
    const std::string & get_username() const {
        return username;
    }

    /** 
     * Get password for PLAIN security mechanism
     */
    const std::string & get_password() const {
        return password;
    }

    /** 
     * Get client_key for CURVE security mechanism
     */
    const std::string & get_client_key() const {
        return client_key;
    }

    /** 
     * Get principal for GSSAPI security mechanism
     */
    const std::string & get_principal() const {
        return principal;
    }

private:
    socket&  	    zap_socket;     //!< Socket we're talking to
    std::string     version;        //!< Version number, must be "1.0"
    std::string     sequence;       //!< Sequence number of request
    std::string     domain;         //!< Server socket domain
    std::string     address;        //!< Client IP address
    std::string     identity;       //!< Server socket idenntity
    std::string     mechanism;      //!< Security mechansim
    std::string     username;       //!< PLAIN user name
    std::string     password;       //!< PLAIN password, in clear text
    std::string     client_key;     //!< CURVE client public key in ASCII
    std::string     principal;      //!< GSSAPI client principal
    bool            verbose;        //!< Log ZAP requests and replies?

    // No copy - private and not implemented
    zap_request(zap_request const&) ZMQPP_EXPLICITLY_DELETED;
    zap_request& operator=(zap_request const&) NOEXCEPT ZMQPP_EXPLICITLY_DELETED;
};

}

#endif

#endif /* ZMQPP_ZAP_REQUEST_HPP_ */
