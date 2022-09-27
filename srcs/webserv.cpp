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

int	webserv(int argc, char *argv[])  
{
	if (argc < 0)
	{
		std::cout << "Aucun argument" << std::endl;
		return (1);
	}
	(void) argv;
	std::cout << "Bonjour!" << std::endl;

	return (0);
}
