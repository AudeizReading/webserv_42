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

// NOTE: argc and argv are offset by -1. argv[0] is the first argument,
// and argc == 0 means no arguments.
int	webserv(int argc, char *argv[])
{
	if (argc == 0)
	{
		Listener	listener;

		std::cout << "Bonjour!" << std::endl;
	}
	else if (argc == 1)
	{
		std::cout << argv[0] << std::endl;
		TOML::Document	config(argv[0]);
		config.parse();
		Listener	listener(config);
		std::cout << "Bonjour!" << std::endl;
	}
	return (0);
}
