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
#include <toml_parser.hpp>

int	main(int argc, char *argv[])  
{
	return (webserv(argc - 1, &argv[1]));
}
