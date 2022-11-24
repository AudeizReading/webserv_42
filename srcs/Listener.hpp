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
#include <vector>
#include <sys/stat.h>
#include <sys/param.h>

#include <toml_parser.hpp>

#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"

#define DEFAULT_PORT	5000
#define LISTEN_BACKLOG	2048 // The maximum length for the queue of pending connections.

class Server;
class Request;
class Response;

class Listener
{
public:
	typedef std::map<int, Request>			map_ir;
	typedef std::pair<int, Request>			pair_ir;

	typedef std::vector<Server>				vector_s;

private:
	int						_fd;
	std::string				_addr; 
	int						_port;
	int						_listen_backlog;
	vector_s				_servers;
	map_ir					_requests;

public:
	Listener(std::string const& listen_addr, int listen_port, int listen_backlog, vector_s::const_iterator servers_first, vector_s::const_iterator servers_last);

	void				start_listener();

	vector_s const&		get_servers() const;

	~Listener();
};
