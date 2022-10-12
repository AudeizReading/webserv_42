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
#include <netinet/in.h>
#include <arpa/inet.h>
#include <toml_parser.hpp>

class Server
{
private:
	std::string		_root; // remove me, it's in location
	std::string		_name; // useless ?
	std::string		_domain; // Is that server_names ?

	struct in_addr				_listen_addr; // The address this server listen to. 0.0.0.0 means all addresses.
	unsigned int				_max_body_size;
	std::vector<std::string>	_server_names;
	int							_port; // Is only here to be used in parsing.
	// TODO: Error pages
	// TODO: Locations
	// TODO: CGI

public:
	Server(std::string root, std::string name, std::string domain);
	template <class InputIt>
	Server(std::string const& listen_address, unsigned int max_body_size, int port,
			InputIt serv_names_first, InputIt serv_names_last,
			typename ft::enable_if< !ft::is_fundamental<InputIt>::value, int >::type = 0);

	~Server();

	std::string get_root() const;
	std::string get_name() const;
	std::string get_domain() const;

	in_addr			get_listen_addr() const		{ return _listen_addr;		}
	unsigned int	get_max_body_size() const	{ return _max_body_size;	}
	int				get_port() const			{ return _port;				}

	std::vector<std::string> const&	get_server_names() const { return _server_names; }
	bool	has_server_name(std::string const& name) const;
};

template <class InputIt>
Server::Server(std::string const& listen_address, unsigned int max_body_size, int port,
				InputIt serv_names_first, InputIt serv_names_last,
				typename ft::enable_if< !ft::is_fundamental<InputIt>::value, int >::type)
: _max_body_size(max_body_size), _port(port)
{
	_server_names.assign(serv_names_first, serv_names_last);

	_listen_addr.s_addr = inet_addr(listen_address.c_str());
	if (_listen_addr.s_addr == INADDR_NONE && listen_address != "255.255.255.255")
		throw std::runtime_error("Invalid listen_addr IP in server block");
}
