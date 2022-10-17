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

Response_Ok::Response_Ok(Request const& request): Response(request)
{
	create();
}

Response_Ok::~Response_Ok()
{
}

void Response_Ok::_init()
{
	const std::string	req_location = _request->get_location();
	std::string	file = req_location.substr(_request->get_server_location()->get_URI().length());
	if (file[0] == '/')
		file.erase(file.begin());

	if (req_location[0] != '/')
	{
		Response				&moi = *this;
		moi = Response_Not_Found(*_request);
		return ;
	}
	_content_path = _request->get_server_location()->get_root();
	if (_content_path.back() != '/')
		_content_path += '/';

	std::cerr << _GRN
		<< "Request location: " << _request->get_location() << ", "
		<< "File: " << file << RESET << std::endl;

	if (!file.empty())
		_content_path += file;
	else
		_content_path += _request->get_server_location()->get_index();

	_status = "200 OK";
}
