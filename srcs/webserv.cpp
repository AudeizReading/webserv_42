/*  */
/*  */
/*  */
/*  */
/*  */
/*  */
/*  */
/*  */
/*  */
/*  */
/*  */

#include <iostream>

#include "webserv.hpp"
#include "Listener.hpp"

#include <csignal>
#include <iostream>

void signal_handler(int signal)
{
	(void) signal;
	exit(0);
}

// NOTE: argc and argv are offset by -1. argv[0] is the first argument,
// and argc == 0 means no arguments.
int	webserv(int argc, char *argv[])
{	
	if (argc != 1)
	{
		std::cerr << "Error: missing configuration file path" << std::endl;
		return 1;
	}
	std::signal(SIGINT, signal_handler);

	std::cout << "Config file: " << argv[0] << std::endl;
	TOML::Document	config(argv[0]);
	config.parse();
	Listener	listener(config);
	std::cout << "Bonjour!" << std::endl;
	return (0);
}
