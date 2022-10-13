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

#include "CGIEnviron.hpp"

CGIEnviron::CGIEnviron(const Request& req, const Server& serv) : _request(req), _server(serv), _env() {
	this->_setEnv();
}

CGIEnviron::~CGIEnviron(void) {
}

CGIEnviron::map_ss	CGIEnviron::getEnv()		const { return this->_env; }
bool				CGIEnviron::_putenv(const char *name, const char *value)
{
	if (::setenv(name, value, 1) == -1)
	{
		throw std::runtime_error(strerror(errno));
		return false;
	}
	return true;
}

CGIEnviron&			CGIEnviron::_setEnv()
{
	std::string					path = _server.get_root() + _request.get_location(); // TODO: utiliser _server.get_name()

	// method POST
	std::string					content = this->_request.get_content();
	std::string					content_length = std::to_string(content.length());

	Request::map_ss				header = this->_request.get_header();
	Request::map_ss::iterator	header_begin = header.begin();
	Request::map_ss::iterator	header_end = header.end();
		
	for (; header_begin != header_end; ++header_begin)
	{
		std::string http_key = header_begin->first;
		std::string http_val = header_begin->second;

		std::transform(http_key.begin(), http_key.end(), http_key.begin(), toupper);
		std::replace(http_key.begin(), http_key.end(), '-', '_');

	//	 std::cerr << " add " << http_key << ": " << header_begin->second << std::endl;

		this->_env.insert(value_type(("HTTP_" + http_key), header_begin->second));
		this->_env.insert(value_type((http_key), header_begin->second));
	}

	std::string	cgi_env[][2] = {\
			{REQUEST_METHOD, this->_request.get_method()},\
			{QUERY_STRING, this->_request.get_query()},\
			{CONTENT_LENGTH, content_length}, \
			{CONTENT_TYPE, header["Content-Type"]}, \
			{GATEWAY_INTERFACE, "\033[43;30mCGI/1.1\033[0m"}, \
			{SERVER_SOFTWARE, "\033[43;30mwebserv\033[0m"}, \
			{SERVER_NAME, _server.get_name()}, \
			{SERVER_PROTOCOL, this->_request.get_http_version()}, \
			{SERVER_PORT, "\033[43;30m4242\033[0m"}, \
			{PATH_INFO, path.c_str()}, \
			{PATH_TRANSLATED, path.c_str()}, \
			{DOCUMENT_ROOT, this->_server.get_root().c_str()}, \
			{SCRIPT_NAME, path.c_str()}, \
			{REMOTE_HOST, "\033[44;37m-> has to be taken from request?\033[0m"}, \
			{REMOTE_ADDR, "\033[44;37m-> has to be taken from request?\033[0m"}, \
			{REMOTE_USER, "\033[44;37m-> has to be taken from request?\033[0m"}, \
			{REMOTE_IDENT, "\033[44;37m-> has to be taken from request?\033[0m"}, \
			{AUTH_TYPE, "\033[44;37m-> has to be taken from request?\033[0m"}\
	};

	for (int i = 0; i != 18; ++i) // -> this is very very ugly I've failed when i've tried with iterator, so as we have no time I take this way it is more faster though I would prefer make it properly
	{
		this->_env.insert(value_type(cgi_env[i][0], cgi_env[i][1]));
	}

	for (map_ss::iterator it = this->_env.begin(); it != this->_env.end(); ++it)
	{
		 try {
			 this->_putenv(it->first.c_str(), it->second.c_str());
		 }
		 catch(const std::exception& e)
		 {
			 std::cerr << "\033[31;1m[CGI]: " << __FILE__ << " " << __LINE__ << " " << e.what() << ": problem with the CGI environment\033[0m" << std::endl;
			 std::cerr << e.what() << '\n';
			 return *this;
		 }
	}
//	this->_putenv(SERVER_PORT, "4242");				// TODO: from _sever

	return *this;
}
