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

#define _DEFINE_RESPONSE(_Name, _Page, _Status) \
		_Name::_Name(Request const& request, Server const& serv, Location const& location): Response(request, serv, location) { create(); } \
		_Name::~_Name() { } \
		void _Name::_init() { _content_path = _Page; _status = _Status; } \

// TODO: setting for custom error path/directory like nginx ???
// Yes, that's something that we need to do

_DEFINE_RESPONSE(Response_Bad_Request, "res/error/400.html", "400 Bad Request")
_DEFINE_RESPONSE(Response_Forbidden, "res/error/403.html", "403 Forbidden")
_DEFINE_RESPONSE(Response_Not_Found, "res/error/404.html", "404 Not Found")
_DEFINE_RESPONSE(Response_Internal_Server_Error, "res/error/500.html", "500 Internal Server Error")

Response_Method_Not_Allowed::Response_Method_Not_Allowed(Request const& request, Server const& serv,
	Location const& location) : Response(request, serv, location)
{
	create();
}
Response_Method_Not_Allowed::~Response_Method_Not_Allowed()
{
}

void	Response_Method_Not_Allowed::_init()
{
	_content_path = "res/error/405.html";
	_status = "405 Method Not Allowed";
	_header.insert(Queryparser::pair_ss("Allow", _location->get_allowed_methods()));
}
