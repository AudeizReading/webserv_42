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

class Request
{
private:
	int				_fd;
	int				_complete;
	std::string		_plaintext;

	struct _Firstline {
		std::string method;
		std::string uri;
		std::string http_version;
	};
	_Firstline		_firstline;

	void		_read_buffer();
	void		_parse();

	void	_read_firstline(const std::string &str);

public:
	Request(int fd);

	~Request();

	int is_complete() const;
};
