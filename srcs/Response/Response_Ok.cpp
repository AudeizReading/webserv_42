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
#include "Response_4XX.hpp"

Response_Ok::Response_Ok(Request const& request, Server const& serv, Location const& location): Response(request, serv, location)
{
	create();
}

Response_Ok::~Response_Ok()
{
}

void Response_Ok::_init()
{
	const std::string	req_location = _request->get_location();
	std::string	file = req_location.substr(_location->URI().length());
	if (file[0] == '/')
		file.erase(file.begin());

	if (req_location[0] != '/')
	{
		Response				&moi = *this;
		moi = Response_Not_Found(*_request, *_server, *_location);
		return ;
	}
	_content_path = _location->get_path();
	if (_content_path.back() != '/')
		_content_path += '/';

	std::cerr << _GRN
		<< "Request location: " << _request->get_location() << '\n'
		<< "File: " << file << RESET << std::endl;

	if (!file.empty())
		_content_path += file;
	else
		_content_path += _location->index();

	_status = "200 OK";
}
