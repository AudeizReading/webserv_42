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

Server::Server(std::string root, std::string name): _root(root), _name(name)
{
}

Server::~Server()
{
}

std::string Server::get_root() const
{
	return (_root);
}

std::string Server::get_name() const
{
	return (_name);
}
