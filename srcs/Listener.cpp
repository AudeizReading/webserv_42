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
#include <sys/event.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <errno.h>

#include "webserv.hpp"
#include "Listener.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Response/Response_Ok.hpp"
#include "Response/Response_4XX.hpp"

#define I_LOVE_ICEBERG 1

Listener::Listener(TOML::Document const& config)
{
	try
	{
		_port				= config.at("server").at("port").Int();
		_listen_backlog		= config.at("server").at("listen_backlog").Int();
		std::string root	= config.at("demo").at("www").at("root").Str();

		// TODO: Array of servers
		_server = new Server(root.c_str());

		if (root.back() == '/')
			root.erase(root.end() - 1);
		this->start_listener();	
	}
	catch (std::exception const& e)
	{
		std::cerr << "FATAL: Caught exception while getting config info: " << e.what() << std::endl;
		throw;
	}
}

int	Listener::_accept(int fd, struct sockaddr_in &address, int sockaddr_in_size)
{
	int							new_socket;

	std::cout << "[listener] accept socket#" << fd << std::endl;
	new_socket = accept(fd, reinterpret_cast<struct sockaddr *>(&address),
					reinterpret_cast<socklen_t *>(&sockaddr_in_size));

	if (new_socket < 0)
	{
		// TODO: ERROR?
		std::cout << "[listener] accept socket error for event#" << fd << std::endl;
		perror("[listener] -- accept socket error");
		return (-1);
	}

	std::cout << "[listener] new socket#" << new_socket << std::endl;
	Request						request(new_socket);
	Response					*response;

	if (!request.is_complete())
		response = new Response_Bad_Request(request, *_server);
	else
	{
		// TODO: Servers dispatch + rootage (request via HOST/LOCATION)
		response = new Response_Ok(request, *_server);

		// Redirect STDERR to file to get primitive log
		// Leave as it for log in console
		if (response->get_ctype().rfind("text/", 0) == 0)
			std::cerr << "\e[30;48;5;245m\n" << *response << RESET << std::endl;
		else
			std::cerr << "\e[30;48;5;245m\n" << "<response:" << response->get_ctype() << ">" << RESET << std::endl;
	}

	std::cout << "[listener] send " << response->get_status() << " to new socket#" << new_socket << std::endl;

	send(new_socket, response->c_str(), response->length(), 0);

	delete response;

	if (1)  // TODO: Doit-t-on close ici ? --> oui si on send, sinon non
	{
		std::cout << "[listener] close new socket#" << new_socket << std::endl;
		close(new_socket);
		return (-1);
	}
	return (new_socket);
}

void	Listener::start_listener()
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

	// src: https://dev.to/frevib/a-tcp-server-with-kqueue-527
	int							kq = kqueue();
	struct kevent				change_event[4], event[4];

	EV_SET(change_event, _fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);

	std::cout << "[listener] register kevent for socket#" << _fd << std::endl;
	if (kevent(kq, change_event, 1, NULL, 0, NULL) == -1)
		throw std::runtime_error(strerror(errno));

	while (I_LOVE_ICEBERG)
	{
		int							new_events;

		std::cout << "[listener] check for new events for socket#" << _fd << std::endl;
		new_events = kevent(kq, NULL, 0, event, 1, NULL);
		if (new_events == -1)
			throw std::runtime_error(strerror(errno));

		for (int i = 0, event_fd; new_events > i; i++)
		{
			event_fd = event[i].ident;

			if (event[i].flags & EV_EOF)
			{
				// TODO: ERROR?
				std::cout << "[listener] client has disconnected for event#" << event_fd << std::endl;
				close(event_fd);
			}
			else if (event_fd == _fd) // = Socket connection
			{
				int					new_socket;

				std::cout << "[listener] client disconnection for socket#" << event_fd << std::endl;

				new_socket = _accept(event_fd, address, sockaddr_in_size);

				if (new_socket < 0)
					continue;

				EV_SET(change_event, new_socket, EVFILT_READ, EV_ADD, 0, 0, NULL);
				if (kevent(kq, change_event, 1, NULL, 0, NULL) < 0)
				{
					// TODO: ERROR?
					std::cout << "[listener] kevent error for event#" << event_fd << std::endl;
					perror("[listener] -- kevent error");
				}
			}
			else if (event[i].filter & EVFILT_READ)
			{
				std::cout << "[listener] read bytes for event#" << event_fd << std::endl;
				char buf[1024];
				size_t bytes_read = recv(event_fd, buf, sizeof(buf), 0);
				std::cout << "[listener] -- " << bytes_read << "for event#" << event_fd << std::endl;
				printf("read %zu bytes\n", bytes_read);
			}
		}
	}
}

Listener::~Listener()
{
	delete _server;
	std::cout << "[listener] shutdown and close socket#" << _fd << std::endl;
	shutdown(_fd, SHUT_RDWR);
	if (close(_fd) < 0)
		throw std::runtime_error(strerror(errno));
}
