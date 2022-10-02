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

#include "Response_Ok.hpp"

Response_Ok::Response_Ok(Request &request, Server &server): Response(request, server)
{
	create();
}

Response_Ok::~Response_Ok()
{
}

void Response_Ok::_init()
{
	std::string					location = _request.get_location();

	if (location[0] != '/')
	{
		// TODO: throw ???
		_content = "Error 404";
		_status = "404 Not Found";
		return ;
	}
	if (*(location.end() - 1) == '/')
		location = location.substr(1) + "index.html"; // TODO: Setting for default home
	else
		location = location.substr(1);

	_content_path = _server.get_root() + location;

	_status = "200 OK";
}