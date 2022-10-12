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
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>

#include "Request.hpp"
#include "Server.hpp"
#include "CGIEnviron.hpp"

#define REQUEST_METHOD "REQUEST_METHOD"
#define QUERY_STRING "QUERY_STRING"
#define CONTENT_LENGTH "CONTENT_LENGTH"  
#define CONTENT_TYPE "CONTENT_TYPE"
#define GATEWAY_INTERFACE "GATEWAY_INTERFACE"
#define SERVER_SOFTWARE "SERVER_SOFTWARE"
#define SERVER_NAME "SERVER_NAME"
#define SERVER_PROTOCOL "SERVER_PROTOCOL"
#define SERVER_PORT "SERVER_PORT"
#define PATH_INFO "PATH_INFO"
#define PATH_TRANSLATED "PATH_TRANSLATED"
#define DOCUMENT_ROOT "DOCUMENT_ROOT"
#define SCRIPT_NAME "SCRIPT_NAME"
#define REMOTE_HOST "REMOTE_HOST"
#define REMOTE_ADDR "REMOTE_ADDR"
#define REMOTE_USER "REMOTE_USER"
#define REMOTE_IDENT "REMOTE_IDENT"
#define AUTH_TYPE "AUTH_TYPE"

#define PRINT(x) std::cerr << std::boolalpha << "\nin " << __FILE__ << ":" << __LINE__ << "\n"<< __func__ << ": " << #x << "\n" << x << std::endl;
// Following https://www.ietf.org/rfc/rfc3875.txt
class CGIManager {
	public:
		typedef std::map<std::string, std::string>			map_ss;

	private:
		CGIEnviron		_environ;
		int				_cgi_response_fds[2];
		int				_cgi_request_fds[2];
		size_t			_content_length;
		std::string		_request_data;
		size_t			_request_data_length;
		std::string		_plaintext;


		CGIManager(void);
		CGIManager(const CGIManager &src);
		CGIManager& operator=(const CGIManager &src);

		void		_close_fds();
		bool		_pipe();
		void		_launchExec() const;
		bool		_getCGIResponse();

	public:
		CGIManager(const Request& req, const Server& serv);
		~CGIManager(void);

		std::string	getPlainText() const;

		static void	signal_pipe_handler(int signo);

		bool		exec();
		
};
