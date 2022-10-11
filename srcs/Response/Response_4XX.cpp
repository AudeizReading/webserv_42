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
		_Name::_Name(Request &request, Server &server): Response(request, server) { create(); } \
		_Name::~_Name() { } \
		void _Name::_init() { _content_path = _Page; _status = _Status; } \

// TODO: setting for custom error path/directory like nginx ???

_DEFINE_RESPONSE(Response_Bad_Request, "res/error/400.html", "400 Bad Request")
_DEFINE_RESPONSE(Response_Forbidden, "res/error/403.html", "403 Forbidden")
_DEFINE_RESPONSE(Response_Not_Found, "res/error/404.html", "404 Not Found")
_DEFINE_RESPONSE(Response_Internal_Server_Error, "res/error/500.html", "500 Internal Server Error")
