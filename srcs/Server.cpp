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

Server::~Server()
{
}

std::string Server::get_name() const
{
	return (_name);
}

bool	Server::has_server_name(std::string const& name) const
{
	return std::find(_server_names.begin(), _server_names.end(), name) != _server_names.end();
}
