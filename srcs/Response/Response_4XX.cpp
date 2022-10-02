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
	// TODO: setting custom error like nginx
	_content_path = "res/error/400.html";

	_status = "400 Bad Request";
}
