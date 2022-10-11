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

#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <unistd.h>

#include <toml_parser.hpp>

#include "webserv.hpp"
#include "Request.hpp"
#include "Queryparser.hpp"

Request::Request(std::string plaintext): _complete(0), _plaintext(plaintext)
{
	_parse();
}

void Request::_parse_firstline(const std::string &str, std::string::const_iterator &it)
{
	Queryparser::Firstline firstline = Queryparser::parse_req_firstline(str, it);

	std::string::size_type			pos = firstline.uri.find("?");

	_method = firstline.method;
	if (pos != std::string::npos)
	{
		_location = firstline.uri.substr(0, pos);
		_query = firstline.uri.substr(pos + 1, firstline.uri.length());
	}
	else
		_location = firstline.uri;

	for (std::string::iterator it = _location.begin(); it != _location.end(); ++it)
	{
		if (!(('a' <= *it && *it <= 'z') || ('A' <= *it && *it <= 'Z') || ('0' <= *it && *it <= '9')
			|| *it == '-' || *it == '_' || *it == '/' || *it == '.'))
		{
			throw std::runtime_error("Bad Request: Forbidden character");
		}
	}

	if (_location.find("/../", 0) != std::string::npos) 
		throw std::runtime_error("Bad Request: Forbidden previous folder");
}

void Request::_parse()
{
	try
	{
		std::string::const_iterator		it = _plaintext.begin();

		_parse_firstline(_plaintext, it);
		_content = Queryparser::parse_otherline(_plaintext, it, _header);

		std::cerr << "\e[30;48;5;245m\n";

		std::cout << "Method: " << _method << " - ";
		std::cout << "Location: " << _location << std::endl;
		if (_query.length() > 0)
			std::cout << "Query: " << _query << std::endl;
		if (_content.length() > 0)
		{
			if (_content.length() < 1400)
				std::cout << "Content: " << _content << std::endl;
			else
				std::cout << "Content: <_content length: "
					<< _content.length() << ">" << std::endl;
		}

		map_ss::iterator it2;
		std::cerr << "Headers (" << _header.size() << "):" << std::endl;
		for (it2 = _header.begin(); it2 != _header.end(); it2++)
			std::cerr << " - " << it2->first << ": " << it2->second << std::endl;

		std::cerr << RESET << std::endl;

		// if you want test 400, uncomment this line:
		// throw std::runtime_error("Bad Request");

		_complete = 1;
	}
	catch(const std::runtime_error& e)
	{
		std::cerr << e.what() << '\n';
		_complete = 0;
	}
}

Request::~Request()
{

}

int	Request::is_complete() const
{
	return (_complete != 0);
}

std::string	Request::get_location() const
{
	return (_location);
}

std::string	Request::get_query() const
{
	return (_query);
}

std::string	Request::get_content() const
{
	return (_content);
}

Request::map_ss	Request::get_header() const
{
	return (_header);
}

std::string	Request::get_method() const
{
	return (_method);
}

