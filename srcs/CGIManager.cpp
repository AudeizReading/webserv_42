
#include "CGIManager.hpp"

// - Constr / Destr ------------------------------------------------------------ 
CGIManager::CGIManager(const Request& req) : _request(req), _env(), _content_length(0), _plaintext() {
	try
	{
		this->pipe();
		this->_setEnv();
	}
	catch(const std::exception& e)
	{
		::close(_fds[1]);
		::close(_fds[0]);
		std::cerr << e.what() << '\n';
		return ;
	}
}

CGIManager::~CGIManager(void) {
	::close(_fds[1]);
	::close(_fds[0]);
}

// - Accessors ----------------------------------------------------------------- 
CGIManager::map_ss	CGIManager::getEnv()		const { return this->_env; }
std::string			CGIManager::getPlainText()	const { return this->_plaintext; }

CGIManager&			CGIManager::_setEnv()
{
	PRINT(_request.get_location());
	// List of the META expected by CGI rfc:
	// meta-variable-name = "AUTH_TYPE" | "CONTENT_LENGTH" |
    //                       "CONTENT_TYPE" | "GATEWAY_INTERFACE" |
    //                       "PATH_INFO" | "PATH_TRANSLATED" |
    //                       "QUERY_STRING" | "REMOTE_ADDR" |
    //                       "REMOTE_HOST" | "REMOTE_IDENT" |
    //                       "REMOTE_USER" | "REQUEST_METHOD" |
    //                       "SCRIPT_NAME" | "SERVER_NAME" |
    //                       "SERVER_PORT" | "SERVER_PROTOCOL" |
    //                       "SERVER_SOFTWARE" | scheme |
    //                       protocol-var-name | extension-var-name
    //  protocol-var-name  = ( protocol | scheme ) "_" var-name
    //  scheme             = alpha *( alpha | digit | "+" | "-" | "." )
    //  var-name           = token
    //  extension-var-name = token
    //
	// GATEWAY_INTERFACE	The GATEWAY_INTERFACE variable MUST be set to the dialect of CGI
	//						being used by the server to communicate with the script.
	//						CGI/1.1
	// GATEWAY_INTERFACE = "CGI" "/" 1*digit "." 1*digit
	//
	// SERVER_NAME
	// SERVER_SOFTWARE
	// SERVER_PROTOCOL
	// SERVER_PORT
	// REQUEST_METHOD
	//
	// PATH_INFO			The PATH_INFO variable specifies a path to be interpreted by the CGI script.  
	//						It identifies the resource or sub-resource to be returned by the CGI script, 
	//						and is derived from the portion of the URI path hierarchy following the part 
	//						that identifies the script itself.
	//
	// PATH_TRANSLATED
	// SCRIPT_NAME
	// DOCUMENT_ROOT
	// QUERY_STRING
	// REMOTE_HOST
	// REMOTE_ADDR
	// REMOTE_USER
	// REMOTE_IDENT
	//
	// AUTH_TYPE		= "" | auth-scheme
    // auth-scheme		= "Basic" | "Digest" | extension-auth
    // extension-auth	= token
    // For HTTP, if the client request required authentication for external
    // access, then the server MUST set the value of this variable from the
    // 'auth-scheme' token in the request Authorization header field.

	// CONTENT_TYPE			If the request includes a message-body, the CONTENT_TYPE variable is
    //						set to the Internet Media Type [6] of the message-body. 
    //						There is no default value for this variable.  
    //						If and only if it is unset, then the script MAY attempt to determine the media type 
    //						from the data received.  If the type remains unknown, then the script MAY choose to 
    //						assume a type of application/octet-stream or it may reject the request with an error
    // CONTENT_TYPE = "" | media-type
    // media-type   = type "/" subtype *( ";" parameter )
    // type         = token
    // subtype      = token
    // parameter    = attribute "=" value
    // attribute    = token
    // value        = token | quoted-string
    //
	// CONTENT_LENGTH		= The CONTENT_LENGTH variable contains the size of the message-body attached to the request
	//
	// HTTP_FROM
	// HTTP_ACCEPT
	// HTTP_USER_AGENT
	// HTTP_REFERER
	return *this;
}

// - Accessors ----------------------------------------------------------------- 
bool				CGIManager::pipe() 
{
	if (::pipe(_fds) == -1)
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

	while ((0 < (tmp_sz = ::read(_fds[0], buffer, PIPE_BUF))))
	{
		buffer[tmp_sz] = 0;
		input_sz += tmp_sz;
		this->_plaintext += buffer;
	}

	size_t	p_size = _plaintext.size();

//	std::cerr << "_plaintext: " << _plaintext << std::endl;
	// It is forbidden by subject to check errno after a write, so check it manually
	// If we have read the same num of octets than writen them, _content_length takes this value, else throws exception
	(input_sz == p_size && (this->_content_length = p_size));
	if (p_size != input_sz)
	{
		throw std::runtime_error(strerror(EIO));
		return false;
	}
	return true;
}

bool				CGIManager::exec() 
{
	pid_t	pid = ::fork();
//	char	buffer[PIPE_BUF] = {0};

	switch (pid)
	{
		case -1:
			throw std::runtime_error(strerror(errno));
			return false;
		case 0:
			::close(_fds[0]);
			::close(STDOUT_FILENO);
			::dup2(_fds[1], STDOUT_FILENO);
			::close(_fds[1]);
			if (!this->launchExec())
				return false;
			break;
		default:
			::close(_fds[1]);
			this->getCGIResponse();
			::close(_fds[0]);
			::waitpid(pid, NULL, 0);
			break;
	}
	return true;
}

bool				CGIManager::launchExec() const
{
	// first trying with execl and ls cmd
	// do not forget to check the PATH rights (only exec has to be set)
	char str[] = "CGI_TEST='What's happend here for the now?'";
	::putenv(str);
	if (::execl("./demo/www/cgi-bin/apply-for-iceberg.pl", "./demo/www/cgi-bin/apply-for-iceberg.pl", NULL) == -1)
	{
		throw std::runtime_error(strerror(errno));
		return false;
	}
	return true;
}
