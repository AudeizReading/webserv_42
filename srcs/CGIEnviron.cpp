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

CGIEnviron::CGIEnviron(const Request& req, const Server& serv, const Location& location) : _request(req), _server(serv), _location(location), _header(this->_request.get_header()), _env() {
	this->_setGlobalEnv();
}

CGIEnviron::~CGIEnviron(void) {
}

CGIEnviron::map_ss	CGIEnviron::getEnv()		const { return this->_env; }

void				CGIEnviron::_setHeaderEnv()
{
	Request::map_ss::iterator	header_begin = _header.begin();
	Request::map_ss::iterator	header_end = _header.end();
		
	for (; header_begin != header_end; ++header_begin)
	{
		std::string http_key = header_begin->first;
		std::string http_val = header_begin->second;

		std::transform(http_key.begin(), http_key.end(), http_key.begin(), toupper);
		std::replace(http_key.begin(), http_key.end(), '-', '_');

		this->_env.insert(value_type(("HTTP_" + http_key), header_begin->second));
		this->_env.insert(value_type((http_key), header_begin->second));
	}
}

void				CGIEnviron::_setEnv()
{
	std::string	location = _request.get_location();
	std::string	ext = location.substr(location.find_last_of(".") + 1);

	if (*location.begin() == '/')
	{
		location = location.substr(location.find_first_of("/") + 1); // delete first / because root has already it at its end
	}

	// La racine de l'endroit où tu es, c'est la root de la Location + son URI.
	// http://nginx.org/en/docs/beginners_guide.html, section "Serving Static Content"
	std::string	root = _location.get_root() + _location.get_URI();
	// Il me semble que SERVER_NAME doit être le champ "Host" de la requête.
	std::string	server_name = "TESTME";

	std::string	port = _server.get_port_str();

//	std::string	remote_host = _request.get_?();
//	std::string	remote_addr = _request.get_?();
//	std::string	remote_user = _request.get_?();

	std::string	script_name = root + location;
	// path_info : This is the path part after the script name (yes it can) -> considered as infos on the path by rfc
	std::string	path_info = location.substr(location.find_last_of(".") + ext.size() + 1);
	// path_translated : This is DOCUMENT_ROOT + PATH_INFO
	std::string	path_translated = root + path_info;

	// method POST
	std::string	content = this->_request.get_content();
	std::string	content_length = std::to_string(content.length());

	this->_setHeaderEnv();

	std::string	cgi_env[][2] = {\
			{REQUEST_METHOD, this->_request.get_method()},\
			{QUERY_STRING, this->_request.get_query()},\
			{CONTENT_LENGTH, content_length}, \
			{CONTENT_TYPE, this->_header["Content-Type"]}, \
			{GATEWAY_INTERFACE, "\033[43;30m MUST CGI/1.1\033[0m"}, \
			{SERVER_SOFTWARE, "\033[43;30m MUST webserv\033[0m"}, \
			{SERVER_NAME, server_name}, \
			{SERVER_PROTOCOL, this->_request.get_http_version()}, \
			{SERVER_PORT, "\033[43;30m MUST 4242\033[0m"}, \
			{PATH_INFO, path_info}, \
			{PATH_TRANSLATED, path_translated}, \
			{DOCUMENT_ROOT, root}, \
			{SCRIPT_NAME, script_name}, \
			{REMOTE_HOST, "\033[44;37m MUST -> has to be taken from request?\033[0m"}, \
			{REMOTE_ADDR, "\033[44;37m MUST -> has to be taken from request?\033[0m"}, \
			{REMOTE_USER, "\033[44;37m-> mandatory if client's request ask for auth\033[0m"}, \
	};

	for (int i = 0; i != get_arr_2D_width(cgi_env); ++i) // -> this is very very ugly I've failed when i've tried with iterator, so as we have no time I take this way it is more faster though I would prefer make it properly
	{
		this->_env.insert(value_type(cgi_env[i][0], cgi_env[i][1]));
	}
}

bool				CGIEnviron::_putenv(const char *name, const char *value)
{
	if (::setenv(name, value, 1) == -1)
	{
		throw std::runtime_error(strerror(errno));
		return false;
	}
	return true;
}

CGIEnviron&			CGIEnviron::_setGlobalEnv()
{
	this->_setEnv();
	
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
	return *this;
}
