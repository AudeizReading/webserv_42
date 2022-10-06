
#pragma once

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

#include "Request.hpp"

// Following https://www.ietf.org/rfc/rfc3875.txt
class CGIManager {
	public:
		typedef std::map<std::string, std::string>			map_ss;

	private:

		const Request&	_request;
		int&			_socket;
		int				_fds[2];
		map_ss			_env;
		size_t			_content_length;


		CGIManager(void);
		CGIManager(const CGIManager &src);
		CGIManager& operator=(const CGIManager &src);

		CGIManager&	_setEnv();

	public:
		CGIManager(const Request& req, int& sock);
		~CGIManager(void);

		map_ss	getEnv() const;

		bool	pipe();
		bool	fork();
		bool	exec() const;
		
};
