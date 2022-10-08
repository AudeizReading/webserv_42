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

Server::Server(std::string root, std::string name, std::string domain): _root(root), _name(name), _domain(domain)
{
	std::cout << "[Server::Server()] new server " << _name << std::endl;
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

std::string Server::get_domain() const
{
	return (_domain);
}
