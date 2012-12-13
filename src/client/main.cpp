/*
 *  Created on: 16 Aug 2011
 *      Author: @benjamg
 */

#include <array>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include <zmqpp/zmqpp.hpp>

#ifndef BUILD_CLIENT_NAME
#define BUILD_CLIENT_NAME "zmqpp"
#endif

boost::program_options::options_description connection_options()
{
	boost::program_options::options_description options("Connection Options");
	options.add_options()
		("bind,b", boost::program_options::value<std::vector<std::string>>(), "bind to specified endpoint")
		("connect,c", boost::program_options::value<std::vector<std::string>>(), "connect to specified endpoint")
		("multipart,m", "enable multipart message sending")
		;

	return options;
}

boost::program_options::options_description miscellaneous_options()
{
	boost::program_options::options_description options("Miscellaneous Options");
	options.add_options()
		("version", "display version")
		("help", "show this help page")
		;

	return options;
}

void usage(std::string const& executable_name)
{
	std::cout << "Usage: " << executable_name << " [options] SOCKETTYPE [--bind|--connect] ENDPOINT" << std::endl;
	std::cout << "0mq command line client tool." << std::endl;
	std::cout << "SOCKETTYPE is one of the supported 0mq socket types." << std::endl;
	std::cout << "ENDPOINT is any valid 0mq endpoint." << std::endl;
}

typedef std::tuple<zmqpp::socket_type, bool, bool, bool> socket_type_data;

int main(int argc, char const* argv[])
{
	boost::program_options::positional_options_description arguments;
	arguments.add("type", 1);
	arguments.add("connect", 1);

	boost::program_options::options_description all;
	all.add_options()
		("type", "0mq socket type")
		;
	all.add(miscellaneous_options());
	all.add(connection_options());

	boost::program_options::variables_map vm;
	bool usage = false;

	std::map<std::string, socket_type_data> socket_types;
	socket_types["push"] = socket_type_data(zmqpp::socket_type::push, true, false, false);
	socket_types["pull"] = socket_type_data(zmqpp::socket_type::pull, false, true, false);

	socket_types["pub"] = socket_type_data(zmqpp::socket_type::publish, true, false, false);
	socket_types["sub"] = socket_type_data(zmqpp::socket_type::subscribe, false, true, false);

	socket_types["req"] = socket_type_data(zmqpp::socket_type::request, true, false, true);
	socket_types["rep"] = socket_type_data(zmqpp::socket_type::reply, false, true, true);

	try {
		boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(all).positional(arguments).run(), vm);
	}
	catch(boost::program_options::too_many_positional_options_error& e)
	{
		std::cout << "Too many arguments provided." << std::endl;
		usage = true;
	}
	catch(boost::program_options::unknown_option& e)
	{
		std::cout << "Unknown option '" << e.get_option_name() << "'." << std::endl;
		usage = true;
	}

	if (vm.count("type") && (socket_types.end() == socket_types.find(vm["type"].as<std::string>())))
	{
		std::cout << "Unknown value '" << vm["type"].as<std::string>() << "' provided for 0mq socket type." << std::endl;
		usage = true;
	}

	if (vm.count("version"))
	{
		uint8_t major, minor, patch;
		zmqpp::zmq_version(major, minor, patch);

		std::cout << BUILD_CLIENT_NAME << " version " << BUILD_VERSION << std::endl;
		std::cout << "  built against 0mq version " << static_cast<int>(major) << "." << static_cast<int>(minor) << "." << static_cast<int>(patch) << std::endl;

		return EXIT_SUCCESS;
	}

	if (usage || (0 == vm.count("type")) || vm.count("help") ||
			((0 == vm.count("connect")) && (0 == vm.count("bind"))))
	{
		std::cout << "Usage: " BUILD_CLIENT_NAME " [options] SOCKETTYPE ENDPOINT" << std::endl;
		std::cout << "0mq command line client tool." << std::endl;
		std::cout << "SOCKETTYPE is one of the supported 0mq socket types." << std::endl;

		auto it = socket_types.begin();
		std::cout << "  " << (*it).first;
		for(++it; it != socket_types.end(); ++it)
		{
			std::cout << ", " << (*it).first;
		}
		std::cout << std::endl;

		std::cout << "ENDPOINT is any valid 0mq endpoint." << std::endl;
		std::cout << std::endl;

		if (!vm.count("help"))
		{
			return EXIT_FAILURE;
		}

		std::cout << connection_options() << std::endl;
		std::cout << miscellaneous_options() << std::endl;
		return EXIT_SUCCESS;
	}

	int standardin = fileno(stdin);
	if (standardin < 0) // really?
	{
		std::cout << "Unable to get standard input, this might be an OS thing, sorry." << std::endl;
		return EXIT_FAILURE;
	}

	socket_type_data data = socket_types[vm["type"].as<std::string>()];
	zmqpp::socket_type type = std::get<0>(data);
	bool can_send = std::get<1>(data);
	bool can_recv = std::get<2>(data);
	bool toggles = std::get<3>(data);

	zmqpp::context context;
	zmqpp::socket socket(context, type);

	if ("sub" == vm["type"].as<std::string>())
	{
		socket.subscribe("");
	}

	if (vm.count("bind"))
	{
		std::vector<std::string> endpoints = vm["bind"].as<std::vector<std::string>>();
		for(size_t i = 0; i < endpoints.size(); ++i)
		{
			std::cout << "binding to " << endpoints[i] << std::endl;
			try
			{
				socket.bind(endpoints[i]);
			}
			catch(zmqpp::zmq_internal_exception& e)
			{
				std::cout << "failed to bind to endpoint: " << e.what() << std::endl;
				return EXIT_FAILURE;
			}
		}
	}

	if (vm.count("connect"))
	{
		std::vector<std::string> endpoints = vm["connect"].as<std::vector<std::string>>();
		for(size_t i = 0; i < endpoints.size(); ++i)
		{
			std::cout << "connecting to " << endpoints[i] << std::endl;
			try
			{
				socket.connect(endpoints[i]);
			}
			catch(zmqpp::zmq_internal_exception& e)
			{
				std::cout << "failed to bind to endpoint: " << e.what() << std::endl;
				return EXIT_FAILURE;
			}
		}
	}

	bool multipart = (vm.count("multipart") > 0);

	zmqpp::poller poller;
	poller.add(socket);
	poller.add(standardin);

	if (can_send || toggles)
	{
		std::cout << "While sending packets is allowed data entered on standard in will be sent to the 0mq socket." << std::endl;
		std::cout << "messages will be considered terminated by newline." << std::endl;
		std::cout << std::endl;

		if (toggles && !can_send)
		{
			std::cout << "Sending starts as disabled for this socket type." << std::endl;
			std::cout << std::endl;
		}
	}

	if (multipart)
	{
		std::cout << "Multipart messages enabled, newline on an empty line will be considered packet end." << std::endl;
		std::cout << "The empty part will not be included." << std::endl;
		std::cout << std::endl;
	}

	zmqpp::message message;
	while(true)
	{
		poller.check_for(socket, (can_recv) ? zmqpp::poller::POLL_IN : zmqpp::poller::POLL_NONE);
		poller.check_for(standardin, (can_send) ? zmqpp::poller::POLL_IN : zmqpp::poller::POLL_NONE);

		if(poller.poll())
		{
			if (poller.has_input(socket))
			{
				assert(can_recv);

				do
				{
					std::string message;
					socket.receive(message);
					std::cout << "<<: " << message << std::endl;
				} while(socket.has_more_parts());
				std::cout << " --- " << std::endl;

				if (toggles)
				{
					can_recv = false;
					can_send = true;
					std::cout << "**: Sending now enabled" << std::endl;
				}
			}

			if (poller.has_input(standardin))
			{
				assert(can_send);

				std::array<char, 512> buffer;
				char* result = fgets(buffer.data(), buffer.size(), stdin);
				if (nullptr == result)
				{
					std::cout << "!!: Error in standard input" << std::endl;
					return EXIT_FAILURE;
				}

				buffer[strlen(buffer.data()) - 1] = 0;
				if (!multipart || (strlen(buffer.data()) > 0))
				{
					message.add(buffer.data(), strlen(buffer.data()));
				}

				if (!multipart || (strlen(buffer.data()) == 0))
				{
					for(size_t i = 0; i < message.parts(); ++i)
					{
						std::cout << ">>: " << message.get<std::string>(i) << std::endl;
					}

					if (!socket.send(message, true))
					{
						std::cout << "!!: Send failed, socket would have blocked" << std::endl;
					}

					if (toggles)
					{
						can_recv = true;
						can_send = false;
						std::cout << "**: Sending now disabled" << std::endl;
					}
				}
			}
		}
	}

	return EXIT_SUCCESS;
}
