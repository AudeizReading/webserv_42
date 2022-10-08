
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

void				CGIManager::_putenv(const char *name, const char *value)
{
	::setenv(name, value, 1);
}

CGIManager&			CGIManager::_setEnv()
{
//	PRINT(_request.get_location()); // maybe need the query part if GET ?
	this->_putenv("GATEWAY_INTERFACE", "CGI/1.1"); // to take from config file
	this->_putenv("SERVER_SOFTWARE", "webserv"); // to take from config file
	this->_putenv("SERVER_NAME", "Groenland"); // Because it is a big Iceberg to take from config file
	this->_putenv("SERVER_PROTOCOL", "HTTP/1.1"); // to take from config file
	this->_putenv("SERVER_PORT", "4242"); // to take from config file

	this->_putenv("REQUEST_METHOD", "POST -> to take from request");
	this->_putenv("QUERY_STRING", "?... -> to take from request");

	this->_putenv("PATH_INFO", "./demo/www/cgi-bin/apply-for-iceberg.pl"); // to take from config file
	this->_putenv("PATH_TRANSLATED", "./demo/www/cgi-bin/apply-for-iceberg.pl"); // to take from config file
	this->_putenv("DOCUMENT_ROOT", "./demo/www/cgi-bin/apply-for-iceberg.pl"); // to take from config file
	this->_putenv("SCRIPT_NAME", "./demo/www/cgi-bin/apply-for-iceberg.pl"); // to take from config file

	this->_putenv("REMOTE_HOST", "-> to take from request");
	this->_putenv("REMOTE_ADDR", "-> to take from request");
	this->_putenv("REMOTE_USER", "-> to take from request");
	this->_putenv("REMOTE_IDENT", "-> to take from request");

	this->_putenv("AUTH_TYPE", "-> to take from request");
	this->_putenv("CONTENT_TYPE", "text/html -> to take from the CGI script");
	this->_putenv("CONTENT_LENGTH", "length of the output -> to take from the CGI script");

	this->_putenv("HTTP_FROM", "-> to take from request");
	this->_putenv("HTTP_ACCEPT", "-> to take from request");
	this->_putenv("HTTP_USER_AGENT", "-> to take from request");
	this->_putenv("HTTP_REFERER", "the last page client has visited before requested our server -> to take from request");
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
	// TODO: convert int into char * without sefault
//	this->_putenv("CONTENT_LENGTH", reinterpret_cast<const char*>(p_size));
	std::stringstream			response;

	response <<  p_size;
//	this->_putenv("CONTENT_LENGTH", reinterpret_cast<const char *>(response.str()));

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
	if (::execl(::getenv("SCRIPT_NAME"), ::getenv("SCRIPT_NAME"), NULL) == -1)
	{
		throw std::runtime_error(strerror(errno));
		return false;
	}
	return true;
}
