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
#include <fcntl.h>

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
		if (root.back() != '/')
			root.push_back('/');

		// TODO: Array of servers
		_server = new Server(root.c_str());
	}
	catch (std::exception const& e)
	{
		std::cerr << "FATAL: Listener::Listener: Error while getting config info: " << e.what() << std::endl;
		throw;
	}
}

void	Listener::_recv(int fd)
{
	std::cout << "[listener] recv socket#" << fd << std::endl;
	Request						request(fd);
	Response					*response;

	if (!request.is_complete())
		response = new Response_Bad_Request(request, *_server);
	else
	{
		// TODO: Servers dispatch + rootage (request via HOST/LOCATION)
		response = new Response_Ok(request, *_server);

		std::cerr << "\e[30;48;5;245m\n";
		if (response->get_ctype().rfind("text/", 0) == 0)
			// Redirect STDERR to file to get primitive log
			// Leave as it for log in console
			if (response->length() < 1400)
				std::cerr << *response;
			else
				std::cerr << "<response length: " << response->length() << ">";
		else
			std::cerr << "<response: " << response->get_ctype() << ">";
		std::cerr << RESET << std::endl;
	}

	std::cout << "[listener] send " << response->get_status() << " to socket#" << fd << std::endl;

	// TODO: What if send() fails ? Or only sends some of the data ?
	// Aparently the subject forbids checking errno here...
	send(fd, response->c_str(), response->length(), MSG_DONTWAIT);

	delete response;

	std::cout << "[listener] close socket#" << fd << std::endl;
	close(fd);
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

	int flags = fcntl(_fd, F_GETFL, 0);
	//flags |= O_NONBLOCK; //TODO: C'est utile ??
	// la 404 (page trop grande) ne fonctionne pas si on met ça
	std::cout << flags << std::endl;
	std::cout << "O_RDONLY: " << (flags & O_RDONLY) << std::endl;
	std::cout << "O_WRONLY: " << (flags & O_WRONLY) << std::endl;
	std::cout << "O_RDWR: " << (flags & O_RDWR) << std::endl;
	std::cout << "O_CREAT: " << (flags & O_CREAT) << std::endl;
	std::cout << "O_EXCL: " << (flags & O_EXCL) << std::endl;
	std::cout << "O_NOCTTY: " << (flags & O_NOCTTY) << std::endl;
	std::cout << "O_TRUNC: " << (flags & O_TRUNC) << std::endl;
	std::cout << "O_APPEND: " << (flags & O_APPEND) << std::endl;
	std::cout << "O_ASYNC: " << (flags & O_ASYNC) << std::endl;
	std::cout << "O_NONBLOCK: " << (flags & O_NONBLOCK) << std::endl;
	if (fcntl(_fd, F_SETFL, flags) < 0)
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

	bzero(reinterpret_cast<char *>(&address), sizeof(address));

	address.sin_family = AF_INET;
	address.sin_port = htons(_port);
	address.sin_addr.s_addr = INADDR_ANY;

	std::cout << "[listener] bind socket#" << _fd << " to port " << _port << std::endl;
	if (bind(_fd, reinterpret_cast<struct sockaddr *>(&address), sizeof(address)) < 0)
		throw std::runtime_error(strerror(errno));
	// NOTE: @gphilipp sizeof() is a compile time macro, there's no need to create a variable
	// containing its return value

	std::cout << "[listener] listen socket#" << _fd << " (max " << _listen_backlog << ")" << std::endl;
	if (listen(_fd, _listen_backlog) < 0)
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
	struct kevent				change_event, event;

	EV_SET(&change_event, _fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);

	std::cout << "[listener] register kevent for socket#" << _fd << std::endl;
	if (kevent(kq, &change_event, 1, NULL, 0, NULL) < 0)
		throw std::runtime_error(strerror(errno));

	while (I_LOVE_ICEBERG)
	{
		int							new_events;

		std::cout << "[listener] check for new events for socket#" << _fd << std::endl;
		new_events = kevent(kq, NULL, 0, &event, 1, NULL);
		if (new_events < 0)
			throw std::runtime_error(strerror(errno));

		for (int i = 0, event_fd; new_events > i; i++)
		{
			event_fd = event.ident;

			if (event.flags & EV_EOF)
			{
				// TODO: ERROR?
				std::cout << "[listener] client has disconnected for event#" << event_fd << std::endl;
				close(event_fd);
			}
			else if (event_fd == _fd) // = Socket connection
			{
				int sockaddr_in_size = sizeof(address);
				int new_socket = accept(event_fd, reinterpret_cast<struct sockaddr *>(&address),
								reinterpret_cast<socklen_t *>(&sockaddr_in_size));

				std::cout << "[listener] accept connection #" << new_socket
							<< " for socket#" << event_fd << std::endl;
				if (new_socket < 0)
				{
					// TODO: ERROR?
					std::cout << "[listener] accept socket error for event#" << event_fd << std::endl;
					perror("[listener] -- accept socket error");
					continue;
				}

				EV_SET(&change_event, new_socket, EVFILT_READ, EV_ADD, 0, 0, NULL);
				if (kevent(kq, &change_event, 1, NULL, 0, NULL) < 0)
				{
					// TODO: ERROR?
					std::cout << "[listener] kevent error for event#" << event_fd << std::endl;
					perror("[listener] -- kevent error");
				}
			}
			else if (event.filter & EVFILT_READ)
			// On utilise `==` https://stackoverflow.com/a/12165298/
			{
				std::cout << "[listener] read bytes for event#" << event_fd << std::endl;
				_recv(event_fd);
			}
<<<<<<< HEAD
=======
			else if (event.filter == EVFILT_WRITE)
			{
				// TODO: HOW TO DO HERE???
				// from Aude: IMO, we need to send here the datas to the socket
				std::cout << "[listener] write bytes for event#" << event_fd << std::endl;
			}
>>>>>>> cgi:env + repsonse header
			else
			{
				std::cout << "[listener] and else? event#" << event_fd << std::endl;
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
