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

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

#include "Listener.hpp"

Listener::Listener()
{
	/*
	* PF_INET: Internet version 4 protocols
	* SOCK_STREAM: TCP
	* protocol toujours 0
	*/
	fd = socket(PF_INET, SOCK_STREAM, 0); 
	std::cout << "create socket#" << fd << std::endl;
	if (fd < 0)
		throw strerror(errno);

	/*
	* SOL_SOCKET: La doc indique directement SOL_SOCKET.
	*/
	//if (setsockopt(fd, SOL_SOCKET, int optname, const void *optval, socklen_t optlen) < 0)
	//	throw strerror(errno);
}

Listener::Listener(Listener const &src)
{
	(*this) = src;
}

Listener::~Listener()
{
	std::cout << "close socket#" << fd << std::endl;
	if (close(fd) < 0)
		throw strerror(errno);
}

Listener	&Listener::operator=(Listener const &src)
{
	(void) src;

	return (*this);
}
