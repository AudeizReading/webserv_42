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
	std::string			_plaintext;

protected:
	Request&			_request;
	Server&				_server;
	std::string			_status;
	std::string			_content_path;
	std::string			_content_type;
	std::string			_content;

	virtual void _init() = 0;

public:
	Response(Request &request, Server &server);

	~Response();

	operator std::string() const;

	const char*	c_str() const;
	int			length() const;

	std::string	get_status() const;
	std::string	get_ctype() const;

	void		create();
};

std::ostream& operator<<(std::ostream& os, const Response& response);
