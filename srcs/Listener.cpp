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
#include <dirent.h>

#include "webserv.hpp"
#include "Listener.hpp"
#include "Response/Response_Ok.hpp"
#include "Response/Response_4XX.hpp"
#include "Response/Response_Redirect.hpp"
#include "Response/Response_Dirlist.hpp"

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

// Only does something when request doesn't end with '/'.
// Checks if requested URL is a directory. If so, redirects the client to the
// directory by appending a '/' at the end of the request's location, with a 301 redirect.
bool	Listener::redirect_if_dir_request(Request const& req, int event_fd)
{
	if (req.get_location().back() == '/')
		return false;

	Location const&	loc = *req.get_server_location();
	std::string	URI = req.get_location().substr(loc.get_URI().length());

	// std::cerr << _YEL << "request location: " << req.get_location() << '\n'
	// 	<< "Matched loc URI: " << loc.get_URI() << '\n'
	// 	<< "loc root: " << loc.get_root() << '\n'
	// 	<< "Extracted URI: " << URI << '\n'
	// 	<< "root + URI: " << loc.get_root() << (URI[0] == '/' ? "" : "/") << URI
	// 	<< RESET << std::endl;

	std::string	URI_full_path = loc.get_root() + (URI[0] == '/' ? "" : "/") + URI;
	struct stat	file_stat;
	std::memset(&file_stat, 0, sizeof(file_stat));
	stat(URI_full_path.c_str(), &file_stat);
	if (!S_ISDIR(file_stat.st_mode))
		return false;

	return _send(event_fd, new Response_Redirect_Permanent(req, req.get_location() + '/'));
}

bool	Listener::prepare_answer(int fd, Request& request, int size)
{
	bool was_sent = false;
	if (request.is_parsed())
	{
		if (!request.is_complete()) {
			std::cerr << _RED << "Hello I'm here" << RESET << std::endl;
			was_sent = _send(fd, new Response_Bad_Request(request));
		}
		else if (!request.is_bind())
		{
			_bind_request(request);
			if (request.get_server() == NULL) // TODO: On peut vraiment avoir une erreur ici ?
				was_sent = _send(fd, new Response_Internal_Server_Error(request));
			else if (request.get_server_location()->has_redirect())
				was_sent = _send(fd, new Response_Redirect(request));
			else if (!request.get_server_location()->allows_method(request.get_method()))
				was_sent = _send(fd, new Response_Method_Not_Allowed(request));
			else
				was_sent = redirect_if_dir_request(request, fd);
		}
		else if (request.get_buffer().length() > request.get_server()->get_max_body_size())
			was_sent = _send(fd, new Response_Payload_Too_Large(request));
	}
	else if (request.get_buffer().length() > MAX_REQ_HEADER_BUFFER)
		was_sent = _send(fd, new Response_Request_Header_Too_Large(request));

	if (was_sent)
		return (true);

	std::string length = "";
	if (request.is_complete())
		length = request.get_header()["Content-Length"]; // FIXME: non const method

	//std::cerr << _MAG << "Received length: " << size << RESET << std::endl;
	std::string type = request.get_header()["Content-Type"];
	if (size == PIPE_BUF || (length != ""
			&& request.get_content().length() < static_cast<unsigned long>(stoi(length))
			&& type.find("multipart/form-data; ") == std::string::npos)
		|| (type.find("multipart/form-data; ") != std::string::npos
			&& length != ""
			&& request.get_content().length() < 0.2 * static_cast<unsigned long>(stoi(length))))
		return (false);

	if (length != "" && request.get_content().length() < static_cast<unsigned long>(stoi(length)))
		std::cout << "[listener] socket partial#" << fd << std::endl;

	std::cerr << request << std::endl;

	request.do_end();

	answer(fd, request);
	return (true);
}

void	Listener::answer(int fd, Request const& request)
{
	std::cout << "[listener] ok answer at socket#" << fd << std::endl;
	Response								*response;
	std::cout << _CYN << "Request location: " << request.get_location() << std::endl;
	if (request.get_location().back() == '/') // Client has requested a directory
	{
		const Location&		serv_loc = *request.get_server_location();
		const std::string&	index_file_name = serv_loc.get_index();
		const std::string	path = serv_loc.get_root() + '/'				// Requested path, when adjusted for location root
			+ request.get_location().substr(serv_loc.get_URI().length());	// This is terrible, I'm sorry

		// Check if index file exists
		std::FILE	*file_index = std::fopen((path + '/' + index_file_name).c_str(), "r");
		DIR			*dir;
		if (file_index != NULL) // If index file exists at requested directory
		{
			fclose(file_index);
			response = new Response_Ok(request);
		}

		// Index doesn't exist but requested directory EXISTS and dir_listing (aka autoindex) is ON
		else if (serv_loc.allows_dir_listing() == true
			&& (dir = opendir(path.c_str())) != NULL )
		{
			closedir(dir);
			response = new Response_Dirlist(request, get_dir_list_html(path));
		}

		// Index doesn't exist, directory doesn't exist, dir_listing is OFF
		else if ( (dir = opendir(path.c_str())) == NULL )
			response = new Response_Not_Found(request);

		// Index doesn't exist, directory exists, and dir_listing (a.k.a. autoindex) is OFF
		else
			response = new Response_Forbidden(request);
	}
	else // Client hasn't requested a directory, just a normal file
	{
		response = new Response_Ok(request);
	}
	_send(fd, response);
}

bool	Listener::_send(int fd, Response* response)
{
	if (_port == 1234)
		throw std::runtime_error("DEBUG: THROWING BECAUSE REQUESTED ON PORT 1234");

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
	request.do_bind();
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
	// fcntl(_fd, F_SETFL, O_NONBLOCK);

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
	struct timeval timeout; // fonctionne peut-être
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

	std::cout << "[listener] bind socket#" << _fd << " to " << _addr << ':' <<_port << std::endl;
	if (bind(_fd, reinterpret_cast<struct sockaddr *>(&address), sizeof(address)) < 0)
		throw std::runtime_error(strerror(errno));

	// if (_port == 1234)
	// 	throw std::runtime_error("DEBUG: TEST EXCEPTION THROW");

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

	// TIMEOUT TO 10sec
	struct timespec				ktimeout; // fonctionne peut-être
	ktimeout.tv_sec = 10;
	ktimeout.tv_nsec = 0;

	std::cout << "[listener] register kevent for socket#" << _fd << std::endl;

	EV_SET(&change_event, _fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
	if (kevent(kq, &change_event, 1, NULL, 0, &ktimeout) < 0)
		throw std::runtime_error(strerror(errno));

	while (I_LOVE_ICEBERG)
	{
		int							new_events;

		new_events = kevent(kq, NULL, 0, &event, 1, &ktimeout);
		if (new_events < 0)
			throw std::runtime_error(strerror(errno));

		for (int i = 0, event_fd; new_events > i; i++)
		{
			event_fd = event.ident;
			if (event.filter == EVFILT_READ)	std::cerr << "EVFILT_READ: ";
			if (event.filter == EVFILT_WRITE)	std::cerr << "EVFILT_WRITE: ";
			//if (event.filter == EVFILT_EMPTY)	std::cerr << "EVFILT_EMPTY: ";
			if (event.filter == EVFILT_AIO)		std::cerr << "EVFILT_AIO: ";
			if (event.filter == EVFILT_VNODE)	std::cerr << "EVFILT_VNODE: ";
			if (event.filter == EVFILT_PROC)	std::cerr << "EVFILT_PROC: ";
			//if (event.filter == EVFILT_PROCDESC)	std::cerr << "EVFILT_PROCDESC: ";
			if (event.filter == EVFILT_SIGNAL)	std::cerr << "EVFILT_SIGNAL: ";
			if (event.filter == EVFILT_TIMER)	std::cerr << "EVFILT_TIMER: ";
			if (event.filter == EVFILT_USER)	std::cerr << "EVFILT_USER: ";

			if (event.flags & EV_ADD)			std::cerr << "EV_ADD ";
			if (event.flags & EV_ENABLE)		std::cerr << "EV_ENABLE ";
			if (event.flags & EV_DISABLE)		std::cerr << "EV_DISABLE ";
			if (event.flags & EV_DISPATCH)		std::cerr << "EV_DISPATCH ";
			if (event.flags & EV_DELETE)		std::cerr << "EV_DELETE ";
			if (event.flags & EV_RECEIPT)		std::cerr << "EV_RECEIPT ";
			if (event.flags & EV_ONESHOT)		std::cerr << "EV_ONESHOT ";
			if (event.flags & EV_CLEAR)			std::cerr << "EV_CLEAR ";
			if (event.flags & EV_EOF)			std::cerr << "EV_EOF ";
			if (event.flags & EV_ERROR)			std::cerr << "EV_ERROR ";

			std::cerr << std::endl;

			if (event.flags & EV_EOF)
			{
				std::cout << "[listener] client has disconnected for event#" << event_fd << std::endl;
				_requests.erase(event_fd);
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
				if (kevent(kq, &change_event, 1, NULL, 0, &ktimeout) < 0)
				{
					std::cout << "[listener] kevent error for event#" << event_fd << std::endl;
					perror("[listener] -- kevent error");
				}
			}
			else if (event.filter == EVFILT_READ)
			/*
			 * This event coming each block of 512 (PIPE_BUF) character of the request, until we :
			 * A) Read all the data (also sometime, content cannot be thrust)
			 * B) close
			 * Also, we can send() before A) or B).
			 */
			{
				char buffer[PIPE_BUF + 1] = {0};
				int size = recv(event_fd, buffer, PIPE_BUF, 0);
				if (size < 0)
				{
					std::cout << "[listener] recv error for event#" << event_fd << std::endl;
					perror("[listener] -- recv error");
					continue;
				}

				Listener::map_ir::iterator search = _requests.find(event_fd);
				if (search == _requests.end())
				{
					_requests.insert(Listener::pair_ir(event_fd, Request(address)));
					search = _requests.find(event_fd);
					search->second.set_s_sloc(&_servers[0], &_servers[0].get_locations()[0]);
					std::cout << "[listener] Start recv socket#" << event_fd
						<< " ip: " << search->second.get_addr()
						<< " host: " << search->second.get_host() << std::endl;
				}
				search->second.append_plaintext(buffer);

				if (prepare_answer(event_fd, search->second, size))
				{
					// Do something after send & close (one time for each request)
					continue ;
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
	_requests.clear();
	if (close(_fd) < 0)
		throw std::runtime_error(strerror(errno));
}
