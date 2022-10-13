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
	const Request*		_request;
	const Server*		_server;
	std::string			_status;
	std::string			_content_path;
	std::string			_content_type;
	std::string			_content;

	virtual void _init() = 0;

public:
	Response(Request const& request, Server const& server);

	virtual ~Response();

	Response	&operator=(Response const &src);

	operator std::string() const;

	const char*	c_str() const;
	int			length() const;

	std::string	get_status() const;
	std::string	get_ctype() const;

	void		create();
};

std::ostream& operator<<(std::ostream& os, const Response& response);
