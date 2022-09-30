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
	std::stringstream			content;

	content << std::ifstream(_server.get_root() + "/index.html").rdbuf();
	_content = content.str();

	_status = "200 OK";
}