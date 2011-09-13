/*
 * compatibility.hpp
 *
 *  Created on: 10 Sep 2011
 *      Author: ron
 */

#ifndef ZMQPP_COMPATIBILITY_HPP_
#define ZMQPP_COMPATIBILITY_HPP_


#ifdef __GNUC__
#if __GNUC__ == 4


// deal with older compilers not supporting C++0x nullptr
#if __GNUC_MINOR__ < 6
#define nullptr NULL
#endif // if __GNUC_MINOR__ < 6


// deal with older compilers not supporting C++0x lambda function
#if __GNUC_MINOR__ < 5
#define ZMQPP_IGNORE_LAMBDA_FUNCTION_TESTS
#endif // if __GNUC_MINOR__ < 6


#endif // if __GNUC_
#endif // ifdef

#endif /* ZMQPP_COMPATIBILITY_HPP_ */
