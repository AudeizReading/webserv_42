
#include "CGIManager.hpp"

// - Constr / Destr ------------------------------------------------------------ 
CGIManager::CGIManager(const Request& req, int& sock) : _request(req), _socket(sock), _env() {
	try
	{
		std::cout << "Hello, it works!" << std::endl;
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
			while ((0 < ::read(_fds[0], buffer, PIPE_BUF)))
			{
				::write(STDERR_FILENO, buffer, PIPE_BUF);
			}
			// read fd[0], but where ?
			// write directly in socket or in ostream?
			::close(_fds[0]);
			::waitpid(pid, NULL, 0);
			break;
	}
	return true;
}

bool				CGIManager::exec() const
{
	// first trying with execl and ls cmd
	if (::execl("/bin/ls", "/bin/ls", "-la", NULL) == -1)
	{
		throw std::runtime_error(strerror(errno));
		return false;
	}
	return true;
}
