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
#include "Queryparser.hpp"

Response::Response(Request const& request): _request(&request)
{

}

void Response::create()
{
	std::string					ext;

	_init();

	if (!_request->get_server_location()->allows_method(_request->get_method()))
	{
		*this = Response_Method_Not_Allowed(*_request);
		return ;
	}

	_content_type = "text/html";
	if (_content_path != "" && _content == "")
	{
		std::cerr << "[Response::create()] " << _content_path << std::endl;
		ext = _content_path.substr(_content_path.find_last_of(".") + 1);

		std::ifstream			file(_content_path);
		int						good = file.good();

		if (good && _content_path.find("/.", 0) != std::string::npos)
		{
			// On peut considérer que c'est un manque de sécu, de ne pas mettre 404 ici.
			*this = Response_Forbidden(*_request);
			return ;
		}
		/*
		 * NOTE: I commented out the CGI part because I didn't want to touch it yet.
		 */
		else if (good && ext == "pl") { // TODO: is cgi extension of application
			// CGI Handling
			try 
			{
				CGIManager cgi(*_request, *_request->get_server(), *_request->get_server_location());
				cgi.exec();
				_plaintext = cgi.getPlainText();
				
				std::cout << "\033[31;1m[CGI]: " << __FILE__ << " " << __LINE__ << ": _plaintext (response of CGI): " << _plaintext << "\033[0m" << std::endl;
			}
			catch(const std::exception& e)
			{
				std::cerr << "[CGI] " << e.what() << std::endl;
				std::cerr << "[STOP] " << _content_path << std::endl;
				*this = Response_Internal_Server_Error(*_request);
				return ;
			}
			std::cerr << "[CONTINUE] " << _content_path << std::endl;
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
		{
			*this = Response_Not_Found(*_request);
			return ;
		}
	}

	time_t						ctime = time(NULL);
	tm							*t = gmtime(&ctime);
	std::stringstream			date;
	// TODO: full date support
	date << "Wed, 28 Sep " << t->tm_year << " "
		<< t->tm_hour << ":" << t->tm_min << ":" << t->tm_sec << " GMT";

	_header.insert(Queryparser::pair_ss("Date", date.str()));
	_header.insert(Queryparser::pair_ss("Server", _request->get_server()->get_name()));
	_header.insert(Queryparser::pair_ss("Cache-Control", "no-cache"));
	_header.insert(Queryparser::pair_ss("Content-Type", _content_type));

	if (ext == "ico" || ext == "png" || ext == "jpg") // Mise en cache
	{
		_header["Cache-Control"] = "public, max-age=604800, immutable";
		_header["Age"] = "0";
	}

	if (_plaintext != "")
	{
		std::string::const_iterator		it = _plaintext.begin();

		try
		{
			Queryparser::parse_resp_firstline(_plaintext, it);
			std::cout << "[CGI] Send a full custom header" << std::endl;
			return ;
		}
		catch(const std::exception& e)
		{
			std::cout << "[CGI] Not a full custom header (" << e.what() << ")" << std::endl;
		}

		try
		{
			map_ss		temp_header;
			it = _plaintext.begin();

			_content = Queryparser::parse_otherline(_plaintext, it, temp_header);
			for (map_ss::iterator it2 = temp_header.begin(); it2 != temp_header.end(); ++it2)
				_header[it2->first] = it2->second;
			std::cout << "[CGI] Use a partial custom header" << std::endl;
		}
		catch(const std::exception& e)
		{
			_content = _plaintext;
			std::cout << "[CGI] No partial custom header (" << e.what() << ")" << std::endl;
		}
	}

	std::stringstream			response;

	response << get_status() << "\r\n";
	std::stringstream	length;
	length << _content.length();
	_header.insert(Queryparser::pair_ss("Content-Length", length.str()));
	Request::map_ss::iterator	it2;
	for (it2 = _header.begin(); it2 != _header.end(); it2++)
		response << it2->first << ": " << it2->second << "\r\n";
	response << "\r\n";
	response << _content;

	_plaintext = response.str();
}

Response::~Response()
{
}

Response	&Response::operator=(Response const &src)
{
	this->_plaintext	= src._plaintext;
	this->_request		= src._request;
	this->_status		= src._status;
	this->_content_path	= src._content_path;
	this->_content_type	= src._content_type;
	this->_content		= src._content;
	this->_header		= src._header;

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
