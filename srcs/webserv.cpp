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

	try
	{
		Listener			listener;

		std::cout << "Bonjour!" << std::endl;
	}
	catch (const std::exception& e)
	{
	    std::cout << "webserv failed because: " << e.what() << std::endl;
		return (1);
	}

	return (0);
}
