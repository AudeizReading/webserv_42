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

#define _DEFINE_RESPONSE(_Name, _Page, _Status, _StatusCode) \
		_Name::_Name(Request const& request, Server const& serv, Location const& location): Response(request, serv, location) \
		{ \
			_content_path = serv.get_error_page(_StatusCode); \
			create(); \
		} \
		_Name::~_Name() { } \
		void _Name::_init() { _status = _Status; } \

_DEFINE_RESPONSE(Response_Bad_Request,				"res/error/400.html", "400 Bad Request",			400)
_DEFINE_RESPONSE(Response_Forbidden, 				"res/error/403.html", "403 Forbidden", 				403)
_DEFINE_RESPONSE(Response_Not_Found,				"res/error/404.html", "404 Not Found", 				404)
_DEFINE_RESPONSE(Response_Internal_Server_Error,	"res/error/500.html", "500 Internal Server Error",	500)

// ============================================================================================== //
// ------------------------------------- Manual Definitions ------------------------------------- //
// ============================================================================================== //

Response_Method_Not_Allowed::Response_Method_Not_Allowed(Request const& request, Server const& serv,
	Location const& location) : Response(request, serv, location)
{
	_content_path = serv.get_error_page(E_METHOD_NOT_ALLOWED);
	create();
}
Response_Method_Not_Allowed::~Response_Method_Not_Allowed()
{
}

void	Response_Method_Not_Allowed::_init()
{
	_status = "405 Method Not Allowed";
	_header.insert(Queryparser::pair_ss("Allow", _location->get_allowed_methods()));
}
