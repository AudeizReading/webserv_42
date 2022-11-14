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

#include <toml_parser.hpp>

#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"

#define DEFAULT_PORT	5000
#define LISTEN_BACKLOG	512 // The maximum length for the queue of pending connections.


template <typename T>
T ToNum(std::string str)
{
    std::istringstream oStream(str);
    T tX = 0;
    oStream >> tX;
    return tX;
}

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

	bool				prepare_answer(int fd, Request& request, int size);
	void				answer(int fd, Request const& request);
	bool				_send(int fd, Response* response);
	void				_bind_request(Request &request);

	Server const*		_get_matching_Server(Request const& req) const;
	Location const*		_get_matching_Location(Request const& req, Server const& serv) const;

	bool				redirect_if_dir_request(Request const& req, int event_fd);

public:
	Listener(std::string const& listen_addr, int listen_port, int listen_backlog, vector_s::const_iterator servers_first, vector_s::const_iterator servers_last);

	void	start_listener();

	~Listener();
};
