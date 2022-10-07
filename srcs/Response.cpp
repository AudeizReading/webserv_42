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
#include "Response/Response_4XX.hpp"
#include "CGIManager.hpp"

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
		std::string				ext = _content_path
									.substr(_content_path.find_last_of(".") + 1);

		if (good && _content_path.find("/.", 0) != std::string::npos)
			// On peut considérer que c'est un manque de sécu, de ne pas mettre 404 ici.
			*this = Response_Forbidden(_request, _server);
		else if (ext == "pl") { // TODO: is cgi extension of application
			// CGI Handling
			try 
			{
				// it would be better to access to the config file than the request because I can reach the location with Request obj,
				// but I can't reach the config file here, maybe could we get a ref on the Document inside the Listener ? -> The Server would have one so it will be handled by accesing the Server's ref config's file
				// and it seems that, with the http://nginx.org/en/docs/http/ngx_http_fastcgi_module.html
				// all infos needed by CGI is setted inside the config file.
				CGIManager cgi(_request);
				cgi.fork();
				response << "content-type: " << _content_type << "\r\n";
				response << "\r\n";
				response << "AUDDDE SUPPPRIME MOI !";
				_plaintext = response.str();
			}
			catch(const std::exception& e)
			{
				// TODO: Send error 500
				*this = Response_Bad_Request(_request, _server);
				std::cerr << "[CGI] " << e.what() << '\n';
			}
			return ;
		}
		else if (good)
		{
			std::stringstream	content;
			content << file.rdbuf();
			_content = content.str();

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
			*this = Response_Not_Found(_request, _server);
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

Response	&Response::operator=(Response const &src)
{
	this->_plaintext = src._plaintext;
	this->_request = src._request;
	this->_server = src._server;
	this->_status = src._status;
	this->_content_path = src._content_path;
	this->_content_type = src._content_type;
	this->_content = src._content;

	return (*this);
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
