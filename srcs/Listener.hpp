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

#include <toml_parser.hpp>

#include "Server.hpp"

#define DEFAULT_PORT	5000
#define LISTEN_BACKLOG	512 // The maximum length for the queue of pending connections.

class Listener
{
private:
	int						_fd;
	int						_port;
	int						_listen_backlog;
	std::vector<Server *>	_servers;

	void	_recv(int fd);

public:
	Listener(TOML::Document const& config);

	void	start_listener();

	~Listener();
};
