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

#include "Request.hpp"
#include "Server.hpp"

class Response
{
private:
	Server&			_server;
	std::string		_plaintext;

public:
	Response(Request &request, Server &server);

	~Response();

	operator std::string() const;

	const char*	c_str() const;
	int			length() const;
};

std::ostream& operator<<(std::ostream& os, const Response& response);
