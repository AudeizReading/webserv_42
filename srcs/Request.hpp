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

class Request
{
public:
	typedef std::map<std::string, std::string>			map_ss;
	typedef std::pair<std::string, std::string>			pair_ss;

private:
	int				_fd;
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

	void		_read_buffer();
	void		_parse();

	void		_parse_firstline(const std::string &str, std::string::const_iterator &it);
	void		_parse_otherline(const std::string &str, std::string::const_iterator &it);

public:
	Request(int fd);

	~Request();

	int			is_complete() const;
	std::string	get_location() const;
};
