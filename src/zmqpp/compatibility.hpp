/**
 * \file
 *
 * \date   10 Sep 2011
 * \author ron
 * \author Ben Gray (\@benjamg)
 *
 * A fair number of C++0x (or more accurately C++11) features are used in this
 * library and as this project is used where I work on older compilers this
 * file was created to help.
 *
 * C++ features and their workaround where not supported:
 * \li lambda functions - disabled, these are only used in the test anyway.
 * \li typesafe enums   - replaced with enum where comparisons needed.
 * \li nullptr          - defined to null.
 *
 * As of the port to version 3.1 (libzmqpp version 1.1.0) this file will also
 * be used to maintain compatablity with multiple versions of 0mq
 */

#ifndef ZMQPP_COMPATIBILITY_HPP_
#define ZMQPP_COMPATIBILITY_HPP_

#include <zmq.h>

// currently we require at least 0mq version 3.0.x
#define ZMQPP_REQUIRED_ZMQ_MAJOR 3
#define ZMQPP_REQUIRED_ZMQ_MINOR 0

#if (ZMQ_VERSION_MAJOR < ZMQPP_REQUIRED_ZMQ_MAJOR) or ((ZMQ_VERSION_MAJOR == ZMQPP_REQUIRED_ZMQ_MAJOR) and (ZMQ_VERSION_MINOR < ZMQPP_REQUIRED_ZMQ_MINOR))
#error zmqpp requires a later version of 0mq
#endif

// experimental feature support
#if (ZMQ_VERSION_MAJOR == 3) and (ZMQ_VERSION_MINOR == 0)
#define ZMQ_EXPERIMENTAL_LABELS
#endif

// currently if your not using gcc or it's a major version other than 4 you'll have to deal with it yourself
#ifdef __GNUC__
#if __GNUC__ == 4

// deal with older compilers not supporting C++0x typesafe enum class name {} comparison
#if __GNUC_MINOR__ < 4
#define ZMQPP_COMPARABLE_ENUM enum
#endif

#if __GNUC_MINOR__ == 4
#if __GNUC_PATCHLEVEL__ < 1
#undef ZMQPP_COMPARABLE_ENUM
#define ZMQPP_COMPARABLE_ENUM enum
#endif // if __GNUC_PATCHLEVEL__ < 1
#endif // if __GNUC_MINOR__ == 4

// deal with older compilers not supporting C++0x lambda function
#if __GNUC_MINOR__ < 5
#define ZMQPP_IGNORE_LAMBDA_FUNCTION_TESTS
#endif // if __GNUC_MINOR__ < 5

// deal with older compilers not supporting C++0x nullptr
#if __GNUC_MINOR__ < 6
#define nullptr NULL
#define noexcept
#endif // if __GNUC_MINOR__ < 6

#endif // if __GNUC_ == 4
#endif // if __GNUC_

// generic state, assume a modern compiler
#ifndef ZMQPP_COMPARABLE_ENUM
#define ZMQPP_COMPARABLE_ENUM enum class
#endif

#endif /* ZMQPP_COMPATIBILITY_HPP_ */

