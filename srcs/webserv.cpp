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

int	webserv(int argc, char *argv[])  
{
	if (argc < 0)
	{
		std::cout << "Aucun argument" << std::endl;
		return (1);
	}
	(void) argv;

	Listener			listener;

	std::cout << "Bonjour!" << std::endl;

	return (0);
}
