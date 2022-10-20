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
#include <map>
#include <netinet/in.h>
#include "Queryparser.hpp"

#include "Server.hpp"
#include "Location.hpp"

class Server;
class Location;

class Request
{
public:
	typedef std::map<std::string, std::string>			map_ss;
	typedef std::pair<std::string, std::string>			pair_ss;
	typedef std::vector<std::string>					vector_str;

private:
	bool			_complete;
	bool			_parsed;
	bool			_bind;

	std::string		_plaintext;
	std::string::difference_type	_content_start;

	std::string		_method;
	std::string		_location;
	std::string		_query;
	std::string		_content;

	std::string		_addr;
	std::string		_host;

	map_ss			_header;

	const Server	*_server;
	const Location	*_server_location;

	void		_parse_firstline(const std::string &str, std::string::const_iterator &it);
	
	Queryparser ::Firstline _get_first_line() const;

public:
	Request(struct sockaddr_in address);

	~Request();

	void			parse();

	void			append_plaintext(std::string const& buffer);

	bool			is_complete() const;
	bool			is_parsed() const;
	bool			is_bind() const;

	void			do_bind();
	void			do_end();

	std::string		get_location() const;
	std::string		get_query() const;
	std::string		get_content() const;
	std::string		get_method() const;
	std::string		get_http_version() const;
	std::string		get_buffer() const;

	std::string		get_addr() const;
	std::string		get_host() const;

	Server const*	get_server() const;
	void			set_server(Server const* src);

	Location const*	get_server_location() const;
	void			set_server_location(Location const* src);

	void			set_s_sloc(Server const* serv, Location const* sloc);

	map_ss &		get_header();
	map_ss const&	get_header() const;

	in_addr			get_client_addr() const;
};

std::ostream& operator<<(std::ostream& os, Request const& request);
