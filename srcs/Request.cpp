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

// Request::Request(std::string plaintext): _complete(0), _plaintext(plaintext)
// {
// 	_parse();
// }

Request::Request(std::string const& plaintext, in_addr client_in_addr) : _complete(0),
	_plaintext(plaintext), _client_addr(client_in_addr)
{
	_parse();
}

void Request::_parse_firstline(const std::string &str, std::string::const_iterator &it)
{
	std::string::const_iterator		end = str.begin();

	_Firstline						firstline;

	while(end < str.end() && *end != '\r')
		end++;

	if (*(end + 1) != '\n')
		throw std::runtime_error("Bad Request: Missparsed end of firstline");

	for (; it < end && *it != ' '; it++)
		firstline.method += toupper(*it);
	it++;
	for (; it < end && *it != ' '; it++)
		firstline.uri += *it;
	it++;
	for (; it < end && *it != ' ' && *it != '\r'; it++)
		firstline.http_version += *it;

	if (*it != '\r')
		throw std::runtime_error("Bad Request: Too many element on firstline");
	it += 2;

	// TODO: check http_version and throw ?
	if (firstline.http_version.rfind("HTTP/", 0) != 0) 
		throw std::runtime_error("Bad Request: Bad HTTP_VERSION");

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

void Request::_parse_otherline(const std::string &str, std::string::const_iterator &it, map_ss &header)
{
	std::string::const_iterator		end = str.end();

	std::string			key;
	std::string			val;

	header.clear();
	for (std::string key, val; it < end && *it != '\r'; it += 2, key = "", val = "")
	{
		for (int i = 0; it < end && *it != ':'; it++, i++)
			key += (i == 0 || *(it - 1) == '-') ? toupper(*it) : tolower(*it);
		if (*(++it) == ' ') it++;
		for (int i = 0; it < end && *it != '\r'; it++, i++)
			val += *it;
		header.insert(Request::pair_ss(key, val));
		if (*it != '\r' || *(it + 1) != '\n')
			throw std::runtime_error("Bad Request: Missparsed header");
	}
	if (*it != '\r' || *(it + 1) != '\n')
		throw std::runtime_error("Bad Request: Missparsed end of header");
	it += 2;
	_content.assign(it, end);
}

void Request::_parse()
{
	try
	{
		std::string::const_iterator		it = _plaintext.begin();

		_parse_firstline(_plaintext, it);
		_parse_otherline(_plaintext, it, _header);

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

Request::map_ss&	Request::get_header()
{
	return (_header);
}

Request::map_ss const&	Request::get_header() const
{
	return (_header);
}

std::string	Request::get_method() const
{
	return (_method);
}
