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

Server::Server(std::string const& name, std::string const& listen_address, unsigned int max_body_size, int port,
		std::pair<vector_str::iterator, vector_str::iterator> serv_names, std::vector<Location> locations,
		std::map<int, std::string> error_pages)
: _name(name), _max_body_size(max_body_size), _port(port), _locations(locations), _error_pages(error_pages)
{
	_server_names.assign(serv_names.first, serv_names.second);

	// Checking this is necessary because inet_addr() is a depreciated piece of... cake.
	// See https://linux.die.net/man/3/inet_addr
	_listen_addr.s_addr = inet_addr(listen_address.c_str());
	if (_listen_addr.s_addr == INADDR_NONE && listen_address != "255.255.255.255")
		throw std::runtime_error("Invalid listen_addr IP in server block");
}

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

in_addr			Server::get_listen_addr() const
{
	return _listen_addr;
}
unsigned int	Server::get_max_body_size() const
{
	return _max_body_size;
}
int				Server::get_port() const
{
	return _port;
}
std::string		Server::get_port_str() const
{
	std::stringstream portstr;
	portstr << _port;
	return portstr.str();
}

std::vector<Location> const&	Server::get_locations() const
{
	return _locations;
}

Server::vector_str const&	Server::get_server_names() const
{
	return _server_names;
}

std::map<int, std::string> const&	Server::get_error_pages() const
{
	return _error_pages;
}

std::string	Server::get_error_page(std::string const& code) const
{
	std::map<int, std::string>::const_iterator	target = _error_pages.find(std::atoi(code.c_str()));
	return (target == _error_pages.end() ? "" : target->second);
}

std::string	Server::get_error_page(int code) const
{
	std::map<int, std::string>::const_iterator	target = _error_pages.find(code);
	return (target == _error_pages.end() ? "" : target->second);
}
