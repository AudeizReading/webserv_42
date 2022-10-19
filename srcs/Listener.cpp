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
#include "Response/Response_Ok.hpp"
#include "Response/Response_4XX.hpp"
#include "Response/Response_Redirect.hpp"

#include <arpa/inet.h>
#include <cassert>

#define I_LOVE_ICEBERG 1

#define MAX_REQ_HEADER_BUFFER 24576

Listener::Listener(std::string const& listen_addr, int listen_port, int listen_backlog,
		vector_s::const_iterator servers_first,
		vector_s::const_iterator servers_last
	): _fd(INT_MIN), _addr(listen_addr), _port(listen_port), _listen_backlog(listen_backlog)
{
	_servers.assign(servers_first, servers_last);
	for (vector_s::const_iterator it = _servers.begin(); it != _servers.end(); ++it)
	{
		if (inet_addr(_addr.c_str()) != inet_addr(it->get_addr().c_str()))
			throw std::runtime_error("Cannot create Listener: port is binded with a different address");
	}
	if (inet_addr(_addr.c_str()) < 0)
		throw std::runtime_error("Cannot create Listener: invalid address");
	// assert(_port >= 0 && _port <= 65535);
	if (_port < 0 || _port > 65535)
		throw std::runtime_error("Cannot create Listener: invalid port number");
}

// Returns the server that matches the request, based on the listen_addr and server_name fields.
Server const*	Listener::_get_matching_Server(Request const& req) const
{
	const Request::map_ss::const_iterator find_host = req.get_header().find("Host");
	std::string	host = (find_host == req.get_header().end() ? "" : find_host->second);
	if (host.find(':') != std::string::npos)
		host.erase(host.find(':'));

	// Go through candidate servers, and find the one with the matching server_name
	Server const	*target = &*_servers.begin(); // If no server_name matches, get the first server
	for (vector_s::const_iterator it = _servers.begin(); it != _servers.end(); ++it)
	{
		if (it->has_server_name(host)) // Should I break here ?
			target = &*it;
	}
	return target;
}

// TESTME !!!
Location const*	Listener::_get_matching_Location(Request const& req, Server const& serv) const
{
	std::string req_location_URI = req.get_location();
	
	std::cerr << BCYN << "Request URI:  " << req_location_URI << RESET << ", "; // DEBUG

	// Locations are sorted from least to most complete.
	const Location*	target = &serv.get_locations().front();
	for (std::vector<Location>::const_iterator it = serv.get_locations().begin();
		it != serv.get_locations().end();
		++it)
	{
		std::cerr << _CYN << "Location URI: " << it->get_URI() << RESET << ", "; // DEBUG
		if (req_location_URI.find(it->get_URI()) != std::string::npos)
			target = &*(it);
	}
	std::cerr << std::endl;
	return target;
}

bool	Listener::_send(int fd, Response* response)
{
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

	std::cout << "[listener] send " << response->get_status() << " to socket#" << fd << std::endl;

	std::string	will_be_send = *response;
	long size = 0;

	do
	{
		will_be_send = will_be_send.substr(size);
		size = send(fd, will_be_send.c_str(), will_be_send.length(), MSG_DONTWAIT);
		std::cout << "[listener] send size: " << size << " to socket#" << fd << std::endl;
	}
	while (size > -1 && static_cast<unsigned long>(size) < will_be_send.length());

	if (size < 0)
		std::cout << "Cannot send!" << std::endl;

	delete response;

	std::cout << "[listener] close socket#" << fd << std::endl;
	_requests.erase(fd);
	close(fd);
	return (true);
}

void	Listener::_bind_request(Request &request)
{
	const Server	*server = _get_matching_Server(request);
	if (server != NULL)
	{
		std::cerr << "[listener] matched server "
			<< server->get_addr() << ':' << server->get_port() << ' '
			<< "with first server_name: " << server->get_server_names()[0] << std::endl;
		request.set_server(server);

		const Location	*location = _get_matching_Location(request, *server);
		std::cerr << "[listener] matched location: " << location->get_URI() << std::endl;
		request.set_server_location(location);
	}
	request.binded();
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
	// SO_REUSEADDR => Comportement indéterminé !, vaut mieux désactiver le TIME_WAIT avec SO_LINGER = OFF
	unsigned re_use_addr = 1;
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &re_use_addr, sizeof(re_use_addr)) < 0)
		throw std::runtime_error(strerror(errno));

	// Disable TIME_WAIT
	struct linger so_linger = { 0, 0 }; // l_onoff = 0 -> OFF, l_linger = 0sec (no timeout because OFF)
	if (setsockopt(_fd, SOL_SOCKET, SO_LINGER, &so_linger, sizeof(so_linger)) < 0)
		throw std::runtime_error(strerror(errno));

	// TIMEOUT TO 10sec
	struct timeval timeout;
	timeout.tv_sec = 10;
	timeout.tv_usec = 0;

	if (setsockopt(_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
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
	address.sin_addr.s_addr = inet_addr(_addr.c_str());

	std::cout << "[listener] bind socket#" << _fd << " to port " << _port << std::endl;
	if (bind(_fd, reinterpret_cast<struct sockaddr *>(&address), sizeof(address)) < 0)
		throw std::runtime_error(strerror(errno));

	std::cout << "[listener] listen socket#" << _fd << " (max " << _listen_backlog << ")" << std::endl;
	if (listen(_fd, _listen_backlog) < 0)
	{
		/*
		 * If a connection request arrives with the queue full, the client may receive an error
		 * with an indication of ECONNREFUSED. Alternatively, if the underlying protocol supports
		 * retransmission, the request may be ignored so that retries may succeed.
		 */
		std::cout << "[listener] Cannot listen, please retry" << std::endl;
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

		//std::cout << "[listener] check for new events for socket#" << _fd << std::endl;
		struct timespec timeout;
		timeout.tv_sec = 10;
		timeout.tv_nsec = 0;
		new_events = kevent(kq, NULL, 0, &event, 1, &timeout);
		if (new_events < 0)
			throw std::runtime_error(strerror(errno));

		for (int i = 0, event_fd; new_events > i; i++)
		{
			event_fd = event.ident;

			if (event.flags & EV_EOF)
			{
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
					std::cout << "[listener] accept socket error for event#" << event_fd << std::endl;
					perror("[listener] -- accept socket error");
					continue;
				}

				EV_SET(&change_event, new_socket, EVFILT_READ, EV_ADD, 0, 0, NULL);
				if (kevent(kq, &change_event, 1, NULL, 0, &timeout) < 0)
				{
					std::cout << "[listener] kevent error for event#" << event_fd << std::endl;
					perror("[listener] -- kevent error");
				}
			}
			else if (event.filter & EVFILT_READ)
			{
				char buffer[PIPE_BUF + 1] = {0};
				int size = recv(event_fd, buffer, PIPE_BUF, 0);
				// std::cerr << "[listener] read " << size << "/" << PIPE_BUF << " bytes for event#" << event_fd << std::endl;
				if (size < 0)
				{
					std::cout << "[listener] recv error for event#" << event_fd << std::endl;
					perror("[listener] -- recv error");
					continue;
				}

				// std::cerr << BRED"[listener]: " << __FILE__ << " " << __LINE__ << ": buffer: " << buffer << "\nsize: " << std::string(buffer).size() << RESET << std::endl;
				Listener::map_ir::iterator search = _requests.find(event_fd);
				if (search == _requests.end())
				{
					_requests.insert(Listener::pair_ir(event_fd, Request(buffer, address.sin_addr)));
					search = _requests.find(event_fd);
					search->second.set_s_sloc(&_servers[0], &_servers[0].get_locations()[0]);
				}
				else
				{
					search->second.append_plaintext(buffer);
				}

				bool C = false;
				Request &request = search->second;
				std::string length = "";
				if (request.is_parsed())
				{
					if (!request.is_complete())
						C = _send(event_fd, new Response_Bad_Request(request));
					else if (!request.is_bind())
					{
						_bind_request(request);
						if (request.get_server() == NULL) // TODO: On peut vraiment avoir une erreur ici ?
							C = _send(event_fd, new Response_Internal_Server_Error(request));
						else if (request.get_server_location()->get_redirect() != "")
							C = _send(event_fd, new Response_Redirect(request));
					}
					else if (request.get_buffer().length() > request.get_server()->get_max_body_size())
						C = _send(event_fd, new Response_Payload_Too_Large(request));
					else if (!request.get_server_location()->allows_method(request.get_method()))
						C = _send(event_fd, new Response_Method_Not_Allowed(request));
				}
				else if (request.get_buffer().length() > MAX_REQ_HEADER_BUFFER)
					C = _send(event_fd, new Response_Request_Header_Too_Large(request));
				if (C)
					continue;

				if (request.is_complete())
					length = request.get_header()["Content-Length"];

				if (length != "" && request.get_content().length() < static_cast<unsigned long>(stoi(length)))
					continue;

				request.parse();

				char	addr_str[INET_ADDRSTRLEN];
				std::cout << _RED << "[listener] Client address: "
					<< inet_ntop(AF_INET, static_cast<void*>(&address.sin_addr), addr_str, INET_ADDRSTRLEN)
					<< RESET << std::endl;

				std::cout << "[listener] ok answer at socket#" << event_fd << std::endl;
				_send(event_fd, new Response_Ok(request));
			}
			else
			{
				std::cout << "[listener] and else? event#" << event_fd << std::endl;
			}
		}
	}
}

Listener::~Listener()
{
	if (_fd == INT_MIN) // Listener hasn't been started
		return ;
	std::cout << "[listener] shutdown and close socket#" << _fd << std::endl;
	shutdown(_fd, SHUT_RDWR);
	if (close(_fd) < 0)
		throw std::runtime_error(strerror(errno));
}
