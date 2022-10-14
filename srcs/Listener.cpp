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

#include <arpa/inet.h>
#include <cassert>

#define I_LOVE_ICEBERG 1

// Returns the server that matches the request, based on the listen_addr and server_name fields.
const Server*	Listener::_get_matching_Server(Request const& req) const
{
	std::vector<const Server*>	candidates; // Servers that match the given client address

	for (std::vector<Server>::const_iterator it = _servers.begin(); it != _servers.end(); ++it)
	{
		if (it->get_listen_addr().s_addr == 0 // Means 0.0.0.0, i.e. listen to everyone
			|| it->get_listen_addr().s_addr == req.get_client_addr().s_addr)
			candidates.push_back(&(*it));
	}
	if (candidates.empty())
		return NULL;

	const Request::map_ss::const_iterator find_host = req.get_header().find("Host");
	std::string	host = (find_host == req.get_header().end() ? "" : find_host->second);
	if (host.find(':') != std::string::npos)
		host.erase(host.find(':'));

	// Go through candidate servers, and find the one with the matching server_name
	const Server	*target = candidates.front(); // If no server_name matches, get the first server
	for (std::vector<const Server*>::iterator it = candidates.begin(); it != candidates.end(); ++it)
	{
		if ((*it)->has_server_name(host)) // Should I break here ?
			target = *it;
	}
	return target;
}

// TESTME !!!
Location const&	Listener::_get_matching_Location(Request const& req, Server const& serv) const
{
	std::string req_location_URI = req.get_location();
	
	std::cerr << BCYN << "Request URI:  " << req_location_URI << RESET << std::endl; // DEBUG

	// Locations are sorted from least to most complete.
	const Location*	target = &serv.get_locations().front();
	for (std::vector<Location>::const_iterator it = serv.get_locations().begin();
		it != serv.get_locations().end();
		++it)
	{
		std::cerr << _CYN << "Location URI: " << it->URI() << RESET << '\n'; // DEBUG
		if (req_location_URI.find(it->URI()) != std::string::npos)
			target = &*(it);
	}
	return *target;
}

void	Listener::_send(int fd, Request request)
{
	std::cout << "[listener] recv socket#" << fd << std::endl;
	Response					*response;
	std::string					length = request.get_header()["Content-Length"];
	std::string					test[fd];

	if (!request.is_complete())
	{
		response = new Response_Bad_Request(request, _servers[0], _servers[0].get_locations()[0]); // TESTME
	}
	else
	{
		if (length != "" && request.get_content().length() < static_cast<unsigned long>(stoi(length)))
			std::cout << "[listener] socket partial#" << fd << std::endl;
		
		const Server	*server = _get_matching_Server(request);
		if (server == NULL)	// No server found: forbidden access.
		{
			response = new Response_Forbidden(request, _servers[0], _servers[0].get_locations()[0]);
			send(fd, response->c_str(), response->length(), MSG_DONTWAIT);
			delete response;
			close(fd);
			return ;
		}
		// FIXME: Forbidden function inet_ntoa
		std::cerr << "[listener] matched server "
			<< inet_ntoa(server->get_listen_addr()) << ':' << server->get_port() << ' '
			<< "with first server_name: " << server->get_server_names()[0] << std::endl;

		const Location&	location = _get_matching_Location(request, *server);

		std::cerr << "[listener] matched location: " << location.URI() << std::endl;

		if (!location.allows_method(request.get_method()))
			response = new Response_Method_Not_Allowed(request, *server, location);
		else
			response = new Response_Ok(request, *server, location);

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

		//std::cout << "[listener] check for new events for socket#" << _fd << std::endl;
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
			//	std::cerr << "\033[32;1m[SOCKET]: " << __FILE__ << " " << __LINE__ << "\r\naddress.sin_port: " << address.sin_port << "\r\n address.sin_addr.s_addr: " << address.sin_addr.s_addr << "\033[0m" << std::endl;
				char buffer[PIPE_BUF + 1] = {0};
				int size = recv(event_fd, buffer, PIPE_BUF, 0);
				// TODO: Limit client body size.
				std::cout << "[listener] read " << size << "/" << PIPE_BUF << " bytes for event#" << event_fd << std::endl;
				if (size < 0)
					throw "WRONG"; // TODO: ERROR ?

				std::cout << BRED"[listener]: " << __FILE__ << " " << __LINE__ << ": buffer: " << buffer << "\nsize: " << std::string(buffer).size() << RESET << std::endl;
				Listener::map_is::iterator search = _requests.find(event_fd);
				if (search == _requests.end())
				{
					_requests.insert(Listener::pair_is(event_fd, std::string(buffer)));
					search = _requests.find(event_fd);
				}
				else
					search->second += buffer;

				if (size < PIPE_BUF)
				{
					std::cout << "[listener] ok recv event#" << event_fd << std::endl;
					char	addr_str[INET_ADDRSTRLEN];
					std::cout << _RED << "[listener] Client address: "
						<< inet_ntop(AF_INET, static_cast<void*>(&address.sin_addr), addr_str, INET_ADDRSTRLEN)
						<< RESET << std::endl;
					_send(event_fd, Request(search->second, address.sin_addr));
					_requests.erase(event_fd);
				}
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
