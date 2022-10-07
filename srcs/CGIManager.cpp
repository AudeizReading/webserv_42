
#include "CGIManager.hpp"

// - Constr / Destr ------------------------------------------------------------ 
CGIManager::CGIManager(const Request& req) : _request(req), _env(), _content_length(0), _plaintext() {
	try
	{
		// un blabla de commit
		this->pipe();
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
	if (::execl("./demo/www/cgi-bin/apply-for-iceberg.pl", "./demo/www/cgi-bin/apply-for-iceberg.pl", NULL) == -1)
	{
		throw std::runtime_error(strerror(errno));
		return false;
	}
	return true;
}
