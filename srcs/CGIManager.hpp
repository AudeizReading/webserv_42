
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
		int				_fds[2];
		map_ss			_env;
		size_t			_content_length;
		std::string		_plaintext;


		CGIManager(void);
		CGIManager(const CGIManager &src);
		CGIManager& operator=(const CGIManager &src);

		CGIManager&	_setEnv();

	public:
		CGIManager(const Request& req);
		~CGIManager(void);

		map_ss		getEnv() const;
		std::string	getPlainText() const;

		bool	pipe();
		bool	getCGIResponse();
		bool	exec();
		bool	launchExec() const;
		
};
