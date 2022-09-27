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
	* SO_REUSEADDR: Allows the socket to be bound to an address that is already in use.
	*
	* doc: https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-setsockopt
	*/
	/*
	// TODO: Est-ce que c'est ça que le sujet nous demande de faire ???
	unsigned re_use_addr = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &re_use_addr, sizeof(re_use_addr)) < 0)
		throw strerror(errno);
	*/
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
