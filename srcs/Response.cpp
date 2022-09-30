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

#include "Response.hpp"

Response::Response(Request &request, Server &server): _server(server)
{
	(void) request;
}

void Response::create()
{
	std::stringstream			response;

	_init();

	response << "HTTP/1.1" << _status << "\r\n";
	response << "date: Wed, 28 Sep 2022 07:25:41 GMT\r\n";
	response << "server: 42webserv\r\n";
	response << "Cache-Control: no-cache\r\n";
	response << "content-length: " << _content.length() << "\r\n";
	response << "content-type: text/html\r\n";
	response << "\r\n";
	response << _content;
	response << "\r\n";

	_plaintext = response.str();
}

Response::~Response()
{
}

Response::operator std::string() const
{
	if (_plaintext == "")
		throw std::runtime_error("Response not created before use");
	return (_plaintext);
}

const char *Response::c_str() const
{
	if (_plaintext == "")
		throw std::runtime_error("Response not created before use");
	return (_plaintext.c_str());
}

int	Response::length() const
{
	if (_plaintext == "")
		throw std::runtime_error("Response not created before use");
	return (_plaintext.length());
}

std::ostream& operator<<(std::ostream& os, const Response& response)
{
	os << static_cast<std::string>(response);
	return (os);
}
