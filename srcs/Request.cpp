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

Request::Request(int fd): _fd(fd)
{
	int							size;
	std::stringstream			request;

	do {
		char buffer[READ_BUFFER_SIZE] = {0};
		size = read(_fd, buffer, READ_BUFFER_SIZE - 1);
		if (size < 0)
			throw std::runtime_error(strerror(errno));
		request << buffer;
	} while(size == READ_BUFFER_SIZE - 1);

	std::cout << request.str() << std::endl;

	// TODO: Parsing
}

Request::~Request()
{

}