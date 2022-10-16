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

class Location;

class Server
{
public:
	typedef std::vector<std::string>	vector_str;

private:
	std::string		_name; // useless ? For debugging only ?

	struct in_addr						_listen_addr; // The address this server listen to. 0.0.0.0 means all addresses.
	unsigned int						_max_body_size;
	vector_str							_server_names;
	int									_port;
	std::vector<Location>				_locations;
	std::map<int, std::string>			_error_pages;
	// TODO: CGI

public:
	// Not using a reference on locations is intentional. That SHOULD let us use copy elision.
	Server(std::string const& name, std::string const& listen_address, unsigned int max_body_size, int port,
			std::pair<vector_str::iterator, vector_str::iterator> serv_names, std::vector<Location> locations,
			std::map<int, std::string> error_pages);

	~Server();

	std::string get_name() const;

	in_addr			get_listen_addr()	const;
	unsigned int	get_max_body_size()	const;
	int				get_port() 			const;
	std::string		get_port_str()		const;

	std::vector<Location> const&		get_locations() const;
	vector_str const&					get_server_names() const;
	std::map<int, std::string> const&	get_error_pages() const;
	std::string							get_error_page(std::string const& code) const;
	std::string							get_error_page(int code) const;

	bool	has_server_name(std::string const& name) const;
};
