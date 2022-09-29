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
#include <fstream>

#include "Response.hpp"

Response::Response(Request &request, Server &server): _server(server)
{
	std::stringstream			response;
	std::stringstream			content;

	(void) request;

	content << std::ifstream(_server.get_root() + "/index.html").rdbuf();

	response << "HTTP/1.1 200 OK\r\n";
	response << "date: Wed, 28 Sep 2022 07:25:41 GMT\r\n";
	response << "server: 42webserv\r\n";
	response << "Cache-Control: no-cache\r\n";
	response << "content-length: " << content.str().length() << "\r\n";
	response << "content-type: text/html\r\n";
	response << "\r\n";
	std::cout << _server.get_root() << std::endl;
	response << content.str();
	response << "\r\n";

	_plaintext = response.str();
}

Response::~Response()
{

}

Response::operator std::string() const
{
	return (_plaintext);
}

const char *Response::c_str() const
{
	return (_plaintext.c_str());
}

int	Response::length() const
{
	return (_plaintext.length());
}

std::ostream& operator<<(std::ostream& os, const Response& response)
{
	os << static_cast<std::string>(response);
	return (os);
}
