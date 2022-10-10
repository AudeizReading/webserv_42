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

#include <iostream>
#include <sstream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "Request.hpp"
#include "Server.hpp"

#define PRINT(x) std::cerr << std::boolalpha << "\nin " << __FILE__ << ":" << __LINE__ << "\n"<< __func__ << ": " << #x << "\n" << x << std::endl;
// Following https://www.ietf.org/rfc/rfc3875.txt
class CGIManager {
	public:
		typedef std::map<std::string, std::string>			map_ss;

	private:

		const Request&	_request;
		const Server&	_server;
		int				_fds[2];
		map_ss			_env;
		size_t			_content_length;
		std::string		_plaintext;


		CGIManager(void);
		CGIManager(const CGIManager &src);
		CGIManager& operator=(const CGIManager &src);

		void		_putenv(const char *name, const char *value);
		CGIManager&	_setEnv();

	public:
		CGIManager(const Request& req, const Server& serv);
		~CGIManager(void);

		map_ss		getEnv() const;
		std::string	getPlainText() const;

		bool	pipe();
		bool	getCGIResponse();
		bool	exec();
		bool	launchExec() const;
		
};
