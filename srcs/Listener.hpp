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
	typedef std::map<int, Request>			map_ir;
	typedef std::pair<int, Request>			pair_ir;

private:
	int						_fd;
	int						_port;
	int						_listen_backlog;
	std::vector<Server>		_servers;
	map_ir					_requests;

	void				answer(int fd, Request &request);
	void				bind_request(Request &request);

	Server const*		get_matching_Server(Request const& req) const;
	Location const*		get_matching_Location(Request const& req, Server const& serv) const;

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
