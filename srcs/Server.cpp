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

#include "Server.hpp"

Server::Server(): _root("")
{
}

Server::Server(std::string root): _root(root)
{
}

Server::~Server()
{
}

std::string Server::get_root()
{
	return (_root);
}
