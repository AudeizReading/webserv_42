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

#include "Response_4XX.hpp"

Response_Bad_Request::Response_Bad_Request(Request &request, Server &server): Response(request, server)
{
	create();
}

Response_Bad_Request::~Response_Bad_Request()
{
}

void Response_Bad_Request::_init()
{
	std::stringstream			content;

	// TODO: setting custom error like nginx
	content << std::ifstream("res/error/400.html").rdbuf();
	_content = content.str();

	_status = "400 Bad Request";
}
