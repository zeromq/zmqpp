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
