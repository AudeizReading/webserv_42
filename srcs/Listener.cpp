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
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <errno.h>

#include "Listener.hpp"

Listener::Listener(int port): port(port)
{
	/*
	* PF_INET: Internet version 4 protocols
	* SOCK_STREAM: TCP
	* protocol toujours 0
	*/
	fd = socket(PF_INET, SOCK_STREAM, 0); 
	std::cout << "[listener] create socket#" << fd << std::endl;
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

	/*
	* En fonction du premier argument de socket, la doc nous guide vers sockaddr_in
	* doc: https://man7.org/linux/man-pages/man7/ip.7.html
	* 
	* struct sockaddr_in {
	* 	sa_family_t			sin_family;	// address family: AF_INET
	* 	in_port_t			sin_port;	// port in network byte order
	* 	struct in_addr		sin_addr;	// internet address
	* };
	* 
	*/
	struct sockaddr_in			address;
	bzero(reinterpret_cast<char *>(&address), sizeof(address));

	address.sin_family = AF_INET;
	address.sin_port = port;
	address.sin_addr.s_addr = INADDR_ANY;

	std::cout << "[listener] bind socket#" << fd << " to port " << port << std::endl;
	if (bind(fd, reinterpret_cast<struct sockaddr *>(&address), sizeof(struct sockaddr_in)) < 0)
		throw strerror(errno);

	std::cout << "[listener] listen socket#" << fd << " (max " << LISTEN_BACKLOG << ")" << std::endl;
	if (listen(fd, LISTEN_BACKLOG) == -1)
	{
		/*
		 * If a connection request arrives with the queue full, the client may receive an error
		 * with an indication of ECONNREFUSED. Alternatively, if the underlying protocol supports
		 * retransmission, the request may be ignored so that retries may succeed.
		 */
		// TODO: No throw?
		std::cout << "pas obligé de throw ici :/ " << std::endl;
		throw strerror(errno);
	}
}

Listener::Listener(Listener const &src)
{
	(*this) = src;
}

Listener::~Listener()
{
	std::cout << "[listener] close socket#" << fd << std::endl;
	if (close(fd) < 0)
		throw strerror(errno);
}

Listener	&Listener::operator=(Listener const &src)
{
	this->fd = src.fd;
	this->port = src.port;

	return (*this);
}
