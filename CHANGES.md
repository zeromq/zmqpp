Version 3.2.0
=============

* Reworked client application, this is techically breaking but its only the
  the client so noone should care. Multipart support that works with linux
  pipes is now the default.
* Added a new -d detailed verbose option, the only reason it is not -vv is
  lack of boost option support. This provides diagnostic infomation.
* Added -s to support single part messages, incase anyone actually used that.

Version 3.1.0
=============

* Support for 3.2 socket disconnect and unbind.

Version 3.0.0
=============

Breaking
--------

* All constants are now lower_case rather the FULL_CAPITAL, this was due to
  defintiations conflicting with some of the values
* Message class methods that read data are now marked as const. This is only
  potentially breaking and probably will not harm most people.


Other changes
-------------

* Support for 3.2 style contexts and context options
  * context_option::io_threads to set the number of threads required. This
    must be set before sockets are created to have any effect. Default 1.
  * context_option::max_sockets to set the maximum number of sockets allowed
    by this context. Default 1024.
* Support for new 3.2 socket options
  * socket_option::delay_attach_on_connect to force connections to delay
    creating internal buffers until the connection to the remote endpoint.
  * socket_option::last_endpoint to get the last endpoint this socket was
    bound to.
  * socket_option::router_mandatory to enable routablity error checking on
    router sockets.
  * socket_option::xpub_verbose to force all subscriptions to bubble up the
    system rather than just new subscriptions.
  * socket_option::tcp_keepalive to enable tcp level keepalives, a special
    value of -1 is used for os default.
  * socket_option::tcp_keepalive_idle to force an overide of the keepalive
    count, often this is the retry count.
  * socket_option::tcp_keepalive_count to force an overide of the retry count.
  * socket_option::tcp_keepalive_interval to alter time between keepalives.
  * socket_option::tcp_accept_filter to add address based whitelist for
    incomming connections.

