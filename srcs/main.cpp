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

#include "webserv.hpp"

int	main(int argc, char *argv[])  
{
	try
	{
		return webserv(argc - 1, &argv[1]);
	}
	catch(const std::exception& e)
	{
		std::cerr << BRED << e.what() << RESET << '\n';
		return (1);
	}
}
