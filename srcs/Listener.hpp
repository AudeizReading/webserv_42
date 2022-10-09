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
	std::vector<Server *>	_servers;
	map_is					_requests;

	void	_send(int fd, std::string plaintext);

public:
	Listener(TOML::Document const& config);

	void	start_listener();

	~Listener();
};
