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

#pragma once

#include <map>
#include <toml_parser.hpp>

#include "Server.hpp"
#include "Request.hpp"

#define DEFAULT_PORT	5000
#define LISTEN_BACKLOG	512 // The maximum length for the queue of pending connections.

class Listener
{
public:
	typedef std::map<int, std::string>			map_is;
	typedef std::pair<int, std::string>			pair_is;

private:
	int						_fd;
	int						_port;
	int						_listen_backlog;
	std::vector<Server>		_servers;
	map_is					_requests;

	void				_send(int fd, Request request);
	const Server*		_get_matching_Server(Request const& req) const;
	const Location&		_get_matching_Location(Request const& req, Server const& serv) const;

public:
	template <class InputIt>
	Listener(int listen_port, int listen_backlog, InputIt servers_first, InputIt servers_last,
		typename ft::enable_if< !ft::is_fundamental<InputIt>::value, int >::type = 0);

	void	start_listener();

	~Listener();
};

template <class InputIt>
Listener::Listener(int listen_port, int listen_backlog, InputIt servers_first, InputIt servers_last,
		typename ft::enable_if< !ft::is_fundamental<InputIt>::value, int >::type)
: _fd(INT_MIN), _port(listen_port), _listen_backlog(listen_backlog)
{
	_servers.assign(servers_first, servers_last);
	// assert(_port >= 0 && _port <= 65535);
	if (_port < 0 || _port > 65535)
		throw std::runtime_error("Cannot create Listener: illegal port number");
}
