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

#include "CGIManager.hpp"

// - Constr / Destr ------------------------------------------------------------ 
CGIManager::CGIManager(const Request& req, const Server& serv) : _request(req), _server(serv), _env(), _content_length(0), _plaintext() {
	try
	{
		this->pipe();
		this->_setEnv();
	}
	catch(const std::exception& e)
	{
		::close(_cgi_response_fds[0]);
		::close(_cgi_response_fds[1]);
		::close(_cgi_request_fds[0]);
		::close(_cgi_request_fds[1]);
		std::cerr << e.what() << '\n';
		return ;
	}
}

CGIManager::~CGIManager(void) {
	::close(_cgi_response_fds[0]);
	::close(_cgi_response_fds[1]);
	::close(_cgi_request_fds[0]);
	::close(_cgi_request_fds[1]);
}

// - Accessors ----------------------------------------------------------------- 
CGIManager::map_ss	CGIManager::getEnv()		const { return this->_env; }
std::string			CGIManager::getPlainText()	const { return this->_plaintext; }

void				CGIManager::_putenv(const char *name, const char *value)
{
	::setenv(name, value, 1);
}

CGIManager&			CGIManager::_setEnv()
{
	std::string query_string_key	= "QUERY_STRING";	// to take from config file
	std::string request_method_key	= "REQUEST_METHOD";	// to take from config file
	std::string content_length_key	= "CONTENT_LENGTH";	// to take from config file
	std::string content_type_key	= "CONTENT_TYPE";	// to take from config file

	this->_putenv(GATEWAY_INTERFACE, "CGI/1.1"); 		// to take from config file
	this->_putenv(SERVER_SOFTWARE, "webserv");
	this->_putenv(SERVER_NAME, _server.get_name().c_str());
	this->_putenv(SERVER_PROTOCOL, "HTTP/1.1");	// TODO: va falloir stocker firstline.http_version dans _request._http_version et Je te laisse faire les getters :p 
	this->_putenv(SERVER_PORT, "4242");				// TODO: from _sever

	this->_putenv(request_method_key.c_str(), _request.get_method().c_str());

	this->_putenv(query_string_key.c_str(), _request.get_query().c_str());
	this->_putenv(query_string_key.c_str(), _request.get_query().c_str());

	std::string path = _server.get_root() + _request.get_location(); // TODO: utiliser _server.get_name()
	this->_putenv(PATH_INFO, path.c_str());
	this->_putenv(PATH_TRANSLATED, path.c_str());
	this->_putenv(DOCUMENT_ROOT, _server.get_root().c_str());
	this->_putenv(SCRIPT_NAME, path.c_str());
//	PRINT(path.c_str());

	this->_putenv(REMOTE_HOST, "-> to take from request");
	this->_putenv(REMOTE_ADDR, "-> to take from request");
	this->_putenv(REMOTE_USER, "-> to take from request");
	this->_putenv(REMOTE_IDENT, "-> to take from request");

	//Utile celui la?
	//this->_putenv("AUTH_TYPE", "-> to take from request");

	Request::map_ss				header = _request.get_header();
	Request::map_ss::iterator	it2;
		
	for (it2 = header.begin(); it2 != header.end(); it2++)
	{
		std::string http_key = it2->first;
		std::transform(http_key.begin(), http_key.end(),http_key.begin(), toupper);
		std::replace(http_key.begin(), http_key.end(), '-', '_');
		// std::cerr << " add " << http_key << ": " << it2->second << std::endl;
		this->_putenv(("HTTP_" + http_key).c_str(), it2->second.c_str());
	}

	std::string					content = _request.get_content();
	this->_putenv(content_length_key.c_str(), std::to_string(content.length()).c_str());
	this->_putenv(content_type_key.c_str(), header["Content-Type"].c_str());

	return *this;
}

// - Accessors ----------------------------------------------------------------- 
bool				CGIManager::pipe() 
{
	if (::pipe(_cgi_response_fds) == -1)
	{
		throw std::runtime_error(strerror(errno));
		return false;
	}
	if (::pipe(_cgi_request_fds) == -1)
	{
		throw std::runtime_error(strerror(errno));
		return false;
	}
	if (::signal(SIGPIPE, signal_pipe_handler) == SIG_ERR)
	{
		throw std::runtime_error(strerror(errno));
		return false;
	}
	return true;
}

bool				CGIManager::getCGIResponse()
{
	char	buffer[PIPE_BUF] = {0};
	size_t	tmp_sz = 0;
	size_t	input_sz = 0;

	while ((0 < (tmp_sz = ::read(_cgi_response_fds[0], buffer, PIPE_BUF))))
	{
		buffer[tmp_sz] = 0;
		input_sz += tmp_sz;
		this->_plaintext += buffer;
	}

	size_t	p_size = _plaintext.size();
	// TODO: convert int into char * without segfault
//	this->_putenv("CONTENT_LENGTH", reinterpret_cast<const char*>(p_size));
	std::stringstream			response;

	response <<  p_size;
//	this->_putenv("CONTENT_LENGTH", reinterpret_cast<const char *>(response.str()));

	// It is forbidden by subject to check errno after a write, so check it manually
	// If we have read the same num of octets than writen them, _content_length takes this value, else throws exception
	(input_sz == p_size && (this->_content_length = p_size));
// This throws an error but for the now i will comment it bc, i'm not sure i've the right content_lenght and it is not the purpose of the day so only commenting it for passing and checking if errors are throwns as we want
/*	if (p_size != input_sz)

	{
		throw std::runtime_error(strerror(EIO));
		return false;
	}*/
	return true;
}

void				CGIManager::signal_pipe_handler(int signo)
{
	if (signo == SIGPIPE)
	{
		std::cerr << "\033[31;1m[SIGPIPE]: " << __FILE__ << " " << __LINE__ << ": SIGPIPE received from the fds dedicated to to CGI\033[0m" << std::endl;
		throw std::runtime_error(strerror(errno));
		exit(-1);
	}
}

bool				CGIManager::exec() 
{

	int	exit_status = 0;
	pid_t	pid = ::fork();
	switch (pid)
	{
		case -1:
			throw std::runtime_error(strerror(errno));
			return false;
		case 0:
			// try catch here
		//	try
		//	{
				::close(_cgi_response_fds[0]);

				::close(STDOUT_FILENO);
				::dup2(_cgi_response_fds[1], STDOUT_FILENO);
				//		::write(_cgi_response_fds[1], _request.get_content().c_str(), _request.get_content().size());

				::close(_cgi_response_fds[1]);
				this->launchExec();
	//		}
		//	catch(const std::exception& e)
		//	{
				std::cerr << "\033[31;1m[CGI]: " << __FILE__ << " " << __LINE__ << ": problem with the CGI executable\033[0m" << std::endl;
				return false;
		//	}
			break;
		default:
		//	try
		//	{
				::close(_cgi_response_fds[1]);
				this->getCGIResponse();
				::close(_cgi_response_fds[0]);

				::waitpid(pid, &exit_status, 0);
				if (exit_status != 0)
					throw std::runtime_error(strerror(WEXITSTATUS(exit_status)));
		//	}
		//	catch(const std::exception& e)
		//	{
	//			std::cerr << "\033[31;1m[CGI]: " << __FILE__ << " " << __LINE__ << ": problem with the CGI response\033[0m" << std::endl;
		//		return false;
		//	}
			break;
	}
	return true;
}

void				CGIManager::launchExec() const
{
	// first trying with execl and ls cmd
	// do not forget to check the PATH rights (only exec has to be set)
	if (::execl(::getenv("SCRIPT_NAME"), ::getenv("SCRIPT_NAME"), NULL) == -1)
	{
	//	throw std::runtime_error(strerror(errno));
	//	return false;
		exit(errno);
		std::cerr << "\033[31;1m[CGI]: " << __FILE__ << " " << __LINE__ << "Cette phrase ne doit jamais etre visible\033[0m" << std::endl;
	}
}
