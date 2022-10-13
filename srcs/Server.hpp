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

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <toml_parser.hpp>
#include "Location.hpp"

class Server
{
private:
	std::string		_name; // useless ? For debugging only ?

	struct in_addr				_listen_addr; // The address this server listen to. 0.0.0.0 means all addresses.
	unsigned int				_max_body_size;
	std::vector<std::string>	_server_names;
	int							_port; // Is only here to be used in parsing.
	std::vector<Location>		_locations;
	// TODO: Error pages
	// TODO: CGI

public:
	// Not using a reference on locations is intentional. That SHOULD let us use copy elision.
	template <class InputIt>
	Server(std::string const& listen_address, unsigned int max_body_size, int port,
			std::pair<InputIt, InputIt> serv_names, std::vector<Location> locations,

			typename ft::enable_if<
				!ft::is_fundamental<InputIt>::value, int
			>::type = 0);

	~Server();

	std::string get_name() const;

	in_addr			get_listen_addr()	const { return _listen_addr;	}
	unsigned int	get_max_body_size()	const { return _max_body_size;	}
	int				get_port() 			const { return _port;			}

	std::vector<Location> const&	get_locations() const		{ return _locations;	}
	std::vector<std::string> const&	get_server_names() const	{ return _server_names;	}

	bool	has_server_name(std::string const& name) const;
};

template <class InputIt>
Server::Server(std::string const& listen_address, unsigned int max_body_size, int port,
		std::pair<InputIt, InputIt> serv_names, std::vector<Location> locations,

		typename ft::enable_if<
			!ft::is_fundamental<InputIt>::value, int
		>::type)
: _name("HELLO WORD"), _max_body_size(max_body_size), _port(port), _locations(locations)
{
	_server_names.assign(serv_names.first, serv_names.second);

	// Checking this is necessary because inet_addr() is a depreciated piece of... cake.
	// See https://linux.die.net/man/3/inet_addr
	_listen_addr.s_addr = inet_addr(listen_address.c_str());
	if (_listen_addr.s_addr == INADDR_NONE && listen_address != "255.255.255.255")
		throw std::runtime_error("Invalid listen_addr IP in server block");
}
