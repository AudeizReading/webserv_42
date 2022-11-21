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
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#include <toml_parser.hpp>

#include "webserv.hpp"
#include "Request.hpp"

Request::Request(struct sockaddr_in address) : _complete(0), _parsed(0),
	_bind(false), _plaintext("")
{
	_server = NULL;
	_server_location = NULL;

	char	addr_str[INET_ADDRSTRLEN] = {0};
	char	host_str[NI_MAXHOST] = {0};
	inet_ntop(AF_INET, static_cast<void*>(&address.sin_addr), addr_str, INET_ADDRSTRLEN);
	getnameinfo(reinterpret_cast<struct sockaddr *>(&address), sizeof(struct sockaddr_in),
		host_str, NI_MAXHOST, NULL, 0, NI_NAMEREQD);
	_addr = addr_str;
	_host = host_str;
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

std::ostream& operator<<(std::ostream& os, Request const& request)
{
	os << "\e[30;48;5;245m\n";

	os << "Method: " << request.get_method() << " - ";
	os << "Location: " << request.get_location() << std::endl;
	if (request.get_query().length() > 0)
		os << "Query: " << request.get_query() << std::endl;
	if (request.get_content().length() > 0)
	{
		if (request.get_content().length() < 1400)
			os << "Content: " << request.get_content() << std::endl;
		else
			os << "Content: <_content length: "
				<< request.get_content().length() << ">" << std::endl;
	}

	os << "Headers (" << request.get_header().size() << "):" << std::endl;
	for (Request::map_ss::const_iterator it2 = request.get_header().begin();
			it2 != request.get_header().end(); it2++)
		os << " - " << it2->first << ": " << it2->second << std::endl;

	os << RESET << std::endl;
	return (os);
}

void Request::parse()
{
	_parsed = 1;
	try
	{
		std::string::const_iterator		it = _plaintext.begin();

		_parse_firstline(_plaintext, it);
		_content = Queryparser::parse_otherline(_plaintext, it, _header);
		_content_start = it - _plaintext.begin();

		std::cerr << "Request parsed: " << _method << " " << _location << std::endl;
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
	if (!_parsed && _plaintext.find("\r\n\r\n") != std::string::npos)
		parse();
	if (_parsed)
		_content = _plaintext.substr(_content_start);
	//std::cerr << "[request]: _plaintext [\033[35m" << _plaintext << "\033[0m] end buffer\n";
	//std::cerr << "[request]: _content [\033[36m" << _content << "\033[0m] end buffer\n";
}


void	Request::do_end()
{
	// ACTUELLEMENT NON UTILISE mais peut etre utile pour le CGI et avoir une map
	// de chacun des fichiers envoyées en upload
	// Necessite de comprendre comment convertir de l'octect-stream (string) en
	// binary

	std::string					type = _header["Content-Type"];

	if (type.find("multipart/form-data; ") == std::string::npos
		|| type.find("boundary=") == std::string::npos)
		return ;

	std::cout << "multipart + boundary detected" << std::endl;
	vector_str					files;  // TODO: finish this?

	std::string					boundary = "--" + type.substr(type.find("boundary=") + 9);
	if (boundary == "--")
		return ;

	std::string					str = _content;
	std::string::size_type		next_boundary = str.find(boundary);

	if (next_boundary == std::string::npos)
		std::cout << "no boundary in content :(" << std::endl;

	while (next_boundary != std::string::npos) {
		std::string::const_iterator
			it = str.begin() + next_boundary + boundary.length() + 2;


		Queryparser::map_ss		header;
		Queryparser::parse_otherline(str, it, header);
		for (Queryparser::map_ss::const_iterator it2 = header.begin();
			it2 != header.end(); it2++)
			std::cerr << " - " << it2->first << ": " << it2->second << std::endl;
		str = str.substr(it - str.begin());

		std::ofstream outfile ("test.txt", std::ofstream::binary); // TODO: finish this?

		next_boundary = str.find(boundary);
		std::string out = str;
		if (next_boundary != std::string::npos)
			out = out.substr(0, next_boundary);
		// outfile.write(reinterpret_cast<const char*>(out.c_str()), out.length());
		outfile << out << std::endl;

		outfile.close();
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

bool	Request::is_complete() const
{
	return (_complete);
}

bool	Request::is_parsed() const
{
	return (_parsed);
}

bool	Request::is_bind() const
{
	return (_bind);
}

void	Request::do_bind()
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

std::string	Request::get_buffer() const
{
	return (_plaintext);
}

std::string	Request::get_addr() const
{
	return (_addr);
}

std::string	Request::get_host() const
{
	return (_host);
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

void	Request::set_s_sloc(Server const* serv, Location const* sloc)
{
	set_server(serv);
	set_server_location(sloc);
}

Request::map_ss&	Request::get_header()
{
	return (_header);
}

Request::map_ss const&	Request::get_header() const
{
	return (_header);
}
