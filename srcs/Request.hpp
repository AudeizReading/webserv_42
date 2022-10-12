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

class Request
{
public:
	typedef std::map<std::string, std::string>			map_ss;
	typedef std::pair<std::string, std::string>			pair_ss;

private:
	int				_complete;
	std::string		_plaintext;

	struct _Firstline {
		std::string method;
		std::string uri;
		std::string http_version;
	};

	std::string		_method;
	std::string		_location;
	std::string		_query;
	std::string		_content;

	map_ss			_header;

	in_addr			_client_addr;

	void		_parse();

	void		_parse_firstline(const std::string &str, std::string::const_iterator &it);
	void		_parse_otherline(const std::string &str, std::string::const_iterator &it, map_ss &header);

public:
	Request(std::string plaintext);
	Request(std::string const& plaintext, in_addr client_in_addr);

	~Request();

	int			is_complete() const;

	std::string	get_location() const;
	std::string	get_query() const;
	std::string	get_content() const;
	std::string	get_method() const;

	map_ss &		get_header();
	map_ss const&	get_header() const;

	in_addr		get_client_addr() const	{ return _client_addr; }
};
