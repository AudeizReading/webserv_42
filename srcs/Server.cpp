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
	for (std::vector<std::string>::const_iterator it = _server_names.begin();
		it != _server_names.end();
		++it)
	{
		if (name == *it)
			return true;
	}
	return false;
}
