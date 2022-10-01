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

#include <unistd.h>

#include "webserv.hpp"
#include "Request.hpp"

Request::Request(int fd): _fd(fd), _complete(0)
{
	_read_buffer();

	_parse();
}

void Request::_read_buffer()
{
	int							size;

	_plaintext = "";
	do {
		char buffer[READ_BUFFER_SIZE] = {0};
		size = read(_fd, buffer, READ_BUFFER_SIZE - 1);
		if (size < 0)
			throw std::runtime_error(strerror(errno));
		_plaintext += buffer;
	} while(size == READ_BUFFER_SIZE - 1);

	std::cout << _plaintext << std::endl;
}

void Request::_read_firstline(const std::string &str)
{
	std::string::const_iterator		it = str.begin();
	std::string::const_iterator		end = str.begin();

	while(end < str.end() && *end != '\r')
		end++;

	if (*(end + 1) != '\n')
		throw std::runtime_error("Bad Request");

	while(it < end && *it != ' ')
		_firstline.method += *it++;
	it++;
	while(it < end && *it != ' ')
		_firstline.uri += *it++;
	it++;
	while(it < end && *it != ' ' && *it != '\r')
		_firstline.http_version += *it++;

	if (*it != '\r')
		throw std::runtime_error("Bad Request");

	// TODO: check http_version and throw ?

	std::cout << "--> '" << _firstline.method << "' '" << _firstline.uri
			<< "' '" << _firstline.http_version << "'\n";
}

void Request::_parse()
{
	try
	{
		_read_firstline(_plaintext);

		// if you want test 400:
		// throw std::runtime_error("Bad Request");

		_complete = 1;
		// TODO: Parsing
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		_complete = 0;
	}
}

Request::~Request()
{

}

int	Request::is_complete() const
{
	return (_complete != 0);
}
