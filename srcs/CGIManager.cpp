
#include "CGIManager.hpp"

// - Constr / Destr ------------------------------------------------------------ 
CGIManager::CGIManager(const Request& req, int& sock) : _request(req), _socket(sock), _env(), _content_length(0) {
	try
	{
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
CGIManager::map_ss	CGIManager::getEnv() const { return this->_env; }

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

bool				CGIManager::fork() 
{
	pid_t	pid = ::fork();
	char	buffer[PIPE_BUF] = {0};

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
			if (!this->exec())
				return false;
			break;
		default:
			::close(_fds[1]);

			size_t	tmp_sz = 0;
			size_t	input_sz = 0;
			size_t	output_sz = 0;

			// read fd[0], but where ?
			// write directly in socket or in ostream?
			while ((0 < (tmp_sz = ::read(_fds[0], buffer, PIPE_BUF))))
			{
				input_sz += tmp_sz;
				output_sz += ::write(STDERR_FILENO, buffer, tmp_sz);
			}

			// It is forbidden by subject to check errno after a write, so check it manually
			// If we have read the same num of octets than writen them, _content_length takes this value, else throws exception
			(output_sz == input_sz && (this->_content_length = output_sz));
			if (output_sz != input_sz)
			{
				throw std::runtime_error(strerror(EIO));
				return false;
			}
			::close(_fds[0]);
			::waitpid(pid, NULL, 0);
			break;
	}
	return true;
}

bool				CGIManager::exec() const
{
	// first trying with execl and ls cmd
	// do not forget to check the PATH rights (only exec has to be set)
	if (::execl("/bin/ls", "/bin/ls", "-la", NULL) == -1)
	{
		throw std::runtime_error(strerror(errno));
		return false;
	}
	return true;
}
