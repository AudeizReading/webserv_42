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
#include <ctime>

#include "Response.hpp"

Response::Response(Request &request, Server &server): _request(request), _server(server)
{

}

void Response::create()
{
	std::stringstream			response;

	_init();

	_content_type = "text/html";
	if (_content_path != "" && _content == "")
	{
		std::ifstream			file(_content_path);
		int						good = file.good();

		if (good && _content_path.find("/.", 0) != std::string::npos)
		{
			// TODO: throw ???
			_content = "Error 403";
			_status = "403 Forbidden";
		}
		else if (good)
		{
			std::stringstream	content;
			content << file.rdbuf();
			_content = content.str();

			std::string ext = _content_path.substr(_content_path.find_last_of(".") + 1);
			// https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Common_types
			// TODO: Support more MIME types
			if (ext == "ico")
				_content_type = "image/vnd.microsoft.icon";
			else if (ext == "png")
				_content_type = "image/png";
			else if (ext == "html")
				_content_type = "text/html";
			else
				_content_type = "text/plain";
		}
		else
		{
			// TODO: throw ???
			_content = "Error 404";
			_status = "404 Not Found";
		}
	}

	time_t						ctime = time(NULL);
	tm							*t = gmtime(&ctime);

	response << get_status() << "\r\n";
	// TODO: full date support
	response << "date: Wed, 28 Sep " << t->tm_year << " "
		<< t->tm_hour << ":" << t->tm_min << ":" << t->tm_sec << " GMT" << "\r\n";
	response << "server: 42webserv\r\n";
	response << "Cache-Control: no-cache\r\n";
	response << "content-length: " << _content.length() << "\r\n";
	response << "content-type: " << _content_type << "\r\n";
	response << "\r\n";
	response << _content;

	_plaintext = response.str();
}

Response::~Response()
{
}

Response::operator std::string() const
{
	if (_plaintext == "")
		throw std::runtime_error("Response not created before use");
	return (_plaintext);
}

const char *Response::c_str() const
{
	if (_plaintext == "")
		throw std::runtime_error("Response not created before use");
	return (_plaintext.c_str());
}

int	Response::length() const
{
	if (_plaintext == "")
		throw std::runtime_error("Response not created before use");
	return (_plaintext.length());
}

std::string	Response::get_status() const
{
	return ("HTTP/1.1 " + _status);
}

std::string	Response::get_ctype() const
{
	return (_content_type);
}

std::ostream& operator<<(std::ostream& os, const Response& response)
{
	os << static_cast<std::string>(response);
	return (os);
}
