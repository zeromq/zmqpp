/*
 * options.hpp
 *
 *  Created on: 6 Nov 2013
 *      Author: bgray
 */

#ifndef OPTIONS_HPP_
#define OPTIONS_HPP_

#include <vector>

#include <zmqpp/zmqpp.hpp>

struct client_options
{
	bool show_usage;
	bool show_help;
	bool show_version;

	bool verbose;
	bool detailed;
	bool annotate;

	zmqpp::socket_type type;

	std::vector<zmqpp::endpoint_t> binds;
	std::vector<zmqpp::endpoint_t> connects;

	bool singlepart;
	bool exit_on_empty;

	client_options()
		: show_usage( false ), show_help( false ), show_version( false )
		, verbose( false ), detailed( false ), annotate( false )
		, type( zmqpp::socket_type::pair ), binds(), connects()
		, singlepart( false ), exit_on_empty( false )
	{ }
};

client_options process_command_line(int argc, char const* argv[]);
std::ostream& show_usage(std::ostream& stream, std::string const& application_name);
std::ostream& show_help(std::ostream& stream);

#endif /* OPTIONS_HPP_ */
