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

Request::Request(int fd): _fd(fd), _complete(0)
{
	_read_buffer();

	_parse();
}

void Request::_read_buffer()
{
	int							size;

	_plaintext = "";
	do {
		char buffer[PIPE_BUF] = {0};
		// or read: https://stackoverflow.com/q/1790750/
		size = recv(_fd, buffer, PIPE_BUF - 1, 0);
		if (size < 0)
			throw std::runtime_error(strerror(errno));
		_plaintext += buffer;
	} while(size == PIPE_BUF - 1); // TODO: On est bloquant en faisant ça ^^"

	// std::cerr << "\e[48;5;19m\n" << _plaintext << RESET << std::endl;
}

void Request::_parse_firstline(const std::string &str, std::string::const_iterator &it)
{
	std::string::const_iterator		end = str.begin();

	_Firstline						firstline;

	while(end < str.end() && *end != '\r')
		end++;

	if (*(end + 1) != '\n')
		throw std::runtime_error("Bad Request");

	for (; it < end && *it != ' '; it++)
		firstline.method += toupper(*it);
	it++;
	for (; it < end && *it != ' '; it++)
		firstline.uri += *it;
	it++;
	for (; it < end && *it != ' ' && *it != '\r'; it++)
		firstline.http_version += *it;

	if (*it != '\r')
		throw std::runtime_error("Bad Request");
	it += 2;

	// TODO: check http_version and throw ?
	if (firstline.http_version.rfind("HTTP/", 0) != 0) 
		throw std::runtime_error("Bad Request");

	std::string::size_type			pos = firstline.uri.find("?");

	_method = firstline.method;
	if (pos != std::string::npos)
	{
		_location = firstline.uri.substr(0, pos);
		_query = firstline.uri.substr(pos + 1, firstline.uri.length());
	}
	else
		_location = firstline.uri;

	if (_location.find("/../", 0) != std::string::npos) 
		throw std::runtime_error("Bad Request");
}

void Request::_parse_otherline(const std::string &str, std::string::const_iterator &it)
{
	std::string::const_iterator		end = str.end();

	std::string			key;
	std::string			val;

	_header.clear();
	for (std::string key, val; it < end && *it != '\r'; it += 2, key = "", val = "")
	{
		for (int i = 0; it < end && *it != ':'; it++, i++)
			key += (i == 0 || *(it - 1) == '-') ? toupper(*it) : tolower(*it);
		if (*(++it) == ' ') it++;
		for (int i = 0; it < end && *it != '\r'; it++, i++)
			val += *it;
		_header.insert(Request::pair_ss(key, val));
		if (*it != '\r' || *(it + 1) != '\n')
			throw std::runtime_error("Bad Request");
	}
	if (*it != '\r' || *(it + 1) != '\n')
		throw std::runtime_error("Bad Request");
	it += 2;
	_content.assign(it, end);
}

void Request::_parse()
{
	try
	{
		std::string::const_iterator		it = _plaintext.begin();

		_parse_firstline(_plaintext, it);
		_parse_otherline(_plaintext, it);

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
