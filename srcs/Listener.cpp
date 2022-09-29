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

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <errno.h>

#include "Listener.hpp"

Listener::Listener(int port): _port(port), _listen_backlog(LISTEN_BACKLOG)
{
	this->test_start();
}

Listener::Listener(TOML::Document const& config)
{
	try
	{
		_port			= config.at("server").at("port")		  .Int();
		_listen_backlog	= config.at("server").at("listen_backlog").Int();
	}
	catch (std::exception const& e)
	{
		std::cerr << "FATAL: Caught exception while getting config info: " << e.what() << std::endl;
		throw;
	}
	this->test_start();
}

void	Listener::test_start()
{
	/*
	* PF_INET: Internet version 4 protocols
	* SOCK_STREAM: TCP
	* protocol toujours 0
	*/
	_fd = socket(PF_INET, SOCK_STREAM, 0); 
	std::cout << "[listener] create socket#" << _fd << std::endl;
	if (_fd < 0)
		throw std::runtime_error(strerror(errno));

	/*
	* SOL_SOCKET: La doc indique directement SOL_SOCKET.
	* SO_REUSEADDR: Allows the socket to be bound to an address that is already in use.
	*
	* doc: https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-setsockopt
	*/
	unsigned re_use_addr = 1;
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &re_use_addr, sizeof(re_use_addr)) < 0)
		throw std::runtime_error(strerror(errno));

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
	int							sockaddr_in_size = sizeof(address);

	bzero(reinterpret_cast<char *>(&address), sockaddr_in_size);

	address.sin_family = AF_INET;
	address.sin_port = htons(_port);
	address.sin_addr.s_addr = INADDR_ANY;

	std::cout << "[listener] bind socket#" << _fd << " to port " << _port << std::endl;
	if (bind(_fd, reinterpret_cast<struct sockaddr *>(&address), sockaddr_in_size) < 0)
		throw std::runtime_error(strerror(errno));

	std::cout << "[listener] listen socket#" << _fd << " (max " << _listen_backlog << ")" << std::endl;
	if (listen(_fd, _listen_backlog) == -1)
	{
		/*
		 * If a connection request arrives with the queue full, the client may receive an error
		 * with an indication of ECONNREFUSED. Alternatively, if the underlying protocol supports
		 * retransmission, the request may be ignored so that retries may succeed.
		 */
		// TODO: No throw?
		std::cout << "pas obligé de throw ici :/ " << std::endl;
		throw std::runtime_error(strerror(errno));
	}

	// TODO: kqueue

	int							new_socket;

	std::cout << "[listener] accept socket#" << _fd << std::endl;
	if ((new_socket = accept(_fd, reinterpret_cast<struct sockaddr *>(&address),
			reinterpret_cast<socklen_t *>(&sockaddr_in_size))) < 0)
		throw std::runtime_error(strerror(errno));

	std::cout << "[listener] new socket#" << new_socket << std::endl;
	int							size;
	char						buffer[1024] = {0};
	std::stringstream			response;
	std::string					plaintext;

	size = read(new_socket, buffer, 1024);
	if (size < 0)
		throw std::runtime_error(strerror(errno));

	std::cout << buffer << std::endl;

	response << "HTTP/2 200 OK\r\n";
	response << "date: Wed, 28 Sep 2022 07:25:41 GMT\r\n";
	response << "server: Apache\r\n";
	response << "Cache-Control: no-cache\r\n";
	response << "content-length: 205\r\n";
	response << "content-type: text/html\r\n";
	response << "\r\n";
	response << "<html><head><title>Vous Etes Perdu ?</title></head>";
	response << "<body><h1>Perdu sur l'Internet ?</h1>";
	response << "<h2>Pas de panique, on va vous aider</h2>";
	response << "<strong><pre>    * <----- votre iceberg est ici</pre></strong>";
	response << "</body></html>\r\n";

	plaintext = response.str();

	std::cout << "[listener] send response to new socket#" << new_socket << std::endl;
	send(new_socket, plaintext.c_str(), strlen(plaintext.c_str()), 0);

	std::cout << "[listener] close new socket#" << new_socket << std::endl;
	close(new_socket);

}


Listener::Listener(Listener const &src)
{
	(*this) = src;
}

Listener::~Listener()
{
	std::cout << "[listener] shutdown and close socket#" << _fd << std::endl;
	shutdown(_fd, SHUT_RDWR);
	if (close(_fd) < 0)
		throw std::runtime_error(strerror(errno));
}

Listener	&Listener::operator=(Listener const &src)
{
	this->_fd = src._fd;
	this->_port = src._port;

	return (*this);
}
