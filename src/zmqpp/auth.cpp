/**
 * \file
 *
 * \date   25 Nov 2014
 * \author Prem Shankar Kumar (\@meprem)
 */

#include <string>
#include <utility>

#include "auth.hpp"
#include "message.hpp"
#include "exception.hpp"
#include "socket_types.hpp"
#include "signal.hpp"

#define ZAP_ENDPOINT  "inproc://zeromq.zap.01"

namespace zmqpp
{
/*!
 * Consturctor. A auth actor takes over authentication for all incoming connections in
 * its context. You can whitelist or blacklist peers based on IP address,
 * and define policies for securing PLAIN, CURVE, and GSSAPI connections.
 *
 */
auth::auth(context& ctx): terminated {false} {
    auto zap_auth_server = [this] (socket * pipe, context& auth_ctx) -> bool {
        // spawn ZAP handler
        socket zap_handler(auth_ctx, socket_type::reply);
        try {
            zap_handler.bind(ZAP_ENDPOINT);
            pipe->send(signal::ok);
        }
        catch (zmq_internal_exception &e) {
            // by returning false here, the actor will send signal::ko
            // this will make the actor constructor throw.
            // we could also to the ourselves: pipe->send(signal::ko);)
            return false;
        }

        auth_poller.add(*pipe);
        auth_poller.add(zap_handler);

        while (!terminated && auth_poller.poll()) {
            if (auth_poller.has_input(zap_handler)) {
                authenticate(zap_handler);
            }
            if (auth_poller.has_input(*pipe)) {
                handle_command(*pipe);
            }
        }
        zap_handler.unbind(ZAP_ENDPOINT);
        return true;        
    };

    // Start ZAP Authentication Server
    std::cout << "auth: Starting ZAP Authentication Server" << std::endl; 
    authenticator = std::make_shared<actor>(std::bind(zap_auth_server, std::placeholders::_1, std::ref(ctx)));
}

/*!
 * Destructor.
 *
 */
auth::~auth() {
    message msg;
	msg << "TERMINATE";
	authenticator->pipe()->send(msg);
    authenticator->pipe()->wait();
}

/*!
 * Allow (whitelist) a single IP address. For NULL, all clients from this
 * address will be accepted. For PLAIN and CURVE, they will be allowed to
 * continue with authentication. You can call this method multiple times
 * to whitelist multiple IP addresses. If you whitelist a single address,
 * any non-whitelisted addresses are treated as blacklisted.
 *
 */
void auth::allow(std::string address) {
	message msg;
	msg << "ALLOW" << address;
	authenticator->pipe()->send(msg);
	authenticator->pipe()->wait();
}

/*!
 * Deny (blacklist) a single IP address. For all security mechanisms, this
 * rejects the connection without any further authentication. Use either a
 * whitelist, or a blacklist, not not both. If you define both a whitelist
 * and a blacklist, only the whitelist takes effect.
 *
 */
void auth::deny(std::string address) {
	message msg;
	msg << "DENY" << address;
	authenticator->pipe()->send(msg);
	authenticator->pipe()->wait();
}

/*!
 * Configure a ZAP domain. To cover all domains, use "*".
 */
void auth::configure_domain(std::string domain) {
	message msg;
	assert(!domain.empty());
	msg << "DOMAIN" << domain;
	authenticator->pipe()->send(msg);
	authenticator->pipe()->wait();
}

/*!
 * Configure PLAIN authentication. PLAIN authentication uses a plain-text 
 * username and password.
 *
 */
void auth::configure_plain(std::string username, std::string password) {
	message msg;
	assert(!username.empty());
	assert(!password.empty());
	msg << "PLAIN" << username << password;

    if (verbose) {
        std::cout << "auth: configure PLAIN - username:" << username << " password:" << password << std::endl; 
    }

	authenticator->pipe()->send(msg);
    authenticator->pipe()->wait();
}

/*!
 * Configure CURVE authentication. CURVE authentication uses client public keys. 
 * This method can be called multiple times. To cover all domains, use "*". 
 * To allow all client keys without checking, specify CURVE_ALLOW_ANY for the client_public_key.
 *
 */
void auth::configure_curve(std::string client_public_key) {
	message msg;
	assert(!client_public_key.empty());
	msg << "CURVE" << client_public_key;

    if (verbose) {
        std::cout << "auth: configure CURVE - client public key:" << client_public_key << std::endl; 
    }

    authenticator->pipe()->send(msg);
	authenticator->pipe()->wait();
}

/*!
 * Configure GSSAPI authentication. GSSAPI authentication uses an underlying 
 * mechanism (usually Kerberos) to establish a secure context and perform mutual 
 * authentication.
 *
 */
void auth::configure_gssapi() {
	message msg;
	msg << "GSSAPI";

    if (verbose) {
        std::cout << "auth: configure GSSAPI - Not implemented yet!!!" << std::endl; 
    }

    authenticator->pipe()->send(msg);
	authenticator->pipe()->wait();
}

/*!
 * Enable verbose tracing of commands and activity.
 *
 */
void auth::set_verbose(bool verbose) {
    std::string verbose_string = (true == verbose) ? "true" : "false"; 
	message msg;
   	msg << "VERBOSE" << verbose_string;
    
    if(verbose) {
            std::cout << "auth: verbose:" << verbose_string << std::endl;
    }
        
    authenticator->pipe()->send(msg);
    authenticator->pipe()->wait();
}

/*!
 * Handle an authentication command from calling application.
 *
 */
void auth::handle_command(socket& pipe) {
    // Get the whole message off the pipe in one go
	message msg;
	pipe.receive(msg);

    if(0 == msg.parts())
        return;     // Interrupted

    // authentication command
	std::string command = msg.get(0);
	if (verbose) {
    	std::cout <<"auth: API command=" << command << std::endl;
    }

    if("ALLOW" == command) {
    	std::string address = msg.get(1);
    	if(verbose) {
    		std::cout << "auth: whitelisting ipaddress=" << address << std::endl;
    	}

    	whitelist.insert(address);
    	pipe.send(signal::ok); 

    } else if("DENY" == command) {
    	std::string address = msg.get(1);
    	if(verbose) {
    		std::cout << "auth: blacklisting ipaddress=" << address << std::endl;
    	}

    	blacklist.insert(address);
    	pipe.send(signal::ok); 
    	
    } else if("DOMAIN" == command) {
    	std::string domain = msg.get(1);
    	if(verbose) {
    		std::cout << "auth: domain=" << domain << std::endl;
    	}

    	this->domain = domain;
    	pipe.send(signal::ok); 
    	
    } else if("PLAIN" == command) {
    	std::string user = msg.get(1);
    	std::string pass = msg.get(2);

        if (verbose) {
            std::cout << "auth: configured PLAIN - user:" << user << std::endl; 
        }

    	passwords.insert(std::make_pair(user, pass));
    	pipe.send(signal::ok); 

    } else if("CURVE" == command) {
    	// If client_public_key is CURVE_ALLOW_ANY, allow all clients. Otherwise
    	// treat client_public_key as client public key certificate.
    	std::string client_public_key = msg.get(1);

    	if("CURVE_ALLOW_ANY" == client_public_key) {
    		allow_any = true;
            if(verbose) {
    		  std::cout << "auth: configured CURVE - allow ALL clients" << std::endl;
            }
    	} else {
    		allow_any = false;
    		client_keys.insert(client_public_key);
            if(verbose) {
    		  std::cout << "auth: configured CURVE - allow client with public key:" << client_public_key << std::endl;
            }
    	}
		pipe.send(signal::ok); 

    } else if("GSSAPI" == command) {
    	// GSSAPI authentication is not yet implemented here
        if(verbose) {
    	   std::cout << "auth: configure GSSAPI authentication is not yet implemented here" << std::endl;
        }
    	pipe.send(signal::ok); 

    } else if("VERBOSE" == command) {
    	std::string verbose_string = msg.get(1);

    	verbose = ("true" == verbose_string)? true : false;
    	pipe.send(signal::ok); 

    } else if("TERMINATE" == command) {
    	std::cout << "auth: Shutdown ZAP Authentication Server" << std::endl;
        
    	terminated = true;
        pipe.send(signal::ok); 

    } else {
        if(verbose) {
    	   std::cout << "auth: Invalid command=" << command << std::endl;
        }
    	assert(false);
    }
}

/*!
 * Handle a PLAIN authentication request from libzmq core
 *
 */
bool auth::authenticate_plain(zap_request& request) {
	auto search = passwords.find(request.get_username());
    if((search != passwords.end()) && (search->second == request.get_password())) {
        if (verbose) {
            std::cout << "auth: allowed (PLAIN) username=" << request.get_username()
        		<< " password=" << request.get_password() << std::endl;
        }
        return true;
    }
    else {
    	if (verbose) {
            std::cout << "auth: denied (PLAIN) username=" << request.get_username()
        		<< " password=" << request.get_password() << std::endl;
        }
        return false;
    }
}

/*!
 * Handle a CURVE authentication request from libzmq core
 *
 */
bool auth::authenticate_curve(zap_request& request) {
	if (allow_any) {
    	if (verbose) {
        	std::cout << "auth: allowed (CURVE allow any client)" << std::endl;
        }
    	return true;
	} else {
		auto search = client_keys.find(request.get_client_key());
    	if(search != client_keys.end()) {
    		if (verbose) {
        		std::cout << "auth: allowed (CURVE) client_key=" << request.get_client_key() << std::endl;
            }
    		return true;
    	}
    	else {
    		if (verbose) {
        		std::cout << "auth: denied (CURVE) client_key=" << request.get_client_key() << std::endl;
            }
    		return false;
    	}
	}    	
}

/*!
 * Handle a GSSAPI authentication request from libzmq core
 *
 */
bool auth::authenticate_gssapi(zap_request& request) {
	if (verbose) {
    	std::cout << "auth: allowed (GSSAPI) principal=" << request.get_principal() 
    		<< " identity=" << request.get_identity() << std::endl;
    }
	return true;	
}

/*!
 * Authentication.
 *
 */
void auth::authenticate(socket& sock) {
    // Receive a ZAP request.
	zap_request request(sock, verbose);

	if(request.get_version().empty()) {        // Interrupted
		request.reply("500", "Internal error");
    	return;     
	}

    // Is address explicitly whitelisted or blacklisted?
    bool allowed = false;
    bool denied = false;

    if(whitelist.size()) {
    	auto search = whitelist.find(request.get_address());
    	if (search != whitelist.end()) {
            allowed = true;
            if (verbose) {
                std::cout << "auth: passed (whitelist) address=" << request.get_address() << std::endl;
            }
        }
        else {
            denied = true;
            if (verbose) {
                std::cout << "auth: denied (not in whitelist) address=" << request.get_address() << std::endl;
            }
        }

    } else if(blacklist.size()) {
    	auto search = blacklist.find(request.get_address());
    	if (search != blacklist.end()) {
            denied = true;
            if (verbose) {
                std::cout << "auth: denied (blacklist) address=" << request.get_address() << std::endl;
            }
        }
        else {
            allowed = true;
            if (verbose) {
                std::cout << "auth: passed (not in blacklist) address=" << request.get_address() << std::endl;
            }
        }
    }

    // Mechanism-specific checks
    if(!denied) {
    	if (("NULL" == request.get_mechanism()) && !allowed) {
            // For NULL, we allow if the address wasn't blacklisted
            if (verbose) {
                std::cout << "auth: allowed (NULL)" << std::endl;
            }
            allowed = true;

        } else if ("PLAIN" == request.get_mechanism()) {
            // For PLAIN, even a whitelisted address must authenticate
            allowed = authenticate_plain(request);

        } else if ("CURVE" == request.get_mechanism()) {
            // For CURVE, even a whitelisted address must authenticate
            allowed = authenticate_curve(request);

        } else if ("GSSAPI" == request.get_mechanism()) {
            // For GSSAPI, even a whitelisted address must authenticate
            allowed = authenticate_gssapi(request);
        }
    }
    if (allowed)
    	request.reply("200", "OK");
    else
        request.reply("400", "No access");
}

}
