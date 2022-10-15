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

Request::Request(std::string const& plaintext, in_addr client_in_addr) : _complete(0), _parsed(0),
	_plaintext(""), _client_addr(client_in_addr)
{
	_server = NULL;
	_server_location = NULL;
	append_plaintext(plaintext);
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

void Request::parse()
{
	_parsed = 1;
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

void	Request::append_plaintext(std::string const& buffer)
{
	_plaintext += buffer;
	if (_plaintext.find("\r\n\r\n") != std::string::npos)
	{
		parse();
		// TODO: On pourrait déjà envoyer une bad request ici
	}
}

Queryparser::Firstline Request::_get_first_line() const
{
	try
	{
		std::string::const_iterator	it = _plaintext.begin();
		Queryparser::Firstline		firstline = Queryparser::parse_req_firstline(_plaintext, it);

		return firstline;
	}
	catch(const std::runtime_error& e)
	{
		std::cerr << e.what() << '\n';
	}
	return Queryparser::Firstline();
}


Request::~Request()
{

}

int	Request::is_complete() const
{
	return (_complete != 0);
}

int	Request::is_parsed() const
{
	return (_parsed != 0);
}

int	Request::is_bind() const
{
	return (_bind != 0);
}

void	Request::binded()
{
	_bind = 1;
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

std::string	Request::get_method() const
{
	return (_method);
}

std::string	Request::get_http_version() const
{
	Queryparser::Firstline	firstline = this->_get_first_line();

	return (firstline.http_version);
}

Server const*	Request::get_server() const
{
	if (!_server)
		throw std::runtime_error("Server request not binded!");
	return (_server);
}

void	Request::set_server(Server const* src)
{
	_server = src;
}

Location const*	Request::get_server_location() const
{
	if (!_server_location)
		throw std::runtime_error("Server location request not binded!");
	return (_server_location);
}

void	Request::set_server_location(Location const* src)
{
	_server_location = src;
}

Request::map_ss&	Request::get_header()
{
	return (_header);
}

Request::map_ss const&	Request::get_header() const
{
	return (_header);
}

in_addr		Request::get_client_addr() const
{
	return _client_addr;
}
